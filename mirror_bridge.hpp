#pragma once

// ═══════════════════════════════════════════════════════════════════════════
// Mirror Bridge - Automatic C++ to Python Bindings via C++26 Reflection
// ═══════════════════════════════════════════════════════════════════════════
//
// A showcase of modern C++26 techniques for zero-overhead metaprogramming:
//
// • C++26 Reflection (P2996): Compile-time class introspection
//   - std::meta::members_of(^^T) - discover class structure
//   - [:expr:] splicers - inject reflected code back into source
//   - consteval - guaranteed compile-time evaluation
//
// • C++20 Concepts: Type constraints without SFINAE noise
//   - Arithmetic, Container, SmartPointer concept definitions
//   - Enables elegant function overloading based on type properties
//
// • Modern Template Metaprogramming:
//   - Variadic templates with parameter packs (Args...)
//   - Fold expressions (expr, ...) for pack expansion
//   - std::index_sequence for compile-time iteration
//   - Template recursion instead of runtime loops in consteval contexts
//
// • SFINAE via std::enable_if_t: When concepts aren't enough
//   - Forward declarations for complex type hierarchies
//   - Ensures proper template instantiation for nested types
//
// Zero Runtime Overhead:
//   All binding code is generated at compile-time. No vtables, no RTTI lookups,
//   no runtime type checking. Direct member access via compile-time offsets.
//
// Requirements:
//   - Bloomberg clang-p2996 with C++26 reflection support
//   - Compile flags: -std=c++2c -freflection -freflection-latest -stdlib=libc++
//   - Python 3.7+ development headers
//
// Usage:
//   #include "mirror_bridge.hpp"
//   MIRROR_BRIDGE_MODULE(my_module,
//       mirror_bridge::bind_class<MyClass>(m, "MyClass");
//   )
//
// ═══════════════════════════════════════════════════════════════════════════

#include <meta>
#include <Python.h>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <type_traits>
#include <concepts>
#include <functional>
#include <memory>
#include <unordered_map>
#include <ranges>
#include <sstream>

namespace mirror_bridge {

// ============================================================================
// Type Traits and Concepts (inspired by simdjson's approach)
// ============================================================================

// Concept to identify arithmetic types (int, float, double, etc.)
template<typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

// Concept to identify string-like types (std::string, std::string_view, const char*, etc.)
// Using simdjson's pattern: remove cv-qualifiers and references before checking
template<typename T>
concept StringLike =
    std::is_same_v<std::remove_cvref_t<T>, std::string> ||
    std::is_same_v<std::remove_cvref_t<T>, std::string_view> ||
    std::is_same_v<std::remove_cvref_t<T>, const char*> ||
    std::is_same_v<std::remove_cvref_t<T>, char*>;

// Concept to identify smart pointers (unique_ptr, shared_ptr)
template<typename T>
concept SmartPointer = requires {
    typename std::remove_cvref_t<T>::element_type;
} && (std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<typename std::remove_cvref_t<T>::element_type>> ||
      std::is_same_v<std::remove_cvref_t<T>, std::shared_ptr<typename std::remove_cvref_t<T>::element_type>>);

// Concept to identify C++ standard containers (vector, array, list, etc.)
// Following simdjson: use ranges to detect containers, but exclude strings
template<typename T>
concept Container =
    std::ranges::input_range<std::remove_cvref_t<T>> &&
    !StringLike<T> &&
    !SmartPointer<T> &&
    requires(std::remove_cvref_t<T> c) {
        typename std::remove_cvref_t<T>::value_type;
    };

// Concept for types that can be bound to Python (classes with reflectable members)
template<typename T>
concept Bindable = std::is_class_v<std::remove_cvref_t<T>> && requires {
    { std::meta::nonstatic_data_members_of(^^std::remove_cvref_t<T>, std::meta::access_context::current()) };
};

// Concept for nested bindable classes (used as member types in other classes)
template<typename T>
concept NestedBindable = Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T> && !SmartPointer<T>;

// Forward declarations for nested bindable type conversion
// These match the SFINAE templates defined later in the file
template<typename T>
std::enable_if_t<
    Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T> && !SmartPointer<T>,
    PyObject*
>
to_python(const T& obj);

template<typename T>
std::enable_if_t<
    Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T> && !SmartPointer<T>,
    bool
>
from_python(PyObject* obj, T& out);

// ============================================================================
// Python Type Conversion Utilities
// ============================================================================

// Convert C++ types to Python objects
template<Arithmetic T>
PyObject* to_python(const T& value) {
    if constexpr (std::is_floating_point_v<T>) {
        return PyFloat_FromDouble(static_cast<double>(value));
    } else if constexpr (std::is_signed_v<T>) {
        return PyLong_FromLong(static_cast<long>(value));
    } else {
        return PyLong_FromUnsignedLong(static_cast<unsigned long>(value));
    }
}

template<StringLike T>
PyObject* to_python(const T& value) {
    using BaseType = std::remove_cvref_t<T>;
    if constexpr (std::is_same_v<BaseType, std::string>) {
        return PyUnicode_FromStringAndSize(value.data(), value.size());
    } else if constexpr (std::is_same_v<BaseType, std::string_view>) {
        return PyUnicode_FromStringAndSize(value.data(), value.size());
    } else {
        return PyUnicode_FromString(value);
    }
}

// Smart pointer conversion - converts pointee, not pointer itself
template<SmartPointer T>
inline PyObject* to_python(const T& ptr) {
    if (!ptr) {
        Py_RETURN_NONE;
    }
    using ElementType = typename std::remove_cvref_t<T>::element_type;
    // Let overload resolution choose - non-template overloads have higher priority
    return to_python(*ptr);
}

// Convert Python objects to C++ types
template<Arithmetic T>
bool from_python(PyObject* obj, T& out) {
    if constexpr (std::is_floating_point_v<T>) {
        if (!PyFloat_Check(obj) && !PyLong_Check(obj)) return false;
        out = static_cast<T>(PyFloat_AsDouble(obj));
        return true;
    } else if constexpr (std::is_signed_v<T>) {
        if (!PyLong_Check(obj)) return false;
        out = static_cast<T>(PyLong_AsLong(obj));
        return true;
    } else {
        if (!PyLong_Check(obj)) return false;
        out = static_cast<T>(PyLong_AsUnsignedLong(obj));
        return true;
    }
}

template<StringLike T>
inline bool from_python(PyObject* obj, T& out) {
    using BaseType = std::remove_cvref_t<T>;
    if constexpr (std::is_same_v<BaseType, std::string>) {
        if (!PyUnicode_Check(obj)) return false;
        Py_ssize_t size;
        const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
        if (!data) return false;
        out = std::string(data, size);
        return true;
    } else if constexpr (std::is_same_v<BaseType, std::string_view>) {
        // Note: string_view requires the underlying string to remain valid
        // For Python->C++ conversion, we typically need to store strings, not views
        // This is mainly useful for temporary conversions
        if (!PyUnicode_Check(obj)) return false;
        Py_ssize_t size;
        const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
        if (!data) return false;
        out = std::string_view(data, size);
        return true;
    } else {
        // const char* and char* - these are less safe for conversion from Python
        // since Python strings are managed objects, but we support them for compatibility
        if (!PyUnicode_Check(obj)) return false;
        const char* data = PyUnicode_AsUTF8(obj);
        if (!data) return false;
        out = data;
        return true;
    }
}

// Smart pointer conversion from Python
// Creates a new instance from the Python value
template<SmartPointer T>
inline bool from_python(PyObject* obj, T& out) {
    using ElementType = typename std::remove_cvref_t<T>::element_type;

    if (obj == Py_None) {
        out.reset();
        return true;
    }

    ElementType value;
    // Let overload resolution choose - non-template overloads have higher priority
    if (!from_python(obj, value)) {
        return false;
    }

    if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::unique_ptr<ElementType>>) {
        out = std::make_unique<ElementType>(std::move(value));
    } else {
        out = std::make_shared<ElementType>(std::move(value));
    }
    return true;
}

// Convert C++ containers to Python lists
// Recursively converts each element using to_python
template<Container T>
PyObject* to_python(const T& container) {
    PyObject* list = PyList_New(container.size());
    if (!list) return nullptr;

    Py_ssize_t index = 0;
    for (const auto& item : container) {
        PyObject* py_item = to_python(item);
        if (!py_item) {
            Py_DECREF(list);
            return nullptr;
        }
        PyList_SET_ITEM(list, index++, py_item);
    }
    return list;
}

// Convert Python lists to C++ containers
// Supports any container with push_back (vector, list, deque) or insert (set, etc.)
template<Container T>
bool from_python(PyObject* obj, T& container) {
    if (!PyList_Check(obj)) return false;

    Py_ssize_t size = PyList_Size(obj);
    using ValueType = typename std::remove_cvref_t<T>::value_type;

    // Clear the container if it supports clear(), otherwise skip
    if constexpr (requires { container.clear(); }) {
        container.clear();
    }
    if constexpr (requires { container.reserve(size); }) {
        container.reserve(size);
    }

    Py_ssize_t i = 0;
    for (; i < size && i < static_cast<Py_ssize_t>(container.size()); ++i) {
        PyObject* py_item = PyList_GetItem(obj, i);  // Borrowed reference
        ValueType cpp_item;
        if (!from_python(py_item, cpp_item)) {
            return false;
        }

        // For indexed containers (array), use operator[]
        if constexpr (requires { container[i]; }) {
            container[i] = std::move(cpp_item);
        }
        // For sequential containers, use push_back
        else if constexpr (requires { container.push_back(cpp_item); }) {
            container.push_back(std::move(cpp_item));
        }
        // For associative containers, use insert
        else if constexpr (requires { container.insert(cpp_item); }) {
            container.insert(std::move(cpp_item));
        } else {
            static_assert(requires { container.push_back(cpp_item); },
                         "Container must support indexing, push_back, or insert");
        }
    }

    // For push_back containers, add remaining elements if list is larger
    if constexpr (requires { container.push_back(ValueType{}); }) {
        for (; i < size; ++i) {
            PyObject* py_item = PyList_GetItem(obj, i);
            ValueType cpp_item;
            if (!from_python(py_item, cpp_item)) {
                return false;
            }
            container.push_back(std::move(cpp_item));
        }
    }

    return true;
}

// Helper to get member count for nested bindable classes
template<typename T>
consteval std::size_t get_nested_member_count() {
    return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()).size();
}

// Helper to get nested member at index
template<typename T, std::size_t Index>
consteval auto get_nested_member() {
    return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index];
}

// Helper to get nested member name at index
template<typename T, std::size_t Index>
consteval const char* get_nested_member_name() {
    return std::meta::identifier_of(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index]
    ).data();
}

// Helper to get nested member type at index
template<typename T, std::size_t Index>
using NestedMemberType = typename [:std::meta::type_of(
    std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index]
):];

// Note: Conversion functions for nested bindable classes are now auto-generated
// via ConversionOverloadGenerator and the template overloads below

// ============================================================================
// Class Metadata and Registry
// ============================================================================

// Stores metadata about a registered class for hash-based change detection
struct ClassMetadata {
    std::string name;
    std::string type_signature;  // Reflection-derived signature for change detection
    size_t hash;                 // Hash of the type signature
    PyTypeObject* py_type;       // Python type object (nullptr before binding generation)

    // Compute hash from type signature
    void compute_hash() {
        std::hash<std::string> hasher;
        hash = hasher(type_signature);
    }

    // Check if this class needs recompilation by comparing signatures
    bool needs_recompilation(const std::string& new_signature) const {
        return type_signature != new_signature;
    }
};

// Global registry for all classes that need Python bindings
class Registry {
public:
    static Registry& instance() {
        static Registry reg;
        return reg;
    }

    // Register a class with its metadata
    void register_class(const std::string& name, const std::string& signature) {
        ClassMetadata metadata{name, signature, 0, nullptr};
        metadata.compute_hash();
        classes_[name] = metadata;
    }

    // Get metadata for a registered class
    const ClassMetadata* get_metadata(const std::string& name) const {
        auto it = classes_.find(name);
        return (it != classes_.end()) ? &it->second : nullptr;
    }

    // Get all registered classes
    const std::unordered_map<std::string, ClassMetadata>& get_all() const {
        return classes_;
    }

    // Update Python type object after binding generation
    void set_py_type(const std::string& name, PyTypeObject* type) {
        auto it = classes_.find(name);
        if (it != classes_.end()) {
            it->second.py_type = type;
        }
    }

private:
    Registry() = default;
    std::unordered_map<std::string, ClassMetadata> classes_;
};

// ============================================================================
// Reflection-Based Binding Generator
// ============================================================================

// Helper templates for type signature generation - use immediate evaluation
template<typename T, std::size_t Index>
consteval const char* get_data_member_name() {
    return std::meta::identifier_of(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index]
    ).data();
}

template<typename T, std::size_t Index>
consteval const char* get_data_member_type() {
    return std::meta::display_string_of(
        std::meta::type_of(
            std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index]
        )
    ).data();
}

// Generate a type signature for a given class using reflection
template<typename T>
std::string generate_type_signature(const char* file_hash = nullptr) {
    std::string sig{std::meta::identifier_of(^^T)};
    sig += "{";

    // Data members section - get size at compile-time
    constexpr std::size_t data_member_count = []() consteval {
        return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()).size();
    }();

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            if (Is > 0) sig += ",";
            sig += get_data_member_type<T, Is>();
            sig += " ";
            sig += get_data_member_name<T, Is>();
        }(), ...);
    }(std::make_index_sequence<data_member_count>{});

    // Member functions section
    sig += "|methods:";

    // Include file content hash if provided by build system
    // This catches implementation changes that reflection cannot detect
    if (file_hash && file_hash[0] != '\0') {
        sig += "|file_hash:";
        sig += file_hash;
    }

    sig += "}";
    return sig;
}

// Python object wrapper for C++ classes
template<typename T>
struct PyWrapper {
    PyObject_HEAD
    T* cpp_object;  // Pointer to the actual C++ object
    bool owns;      // Whether this wrapper owns the object (for cleanup)
};

// Deallocator for Python wrapper objects
template<typename T>
void py_dealloc(PyObject* self) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);
    if (wrapper->owns && wrapper->cpp_object) {
        delete wrapper->cpp_object;
    }
    Py_TYPE(self)->tp_free(self);
}

// ============================================================================
// Constructor Binding Support
// ============================================================================

// Count constructors (exclude default, copy, move)
template<typename T>
consteval std::size_t get_constructor_count() {
    auto all_members = std::meta::members_of(^^T, std::meta::access_context::current());
    std::size_t count = 0;
    for (auto member : all_members) {
        if (std::meta::is_constructor(member) &&
            !std::meta::is_copy_constructor(member) &&
            !std::meta::is_move_constructor(member)) {
            auto params = std::meta::parameters_of(member);
            // Only count non-default constructors
            if (params.size() > 0) {
                count++;
            }
        }
    }
    return count;
}

// Get the Nth non-default constructor
template<typename T, std::size_t Index>
consteval auto get_constructor() {
    auto all_members = std::meta::members_of(^^T, std::meta::access_context::current());
    std::size_t ctor_index = 0;
    for (auto member : all_members) {
        if (std::meta::is_constructor(member) &&
            !std::meta::is_copy_constructor(member) &&
            !std::meta::is_move_constructor(member)) {
            auto params = std::meta::parameters_of(member);
            if (params.size() > 0) {
                if (ctor_index == Index) {
                    return member;
                }
                ctor_index++;
            }
        }
    }
    return all_members[0];
}

// Get constructor parameter count
template<typename T, std::size_t CtorIndex>
consteval std::size_t get_constructor_param_count() {
    constexpr auto ctor = get_constructor<T, CtorIndex>();
    return std::meta::parameters_of(ctor).size();
}

// Get constructor parameter type
template<typename T, std::size_t CtorIndex, std::size_t ParamIndex>
consteval auto get_constructor_param_type() {
    constexpr auto ctor = get_constructor<T, CtorIndex>();
    auto params = std::meta::parameters_of(ctor);
    return std::meta::type_of(params[ParamIndex]);
}

// Variadic helper to call constructor with N parameters
// This uses compile-time index sequence to extract and convert each parameter
template<typename T, std::size_t CtorIndex, std::size_t... Is>
T* call_constructor_impl(PyObject* args, std::index_sequence<Is...>) {
    // Extract each parameter type
    using ParamTypes = std::tuple<
        std::remove_cvref_t<typename [:get_constructor_param_type<T, CtorIndex, Is>():]>...
    >;

    // Convert each Python argument to C++ type
    std::tuple<std::remove_cvref_t<typename [:get_constructor_param_type<T, CtorIndex, Is>():]>...> cpp_args;

    bool success = true;
    ([&] {
        if (!success) return;
        // Let overload resolution choose - non-template overloads have higher priority
        if (!from_python(PyTuple_GET_ITEM(args, Is), std::get<Is>(cpp_args))) {
            success = false;
        }
    }(), ...);

    if (!success) {
        return nullptr;
    }

    // Call constructor with unpacked arguments
    return new T(std::move(std::get<Is>(cpp_args))...);
}

// Initialize Python wrapper with parameterized constructor
// Uses reflection to find the best matching constructor
template<typename T>
int py_init(PyObject* self, PyObject* args, PyObject* kwds) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);

    Py_ssize_t nargs = PyTuple_Size(args);

    // Default constructor case
    if (nargs == 0) {
        wrapper->cpp_object = new T();
        wrapper->owns = true;
        return 0;
    }

    // Try to find matching constructor by parameter count
    constexpr std::size_t ctor_count = get_constructor_count<T>();

    bool found = false;
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            if (found) return;

            constexpr std::size_t param_count = get_constructor_param_count<T, Is>();
            if (nargs == static_cast<Py_ssize_t>(param_count)) {
                // Try to call this constructor
                T* obj = [&]() {
                    return call_constructor_impl<T, Is>(args,
                        std::make_index_sequence<param_count>{});
                }();

                if (obj) {
                    wrapper->cpp_object = obj;
                    wrapper->owns = true;
                    found = true;
                }
            }
        }(), ...);
    }(std::make_index_sequence<ctor_count>{});

    if (!found) {
        PyErr_SetString(PyExc_TypeError, "No matching constructor found");
        return -1;
    }

    return 0;
}

// Allocator for Python wrapper objects (used with tp_init)
template<typename T>
PyObject* py_new_with_init(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    auto* self = reinterpret_cast<PyWrapper<T>*>(type->tp_alloc(type, 0));
    if (self) {
        self->cpp_object = nullptr;  // Will be set by py_init
        self->owns = false;
    }
    return reinterpret_cast<PyObject*>(self);
}

// Fallback constructor for classes without parameterized constructors
template<typename T>
PyObject* py_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    auto* self = reinterpret_cast<PyWrapper<T>*>(type->tp_alloc(type, 0));
    if (self) {
        self->cpp_object = new T();
        self->owns = true;
    }
    return reinterpret_cast<PyObject*>(self);
}

// ============================================================================
// Member Access Binding
// ============================================================================

// Helper to get member reflection info at compile-time using index
template<typename T, std::size_t Index>
consteval auto get_member_info() {
    return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index];
}

// Getter for class members using reflection
template<typename T, std::size_t Index>
PyObject* py_getter(PyObject* self, void* closure) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);
    if (!wrapper->cpp_object) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid C++ object");
        return nullptr;
    }

    // Use reflection to access the member at the given index
    constexpr auto member = get_member_info<T, Index>();
    using MemberType = typename [:std::meta::type_of(member):];

    // Access the member value through reflection
    auto& value = wrapper->cpp_object->[:member:];
    // Let overload resolution choose - non-template overloads have higher priority
    return to_python(value);
}

// Setter for class members using reflection
template<typename T, std::size_t Index>
int py_setter(PyObject* self, PyObject* value, void* closure) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);
    if (!wrapper->cpp_object) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid C++ object");
        return -1;
    }

    if (!value) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete attribute");
        return -1;
    }

    // Use reflection to access the member at the given index
    constexpr auto member = get_member_info<T, Index>();
    using MemberType = typename [:std::meta::type_of(member):];

    // Convert Python value to C++ type and set the member
    MemberType cpp_value;
    // Let overload resolution choose - non-template overloads have higher priority
    if (!from_python(value, cpp_value)) {
        PyErr_SetString(PyExc_TypeError, "Type conversion failed");
        return -1;
    }

    // Use move semantics for move-only types (like unique_ptr)
    wrapper->cpp_object->[:member:] = std::move(cpp_value);
    return 0;
}

// ============================================================================
// Method Binding Support with Variadic Parameters
// ============================================================================

// Helper templates for member function reflection
// Count member functions without storing the list (avoid heap allocation)
// Exclude special member functions (constructors, destructors, operators)
template<typename T>
consteval std::size_t get_member_function_count() {
    auto all_members = std::meta::members_of(^^T, std::meta::access_context::current());
    std::size_t count = 0;
    for (auto member : all_members) {
        if (std::meta::is_function(member) &&
            !std::meta::is_static_member(member) &&
            !std::meta::is_constructor(member) &&
            !std::meta::is_special_member_function(member) &&
            !std::meta::is_operator_function(member)) {
            count++;
        }
    }
    return count;
}

// Get the Nth member function by skipping non-functions and special members
template<typename T, std::size_t Index>
consteval auto get_member_function() {
    auto all_members = std::meta::members_of(^^T, std::meta::access_context::current());
    std::size_t func_index = 0;
    for (auto member : all_members) {
        if (std::meta::is_function(member) &&
            !std::meta::is_static_member(member) &&
            !std::meta::is_constructor(member) &&
            !std::meta::is_special_member_function(member) &&
            !std::meta::is_operator_function(member)) {
            if (func_index == Index) {
                return member;
            }
            func_index++;
        }
    }
    // Should never reach here if Index is valid
    return all_members[0];
}

template<typename T, std::size_t Index>
consteval const char* get_member_function_name() {
    constexpr auto func = get_member_function<T, Index>();
    return std::meta::identifier_of(func).data();
}

// Helper functions for method parameter reflection - computed inline to avoid heap allocation
template<typename T, std::size_t FuncIndex>
consteval std::size_t get_method_param_count() {
    constexpr auto member_func = get_member_function<T, FuncIndex>();
    return std::meta::parameters_of(member_func).size();
}

template<typename T, std::size_t FuncIndex, std::size_t ParamIndex>
consteval auto get_method_param_type() {
    constexpr auto member_func = get_member_function<T, FuncIndex>();
    auto params = std::meta::parameters_of(member_func);
    return std::meta::type_of(params[ParamIndex]);
}

template<typename T, std::size_t FuncIndex>
consteval auto get_method_return_type() {
    constexpr auto member_func = get_member_function<T, FuncIndex>();
    return std::meta::return_type_of(member_func);
}

// Modern C++ Variadic Parameter Handling
//
// This function demonstrates advanced C++26 techniques:
// 1. Template parameter packs (Is...) for compile-time iteration
// 2. Reflection splicers ([:expr:]) to "inject" reflected code
// 3. Fold expressions for elegant parameter pack expansion
// 4. std::tuple for heterogeneous parameter storage
//
// Flow: Python tuple → C++ tuple → method call with perfect unpacking
template<typename T, std::size_t FuncIndex, std::size_t... Is>
PyObject* call_method_impl(PyWrapper<T>* wrapper, PyObject* args, std::index_sequence<Is...>) {
    // Use reflection to get method metadata at compile-time
    constexpr auto member_func = get_member_function<T, FuncIndex>();
    constexpr auto return_type = get_method_return_type<T, FuncIndex>();
    using ReturnType = typename [:return_type:];  // Reflection splicer: inject type

    // Create tuple with exact types from reflection
    // The tuple type is: std::tuple<Param1Type, Param2Type, ...>
    // Each type is extracted via reflection and stripped of cv-qualifiers
    std::tuple<std::remove_cvref_t<typename [:get_method_param_type<T, FuncIndex, Is>():]>...> cpp_args;

    // Modern C++ fold expression: (expr, ...)
    // Expands to: lambda(0), lambda(1), lambda(2), ... for each Is
    // Each lambda converts one Python argument to its C++ type
    bool success = true;
    ([&] {
        if (!success) return;  // Short-circuit on first failure
        // from_python is overloaded - compiler picks the right one at compile-time
        if (!from_python(PyTuple_GET_ITEM(args, Is), std::get<Is>(cpp_args))) {
            PyErr_Format(PyExc_TypeError, "Argument %zu type conversion failed", Is);
            success = false;
        }
    }(), ...);  // <- Fold expression: repeat for each Is

    if (!success) {
        return nullptr;
    }

    // Call the C++ method using reflection splicer and parameter pack expansion
    // [:member_func:] is injected as the actual member function (e.g., &T::foo)
    // std::get<Is>(cpp_args)... expands to: get<0>(cpp_args), get<1>(cpp_args), ...
    if constexpr (std::is_void_v<ReturnType>) {
        (wrapper->cpp_object->[:member_func:])(std::move(std::get<Is>(cpp_args))...);
        Py_RETURN_NONE;
    } else {
        ReturnType result = (wrapper->cpp_object->[:member_func:])(std::move(std::get<Is>(cpp_args))...);
        return to_python(result);  // Convert C++ return value back to Python
    }
}

// Python method wrapper template - now supports arbitrary parameter count!
template<typename T, std::size_t Index>
PyObject* py_method(PyObject* self, PyObject* args) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);
    if (!wrapper->cpp_object) {
        PyErr_SetString(PyExc_RuntimeError, "Invalid C++ object");
        return nullptr;
    }

    constexpr std::size_t param_count = get_method_param_count<T, Index>();

    if (PyTuple_Size(args) != static_cast<Py_ssize_t>(param_count)) {
        PyErr_SetString(PyExc_TypeError, "Incorrect number of arguments");
        return nullptr;
    }

    try {
        // Dispatch to variadic helper with compile-time index sequence
        return call_method_impl<T, Index>(wrapper, args, std::make_index_sequence<param_count>{});
    } catch (const std::exception& e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        return nullptr;
    } catch (...) {
        PyErr_SetString(PyExc_RuntimeError, "Unknown C++ exception");
        return nullptr;
    }
}

// ============================================================================
// Method Overloading Support via Name Mangling
// ============================================================================

// Generate a mangled name for a method based on its parameter types
// E.g., "foo(int, double)" -> "foo_int_double"
template<typename T, std::size_t FuncIndex>
consteval auto generate_mangled_method_name() {
    constexpr auto func = get_member_function<T, FuncIndex>();
    constexpr auto base_name = std::meta::identifier_of(func);
    constexpr std::size_t param_count = get_method_param_count<T, FuncIndex>();

    // For zero parameters, no mangling needed
    if constexpr (param_count == 0) {
        return base_name;
    } else {
        // Build mangled name with parameter types
        // Note: We return the base name here as the mangling happens at runtime
        // in generate_methods() below
        return base_name;
    }
}

// Helper to generate type suffix for mangling
template<typename T, std::size_t FuncIndex>
std::string get_method_type_suffix() {
    constexpr std::size_t param_count = get_method_param_count<T, FuncIndex>();

    if (param_count == 0) {
        return "";
    }

    std::ostringstream oss;
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            constexpr auto param_type = get_method_param_type<T, FuncIndex, Is>();
            auto type_str = std::meta::display_string_of(param_type);

            // Simplify type name: strip namespaces, cv-qualifiers, references
            std::string simplified;
            bool in_template = false;
            for (char c : std::string_view(type_str.data(), type_str.size())) {
                if (c == '<') in_template = true;
                if (c == '>') in_template = false;
                if (!in_template && (c == ':' || c == ' ' || c == '&' || c == '*')) continue;
                if (c == ',') c = '_';
                simplified += c;
            }

            oss << "_" << simplified;
        }(), ...);
    }(std::make_index_sequence<param_count>{});

    return oss.str();
}

// Template Recursion for Compile-Time Overload Detection
//
// Why template recursion? In consteval contexts, we can't use runtime loops
// because loop indices aren't compile-time constants. Instead, we use template
// recursion where the template parameter itself IS the loop counter.
//
// This pattern is common in C++ metaprogramming:
// - Each "iteration" is a template instantiation
// - The compiler unrolls everything at compile-time
// - No runtime overhead - just a compile-time bool result
//
// Example: For a class with methods [foo(int), foo(double), bar(int)]:
//   is_method_overloaded<T, 0>() checks if foo(int) has overloads
//   → Recurses checking: foo(int) vs foo(double) → names match → return true
template<typename T, std::size_t Index, std::size_t CheckIndex = 0>
consteval bool is_method_overloaded() {
    constexpr std::size_t func_count = get_member_function_count<T>();

    // Base case: checked all methods, no overload found
    if constexpr (CheckIndex >= func_count) {
        return false;
    }
    // Skip comparing a method with itself
    else if constexpr (CheckIndex == Index) {
        return is_method_overloaded<T, Index, CheckIndex + 1>();  // Recursive call
    }
    // Compare method names
    else {
        constexpr auto name_index = get_member_function_name<T, Index>();
        constexpr auto name_check = get_member_function_name<T, CheckIndex>();

        // If names match, we found an overload!
        if constexpr (std::string_view(name_index) == std::string_view(name_check)) {
            return true;
        } else {
            return is_method_overloaded<T, Index, CheckIndex + 1>();  // Continue recursing
        }
    }
}

// ============================================================================
// Code Generation Helpers
// ============================================================================

// Helper template to get member info at compile-time - uses immediate evaluation
template<typename T, std::size_t Index>
consteval const char* get_member_name() {
    return std::meta::identifier_of(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())[Index]
    ).data();
}

// Generate Python getters/setters for all members of a class
template<typename T, std::size_t... Indices>
consteval auto generate_getsetters(std::index_sequence<Indices...>) {
    constexpr std::size_t count = sizeof...(Indices);

    // Create array of PyGetSetDef structures (one per member + sentinel)
    std::array<PyGetSetDef, count + 1> getsets{};

    // Populate entries using fold expression
    ((getsets[Indices] = PyGetSetDef{
        .name = get_member_name<T, Indices>(),
        .get = py_getter<T, Indices>,
        .set = py_setter<T, Indices>,
        .doc = nullptr,
        .closure = nullptr
    }), ...);

    // Sentinel entry (all nulls)
    getsets[count] = PyGetSetDef{nullptr, nullptr, nullptr, nullptr, nullptr};

    return getsets;
}

// Generate Python methods for all member functions of a class
// Handles overloading by mangling names with type suffixes
template<typename T, std::size_t... Indices>
auto generate_methods(std::index_sequence<Indices...>) {
    constexpr std::size_t count = sizeof...(Indices);

    // Pre-allocate storage for mangled names (if needed)
    static std::array<std::string, count> mangled_names;

    // Build methods array
    std::array<PyMethodDef, count + 1> methods{};

    // Populate entries - check each method individually
    ([&] {
        constexpr auto base_name = get_member_function_name<T, Indices>();
        constexpr bool is_overloaded = is_method_overloaded<T, Indices>();

        if constexpr (is_overloaded) {
            // Generate mangled name with type suffix
            mangled_names[Indices] = std::string(base_name) + get_method_type_suffix<T, Indices>();
            methods[Indices] = PyMethodDef{
                .ml_name = mangled_names[Indices].c_str(),
                .ml_meth = reinterpret_cast<PyCFunction>(py_method<T, Indices>),
                .ml_flags = METH_VARARGS,
                .ml_doc = nullptr
            };
        } else {
            // No overload, use base name
            methods[Indices] = PyMethodDef{
                .ml_name = base_name,
                .ml_meth = reinterpret_cast<PyCFunction>(py_method<T, Indices>),
                .ml_flags = METH_VARARGS,
                .ml_doc = nullptr
            };
        }
    }(), ...);

    // Sentinel entry
    methods[count] = PyMethodDef{nullptr, nullptr, 0, nullptr};

    return methods;
}

// Helper to auto-generate non-template conversion overloads using reflection
// This ensures proper overload resolution for types used in smart pointers
template<typename T>
struct ConversionOverloadGenerator {
    // Generate to_python overload at compile time
    static PyObject* to_python_impl(const T& obj) {
        PyObject* dict = PyDict_New();
        if (!dict) return nullptr;

        constexpr std::size_t member_count = get_nested_member_count<T>();
        bool success = true;

        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ([&] {
                if (!success) return;

                constexpr auto member = get_nested_member<T, Is>();
                constexpr auto name = get_nested_member_name<T, Is>();

                const auto& value = obj.[:member:];
                PyObject* py_value = to_python(value);

                if (!py_value || PyDict_SetItemString(dict, name, py_value) < 0) {
                    success = false;
                    Py_XDECREF(py_value);
                } else {
                    Py_DECREF(py_value);
                }
            }(), ...);
        }(std::make_index_sequence<member_count>{});

        if (!success) {
            Py_DECREF(dict);
            return nullptr;
        }

        return dict;
    }

    // Generate from_python overload at compile time
    static bool from_python_impl(PyObject* obj, T& out) {
        if (!PyDict_Check(obj)) return false;

        constexpr std::size_t member_count = get_nested_member_count<T>();
        bool success = true;

        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ([&] {
                if (!success) return;

                constexpr auto member = get_nested_member<T, Is>();
                constexpr auto name = get_nested_member_name<T, Is>();
                using MemberType = NestedMemberType<T, Is>;

                PyObject* py_value = PyDict_GetItemString(obj, name);
                if (!py_value) {
                    success = false;
                    return;
                }

                MemberType cpp_value;
                if (!from_python(py_value, cpp_value)) {
                    success = false;
                    return;
                }

                out.[:member:] = std::move(cpp_value);
            }(), ...);
        }(std::make_index_sequence<member_count>{});

        return success;
    }
};

// Auto-generated conversion overloads using SFINAE (not requires clause)
// SFINAE ensures clean template symbols without constraint mangling
template<typename T>
std::enable_if_t<
    Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T> && !SmartPointer<T>,
    PyObject*
>
to_python(const T& obj) {
    return ConversionOverloadGenerator<T>::to_python_impl(obj);
}

template<typename T>
std::enable_if_t<
    Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T> && !SmartPointer<T>,
    bool
>
from_python(PyObject* obj, T& out) {
    return ConversionOverloadGenerator<T>::from_python_impl(obj, out);
}

// Main binding function - generates Python type object for a C++ class
// Optionally accepts a file content hash for implementation change detection
template<Bindable T>
PyTypeObject* bind_class(PyObject* module, const char* name, const char* file_hash = nullptr) {
    // Generate type signature for hash-based change detection
    auto signature = generate_type_signature<T>(file_hash);

    // Register class in the global registry
    Registry::instance().register_class(name, signature);

    // Get member count for generating getters/setters at compile-time
    constexpr std::size_t member_count = []() consteval {
        return std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()).size();
    }();

    // Generate getters/setters using reflection
    static auto getsetters = generate_getsetters<T>(std::make_index_sequence<member_count>{});

    // Generate methods using reflection
    constexpr std::size_t method_count = get_member_function_count<T>();
    static auto methods = generate_methods<T>(std::make_index_sequence<method_count>{});

    // Check if class has parameterized constructors
    constexpr std::size_t ctor_count = get_constructor_count<T>();

    // Define the Python type structure
    static PyTypeObject type_object = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        .tp_name = name,
        .tp_basicsize = sizeof(PyWrapper<T>),
        .tp_itemsize = 0,
        .tp_dealloc = py_dealloc<T>,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Auto-generated binding via mirror_bridge reflection",
        .tp_methods = methods.data(),
        .tp_getset = getsetters.data(),
        .tp_init = (ctor_count > 0) ? (initproc)py_init<T> : nullptr,
        .tp_new = (ctor_count > 0) ? py_new_with_init<T> : py_new<T>,
    };

    // Initialize and register the type with Python
    if (PyType_Ready(&type_object) < 0) {
        return nullptr;
    }

    Py_INCREF(&type_object);
    if (PyModule_AddObject(module, name, reinterpret_cast<PyObject*>(&type_object)) < 0) {
        Py_DECREF(&type_object);
        return nullptr;
    }

    // Update registry with the Python type object
    Registry::instance().set_py_type(name, &type_object);

    return &type_object;
}

} // namespace mirror_bridge

// ============================================================================
// Convenience Macros
// ============================================================================

// Register a class for Python binding
#define MIRROR_BRIDGE_REGISTER(ClassName) \
    MIRROR_BRIDGE_REGISTER_WITH_HASH(ClassName, nullptr)

// Register a class with an explicit file content hash
#define MIRROR_BRIDGE_REGISTER_WITH_HASH(ClassName, FileHash) \
    namespace { \
        struct ClassName##_Registrar { \
            ClassName##_Registrar() { \
                auto sig = mirror_bridge::generate_type_signature<ClassName>(FileHash); \
                mirror_bridge::Registry::instance().register_class(#ClassName, sig); \
            } \
        }; \
        static ClassName##_Registrar ClassName##_registrar_instance; \
    }

// Create a Python module with registered classes
#define MIRROR_BRIDGE_MODULE(module_name, ...) \
    static PyModuleDef module_name##_def = { \
        PyModuleDef_HEAD_INIT, \
        #module_name, \
        "Auto-generated module via mirror_bridge", \
        -1, \
        nullptr \
    }; \
    PyMODINIT_FUNC PyInit_##module_name() { \
        PyObject* m = PyModule_Create(&module_name##_def); \
        if (!m) return nullptr; \
        __VA_ARGS__ \
        return m; \
    }

#pragma once

// Mirror Bridge - Single-header C++ to Python binding generator using C++26 reflection
// Leverages P2996 reflection proposal for automatic, zero-overhead foreign function interfaces
//
// Requirements:
//   - Bloomberg clang-p2996 compiler with libcxx
//   - Compile with: -std=c++2c -freflection -freflection-latest
//   - Requires: #include <meta> (provided by libcxx in clang-p2996)

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

namespace mirror_bridge {

// ============================================================================
// Type Traits and Concepts (inspired by simdjson's approach)
// ============================================================================

// Forward declarations for nested type support
template<typename T>
PyObject* to_python(const T& value);

template<typename T>
bool from_python(PyObject* obj, T& out);

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

// Concept to identify C++ standard containers (vector, array, list, etc.)
// Following simdjson: use ranges to detect containers, but exclude strings
template<typename T>
concept Container =
    std::ranges::input_range<std::remove_cvref_t<T>> &&
    !StringLike<T> &&
    requires(std::remove_cvref_t<T> c) {
        typename std::remove_cvref_t<T>::value_type;
    };

// Concept for types that can be bound to Python (classes with reflectable members)
template<typename T>
concept Bindable = std::is_class_v<std::remove_cvref_t<T>> && requires {
    { std::meta::nonstatic_data_members_of(^^std::remove_cvref_t<T>) };
};

// Concept for nested bindable classes (used as member types in other classes)
template<typename T>
concept NestedBindable = Bindable<T> && !StringLike<T> && !Container<T> && !Arithmetic<T>;

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

template<>
inline bool from_python<std::string>(PyObject* obj, std::string& out) {
    if (!PyUnicode_Check(obj)) return false;
    Py_ssize_t size;
    const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
    if (!data) return false;
    out = std::string(data, size);
    return true;
}

template<>
inline bool from_python<std::string_view>(PyObject* obj, std::string_view& out) {
    // Note: string_view requires the underlying string to remain valid
    // For Python->C++ conversion, we typically need to store strings, not views
    // This is mainly useful for temporary conversions
    if (!PyUnicode_Check(obj)) return false;
    Py_ssize_t size;
    const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
    if (!data) return false;
    out = std::string_view(data, size);
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

    // Clear the container and reserve space if possible
    container.clear();
    if constexpr (requires { container.reserve(size); }) {
        container.reserve(size);
    }

    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* py_item = PyList_GetItem(obj, i);  // Borrowed reference
        ValueType cpp_item;
        if (!from_python(py_item, cpp_item)) {
            return false;
        }

        // Use push_back if available, otherwise use insert
        if constexpr (requires { container.push_back(cpp_item); }) {
            container.push_back(std::move(cpp_item));
        } else if constexpr (requires { container.insert(cpp_item); }) {
            container.insert(std::move(cpp_item));
        } else {
            // Fallback for other container types
            static_assert(requires { container.push_back(cpp_item); },
                         "Container must support push_back or insert");
        }
    }
    return true;
}

// Convert nested bindable classes to Python dictionaries
// Uses reflection to iterate over all members and create a dict
template<NestedBindable T>
PyObject* to_python(const T& obj) {
    PyObject* dict = PyDict_New();
    if (!dict) return nullptr;

    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    bool success = true;

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            if (!success) return;

            constexpr auto member = members[Is];
            constexpr auto name = std::meta::identifier_of(member);

            // Get the member value and convert it to Python
            const auto& value = obj.[:member:];
            PyObject* py_value = to_python(value);

            if (!py_value || PyDict_SetItemString(dict, name, py_value) < 0) {
                success = false;
                Py_XDECREF(py_value);
            } else {
                Py_DECREF(py_value);  // Dict keeps its own reference
            }
        }(), ...);
    }(std::make_index_sequence<members.size()>{});

    if (!success) {
        Py_DECREF(dict);
        return nullptr;
    }

    return dict;
}

// Convert Python dictionaries to nested bindable classes
// Uses reflection to match dict keys to member names
template<NestedBindable T>
bool from_python(PyObject* obj, T& out) {
    if (!PyDict_Check(obj)) return false;

    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    bool success = true;

    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ([&] {
            if (!success) return;

            constexpr auto member = members[Is];
            constexpr auto name = std::meta::identifier_of(member);
            using MemberType = typename std::meta::type_of(member);

            // Get the value from the dict
            PyObject* py_value = PyDict_GetItemString(obj, name);  // Borrowed reference
            if (!py_value) {
                // Member not found in dict - could use default value or fail
                success = false;
                return;
            }

            // Convert Python value to C++ type
            MemberType cpp_value;
            if (!from_python(py_value, cpp_value)) {
                success = false;
                return;
            }

            out.[:member:] = std::move(cpp_value);
        }(), ...);
    }(std::make_index_sequence<members.size()>{});

    return success;
}

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

// Generate a type signature from reflection metadata
// This signature is used for hash-based change detection and includes:
//   1. Data member names and types
//   2. Member function signatures (name, return type, parameters, cv-qualifiers)
//   3. Optional source file content hash
//
// Why file content hashing is needed:
//   P2996 reflection can only inspect declarations and signatures, NOT implementations.
//   If a method body changes but its signature stays the same, reflection cannot detect it.
//   To catch implementation changes, the build system should compute a hash of the source
//   file and pass it via -DFILE_CONTENT_HASH_<ClassName>="<hash>" to the compiler.
//
// Example usage in build script:
//   FILE_HASH=$(sha256sum vector3.hpp | cut -d' ' -f1)
//   clang++ -DFILE_CONTENT_HASH_Vector3=\"$FILE_HASH\" ...
//
template<typename T>
consteval std::string generate_type_signature(const char* file_hash = nullptr) {
    std::string sig = std::meta::identifier_of(^^T);
    sig += "{";

    // Data members section
    constexpr auto data_members = std::meta::nonstatic_data_members_of(^^T);
    for (std::size_t i = 0; i < data_members.size(); ++i) {
        constexpr auto member = data_members[i];
        constexpr auto member_name = std::meta::identifier_of(member);
        constexpr auto member_type = std::meta::type_of(member);

        if (i > 0) sig += ",";
        sig += std::meta::identifier_of(member_type);
        sig += " ";
        sig += member_name;
    }

    // Member functions section - detects when method signatures change
    // (but NOT when only the implementation changes - see note above)
    sig += "|methods:";
    constexpr auto all_members = std::meta::members_of(^^T);
    bool first_method = true;
    for (std::size_t i = 0; i < all_members.size(); ++i) {
        constexpr auto member = all_members[i];
        // Filter for member functions (excludes data members, constructors, destructors)
        if constexpr (std::meta::is_function(member) &&
                     !std::meta::is_constructor(member) &&
                     !std::meta::is_destructor(member)) {
            if (!first_method) sig += ",";
            first_method = false;

            // Include function signature: return_type name(params) cv-qualifiers
            constexpr auto func_type = std::meta::type_of(member);
            sig += std::meta::identifier_of(func_type);
            sig += " ";
            sig += std::meta::identifier_of(member);
        }
    }

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

// Constructor for Python wrapper objects (default constructor)
template<typename T>
PyObject* py_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    auto* self = reinterpret_cast<PyWrapper<T>*>(type->tp_alloc(type, 0));
    if (self) {
        self->cpp_object = new T();
        self->owns = true;
    }
    return reinterpret_cast<PyObject*>(self);
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
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr auto member = members[Index];

    // Access the member value through reflection
    auto& value = wrapper->cpp_object->[:member:];
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
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr auto member = members[Index];
    using MemberType = typename std::meta::type_of(member);

    // Convert Python value to C++ type and set the member
    MemberType cpp_value;
    if (!from_python(value, cpp_value)) {
        PyErr_SetString(PyExc_TypeError, "Type conversion failed");
        return -1;
    }

    wrapper->cpp_object->[:member:] = cpp_value;
    return 0;
}

// Generate Python getters/setters for all members of a class
template<typename T, std::size_t... Indices>
consteval auto generate_getsetters(std::index_sequence<Indices...>) {
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr std::size_t count = members.size();

    // Create array of PyGetSetDef structures (one per member + sentinel)
    std::array<PyGetSetDef, count + 1> getsets{};

    // Lambda to populate a single PyGetSetDef entry
    auto populate_entry = [&]<std::size_t I>() {
        constexpr auto member = members[I];
        constexpr auto name = std::meta::identifier_of(member);

        getsets[I] = PyGetSetDef{
            .name = name,
            .get = py_getter<T, I>,
            .set = py_setter<T, I>,
            .doc = nullptr,
            .closure = nullptr
        };
    };

    // Populate all entries
    (populate_entry.template operator()<Indices>(), ...);

    // Sentinel entry (all nulls)
    getsets[count] = PyGetSetDef{nullptr, nullptr, nullptr, nullptr, nullptr};

    return getsets;
}

// Main binding function - generates Python type object for a C++ class
// Optionally accepts a file content hash for implementation change detection
template<Bindable T>
PyTypeObject* bind_class(PyObject* module, const char* name, const char* file_hash = nullptr) {
    // Generate type signature for hash-based change detection
    constexpr auto signature = generate_type_signature<T>(file_hash);

    // Register class in the global registry
    Registry::instance().register_class(name, signature);

    // Get member count for generating getters/setters
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr std::size_t member_count = members.size();

    // Generate getters/setters using reflection
    static auto getsetters = generate_getsetters<T>(std::make_index_sequence<member_count>{});

    // Define the Python type structure
    static PyTypeObject type_object = {
        PyVarObject_HEAD_INIT(nullptr, 0)
        .tp_name = name,
        .tp_basicsize = sizeof(PyWrapper<T>),
        .tp_itemsize = 0,
        .tp_dealloc = py_dealloc<T>,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Auto-generated binding via mirror_bridge reflection",
        .tp_getset = getsetters.data(),
        .tp_new = py_new<T>,
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
// Optionally, the build system can define FILE_CONTENT_HASH_<ClassName> to include
// implementation change detection. If not defined, only signature changes are detected.
//
// Example: clang++ -DFILE_CONTENT_HASH_Vector3=\"abc123...\" vector3_binding.cpp
#define MIRROR_BRIDGE_REGISTER(ClassName) \
    MIRROR_BRIDGE_REGISTER_WITH_HASH(ClassName, nullptr)

// Register a class with an explicit file content hash
#define MIRROR_BRIDGE_REGISTER_WITH_HASH(ClassName, FileHash) \
    namespace { \
        struct ClassName##_Registrar { \
            ClassName##_Registrar() { \
                constexpr auto sig = mirror_bridge::generate_type_signature<ClassName>(FileHash); \
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

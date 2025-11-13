#pragma once

// Mirror Bridge - Single-header C++ to Python binding generator using C++26 reflection
// Leverages P2996 reflection proposal for automatic, zero-overhead foreign function interfaces
//
// Compile with: -std=c++2c -freflection -freflection-latest
// Reflection is compiler-built-in (no header needed) when using Bloomberg's clang-p2996

#include <Python.h>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <concepts>
#include <functional>
#include <memory>
#include <unordered_map>

namespace mirror_bridge {

// ============================================================================
// Type Traits and Concepts
// ============================================================================

// Concept to identify arithmetic types (int, float, double, etc.)
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// Concept to identify string-like types
template<typename T>
concept StringLike = std::is_convertible_v<T, std::string_view> ||
                      std::is_same_v<T, std::string> ||
                      std::is_same_v<T, const char*>;

// Concept for types that can be bound to Python
template<typename T>
concept Bindable = std::is_class_v<T> && requires {
    { std::meta::nonstatic_data_members_of(^^T) };
};

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
    if constexpr (std::is_same_v<T, std::string>) {
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
// This signature is used for hash-based change detection
template<typename T>
consteval std::string generate_type_signature() {
    std::string sig = std::meta::identifier_of(^^T);
    sig += "{";

    // Iterate over all non-static data members using reflection
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    for (std::size_t i = 0; i < members.size(); ++i) {
        constexpr auto member = members[i];
        constexpr auto member_name = std::meta::identifier_of(member);
        constexpr auto member_type = std::meta::type_of(member);

        if (i > 0) sig += ",";
        sig += std::meta::identifier_of(member_type);
        sig += " ";
        sig += member_name;
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
template<Bindable T>
PyTypeObject* bind_class(PyObject* module, const char* name) {
    // Generate type signature for hash-based change detection
    constexpr auto signature = generate_type_signature<T>();

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
#define MIRROR_BRIDGE_REGISTER(ClassName) \
    namespace { \
        struct ClassName##_Registrar { \
            ClassName##_Registrar() { \
                constexpr auto sig = mirror_bridge::generate_type_signature<ClassName>(); \
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

#pragma once

// ═══════════════════════════════════════════════════════════════════════════
// Mirror Bridge - Automatic C++ Bindings to Multiple Languages via C++26 Reflection
// ═══════════════════════════════════════════════════════════════════════════
//
// Multi-language support: Python, JavaScript (Node.js N-API), Lua
//
// This header provides backward compatibility by including Python bindings by default.
// For other languages, include the specific binding header:
//   - python/mirror_bridge_python.hpp
//   - javascript/mirror_bridge_javascript.hpp  
//   - lua/mirror_bridge_lua.hpp
//
// ═══════════════════════════════════════════════════════════════════════════

// Default: Include Python bindings for backward compatibility
#include "python/mirror_bridge_python.hpp"

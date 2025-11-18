#pragma once

// ═══════════════════════════════════════════════════════════════════════════
// Mirror Bridge - Precompiled Header (OPTIONAL)
// ═══════════════════════════════════════════════════════════════════════════
//
// This file is designed to be precompiled for faster compilation times.
// It includes the heavy dependencies that rarely change.
//
// USAGE (OPTIONAL - NOT REQUIRED):
//
// Step 1: Precompile this header (do once):
//   clang-21 -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
//     -x c++-header mirror_bridge_pch.hpp -o mirror_bridge_pch.hpp.gch
//
// Step 2: Use the precompiled header in your bindings:
//   clang-21 -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
//     -include mirror_bridge_pch.hpp your_bindings.cpp
//
// CMake users: See mirror_bridge_pch.cmake for automatic PCH support
//
// COMPATIBILITY:
//   - Works with C++26 reflection (tested with Bloomberg clang-p2996)
//   - The PCH must be rebuilt if you change compiler flags or upgrade compiler
//   - Each build configuration (Debug/Release) needs its own PCH
//
// PERFORMANCE IMPACT:
//   - Expected improvement: 30-50% faster compilation
//   - First compilation (building PCH): slightly slower
//   - Subsequent compilations: significantly faster
//
// NOTE: You can still use mirror_bridge.hpp directly without PCH.
// This file is purely optional for users who want maximum compile speed.
//
// ═══════════════════════════════════════════════════════════════════════════

// Include the main header which contains everything
#include "mirror_bridge.hpp"

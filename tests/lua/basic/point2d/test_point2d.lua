local point2d = require("point2d")

print("=== Point2D Lua Binding Test ===\n")

-- Test 1: Creation
print("Test 1: Creating Point2D...")
local p = point2d.Point2D()
assert(p.x == 0.0 and p.y == 0.0)
print(string.format("  ✓ Created: (%.1f, %.1f)", p.x, p.y))

-- Test 2: Setting values
print("\nTest 2: Setting values...")
p.x = 3.0
p.y = 4.0
assert(p.x == 3.0 and p.y == 4.0)
print(string.format("  ✓ Set to: (%.1f, %.1f)", p.x, p.y))

-- Test 3: Multiple instances
print("\nTest 3: Multiple instances...")
local p1 = point2d.Point2D()
local p2 = point2d.Point2D()
p1.x = 1.0
p2.x = 2.0
assert(p1.x == 1.0 and p2.x == 2.0)
print(string.format("  ✓ p1.x=%.1f, p2.x=%.1f", p1.x, p2.x))

print("\n" .. string.rep("=", 40))
print("✓ All tests passed!")
print(string.rep("=", 40))

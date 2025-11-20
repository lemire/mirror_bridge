-- Test auto-discovered Lua bindings
local geometry = require("geometry_lua")

print("=== Lua Auto-Discovery Test ===\n")

-- Test Point class
print("Test 1: Point class...")
local p = geometry.Point()
p.x = 3.0
p.y = 4.0
assert(p.x == 3.0 and p.y == 4.0, "Point properties failed")
local dist = p:distance_from_origin()
assert(math.abs(dist - 5.0) < 0.001, "distance_from_origin failed")
print("  ✓ Point: x=" .. p.x .. ", y=" .. p.y .. ", distance=" .. dist)

-- Test Circle class
print("\nTest 2: Circle class...")
local c = geometry.Circle()
c.radius = 10.0
assert(c.radius == 10.0, "Circle radius property failed")
local area = c:area()
assert(math.abs(area - 314.159) < 0.01, "area failed")
print("  ✓ Circle: radius=" .. c.radius .. ", area=" .. area)

-- Test Circle with Point (nested objects work as read-only tables)
print("\nTest 3: Circle center (nested Point)...")
print("  ✓ Circle has nested center point: x=" .. c.center.x .. ", y=" .. c.center.y)
print("  (Note: Nested objects are converted to Lua tables)")

print("\n" .. string.rep("=", 40))
print("✓ All auto-discovery tests passed!")
print(string.rep("=", 40))

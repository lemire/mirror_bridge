-- Test comprehensive features in Lua
local features = require("features_lua")

print("=== Lua Comprehensive Features Test ===\n")

-- Test 1: Container (vector) support
print("Test 1: Vector containers...")
local v = features.VectorTest()
v.numbers = {1, 2, 3, 4, 5}
v.names = {"Alice", "Bob", "Charlie"}
local sum = v:get_sum()
local count = v:count_names()
assert(sum == 15, "Vector sum failed")
assert(count == 3, "Vector count failed")
print("  ✓ Vector<int>: sum = " .. sum)
print("  ✓ Vector<string>: count = " .. count)

-- Test 2: Nested objects
print("\nTest 2: Nested objects...")
local p = features.Person()
p.name = "Alice"
p.age = 30
p.address = {street = "123 Main St", city = "Springfield", zip = 12345}
print("  ✓ Person: " .. p.name .. ", age " .. p.age)
print("  ✓ Address: " .. p.address.street .. ", " .. p.address.city)
local full_addr = p:get_full_address()
print("  ✓ Full address: " .. full_addr)

-- Test 3: Enum support
print("\nTest 3: Enum support...")
local shape = features.ColoredShape()
shape.color = 1  -- Color::Green
shape.size = 5.0
local color_val = shape:get_color_value()
assert(color_val == 1, "Enum value failed")
print("  ✓ ColoredShape: color = " .. color_val .. ", size = " .. shape.size)

-- Test 4: Resource (without smart pointer for now)
print("\nTest 4: Resource...")
local r = features.Resource()
r.name = "TestResource"
r.value = 42
local info = r:get_info()
assert(info == "TestResource:42", "Resource info failed")
print("  ✓ Resource info: " .. info)

print("\n" .. string.rep("=", 40))
print("✓ All 4 comprehensive feature tests passed!")
print(string.rep("=", 40))

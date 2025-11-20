-- Test single-header Lua binding
local robot = require("robot_lua")

print("=== Lua Single-Header Test ===\n")

-- Test 1: Create robot
print("Test 1: Creating robot...")
local r = robot.Robot()
r.name = "RoboBot"
r.battery = 100.0
assert(r.name == "RoboBot")
assert(r.battery == 100.0)
print(string.format("  ✓ Robot created: %s, battery: %.1f%%", r.name, r.battery))

-- Test 2: Move forward
print("\nTest 2: Moving forward...")
r:move_forward(50.0)
assert(r.battery == 95.0)
print(string.format("  ✓ Moved 50m, battery: %.1f%%", r.battery))

-- Test 3: Get status
print("\nTest 3: Get status...")
local status = r:get_status()
assert(status:find("RoboBot"))
assert(status:find("95"))
print("  ✓ Status: " .. status)

-- Test 4: Commands
print("\nTest 4: Add commands...")
r.commands = {"forward", "left", "forward"}
local count = r:command_count()
assert(count == 3)
print(string.format("  ✓ Added %d commands", count))

-- Test 5: Reset
print("\nTest 5: Reset...")
r:reset()
assert(r.battery == 100.0)
assert(r:command_count() == 0)
print(string.format("  ✓ Reset: battery=%.1f%%, commands=%d", r.battery, r:command_count()))

print("\n" .. string.rep("=", 40))
print("✓ All Lua single-header tests passed!")
print(string.rep("=", 40))

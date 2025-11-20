local calculator = require("calculator")

print("=== Calculator Lua Comprehensive Test ===\n")

-- Test 1: Constructor (default)
print("Test 1: Default constructor...")
local calc = calculator.Calculator()
assert(calc.value == 0.0, "Default value should be 0.0")
print("  ✓ Default constructor works, value = " .. calc.value)

-- Test 2: Property access
print("\nTest 2: Property setter...")
calc.value = 10.0
assert(calc.value == 10.0, "Value should be 10.0 after setting")
print("  ✓ Property setter works, value = " .. calc.value)

-- Test 3: Methods with return values
print("\nTest 3: add() method...")
local result = calc:add(5.0)
assert(result == 15.0, "add(5.0) should return 15.0")
assert(calc.value == 15.0, "value should be 15.0")
print("  ✓ add(5.0) returned " .. result .. ", value = " .. calc.value)

print("\nTest 4: subtract() method...")
result = calc:subtract(3.0)
assert(result == 12.0, "subtract(3.0) should return 12.0")
assert(calc.value == 12.0, "value should be 12.0")
print("  ✓ subtract(3.0) returned " .. result .. ", value = " .. calc.value)

print("\nTest 5: multiply() method...")
result = calc:multiply(2.0)
assert(result == 24.0, "multiply(2.0) should return 24.0")
assert(calc.value == 24.0, "value should be 24.0")
print("  ✓ multiply(2.0) returned " .. result .. ", value = " .. calc.value)

print("\nTest 6: divide() method...")
result = calc:divide(4.0)
assert(result == 6.0, "divide(4.0) should return 6.0")
assert(calc.value == 6.0, "value should be 6.0")
print("  ✓ divide(4.0) returned " .. result .. ", value = " .. calc.value)

-- Test 7: Const method
print("\nTest 7: get_value() const method...")
result = calc:get_value()
assert(result == 6.0, "get_value() should return 6.0")
print("  ✓ get_value() returned " .. result)

-- Test 8: Method with multiple parameters
print("\nTest 8: compute(x, y) with multiple parameters...")
result = calc:compute(3.0, 7.0)
assert(result == 20.0, "compute(3.0, 7.0) should return (3+7)*2 = 20.0")
assert(calc.value == 20.0, "value should be 20.0")
print("  ✓ compute(3.0, 7.0) returned " .. result .. ", value = " .. calc.value)

-- Test 9: Void method
print("\nTest 9: reset() void method...")
calc:reset()
assert(calc.value == 0.0, "value should be 0.0 after reset()")
print("  ✓ reset() works, value = " .. calc.value)

-- Test 10: Method returning string
print("\nTest 10: to_string() method returning string...")
calc.value = 42.5
local str = calc:to_string()
assert(type(str) == "string", "to_string() should return a string")
print("  ✓ to_string() returned: '" .. str .. "'")

-- Test 11: Multiple instances
print("\nTest 11: Multiple independent instances...")
local calc1 = calculator.Calculator()
local calc2 = calculator.Calculator()
calc1.value = 100.0
calc2.value = 200.0
assert(calc1.value == 100.0, "calc1.value should be 100.0")
assert(calc2.value == 200.0, "calc2.value should be 200.0")
print("  ✓ calc1.value = " .. calc1.value .. ", calc2.value = " .. calc2.value)

-- Test 12: Method chaining effect
print("\nTest 12: Method call sequence...")
calc1:reset()
calc1:add(10.0)
calc1:multiply(3.0)
calc1:subtract(5.0)
assert(calc1.value == 25.0, "10*3-5 should be 25.0")
print("  ✓ Chained operations: reset()->add(10)->multiply(3)->subtract(5) = " .. calc1.value)

print("\n" .. string.rep("=", 40))
print("✓ All 12 comprehensive tests passed!")
print(string.rep("=", 40))

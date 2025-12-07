// Calculator V8 Comprehensive Binding Test
// This JavaScript is executed by the embedded V8 engine

print('=== Calculator V8 Comprehensive Test ===\n');

var allPassed = true;

function assert(condition, message) {
    if (!condition) {
        print('  FAIL: ' + message);
        allPassed = false;
        throw new Error(message);
    }
}

function assertEqual(actual, expected, message) {
    if (actual !== expected) {
        print('  FAIL: ' + message + ' (expected ' + expected + ', got ' + actual + ')');
        allPassed = false;
        throw new Error(message);
    }
}

// Test 1: Constructor (default)
print('Test 1: Default constructor...');
var calc = new Calculator();
assertEqual(calc.value, 0.0, 'Default value should be 0.0');
print('  PASS: Default constructor works, value = ' + calc.value);

// Test 2: Property access
print('\nTest 2: Property setter...');
calc.value = 10.0;
assertEqual(calc.value, 10.0, 'Value should be 10.0 after setting');
print('  PASS: Property setter works, value = ' + calc.value);

// Test 3: Methods with return values
print('\nTest 3: add() method...');
var result = calc.add(5.0);
assertEqual(result, 15.0, 'add(5.0) should return 15.0');
assertEqual(calc.value, 15.0, 'value should be 15.0');
print('  PASS: add(5.0) returned ' + result + ', value = ' + calc.value);

print('\nTest 4: subtract() method...');
result = calc.subtract(3.0);
assertEqual(result, 12.0, 'subtract(3.0) should return 12.0');
assertEqual(calc.value, 12.0, 'value should be 12.0');
print('  PASS: subtract(3.0) returned ' + result + ', value = ' + calc.value);

print('\nTest 5: multiply() method...');
result = calc.multiply(2.0);
assertEqual(result, 24.0, 'multiply(2.0) should return 24.0');
assertEqual(calc.value, 24.0, 'value should be 24.0');
print('  PASS: multiply(2.0) returned ' + result + ', value = ' + calc.value);

print('\nTest 6: divide() method...');
result = calc.divide(4.0);
assertEqual(result, 6.0, 'divide(4.0) should return 6.0');
assertEqual(calc.value, 6.0, 'value should be 6.0');
print('  PASS: divide(4.0) returned ' + result + ', value = ' + calc.value);

// Test 7: Const method
print('\nTest 7: get_value() const method...');
result = calc.get_value();
assertEqual(result, 6.0, 'get_value() should return 6.0');
print('  PASS: get_value() returned ' + result);

// Test 8: Method with multiple parameters
print('\nTest 8: compute(x, y) with multiple parameters...');
result = calc.compute(3.0, 7.0);
assertEqual(result, 20.0, 'compute(3.0, 7.0) should return (3+7)*2 = 20.0');
assertEqual(calc.value, 20.0, 'value should be 20.0');
print('  PASS: compute(3.0, 7.0) returned ' + result + ', value = ' + calc.value);

// Test 9: Void method
print('\nTest 9: reset() void method...');
calc.reset();
assertEqual(calc.value, 0.0, 'value should be 0.0 after reset()');
print('  PASS: reset() works, value = ' + calc.value);

// Test 10: Method returning string
print('\nTest 10: to_string() method returning string...');
calc.value = 42.5;
var str = calc.to_string();
assert(typeof str === 'string', 'to_string() should return a string');
print('  PASS: to_string() returned: \'' + str + '\'');

// Test 11: Multiple instances
print('\nTest 11: Multiple independent instances...');
var calc1 = new Calculator();
var calc2 = new Calculator();
calc1.value = 100.0;
calc2.value = 200.0;
assertEqual(calc1.value, 100.0, 'calc1.value should be 100.0');
assertEqual(calc2.value, 200.0, 'calc2.value should be 200.0');
print('  PASS: calc1.value = ' + calc1.value + ', calc2.value = ' + calc2.value);

// Test 12: Method call sequence
print('\nTest 12: Method call sequence...');
calc1.reset();
calc1.add(10.0);
calc1.multiply(3.0);
calc1.subtract(5.0);
assertEqual(calc1.value, 25.0, '10*3-5 should be 25.0');
print('  PASS: Chained operations: reset()->add(10)->multiply(3)->subtract(5) = ' + calc1.value);

print('\n========================================');
if (allPassed) {
    print('All 12 comprehensive tests passed!');
} else {
    print('SOME TESTS FAILED!');
}
print('========================================');

// Return success/failure to C++ harness
allPassed;

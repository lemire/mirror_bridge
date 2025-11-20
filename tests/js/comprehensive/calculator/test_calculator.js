const addon = require('../../../../build/calculator');
const assert = require('assert');

console.log('=== Calculator JavaScript Comprehensive Test ===\n');

// Test 1: Constructor (default)
console.log('Test 1: Default constructor...');
const calc = new addon.Calculator();
assert.strictEqual(calc.value, 0.0, 'Default value should be 0.0');
console.log(`  ✓ Default constructor works, value = ${calc.value}`);

// Test 2: Property access
console.log('\nTest 2: Property setter...');
calc.value = 10.0;
assert.strictEqual(calc.value, 10.0, 'Value should be 10.0 after setting');
console.log(`  ✓ Property setter works, value = ${calc.value}`);

// Test 3: Methods with return values
console.log('\nTest 3: add() method...');
let result = calc.add(5.0);
assert.strictEqual(result, 15.0, 'add(5.0) should return 15.0');
assert.strictEqual(calc.value, 15.0, 'value should be 15.0');
console.log(`  ✓ add(5.0) returned ${result}, value = ${calc.value}`);

console.log('\nTest 4: subtract() method...');
result = calc.subtract(3.0);
assert.strictEqual(result, 12.0, 'subtract(3.0) should return 12.0');
assert.strictEqual(calc.value, 12.0, 'value should be 12.0');
console.log(`  ✓ subtract(3.0) returned ${result}, value = ${calc.value}`);

console.log('\nTest 5: multiply() method...');
result = calc.multiply(2.0);
assert.strictEqual(result, 24.0, 'multiply(2.0) should return 24.0');
assert.strictEqual(calc.value, 24.0, 'value should be 24.0');
console.log(`  ✓ multiply(2.0) returned ${result}, value = ${calc.value}`);

console.log('\nTest 6: divide() method...');
result = calc.divide(4.0);
assert.strictEqual(result, 6.0, 'divide(4.0) should return 6.0');
assert.strictEqual(calc.value, 6.0, 'value should be 6.0');
console.log(`  ✓ divide(4.0) returned ${result}, value = ${calc.value}`);

// Test 7: Const method
console.log('\nTest 7: get_value() const method...');
result = calc.get_value();
assert.strictEqual(result, 6.0, 'get_value() should return 6.0');
console.log(`  ✓ get_value() returned ${result}`);

// Test 8: Method with multiple parameters
console.log('\nTest 8: compute(x, y) with multiple parameters...');
result = calc.compute(3.0, 7.0);
assert.strictEqual(result, 20.0, 'compute(3.0, 7.0) should return (3+7)*2 = 20.0');
assert.strictEqual(calc.value, 20.0, 'value should be 20.0');
console.log(`  ✓ compute(3.0, 7.0) returned ${result}, value = ${calc.value}`);

// Test 9: Void method
console.log('\nTest 9: reset() void method...');
calc.reset();
assert.strictEqual(calc.value, 0.0, 'value should be 0.0 after reset()');
console.log(`  ✓ reset() works, value = ${calc.value}`);

// Test 10: Method returning string
console.log('\nTest 10: to_string() method returning string...');
calc.value = 42.5;
const str = calc.to_string();
assert.strictEqual(typeof str, 'string', 'to_string() should return a string');
console.log(`  ✓ to_string() returned: '${str}'`);

// Test 11: Multiple instances
console.log('\nTest 11: Multiple independent instances...');
const calc1 = new addon.Calculator();
const calc2 = new addon.Calculator();
calc1.value = 100.0;
calc2.value = 200.0;
assert.strictEqual(calc1.value, 100.0, 'calc1.value should be 100.0');
assert.strictEqual(calc2.value, 200.0, 'calc2.value should be 200.0');
console.log(`  ✓ calc1.value = ${calc1.value}, calc2.value = ${calc2.value}`);

// Test 12: Method chaining effect
console.log('\nTest 12: Method call sequence...');
calc1.reset();
calc1.add(10.0);
calc1.multiply(3.0);
calc1.subtract(5.0);
assert.strictEqual(calc1.value, 25.0, '10*3-5 should be 25.0');
console.log(`  ✓ Chained operations: reset()->add(10)->multiply(3)->subtract(5) = ${calc1.value}`);

console.log('\n' + '='.repeat(40));
console.log('✓ All 12 comprehensive tests passed!');
console.log('='.repeat(40));

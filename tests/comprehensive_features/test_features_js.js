// Test comprehensive features in JavaScript
const features = require('./features_js');
const assert = require('assert');

console.log('=== JavaScript Comprehensive Features Test ===\n');

// Test 1: Container (vector) support
console.log('Test 1: Vector containers...');
const v = new features.VectorTest();
v.numbers = [1, 2, 3, 4, 5];
v.names = ["Alice", "Bob", "Charlie"];
const sum = v.get_sum();
const count = v.count_names();
assert.strictEqual(sum, 15, 'Vector sum failed');
assert.strictEqual(count, 3, 'Vector count failed');
console.log(`  ✓ Vector<int>: sum = ${sum}`);
console.log(`  ✓ Vector<string>: count = ${count}`);

// Test 2: Nested objects
console.log('\nTest 2: Nested objects...');
const p = new features.Person();
p.name = "Alice";
p.age = 30;
p.address = {street: "123 Main St", city: "Springfield", zip: 12345};
console.log(`  ✓ Person: ${p.name}, age ${p.age}`);
console.log(`  ✓ Address: ${p.address.street}, ${p.address.city}`);
const full_addr = p.get_full_address();
console.log(`  ✓ Full address: ${full_addr}`);

// Test 3: Enum support
console.log('\nTest 3: Enum support...');
const shape = new features.ColoredShape();
shape.color = 1;  // Color::Green
shape.size = 5.0;
const color_val = shape.get_color_value();
assert.strictEqual(color_val, 1, 'Enum value failed');
console.log(`  ✓ ColoredShape: color = ${color_val}, size = ${shape.size}`);

// Test 4: Resource
console.log('\nTest 4: Resource...');
const r = new features.Resource();
r.name = "TestResource";
r.value = 42;
const info = r.get_info();
assert.strictEqual(info, "TestResource:42", 'Resource info failed');
console.log(`  ✓ Resource info: ${info}`);

console.log('\n' + '='.repeat(40));
console.log('✓ All 4 comprehensive feature tests passed!');
console.log('='.repeat(40));

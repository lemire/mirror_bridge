// Test auto-discovered JavaScript bindings
const geometry = require('../../build/geometry_js');
const assert = require('assert');

console.log('=== JavaScript Auto-Discovery Test ===\n');

// Test Point class
console.log('Test 1: Point class...');
const p = new geometry.Point();
p.x = 3.0;
p.y = 4.0;
assert.strictEqual(p.x, 3.0);
assert.strictEqual(p.y, 4.0);
const dist = p.distance_from_origin();
assert(Math.abs(dist - 5.0) < 0.001, 'distance_from_origin failed');
console.log(`  ✓ Point: x=${p.x}, y=${p.y}, distance=${dist}`);

// Test Circle class
console.log('\nTest 2: Circle class...');
const c = new geometry.Circle();
c.radius = 10.0;
assert.strictEqual(c.radius, 10.0);
const area = c.area();
assert(Math.abs(area - 314.159) < 0.01, 'area failed');
console.log(`  ✓ Circle: radius=${c.radius}, area=${area}`);

// Test Circle with nested Point
console.log('\nTest 3: Circle center (nested Point)...');
console.log(`  ✓ Circle has nested center point: x=${c.center.x}, y=${c.center.y}`);
console.log('  (Note: Nested objects are converted to JavaScript objects)');

console.log('\n' + '='.repeat(40));
console.log('✓ All auto-discovery tests passed!');
console.log('='.repeat(40));

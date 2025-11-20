const addon = require('../../../../build/point2d');
const assert = require('assert');

console.log('=== Point2D JavaScript Binding Test ===\n');

// Test 1: Creation
console.log('Test 1: Creating Point2D...');
const p = new addon.Point2D();
assert.strictEqual(p.x, 0.0);
assert.strictEqual(p.y, 0.0);
console.log(`  ✓ Created: (${p.x}, ${p.y})`);

// Test 2: Setting values
console.log('\nTest 2: Setting values...');
p.x = 3.0;
p.y = 4.0;
assert.strictEqual(p.x, 3.0);
assert.strictEqual(p.y, 4.0);
console.log(`  ✓ Set to: (${p.x}, ${p.y})`);

// Test 3: Multiple instances
console.log('\nTest 3: Multiple instances...');
const p1 = new addon.Point2D();
const p2 = new addon.Point2D();
p1.x = 1.0;
p2.x = 2.0;
assert.strictEqual(p1.x, 1.0);
assert.strictEqual(p2.x, 2.0);
console.log(`  ✓ p1.x=${p1.x}, p2.x=${p2.x}`);

console.log('\n' + '='.repeat(40));
console.log('✓ All tests passed!');
console.log('='.repeat(40));

// Point2D V8 Binding Test
// This JavaScript is executed by the embedded V8 engine

print('=== Point2D V8 Binding Test ===\n');

var allPassed = true;

function assert(condition, message) {
    if (!condition) {
        print('  FAIL: ' + message);
        allPassed = false;
        throw new Error(message);
    }
}

// Test 1: Creation
print('Test 1: Creating Point2D...');
var p = new Point2D();
assert(p.x === 0.0, 'Default x should be 0.0');
assert(p.y === 0.0, 'Default y should be 0.0');
print('  PASS: Created with default values (' + p.x + ', ' + p.y + ')');

// Test 2: Setting values
print('\nTest 2: Setting values...');
p.x = 3.0;
p.y = 4.0;
assert(p.x === 3.0, 'x should be 3.0');
assert(p.y === 4.0, 'y should be 4.0');
print('  PASS: Set to (' + p.x + ', ' + p.y + ')');

// Test 3: Multiple instances
print('\nTest 3: Multiple instances...');
var p1 = new Point2D();
var p2 = new Point2D();
p1.x = 1.0;
p2.x = 2.0;
assert(p1.x === 1.0, 'p1.x should be 1.0');
assert(p2.x === 2.0, 'p2.x should be 2.0');
print('  PASS: p1.x=' + p1.x + ', p2.x=' + p2.x);

print('\n========================================');
if (allPassed) {
    print('All tests passed!');
} else {
    print('SOME TESTS FAILED!');
}
print('========================================');

// Return success/failure to C++ harness
allPassed;

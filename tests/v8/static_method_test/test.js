// Vec3 V8 Binding Test - Static Methods and Const Reference Parameters
// This JavaScript is executed by the embedded V8 engine

print('============================================================');
print('Vec3 V8 Binding Tests');
print('============================================================');
print('');

var allPassed = true;

function assert(condition, message) {
    if (!condition) {
        print('  FAIL: ' + message);
        allPassed = false;
        throw new Error(message);
    }
}

function approxEqual(a, b, epsilon) {
    epsilon = epsilon || 0.001;
    return Math.abs(a - b) < epsilon;
}

// Test 1: Basic construction
print('Testing basic construction...');
var v = new Vec3();
assert(v.x === 0 && v.y === 0 && v.z === 0, 'Default constructor failed');
print('  PASS: Default constructor works');

// Test 2: Setting values
v.x = 1.0;
v.y = 2.0;
v.z = 3.0;
assert(v.x === 1.0 && v.y === 2.0 && v.z === 3.0, 'Setting values failed');
print('  PASS: Setting values works');

// Test 3: Const reference parameter (dot method)
print('');
print('Testing const reference parameter (dot method)...');
var a = new Vec3();
a.x = 1.0; a.y = 2.0; a.z = 3.0;
var b = new Vec3();
b.x = 4.0; b.y = 5.0; b.z = 6.0;

// dot product: 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
var dotResult = a.dot(b);
assert(approxEqual(dotResult, 32.0), 'dot() returned wrong value: ' + dotResult);
print('  PASS: a.dot(b) = ' + dotResult);

// Test 4: length method
print('');
print('Testing length method...');
var v2 = new Vec3();
v2.x = 3.0; v2.y = 4.0; v2.z = 0.0;
var len = v2.length();
assert(approxEqual(len, 5.0), 'length() returned wrong value: ' + len);
print('  PASS: length() = ' + len);

// Test 5: Static method (hot_loop)
print('');
print('Testing static method (hot_loop)...');
var hotResult = Vec3.hot_loop(1000);
assert(hotResult > 0, 'hot_loop() returned non-positive value');
print('  PASS: Vec3.hot_loop(1000) = ' + hotResult.toFixed(2));

// Test 6: Static method with const ref params returning bound type
print('');
print('Testing static method with const ref params (add_vectors)...');
var vA = new Vec3();
vA.x = 1.0; vA.y = 2.0; vA.z = 3.0;
var vB = new Vec3();
vB.x = 4.0; vB.y = 5.0; vB.z = 6.0;

var vSum = Vec3.add_vectors(vA, vB);
assert(approxEqual(vSum.x, 5.0), 'add_vectors x wrong: ' + vSum.x);
assert(approxEqual(vSum.y, 7.0), 'add_vectors y wrong: ' + vSum.y);
assert(approxEqual(vSum.z, 9.0), 'add_vectors z wrong: ' + vSum.z);
print('  PASS: Vec3.add_vectors(a, b) = (' + vSum.x + ', ' + vSum.y + ', ' + vSum.z + ')');

// Test 7: Verify returned object has working methods
print('');
print('Testing that returned object has working methods...');
var vSumLen = vSum.length();
// length of (5, 7, 9) = sqrt(25 + 49 + 81) = sqrt(155) ≈ 12.45
assert(vSumLen > 12.0 && vSumLen < 13.0, 'Returned object length wrong: ' + vSumLen);
print('  PASS: Returned Vec3 has working methods, length() = ' + vSumLen.toFixed(2));

print('');
print('============================================================');
if (allPassed) {
    print('ALL TESTS PASSED!');
} else {
    print('SOME TESTS FAILED!');
}
print('============================================================');

// Return success/failure to C++ harness
allPassed;

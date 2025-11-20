#!/usr/bin/env node
/**
 * Runtime Performance Benchmarks for JavaScript Bindings
 * Compares Mirror Bridge JS vs Plain N-API
 */

// Test iterations
const ITERATIONS = {
    null_call: 10000000,      // 10M iterations for empty call
    arithmetic: 10000000,      // 10M for int/double ops
    string: 1000000,           // 1M for string ops
    vector: 100000,            // 100K for vector ops
    attribute: 10000000,       // 10M for attribute access
    construction: 1000000,     // 1M for object creation
};

// Benchmark function
function benchmark(name, func, iterations) {
    // Warmup
    const warmup = Math.min(1000, Math.floor(iterations / 10));
    for (let i = 0; i < warmup; i++) {
        func();
    }

    // Measure (5 runs)
    const times = [];
    for (let run = 0; run < 5; run++) {
        const start = process.hrtime.bigint();
        for (let i = 0; i < iterations; i++) {
            func();
        }
        const end = process.hrtime.bigint();
        const elapsed = Number(end - start);
        times.push(elapsed / iterations);  // ns per operation
    }

    // Return median
    times.sort((a, b) => a - b);
    return times[2];
}

// Run all benchmarks on a module
function runBenchmarks(moduleName, BenchmarkClass) {
    const results = {};

    // Create test object
    const obj = new BenchmarkClass();

    // 1. Null call overhead
    results.null_call = benchmark(
        'null_call',
        () => obj.null_call(),
        ITERATIONS.null_call
    );

    // 2. Arithmetic - int
    results.arithmetic_int = benchmark(
        'arithmetic_int',
        () => obj.add_int(42),
        ITERATIONS.arithmetic
    );

    // 3. Arithmetic - double
    results.arithmetic_double = benchmark(
        'arithmetic_double',
        () => obj.multiply_double(3.14),
        ITERATIONS.arithmetic
    );

    // 4. String operations - concatenate
    results.string_concat = benchmark(
        'string_concat',
        () => obj.concat_string("_test"),
        ITERATIONS.string
    );

    // 5. String operations - get
    results.string_get = benchmark(
        'string_get',
        () => obj.get_string(),
        ITERATIONS.string
    );

    // 6. String operations - set
    results.string_set = benchmark(
        'string_set',
        () => obj.set_string("benchmark"),
        ITERATIONS.string
    );

    // 7. Vector operations - append
    obj.data = [];
    results.vector_append = benchmark(
        'vector_append',
        () => obj.add_to_vector(1.0),
        ITERATIONS.vector
    );

    // 8. Vector operations - get
    obj.data = [];
    for (let i = 0; i < 100; i++) {
        obj.data.push(1.0);
    }
    results.vector_get = benchmark(
        'vector_get',
        () => obj.get_vector(),
        ITERATIONS.vector
    );

    // 9. Vector operations - set
    const testVec = [1.0, 2.0, 3.0, 4.0, 5.0];
    results.vector_set = benchmark(
        'vector_set',
        () => obj.set_vector(testVec),
        ITERATIONS.vector
    );

    // 10. Attribute access - get
    results.attr_get = benchmark(
        'attr_get',
        () => { const x = obj.counter; },
        ITERATIONS.attribute
    );

    // 11. Attribute access - set
    let counter = 0;
    results.attr_set = benchmark(
        'attr_set',
        () => {
            obj.counter = counter;
            counter++;
        },
        ITERATIONS.attribute
    );

    // 12. Object construction
    results.construction = benchmark(
        'construction',
        () => { const o = new BenchmarkClass(); },
        ITERATIONS.construction
    );

    return results;
}

// Format time
function formatTime(ns) {
    if (ns < 1000) {
        return `${ns.toFixed(1)} ns`;
    } else if (ns < 1000000) {
        return `${(ns / 1000).toFixed(1)} µs`;
    } else {
        return `${(ns / 1000000).toFixed(1)} ms`;
    }
}

// Print results
function printResults(resultsMb, resultsPlain, resultsNodeAddonApi) {
    const benchmarks = [
        ['Null call', 'null_call'],
        ['Add int', 'arithmetic_int'],
        ['Multiply double', 'arithmetic_double'],
        ['Concat string', 'string_concat'],
        ['Get string', 'string_get'],
        ['Set string', 'string_set'],
        ['Vector append', 'vector_append'],
        ['Vector get', 'vector_get'],
        ['Vector set', 'vector_set'],
        ['Attr get', 'attr_get'],
        ['Attr set', 'attr_set'],
        ['Construction', 'construction'],
    ];

    const width = resultsNodeAddonApi ? 105 : 90;
    console.log('\n' + '='.repeat(width));
    console.log(' Runtime Performance Benchmarks - JavaScript (lower is better)');
    console.log('='.repeat(width));

    let header = `\n${'Benchmark'.padEnd(20)} ${'Mirror Bridge'.padEnd(15)} ${'Plain N-API'.padEnd(15)}`;
    if (resultsNodeAddonApi) {
        header += 'node-addon-api'.padEnd(15);
    }
    console.log(header);
    console.log('-'.repeat(width));

    for (const [name, key] of benchmarks) {
        const mb = resultsMb[key];
        const plain = resultsPlain[key];
        const ratio = mb / plain;
        const marker = ratio <= 1.1 ? '✓' : (ratio <= 1.5 ? '⚠' : '✗');

        let line = name.padEnd(20) +
            formatTime(mb).padEnd(15) +
            formatTime(plain).padEnd(15);

        if (resultsNodeAddonApi) {
            line += formatTime(resultsNodeAddonApi[key]).padEnd(15);
        }

        line += `${ratio.toFixed(2)}x`.padEnd(15) + marker.padEnd(10);
        console.log(line);
    }

    console.log('\n' + '='.repeat(width));

    // Summary
    console.log('\nRelative to Plain N-API (lower is better):');
    let mbAvg = 0;
    let nodeAddonAvg = 0;
    for (const [, key] of benchmarks) {
        mbAvg += resultsMb[key] / resultsPlain[key];
        if (resultsNodeAddonApi) {
            nodeAddonAvg += resultsNodeAddonApi[key] / resultsPlain[key];
        }
    }
    mbAvg /= benchmarks.length;
    console.log(`  Mirror Bridge:   ${mbAvg.toFixed(2)}x`);
    if (resultsNodeAddonApi) {
        nodeAddonAvg /= benchmarks.length;
        console.log(`  node-addon-api:  ${nodeAddonAvg.toFixed(2)}x`);
    }

    console.log('='.repeat(width));
}

// Main
console.log('Loading modules...');

let benchMb, benchPlain, benchNodeAddonApi;
try {
    benchMb = require('../../build/bench_mb_js.node');
} catch (e) {
    console.error('Error: Failed to load bench_mb_js module');
    console.error('Please compile the Mirror Bridge JavaScript benchmark first');
    console.error(e.message);
    process.exit(1);
}

try {
    benchPlain = require('../../build/bench_plain_js.node');
} catch (e) {
    console.error('Error: Failed to load bench_plain_js module');
    console.error('Please compile the Plain N-API benchmark first');
    console.error(e.message);
    process.exit(1);
}

try {
    benchNodeAddonApi = require('../../build/bench_node_addon_api.node');
} catch (e) {
    console.log('Warning: node-addon-api not available, skipping');
    benchNodeAddonApi = null;
}

console.log('Running Mirror Bridge JavaScript benchmarks...');
const resultsMb = runBenchmarks('bench_mb_js', benchMb.BenchmarkClass);

console.log('Running Plain N-API benchmarks...');
const resultsPlain = runBenchmarks('bench_plain_js', benchPlain.BenchmarkClass);

let resultsNodeAddonApi = null;
if (benchNodeAddonApi) {
    console.log('Running node-addon-api benchmarks...');
    resultsNodeAddonApi = runBenchmarks('bench_node_addon_api', benchNodeAddonApi.BenchmarkClass);
}

printResults(resultsMb, resultsPlain, resultsNodeAddonApi);

// Save results (optional)
const fs = require('fs');
try {
    fs.writeFileSync('runtime_results_js.json', JSON.stringify({
        mirror_bridge: resultsMb,
        plain_napi: resultsPlain
    }, null, 2));
    console.log('\nResults saved to runtime_results_js.json');
} catch (e) {
    // Ignore if we can't save
}

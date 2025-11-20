#!/usr/bin/env lua5.4
--[[
Runtime Performance Benchmarks for Lua Bindings
Compares Mirror Bridge Lua vs Plain Lua C API
]]--

-- Test iterations
local ITERATIONS = {
    null_call = 10000000,      -- 10M iterations for empty call
    arithmetic = 10000000,      -- 10M for int/double ops
    string = 1000000,           -- 1M for string ops
    vector = 100000,            -- 100K for vector ops
    attribute = 10000000,       -- 10M for attribute access
    construction = 1000000,     -- 1M for object creation
}

-- Benchmark function
local function benchmark(name, func, iterations)
    -- Warmup
    local warmup = math.min(1000, iterations // 10)
    for i = 1, warmup do
        func()
    end

    -- Measure (5 runs)
    local times = {}
    for run = 1, 5 do
        local start = os.clock()
        for i = 1, iterations do
            func()
        end
        local elapsed = os.clock() - start
        table.insert(times, (elapsed * 1e9) / iterations)  -- ns per operation
    end

    -- Return median
    table.sort(times)
    return times[3]
end

-- Run all benchmarks on a module
local function run_benchmarks(module_name, obj_class)
    local results = {}

    -- Create test object
    -- sol2 uses table.new() pattern, others are callable
    local obj
    if type(obj_class) == "table" and obj_class.new then
        obj = obj_class.new()
    else
        obj = obj_class()
    end

    -- 1. Null call overhead
    results.null_call = benchmark(
        'null_call',
        function() obj:null_call() end,
        ITERATIONS.null_call
    )

    -- 2. Arithmetic - int
    results.arithmetic_int = benchmark(
        'arithmetic_int',
        function() obj:add_int(42) end,
        ITERATIONS.arithmetic
    )

    -- 3. Arithmetic - double
    results.arithmetic_double = benchmark(
        'arithmetic_double',
        function() obj:multiply_double(3.14) end,
        ITERATIONS.arithmetic
    )

    -- 4. String operations - concatenate
    results.string_concat = benchmark(
        'string_concat',
        function() obj:concat_string("_test") end,
        ITERATIONS.string
    )

    -- 5. String operations - get
    results.string_get = benchmark(
        'string_get',
        function() obj:get_string() end,
        ITERATIONS.string
    )

    -- 6. String operations - set
    results.string_set = benchmark(
        'string_set',
        function() obj:set_string("benchmark") end,
        ITERATIONS.string
    )

    -- 7. Vector operations - append
    obj.data = {}
    results.vector_append = benchmark(
        'vector_append',
        function() obj:add_to_vector(1.0) end,
        ITERATIONS.vector
    )

    -- 8. Vector operations - get
    obj.data = {}
    for i = 1, 100 do
        table.insert(obj.data, 1.0)
    end
    results.vector_get = benchmark(
        'vector_get',
        function() obj:get_vector() end,
        ITERATIONS.vector
    )

    -- 9. Vector operations - set
    local test_vec = {1.0, 2.0, 3.0, 4.0, 5.0}
    results.vector_set = benchmark(
        'vector_set',
        function() obj:set_vector(test_vec) end,
        ITERATIONS.vector
    )

    -- 10. Attribute access - get
    results.attr_get = benchmark(
        'attr_get',
        function() local x = obj.counter end,
        ITERATIONS.attribute
    )

    -- 11. Attribute access - set
    local counter = 0
    results.attr_set = benchmark(
        'attr_set',
        function()
            obj.counter = counter
            counter = counter + 1
        end,
        ITERATIONS.attribute
    )

    -- 12. Object construction
    results.construction = benchmark(
        'construction',
        function()
            if type(obj_class) == "table" and obj_class.new then
                local o = obj_class.new()
            else
                local o = obj_class()
            end
        end,
        ITERATIONS.construction
    )

    return results
end

-- Format time
local function format_time(ns)
    if ns < 1000 then
        return string.format("%.1f ns", ns)
    elseif ns < 1000000 then
        return string.format("%.1f µs", ns / 1000)
    else
        return string.format("%.1f ms", ns / 1000000)
    end
end

-- Print results
local function print_results(results_mb, results_plain, results_sol2, results_luabridge)
    local benchmarks = {
        {'Null call', 'null_call'},
        {'Add int', 'arithmetic_int'},
        {'Multiply double', 'arithmetic_double'},
        {'Concat string', 'string_concat'},
        {'Get string', 'string_get'},
        {'Set string', 'string_set'},
        {'Vector append', 'vector_append'},
        {'Vector get', 'vector_get'},
        {'Vector set', 'vector_set'},
        {'Attr get', 'attr_get'},
        {'Attr set', 'attr_set'},
        {'Construction', 'construction'},
    }

    local header_cols = {"Benchmark", "Mirror Bridge", "Plain C"}
    local col_widths = {20, 15, 15}

    if results_sol2 then
        table.insert(header_cols, "sol2")
        table.insert(col_widths, 15)
    end
    if results_luabridge then
        table.insert(header_cols, "LuaBridge3")
        table.insert(col_widths, 15)
    end

    local total_width = 0
    for _, w in ipairs(col_widths) do total_width = total_width + w + 1 end

    print("\n" .. string.rep("=", total_width))
    print(" Runtime Performance Benchmarks - Lua (lower is better)")
    print(string.rep("=", total_width))

    -- Header
    local header = "\n"
    for i, col in ipairs(header_cols) do
        header = header .. string.format("%-" .. col_widths[i] .. "s ", col)
    end
    print(header)
    print(string.rep("-", total_width))

    for _, bench in ipairs(benchmarks) do
        local name, key = bench[1], bench[2]
        local mb = results_mb[key]
        local plain = results_plain[key]

        local line = string.format("%-" .. col_widths[1] .. "s ", name)
        line = line .. string.format("%-" .. col_widths[2] .. "s ", format_time(mb))
        line = line .. string.format("%-" .. col_widths[3] .. "s ", format_time(plain))

        if results_sol2 then
            line = line .. string.format("%-" .. col_widths[4] .. "s ", format_time(results_sol2[key]))
        end
        if results_luabridge then
            local idx = results_sol2 and 5 or 4
            line = line .. string.format("%-" .. col_widths[idx] .. "s ", format_time(results_luabridge[key]))
        end

        print(line)
    end

    print("\n" .. string.rep("=", total_width))

    -- Summary statistics
    print("\nRelative to Plain C (lower is better):")
    local mb_avg = 0
    local sol2_avg = 0
    local luabridge_avg = 0
    for _, bench in ipairs(benchmarks) do
        local key = bench[2]
        mb_avg = mb_avg + (results_mb[key] / results_plain[key])
        if results_sol2 then
            sol2_avg = sol2_avg + (results_sol2[key] / results_plain[key])
        end
        if results_luabridge then
            luabridge_avg = luabridge_avg + (results_luabridge[key] / results_plain[key])
        end
    end
    mb_avg = mb_avg / #benchmarks
    print(string.format("  Mirror Bridge: %.2fx", mb_avg))

    if results_sol2 then
        sol2_avg = sol2_avg / #benchmarks
        print(string.format("  sol2:          %.2fx", sol2_avg))
    end
    if results_luabridge then
        luabridge_avg = luabridge_avg / #benchmarks
        print(string.format("  LuaBridge3:    %.2fx", luabridge_avg))
    end

    print(string.rep("=", total_width))
end

-- Main
print("Loading modules...")
package.cpath = package.cpath .. ";../../build/?.so"

-- Load all available benchmarks
local ok_mb, bench_mb = pcall(require, "bench_mb_lua")
local ok_plain, bench_plain = pcall(require, "bench_plain_lua")
local ok_sol2, bench_sol2 = pcall(require, "bench_sol2")
local ok_luabridge, bench_luabridge = pcall(require, "bench_luabridge")

if not ok_mb then
    print("Error: Failed to load bench_mb_lua module")
    print("Please compile the Mirror Bridge Lua benchmark first")
    os.exit(1)
end

if not ok_plain then
    print("Error: Failed to load bench_plain_lua module")
    print("Please compile the Plain Lua C API benchmark first")
    os.exit(1)
end

print("Running Mirror Bridge Lua benchmarks...")
local results_mb = run_benchmarks('bench_mb_lua', bench_mb.BenchmarkClass)

print("Running Plain Lua C API benchmarks...")
local results_plain = run_benchmarks('bench_plain_lua', bench_plain.BenchmarkClass)

local results_sol2, results_luabridge

-- Run sol2 if available
if ok_sol2 then
    print("Running sol2 benchmarks...")
    results_sol2 = run_benchmarks('bench_sol2', bench_sol2.BenchmarkClass)
else
    print("Warning: sol2 not available, skipping")
    results_sol2 = nil
end

-- Skip LuaBridge3 (property access issues)
print("Warning: LuaBridge3 skipped (property access issues)")
results_luabridge = nil

print_results(results_mb, results_plain, results_sol2, results_luabridge)

-- Save results (optional)
local ok, json = pcall(require, 'dkjson')
if ok and json.encode then
    local file = io.open('runtime_results_lua.json', 'w')
    if file then
        file:write(json.encode({
            mirror_bridge = results_mb,
            plain_lua = results_plain
        }, {indent = true}))
        file:close()
        print("\nResults saved to runtime_results_lua.json")
    end
end

# Clang Plugin Demo - Naming Convention Checker

This demonstrates two approaches to code checking with Clang:
1. **Plugin approach** (requires RTTI-enabled LLVM/Clang build)
2. **LibTooling standalone tool** (works with standard builds)

## Important: Build Configuration

**Key insight**: Your plugin/tool must match LLVM's RTTI configuration.

Since LLVM is typically built **without RTTI** (`-fno-rtti`, the default for performance), you must compile your code with the same setting. The CMakeLists.txt is configured to do this automatically:

```cmake
target_compile_options(naming-checker PRIVATE -fno-rtti)
```

**If you see "undefined reference to typeinfo" errors**, it means there's an RTTI mismatch between your code and LLVM. The solution is to use `-fno-rtti` (not `-frtti`).

## Status

✅ **Working!** This demo successfully builds and runs with standard LLVM installations.

```bash
## Building the Standalone Tool

```bash
# Create build directory
mkdir build-tool
cd build-tool

# Configure and build
cmake ..
make
```

## Using the Standalone Tool

```bash
# Run the naming checker tool
./build-tool/naming-checker test.cpp -- -I/usr/lib/gcc/x86_64-linux-gnu/11/include

# You should see warnings for functions that don't follow snake_case
```

The tool will analyze all functions and report naming convention violations.

## Expected Output

The plugin will warn about functions that don't follow snake_case:
- `calculateProduct` - uses camelCase
- `CalculateDifference` - uses PascalCase

Functions like `calculate_sum` and `main` will pass without warnings.

## Plugin Features

- Checks all function declarations
- Ignores compiler-generated functions
- Ignores `main` function
- Reports custom warnings with function names
- Uses Clang's diagnostic system

## Customization

You can modify the plugin to:
- Check different naming conventions (camelCase, PascalCase)
- Check variable names, class names, etc.
- Add command-line arguments for configuration
- Make warnings into errors
- Apply automatic fixes

## Files

- `NamingConventionPlugin.cpp` - Plugin implementation
- `CMakeLists.txt` - Build configuration
- `test.cpp` - Example code to test the plugin
- `README.md` - This file

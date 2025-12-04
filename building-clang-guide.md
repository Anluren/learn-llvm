---
marp: true
theme: default
paginate: true
---

# Building Clang and Clang Extra Tools

A comprehensive guide to building LLVM, Clang, and Clang-tools-extra

---

## Prerequisites

### System Requirements
- **CMake** 3.13.4 or newer
- **Ninja** (recommended) or Make
- **C++ Compiler**: GCC 7.1+ or Clang 5.0+
- **Python** 3.6 or newer
- **Memory**: At least 8GB RAM (16GB+ recommended)
- **Disk Space**: ~20GB for build

---

## Key LLVM Concepts

### Understanding the Basics

**AST (Abstract Syntax Tree)**
- Tree representation of source code structure
- Used by Clang for parsing and analysis
- Nodes represent language constructs (functions, expressions, etc.)

**LLVM IR (Intermediate Representation)**
- Low-level programming language
- Platform-independent representation
- Target for optimizations and code generation

---

## More LLVM Concepts

### Essential Technologies

**TableGen**
- Domain-specific language for LLVM
- Generates C++ code from `.td` files
- Used for: instruction definitions, register info, diagnostics

**Clang AST Matchers**
- Library for matching AST nodes
- Used in clang-tidy and refactoring tools
- Pattern matching for code analysis

**Pass Pipeline**
- Series of optimization and transformation passes
- Each pass analyzes or modifies IR
- Can be customized and extended

---

## LLVM Coding Techniques

### Custom RTTI Implementation

**Why Custom RTTI?**
- C++ RTTI (`dynamic_cast`) disabled for performance
- LLVM uses custom type-safe casting system
- Faster and more lightweight

**Key Functions**:
```cpp
// Instead of dynamic_cast, use:
if (auto *FD = dyn_cast<FunctionDecl>(D)) {
  // FD is FunctionDecl*, or nullptr
}

// Assertions (crashes if wrong type):
auto *FD = cast<FunctionDecl>(D);

// Type checking:
if (isa<FunctionDecl>(D)) { ... }
```

---

## LLVM Data Structures

### Performance-Optimized Containers

**SmallVector<T, N>**
- Inline storage for N elements, heap for larger
- Avoids allocations for small sizes
- `SmallVector<int, 8>` - stores up to 8 ints inline

**StringRef**
- Non-owning string reference (pointer + length)
- No allocations, no copies
- Fast string passing and comparison

**DenseMap / DenseSet**
- Fast hash map/set optimized for pointers
- Better cache locality than std::unordered_map
- Use for frequent lookups

---

## More LLVM Data Structures

### Specialized Containers

**APInt / APFloat**
- Arbitrary precision integers and floats
- Handle any bit width
- Used for constant folding and analysis

**StringMap**
- Specialized map with string keys
- Single allocation for key+value
- Memory efficient for string lookups

**Twine**
- Lazy string concatenation
- Avoids intermediate string allocations
- Automatically converts to string when needed

---

## Getting the Source Code

```bash
# Clone the repository
git clone https://github.com/llvm/llvm-project.git
cd llvm-project

# Optional: checkout a specific version
git checkout llvmorg-17.0.0
```

---

## LLVM Source Code Structure

### Main Directory Layout

```
llvm-project/
├── llvm/              # LLVM core libraries
├── clang/             # Clang C/C++ compiler
├── clang-tools-extra/ # Additional clang tools
├── compiler-rt/       # Compiler runtime libraries
├── libcxx/            # C++ standard library
├── libcxxabi/         # C++ ABI library
├── lld/               # LLVM linker
├── lldb/              # LLVM debugger
├── polly/             # Polyhedral optimizations
└── cmake/             # Build configuration
```

---

## Key Directories Explained

### Important Subdirectories

**llvm/**
- `include/llvm/` - Public API headers
- `lib/` - Core implementation
- `tools/` - LLVM tools (opt, llc, etc.)

**clang/**
- `include/clang/` - Clang headers
- `lib/` - Compiler implementation
- `tools/` - Clang executable and tools

**clang-tools-extra/**
- `clang-tidy/` - Static analyzer
- `clangd/` - Language server
- `clang-format/` - Code formatter

---

## Build Configuration

### Basic Build Setup

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  ../llvm
```

---

## VS Code Build Configuration

### Using .vscode/settings.json

Create `.vscode/settings.json` in your llvm-project root:

```json
{
  "cmake.sourceDirectory": "${workspaceFolder}/llvm",
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.configureSettings": {
    "LLVM_ENABLE_PROJECTS": ["clang", "clang-tools-extra"],
    "CMAKE_BUILD_TYPE": "Release",
    "CMAKE_CXX_STANDARD": "17",
    "LLVM_TARGETS_TO_BUILD": "X86"
  }
}
```

Then use CMake Tools extension to configure and build

---

## Build Types

### Common Build Types

- **Release**: Optimized, no debug info
  ```bash
  -DCMAKE_BUILD_TYPE=Release
  ```

- **Debug**: Full debug info, no optimizations
  ```bash
  -DCMAKE_BUILD_TYPE=Debug
  ```

- **RelWithDebInfo**: Optimized with debug info
  ```bash
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
  ```

---

## Build Options

### Useful CMake Options

```bash
cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  ../llvm
```

---

## Building

### Compile the Project

```bash
# Build everything (uses all CPU cores)
ninja

# Build specific target
ninja clang

# Build clang-tools-extra tools
ninja clang-tidy clangd clang-format
```

**Note**: First build can take 30-60+ minutes depending on your system

---

## Installation

### Install to System

```bash
# Install to CMAKE_INSTALL_PREFIX
ninja install

# Or using CMake
cmake --build . --target install
```

### Verify Installation

```bash
clang --version
clang-tidy --version
clangd --version
```

---

## Clang Extra Tools

### Available Tools in clang-tools-extra

- **clang-tidy**: Static analysis and linting
- **clangd**: Language server for IDEs
- **clang-format**: Code formatter
- **clang-query**: AST query tool
- **clang-rename**: Renaming tool
- **include-cleaner**: Header cleanup tool

---

## LibTooling Setup

### What is LibTooling?

**LibTooling** is a library for writing standalone tools based on Clang
- Build custom code analysis tools
- Create refactoring tools
- Implement code transformations
- Access full Clang AST programmatically

**Use cases**: Custom linters, code generators, migration tools

---

## Installing LibTooling from Source

### Build and Install Clang Libraries

```bash
# Configure with libraries enabled
cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DLLVM_BUILD_LLVM_DYLIB=ON \
  -DLLVM_LINK_LLVM_DYLIB=ON \
  ../llvm

# Build
ninja

# Install headers and libraries
sudo ninja install
```

This installs Clang libraries, headers, and CMake config files needed for LibTooling

---

## Making Installation Available

### Configure Environment for LibTooling

**Option 1: Standard Installation Path**
```bash
# If installed to /usr/local, CMake finds it automatically
cmake .
```

**Option 2: Custom Installation Path**
```bash
# Set CMAKE_PREFIX_PATH to installation directory
cmake -DCMAKE_PREFIX_PATH=/path/to/llvm/install .
```

**Option 3: Using llvm-config**
```bash
# Get compiler and linker flags
export LLVM_DIR=$(llvm-config --cmakedir)
cmake -DLLVM_DIR=$LLVM_DIR .
```

**Verify Installation**:
```bash
llvm-config --libdir    # Library directory
llvm-config --includedir # Header directory
```

---

## Setting Up a LibTooling Project

### Project Structure

```bash
my-tool/
├── CMakeLists.txt
├── MyTool.cpp
└── compile_commands.json  # Generated by CMake
```

**CMakeLists.txt**:
```cmake
cmake_minimum_required(VERSION 3.13.4)
project(MyTool)

find_package(Clang REQUIRED)

add_executable(my-tool MyTool.cpp)
target_link_libraries(my-tool
  clangTooling
  clangBasic
  clangASTMatchers
)
```

---

## Simple LibTooling Example

### Minimal Tool Code

```cpp
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"

using namespace clang::tooling;

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(
      argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  
  return Tool.run(newFrontendActionFactory<
                  clang::SyntaxOnlyAction>().get());
}
```

---

## Building LibTooling Projects

### Compile and Run

```bash
# Configure with Clang installation
cmake -DCMAKE_PREFIX_PATH=/path/to/llvm/build .

# Build the tool
cmake --build .

# Run the tool
./my-tool myfile.cpp -- -std=c++17
```

**Note**: The `--` separates tool options from compiler flags

---

## Key LibTooling Constructs

### Core Components

**ClangTool**
- Main driver class for running tools
- Manages compilation database and source files
- Orchestrates the parsing and analysis

**FrontendAction**
- Defines what happens during compilation
- Override `CreateASTConsumer()` to process AST
- Examples: `SyntaxOnlyAction`, `ASTDumpAction`

**ASTConsumer**
- Receives AST nodes as they're parsed
- Override `HandleTranslationUnit()` for full AST
- Can process declarations, statements, types

---

## More LibTooling Constructs

### Advanced Components

**RecursiveASTVisitor**
- Template class for traversing AST
- Override `VisitXXX()` methods for specific nodes
- Example: `VisitFunctionDecl()`, `VisitCallExpr()`

**ASTMatcher**
- Declarative pattern matching for AST nodes
- More concise than manual traversal
- Example: `functionDecl(hasName("foo"))`

**Rewriter**
- Modify source code programmatically
- Insert, remove, or replace text
- Generate refactored code

---

## LibTooling Example with Visitor

### Using RecursiveASTVisitor

```cpp
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

class MyVisitor : public RecursiveASTVisitor<MyVisitor> {
public:
  bool VisitFunctionDecl(FunctionDecl *FD) {
    llvm::outs() << "Found function: " 
                 << FD->getName() << "\n";
    return true;
  }
};

class MyConsumer : public ASTConsumer {
  MyVisitor Visitor;
public:
  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};
```

---

## Building Individual Tools

### Build Only What You Need

```bash
# Build only clang-tidy
ninja clang-tidy

# Build multiple specific tools
ninja clang-tidy clangd clang-format

# Build all clang-tools-extra
ninja clang-tools-extra
```

---

## Optimizing Build Time

### Speed Up Compilation

1. **Use Ninja** instead of Make
2. **Limit targets**: `-DLLVM_TARGETS_TO_BUILD="X86"`
3. **Use ccache**:
   ```bash
   -DLLVM_CCACHE_BUILD=ON
   ```
4. **Reduce linking**: `-DLLVM_BUILD_LLVM_DYLIB=ON`
5. **Parallel jobs**: `ninja -j8`

---

## Minimal Build Example

### For Development/Testing

```bash
mkdir build-minimal
cd build-minimal

cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_INCLUDE_TESTS=OFF \
  -DLLVM_INCLUDE_DOCS=OFF \
  ../llvm

ninja clang clang-tidy clangd
```

---

## Troubleshooting

### Common Issues

1. **Out of memory**: 
   - Reduce parallel jobs: `ninja -j4`
   - Use shared libraries: `-DBUILD_SHARED_LIBS=ON`

2. **Old compiler**: 
   - Upgrade GCC/Clang
   - Use newer toolchain

3. **CMake version**: 
   - Update CMake to 3.13.4+

---

## Testing Your Build

### Run Tests

```bash
# Run all tests
ninja check-all

# Run clang tests only
ninja check-clang

# Run clang-tools-extra tests
ninja check-clang-tools
```

---

## Using Your Build

### Without Installing

```bash
# Add to PATH
export PATH=/path/to/llvm-project/build/bin:$PATH

# Use directly
./bin/clang --version
./bin/clang-tidy --help
```

---

## Generating AST with Clang

### Abstract Syntax Tree (AST)

Clang can generate and dump the AST of your C/C++ code for analysis

```bash
# Dump AST to terminal
clang -Xclang -ast-dump -fsyntax-only file.cpp

# Dump AST to file
clang -Xclang -ast-dump -fsyntax-only file.cpp > ast.txt
```

---

## AST Output Formats

### Different AST Representations

**JSON format** (more parseable):
```bash
clang -Xclang -ast-dump=json -fsyntax-only file.cpp
```

**With colors** (terminal):
```bash
clang -Xclang -ast-dump -fcolor-diagnostics -fsyntax-only file.cpp
```

**Filter specific function**:
```bash
clang -Xclang -ast-dump -Xclang -ast-dump-filter \
  -Xclang myFunction file.cpp
```

---

## AST Visualization

### Graphical AST View

Generate DOT graph for visualization:
```bash
# Generate DOT file
clang -Xclang -ast-view -fsyntax-only file.cpp

# Or export to DOT format
clang -Xclang -ast-dump -Xclang -ast-dump-format=dot \
  file.cpp > ast.dot

# Convert to image with Graphviz
dot -Tpng ast.dot -o ast.png
```

---

## Practical AST Use Cases

### Why Generate AST?

- **Code Analysis**: Understand code structure
- **Tool Development**: Build custom analyzers
- **Refactoring**: Automated code transformation
- **Documentation**: Generate code documentation
- **Static Analysis**: Find bugs and patterns

**Example**: Find all function declarations
```bash
clang -Xclang -ast-dump -fsyntax-only code.cpp | \
  grep FunctionDecl
```

---

## Compile Without Saving Files

### Using stdin with Clang

No need to create temporary files - compile directly from stdin:

```bash
# Compile C code from stdin
echo 'int main() { return 0; }' | clang -x c -

# Compile C++ code
echo 'int main() { return 0; }' | clang++ -x c++ -

# Generate assembly output
echo 'int add(int a, int b) { return a+b; }' | \
  clang -x c -S -o - -
```

The `-` tells clang to read from stdin

---

## Multi-line Code from stdin

### Using Here-Documents

For larger code snippets without files:

```bash
clang -x c - -o program << 'EOF'
#include <stdio.h>
int main() {
    printf("Hello, World!\n");
    return 0;
}
EOF

# Run the compiled program
./program
```

---

## Quick AST from Code Snippet

### Analyze Code Without Files

```bash
# Dump AST directly from code
echo 'int add(int a, int b) { return a + b; }' | \
  clang -x c -Xclang -ast-dump -

# Generate AST in JSON format
echo 'void foo() { int x = 42; }' | \
  clang -x c -Xclang -ast-dump=json -fsyntax-only -

# Check for syntax errors
echo 'int main() { return 0 }' | \
  clang -x c -fsyntax-only -
```

---

## Using clangd in VS Code

### Setup clangd Extension

1. **Install the extension**:
   - Search for "clangd" in VS Code extensions
   - Install the official **clangd** extension by LLVM

2. **Configure clangd path**:
   ```json
   {
     "clangd.path": "/path/to/llvm-project/build/bin/clangd"
   }
   ```

3. **Disable conflicting extensions**:
   - Disable C/C++ IntelliSense if installed

---

## clangd Features

### What You Get

- **Code Completion**: Smart autocomplete with type information
- **Go to Definition**: Navigate to symbol definitions
- **Find References**: Find all usages of a symbol
- **Code Actions**: Quick fixes and refactorings
- **Diagnostics**: Real-time error and warning detection
- **Formatting**: Code formatting with clang-format
- **Rename Symbol**: Intelligent renaming across files
- **AST Viewer**: Inspect AST nodes at cursor position

---

## Viewing AST in VS Code

### Show AST at Cursor

**Command Palette** (Ctrl+Shift+P):
1. Type "clangd: Show AST"
2. Or right-click → "Show AST"

**Features**:
- View AST node under cursor
- Navigate AST hierarchy
- See node types and properties
- Understand code structure interactively

**Tip**: Helps when writing clang-tidy checks or understanding how clang parses your code

---

## Configuring clangd

### .clangd Configuration File

Create `.clangd` in your project root:

```yaml
CompileFlags:
  Add: [-std=c++17, -Wall]
  Remove: [-W*]

Diagnostics:
  ClangTidy:
    Add: [performance-*, modernize-*]
    Remove: [modernize-use-trailing-return-type]
  UnusedIncludes: Strict
```

---

## Resources

### Documentation & Help

- **LLVM Documentation**: https://llvm.org/docs/
- **Getting Started**: https://llvm.org/docs/GettingStarted.html
- **CMake Options**: https://llvm.org/docs/CMake.html
- **Clang Tools**: https://clang.llvm.org/extra/
- **clangd**: https://clangd.llvm.org/

---

# Questions?

Thank you for your attention!

**Happy Building! 🛠️**

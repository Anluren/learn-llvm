---
marp: true
theme: default
paginate: true
size: 1920px 1200px
style: |
  section {
    font-size: 20px;
  }
  pre {
    font-size: 16px;
  }
  .columns {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 1rem;
  }
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

## Environment Setup

### Preparing Your Development Environment

**Network Configuration (if behind corporate proxy)**:
```bash
# Set proxy for Git
git config --global http.proxy http://proxy.company.com:8080
git config --global https.proxy http://proxy.company.com:8080

# Set proxy for shell (add to ~/.bashrc or ~/.zshrc)
export http_proxy=http://proxy.company.com:8080
export https_proxy=http://proxy.company.com:8080
export HTTP_PROXY=$http_proxy
export HTTPS_PROXY=$https_proxy

# For tools that need it
export no_proxy=localhost,127.0.0.1,.company.com
```

---

## More Environment Setup

### Additional Configurations

**Verify connectivity**:
```bash
# Test GitHub access
git ls-remote https://github.com/llvm/llvm-project.git

# Test HTTPS
curl -I https://github.com
```

**Unset proxy (when not needed)**:
```bash
git config --global --unset http.proxy
git config --global --unset https.proxy
unset http_proxy https_proxy HTTP_PROXY HTTPS_PROXY
```

**Alternative: Use SSH instead of HTTPS**:
```bash
# Clone with SSH (if proxy allows)
git clone git@github.com:llvm/llvm-project.git
```

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
- `SmallVector<int, 8>` - stores up to 8 ints if**
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

## Getting the Source Code & Structure

<div class="columns">
<div>

### Clone the Repository

```bash
# Clone the repository
git clone https://github.com/llvm/llvm-project.git
cd llvm-project

# Optional: checkout a specific version
git checkout llvmorg-17.0.0
```

</div>
<div>

### Main Directory Layout

```
llvm-project/
├── llvm/              # LLVM core
├── clang/             # C/C++ compiler
├── clang-tools-extra/ # Extra tools
├── compiler-rt/       # Runtime libs
├── libcxx/            # C++ std library
├── libcxxabi/         # C++ ABI library
├── lld/               # LLVM linker
├── lldb/              # LLVM debugger
├── polly/             # Optimizations
└── cmake/             # Build config
```

</div>
</div>

---

## Key Directories Explained

<div class="columns">
<div>

### LLVM & Clang Core

**llvm/**
- `include/llvm/` - Public API headers
- `lib/` - Core implementation
- `tools/` - LLVM tools (opt, llc, etc.)

**clang/**
- `include/clang/` - Clang headers
- `lib/` - Compiler implementation
- `tools/` - Clang executable and tools

</div>
<div>

### Clang Extra Tools

**clang-tools-extra/**
- `clang-tidy/` - Static analyzer
- `clangd/` - Language server
- `clang-format/` - Code formatter

</div>
</div>

---

## Build Configuration

<div class="columns">
<div>

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
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  ../llvm
```

</div>
<div>

### VS Code Build Configuration

Create `.vscode/settings.json`:

```json
{
  "cmake.sourceDirectory": "${workspaceFolder}/llvm",
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.configureSettings": {
    "LLVM_ENABLE_PROJECTS": ["clang", "clang-tools-extra"],
    "CMAKE_BUILD_TYPE": "Release",
    "CMAKE_CXX_STANDARD": "17",
    "LLVM_TARGETS_TO_BUILD": "X86",
    "LLVM_USE_LINKER": "lld",
  }
}
```

Then use CMake Tools extension

</div>
</div>

---

## Build Types & Options

<div class="columns">
<div>

### Common Build Types

**Release**: Optimized, no debug info
```bash
-DCMAKE_BUILD_TYPE=Release
```

**Debug**: Full debug info, no optimizations
```bash
-DCMAKE_BUILD_TYPE=Debug
```

**RelWithDebInfo**: Optimized with debug info
```bash
-DCMAKE_BUILD_TYPE=RelWithDebInfo
```

</div>
<div>

### Useful CMake Options

```bash
cmake -G Ninja \
  -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_STANDARD=17 \
  -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DLLVM_TARGETS_TO_BUILD="X86" \
  -DLLVM_USE_LINKER=lld,
  ../llvm
```

</div>
</div>

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

## Changing Install Location

### After Build is Complete

**Option 1: Using --prefix (easiest)**
```bash
cmake --install . --prefix /new/install/path
```

**Option 2: Using DESTDIR**
```bash
DESTDIR=/new/install/path ninja install
```

**Option 3: Reconfigure CMAKE_INSTALL_PREFIX**
```bash
cmake -DCMAKE_INSTALL_PREFIX=/new/install/path .
ninja install
```

**Note**: No rebuild needed - only affects installation step

---

## Managing Multiple Clang Versions

### Using update-alternatives (Debian/Ubuntu)

**Install alternatives**:
```bash
# Add system clang-17
sudo update-alternatives --install /usr/bin/clang clang \
  /usr/local/bin/clang-17 100

# Add custom clang-17 with higher priority
sudo update-alternatives --install /usr/bin/clang clang \
  /home/user/llvm-install/bin/clang-17 200
```

**Switch between versions**:
```bash
sudo update-alternatives --config clang
```

**Verify selection**:
```bash
clang --version
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

## Configure Environment for LibTooling

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

## Out-of-Tree LibTooling Development

### Developing Outside LLVM Source Tree

**Benefits**:
- Independent project repository
- Separate build and versioning
- Use installed LLVM/Clang libraries
- Easier distribution and maintenance

**Prerequisites**:
- LLVM/Clang installed (with headers and libraries)
- CMake 3.13.4+
- Compilation database (optional, for testing)

**Key Point**: Link against installed LLVM/Clang, not source tree

---

## Setting Up a LibTooling Project

<div class="columns">
<div>

### Project Structure

```bash
my-tool/
├── CMakeLists.txt
├── MyTool.cpp
└── compile_commands.json
```

Generated by CMake

</div>
<div>

### CMakeLists.txt

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

</div>
</div>

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

## LibTooling Project Examples

### Real-World Projects on GitHub

**Official LLVM Projects**:
- **clang-tidy** - Static analyzer and linter
- **clangd** - Language server protocol implementation
- **clang-format** - Code formatting tool
- All in: https://github.com/llvm/llvm-project/tree/main/clang-tools-extra

**Community Projects**:
- **include-what-you-use** - Analyze C++ includes
  - https://github.com/include-what-you-use/include-what-you-use
- **clang-rename** - Intelligent symbol renaming
- **clang-query** - Interactive AST matcher tool

---

## LibTooling Learning Resources

### Documentation and Tutorials

**Official Documentation**:
- LibTooling Tutorial: https://clang.llvm.org/docs/LibTooling.html
- AST Matcher Reference: https://clang.llvm.org/docs/LibASTMatchersReference.html
- Clang API Docs: https://clang.llvm.org/doxygen/

**Example Code**:
- Browse clang-tools-extra source for patterns
- Each tool shows different LibTooling features
- Well-documented and production-quality

**Tips**: Start with simple visitors, then explore matchers

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

## Clang Plugins

### Extend Clang at Compile Time

**What are Clang Plugins?**
- Dynamically loaded libraries extending Clang
- Run during compilation to analyze code
- Enforce custom constraints and checks

**Use Cases:**
- Enforce coding standards (naming, patterns)
- Detect security vulnerabilities
- Apply domain-specific rules
- Custom warnings and errors

**Key Difference**: Plugins run during compilation, LibTooling runs standalone

---

## Creating a Clang Plugin

### Basic Plugin Structure

```cpp
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"

class MyPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(
      CompilerInstance &CI, StringRef) override {
    return std::make_unique<MyConsumer>();
  }
  
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<MyPluginAction>
X("my-plugin", "Custom constraint checker");
```

---

## Building and Using Plugins

### Compile and Load

**Build the plugin**:
```cmake
add_library(MyPlugin MODULE MyPlugin.cpp)
target_link_libraries(MyPlugin clangAST clangBasic clangFrontend)
```

**Use the plugin**:
```bash
clang++ -fplugin=./MyPlugin.so \
  -Xclang -plugin -Xclang my-plugin \
  source.cpp
```

**Add plugin arguments**:
```bash
clang++ -fplugin=./MyPlugin.so \
  -Xclang -plugin-arg-my-plugin -Xclang --strict \
  source.cpp
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

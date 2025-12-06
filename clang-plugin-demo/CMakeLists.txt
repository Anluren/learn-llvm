cmake_minimum_required(VERSION 3.13.4)
project(NamingCheckerTool)

# Find Clang and LLVM
find_package(Clang REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

# Add LLVM and Clang include directories
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

# Add LLVM definitions
add_definitions(${LLVM_DEFINITIONS})

# Build the standalone tool
add_executable(naming-checker NamingCheckerTool.cpp)

# Link against Clang libraries
target_link_libraries(naming-checker PRIVATE
  clangAST
  clangBasic
  clangDriver
  clangFrontend
  clangParse
  clangSema
  clangAnalysis
  clangEdit
  clangLex
  clangTooling
  clangASTMatchers
)

# Set C++ standard and disable RTTI to match LLVM
set_target_properties(naming-checker PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

# Disable RTTI to match LLVM's build configuration
target_compile_options(naming-checker PRIVATE -fno-rtti)

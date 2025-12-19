# lld linker logic disabled for now due to build issues
# find_program(LLD_PROGRAM NAMES lld lld-link)
# if(LLD_PROGRAM)
#   message(STATUS "lld linker found: enabling lld as linker")
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
#   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
#   set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
# endif()
# Store common Clang libraries in a variable for reuse
set(CLANG_LIBS
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
# # Build the constant expression evaluator tool
# add_executable(eval-expr EvalExprTool.cpp)
# target_link_libraries(eval-expr PRIVATE
#   clangAST
#   clangBasic
#   clangFrontend
#   clangTooling
#   clangASTMatchers
# )
# target_compile_options(eval-expr PRIVATE -fno-rtti)
# Set C++ standard for all targets
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build the Clang plugin (shared library)
add_library(NamingConventionPlugin MODULE src/NamingConventionPlugin.cpp)

# Match LLVM's RTTI and linker settings for the plugin
target_compile_options(NamingConventionPlugin PRIVATE -fno-rtti)
if(LLD_PROGRAM)
  set_target_properties(NamingConventionPlugin PROPERTIES
    LINK_FLAGS "-fuse-ld=lld"
  )
endif()
# LLD linker logic disabled for now due to build issues
# find_program(LLD_PROGRAM NAMES lld lld-link)
# if(LLD_PROGRAM)
#   message(STATUS "lld linker found: enabling lld as linker")
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
#   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
#   set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
# endif()

cmake_minimum_required(VERSION 3.13.4)
project(NamingCheckerTool)

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "ccache found: enabling compiler cache")
  set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# Find Clang and LLVM
find_package(Clang REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

# Add LLVM and Clang include directories
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

# Add LLVM definitions
add_definitions(${LLVM_DEFINITIONS})

## Build the EvalExprTool standalone tool
add_executable(eval-expr src/EvalExprTool.cpp)
target_link_libraries(eval-expr PRIVATE ${CLANG_LIBS})
target_compile_options(eval-expr PRIVATE -fno-rtti)

# Build the naming-checker standalone tool
add_executable(naming-checker src/NamingCheckerTool.cpp)
target_link_libraries(naming-checker PRIVATE ${CLANG_LIBS})

# Disable RTTI to match LLVM's build configuration
target_compile_options(naming-checker PRIVATE -fno-rtti)

# Build the for-loop-iterator standalone tool
add_executable(for-loop-iterator src/ForLoopIteratorVisitor.cpp)
target_link_libraries(for-loop-iterator PRIVATE ${CLANG_LIBS})
target_compile_options(for-loop-iterator PRIVATE -fno-rtti)

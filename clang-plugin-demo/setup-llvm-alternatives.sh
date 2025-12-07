#!/bin/bash
# Script to register LLVM/Clang 17 tools with update-alternatives
# Edit LLVM_PREFIX if your install path is different


# Require LLVM_PREFIX as the first argument, PRIORITY as optional second
if [ -z "$1" ]; then
  echo "Usage: $0 <LLVM_PREFIX> [PRIORITY]"
  echo "Example: $0 /path/to/llvm/bin 100"
  exit 1
fi
LLVM_PREFIX="$1"
if [ -n "$2" ]; then
  PRIORITY="$2"
else
  PRIORITY=100
fi
echo "Using LLVM_PREFIX: $LLVM_PREFIX"
echo "Using PRIORITY: $PRIORITY"

TOOLS=(
  clang
  clang++
  clang-cpp
  clangd
  clang-format
  clang-tidy
  clang-apply-replacements
  clang-change-namespace
  clang-doc
  clang-query
  clang-rename
  clang-reorder-fields
  clang-refactor
  clang-scan-deps
  llvm-ar
  llvm-as
  llvm-dis
  llvm-link
  llvm-nm
  llvm-objdump
  llvm-ranlib
  llvm-readelf
  llvm-readobj
  llvm-size
  llvm-strip
  llvm-symbolizer
  llvm-cov
  llvm-profdata
  llvm-mc
  llvm-config
)


for tool in "${TOOLS[@]}"; do
  if [ -x "$LLVM_PREFIX/$tool" ]; then
    sudo update-alternatives --install /usr/bin/$tool $tool $LLVM_PREFIX/$tool $PRIORITY
    echo "Registered $tool ($LLVM_PREFIX/$tool)"
  else
    echo "Warning: $tool not found in $LLVM_PREFIX"
  fi
done

echo "Done. You can now use 'sudo update-alternatives --config <tool>' to switch versions."

#!/bin/bash
# test-forloop-iterator.sh
set -e


BINARY="../build/for-loop-iterator"
TESTFILE="../tests/test-forloop.cpp"

if [ ! -x "$BINARY" ]; then
  echo "Error: $BINARY not found or not executable. Build it first."
  exit 1
fi

$BINARY $TESTFILE

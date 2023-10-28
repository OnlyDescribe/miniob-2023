#!/bin/bash
find ./src -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=file -assume-filename=./.clang-format

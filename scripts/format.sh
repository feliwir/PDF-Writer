#!/bin/bash
echo "Formatting code"
find include/ -name *.cpp -or -name *.h | xargs clang-format -i
find src/ -name *.cpp -or -name *.h | xargs clang-format -i
find tests/ -name *.cpp -or -name *.h | xargs clang-format -i
#!/bin/bash
echo "Formatting code"
find src/ -name *.cpp -or -name *.h | xargs clang-format -i
find demo/ -name *.cpp -or -name *.h | xargs clang-format -i
find tests/ -name *.cpp -or -name *.h | xargs clang-format -i
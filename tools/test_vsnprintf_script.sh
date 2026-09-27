#!/bin/bash
set -euo pipefail

g++ tools/test_vsnprintf.cpp lib/common/vsnprintf.cpp lib/common/ctype.cpp lib/common/stdlib.cpp lib/common/math.cpp -o build/test_vsnprintf -I ./ -g -Wall -Wextra
./build/test_vsnprintf
echo ""
echo "--- Result is $? ---"
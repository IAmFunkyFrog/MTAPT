#!/bin/bash

OUT=$(./build/simulator -f ./examples/code_fib.hex)
echo $OUT

if [[ $OUT -eq 8 ]]; then
    echo "Test compute_fib_5: PASSED"
else
    echo "Test compute_fib_5: FAILED"
    exit 1
fi

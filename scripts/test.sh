#!/usr/bin/env bash

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/.."

conan build . "${ROOT}" -o "&:build_tests=True"
"${ROOT}/build/Release/bin/circular_array_test"


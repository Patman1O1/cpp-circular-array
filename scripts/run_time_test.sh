#!/usr/bin/env bash

set -euo pipefail

ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)/.."

conan build "${ROOT}" --target circular_array_run_time_test -o "&:build_tests=True"
"${ROOT}/build/Release/bin/circular_array_run_time_test"


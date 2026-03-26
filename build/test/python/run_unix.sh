#!/usr/bin/env bash
set -euo pipefail

# Python test runner for Unix-like systems (FreeBSD, OpenBSD, etc.)
# Uses build_all_and_pack_unix.sh which calls build/lib/unix_like.

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${DIR}/../../.." && pwd)"
BUILD_SCRIPT="${PROJECT_ROOT}/build/wrapper/python/build_all_and_pack_unix.sh"
INSTALL_DIR="${PROJECT_ROOT}/install/wrapper_python"
TEST_MAIN="${PROJECT_ROOT}/test/python/src/bq/test/test_main.py"

COMPILER="${1:-clang}"

# If ASan is enabled, find the runtime library for LD_PRELOAD.
# When _bqlog.so is compiled with -fsanitize=address but loaded into a
# vanilla Python interpreter via dlopen, ASan symbols are missing.
# We LD_PRELOAD the ASan runtime and set verify_asan_link_order=0 to
# suppress the "ASan runtime does not come first" error.
ASAN_PRELOAD=""
BQ_ENABLE_ASAN_UPPER=""
if [ -n "${BQ_ENABLE_ASAN:-}" ]; then
    BQ_ENABLE_ASAN_UPPER="$(echo "$BQ_ENABLE_ASAN" | tr '[:lower:]' '[:upper:]')"
fi
if [ "$BQ_ENABLE_ASAN_UPPER" = "TRUE" ] || [ "$BQ_ENABLE_ASAN_UPPER" = "ON" ] || [ "$BQ_ENABLE_ASAN_UPPER" = "1" ]; then
    ASAN_LIB=""
    if command -v "${COMPILER}" >/dev/null 2>&1; then
        ASAN_LIB=$("${COMPILER}" -print-file-name=libclang_rt.asan-x86_64.so 2>/dev/null || true)
        if [ ! -f "${ASAN_LIB:-}" ]; then
            ASAN_LIB=$("${COMPILER}" -print-file-name=libclang_rt.asan.so 2>/dev/null || true)
        fi
    fi
    if [ ! -f "${ASAN_LIB:-}" ]; then
        ASAN_LIB=$(find /usr/local/lib/clang /usr/lib/clang /usr/local/llvm* 2>/dev/null \
            -name 'libclang_rt.asan*.so' -print -quit 2>/dev/null || true)
    fi
    if [ -f "${ASAN_LIB:-}" ]; then
        ASAN_PRELOAD="${ASAN_LIB}"
        export ASAN_OPTIONS="${ASAN_OPTIONS:+${ASAN_OPTIONS}:}verify_asan_link_order=0"
        echo "ASan enabled: LD_PRELOAD=${ASAN_PRELOAD}"
        echo "ASan options: ASAN_OPTIONS=${ASAN_OPTIONS}"
    else
        echo "WARNING: BQ_ENABLE_ASAN is set but ASan runtime library not found. Tests may fail."
    fi
fi

for BUILD_TYPE in Debug Release RelWithDebInfo MinSizeRel; do
    echo ""
    echo "========================================"
    echo " Testing build type: ${BUILD_TYPE}"
    echo "========================================"

    echo "----- Building wheel [${BUILD_TYPE}] -----"
    bash "${BUILD_SCRIPT}" "${BUILD_TYPE}" "${COMPILER}"

    echo "----- Installing wheel -----"
    pip3 install --force-reinstall "${INSTALL_DIR}"/*.whl

    echo "----- Running tests [${BUILD_TYPE}] -----"
    if [ -n "${ASAN_PRELOAD}" ]; then
        LD_PRELOAD="${ASAN_PRELOAD}" python3 "${TEST_MAIN}"
    else
        python3 "${TEST_MAIN}"
    fi
    echo "Tests PASSED for ${BUILD_TYPE}"
done

echo ""
echo "All Python tests PASSED!"

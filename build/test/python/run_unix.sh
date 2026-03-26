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

# If ASan is enabled, _bqlog.so is compiled with -shared-libasan so it
# dynamically links libclang_rt.asan.  We need to ensure the ASan shared
# library is findable at runtime via LD_LIBRARY_PATH (LD_PRELOAD of ASan
# into a vanilla Python interpreter causes BUS errors on FreeBSD).
ASAN_LIB_DIR=""
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
        ASAN_LIB_DIR="$(dirname "${ASAN_LIB}")"
        export LD_LIBRARY_PATH="${ASAN_LIB_DIR}${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
        echo "ASan enabled: LD_LIBRARY_PATH includes ${ASAN_LIB_DIR}"
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
    python3 "${TEST_MAIN}"
    echo "Tests PASSED for ${BUILD_TYPE}"
done

echo ""
echo "All Python tests PASSED!"

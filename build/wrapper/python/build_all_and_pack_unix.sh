#!/usr/bin/env bash
set -euo pipefail

# Build BqLog Python Wrapper on Unix-like systems (FreeBSD, OpenBSD, etc.)
# Uses build/lib/unix_like/dont_execute_this.sh for portable Unix builds.
# The resulting wheel is platform-tagged for the local OS and can be
# installed locally via: pip install ./bqlog-*.whl
#
# For Linux, use build_all_and_pack.sh instead (produces manylinux wheels).

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${DIR}/../../.." && pwd)"
BUILD_LIB_DIR="${PROJECT_ROOT}/build/lib/unix_like"
WRAPPER_DIR="${PROJECT_ROOT}/wrapper/python"
ARTIFACTS_DIR="${PROJECT_ROOT}/artifacts"
INSTALL_DIR="${PROJECT_ROOT}/install/wrapper_python"

CONFIG="${1:-RelWithDebInfo}"
COMPILER="${2:-clang}"

# ===== Step 1: Build native library =====
echo "===== Building BqLog Dynamic Library with Python Support (unix_like) [${CONFIG}] ====="
pushd "${BUILD_LIB_DIR}" >/dev/null
chmod +x dont_execute_this.sh
sh dont_execute_this.sh build native "${COMPILER}" OFF OFF ON dynamic_lib "${CONFIG}"
popd >/dev/null

# ===== Step 2: Run wrapper CMake (version sync + staging) =====
echo "===== Running wrapper/python CMake (version sync + staging) ====="
WRAPPER_BUILD_DIR="${DIR}/wrapperProj"
rm -rf "${WRAPPER_BUILD_DIR}"
mkdir -p "${WRAPPER_BUILD_DIR}"
pushd "${WRAPPER_BUILD_DIR}" >/dev/null
cmake "${WRAPPER_DIR}" -G "Unix Makefiles"
cmake --build . --target install
popd >/dev/null

# ===== Step 3: Copy native lib into install dir =====
echo "===== Copying native library to wheel source ====="
LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/${CONFIG}"
if [ ! -d "${LIB_PATH}" ]; then
    if [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/Release" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/Release"
    elif [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo"
    fi
fi

COPIED=0
for f in "${LIB_PATH}"/_bqlog*.so "${LIB_PATH}"/_bqlog*.dylib; do
    if [ -f "$f" ]; then
        cp "$f" "${INSTALL_DIR}/src/bq/"
        cp "$f" "${WRAPPER_DIR}/src/bq/"
        echo "Copied $(basename "$f")"
        COPIED=1
    fi
done
if [ "$COPIED" -eq 0 ]; then
    echo "ERROR: No native library found in ${LIB_PATH}"
    exit 1
fi

# ===== Step 4: Build wheel =====
echo "===== Building wheel package ====="
pushd "${INSTALL_DIR}" >/dev/null
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip wheel . -w "${INSTALL_DIR}"
popd >/dev/null

echo "===== Done ====="
echo "Wheel packages are in: ${INSTALL_DIR}"
ls -la "${INSTALL_DIR}"/*.whl 2>/dev/null || true

#!/usr/bin/env bash
set -euo pipefail

# Build BqLog Python Wrapper and pack into wheel
# Reference: build/wrapper/nodejs/build_all_and_pack.sh

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${DIR}/../../.." && pwd)"
WRAPPER_DIR="${PROJECT_ROOT}/wrapper/python"
INSTALL_DIR="${PROJECT_ROOT}/install/wrapper_python"

CONFIG="${1:-RelWithDebInfo}"

# Detect platform
if [[ "$(uname)" == "Darwin" ]]; then
    BUILD_LIB_DIR="${PROJECT_ROOT}/build/lib/mac"
    echo "===== Building BqLog Dynamic Library with Python Support (macOS) [${CONFIG}] ====="
    pushd "${BUILD_LIB_DIR}" >/dev/null
    zsh dont_execute_this.sh build OFF OFF ON dynamic_lib dylib "${CONFIG}"
    popd >/dev/null
else
    BUILD_LIB_DIR="${PROJECT_ROOT}/build/lib/linux"
    echo "===== Building BqLog Dynamic Library with Python Support (Linux) [${CONFIG}] ====="
    pushd "${BUILD_LIB_DIR}" >/dev/null
    bash dont_execute_this.sh build native gcc OFF OFF ON dynamic_lib "${CONFIG}"
    popd >/dev/null
fi

ARTIFACTS_DIR="${PROJECT_ROOT}/artifacts"

echo "===== Preparing wheel package ====="
mkdir -p "${INSTALL_DIR}"

LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/${CONFIG}"
if [ ! -d "${LIB_PATH}" ]; then
    if [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/Release" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/Release"
    elif [ -d "${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo" ]; then
        LIB_PATH="${ARTIFACTS_DIR}/dynamic_lib/lib/RelWithDebInfo"
    fi
fi

echo "Copying C Extension to wrapper..."
for f in "${LIB_PATH}"/_bqlog*.so "${LIB_PATH}"/_bqlog*.dylib; do
    if [ -f "$f" ]; then
        cp "$f" "${WRAPPER_DIR}/src/bq/"
        echo "Copied $(basename "$f")"
    fi
done

echo "===== Building wheel package ====="
pushd "${WRAPPER_DIR}" >/dev/null
python3 -m pip install --upgrade pip setuptools wheel
python3 -m pip wheel . -w "${INSTALL_DIR}"
popd >/dev/null

if [[ "$(uname -s)" == "Linux" ]]; then
    echo "===== Repairing Linux wheel (auditwheel) ====="
    python3 -m pip install auditwheel patchelf
    for whl in "${INSTALL_DIR}"/*.whl; do
        auditwheel repair "$whl" -w "${INSTALL_DIR}_repaired"
    done
    rm -rf "${INSTALL_DIR}"
    mv "${INSTALL_DIR}_repaired" "${INSTALL_DIR}"
fi

echo "===== Done ====="
echo "Wheel packages are in: ${INSTALL_DIR}"
ls -la "${INSTALL_DIR}"/*.whl 2>/dev/null || true

#!/usr/bin/env bash

set -e

LLVM_VERSION=${1:-"none"}

if [ "${LLVM_VERSION}" = "none" ]; then
    echo "No LLVM version specified, skipping LLVM reinstallation"
    exit 0
fi

# Cleanup temporary directory and associated files when exiting the script.
cleanup() {
    EXIT_CODE=$?
    set +e
    if [[ -n "${TMP_DIR}" ]]; then
        echo "Executing cleanup of tmp files"
        rm -Rf "${TMP_DIR}"
    fi
    exit $EXIT_CODE
}
trap cleanup EXIT


echo "Installing LLVM..."
#sudo apt-get -y purge --auto-remove clang llvm

LLVM_SCRIPT_NAME="llvm.sh"
TMP_DIR=$(mktemp -d -t llvm-XXXXXXXXXX)

echo "${TMP_DIR}"
cd "${TMP_DIR}"

curl -sSL "https://apt.llvm.org/${LLVM_SCRIPT_NAME}" -O
chmod +x "${LLVM_SCRIPT_NAME}"
bash "${TMP_DIR}/${LLVM_SCRIPT_NAME}" "${LLVM_VERSION}" all

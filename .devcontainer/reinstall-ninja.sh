#!/usr/bin/env bash
#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------
#
set -e

NINJA_VERSION=${1:-"none"}

if [ "${NINJA_VERSION}" = "none" ]; then
    echo "No Ninja version specified, skipping Ninja reinstallation"
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


echo "Installing Ninja..."
apt-get -y purge --auto-remove ninja-build
mkdir -p /opt/ninja

architecture=$(dpkg --print-architecture)
case "${architecture}" in
    arm64)
        ARCH=linux-aarch64 ;;
    amd64)
        ARCH=linux ;;
    *)
        echo "Unsupported architecture ${architecture}."
        exit 1
        ;;
esac

NINJA_BINARY_NAME="ninja-${ARCH}.zip"
TMP_DIR=$(mktemp -d -t ninja-XXXXXXXXXX)

echo "${TMP_DIR}"
cd "${TMP_DIR}"

curl -sSL "https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/${NINJA_BINARY_NAME}" -O

unzip "${TMP_DIR}/${NINJA_BINARY_NAME}" -d /opt/ninja

ln -s /opt/ninja/ninja /usr/local/bin/ninja

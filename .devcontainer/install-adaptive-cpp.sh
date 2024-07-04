set -e

ADAPTIVECPP_COMMIT=${1:-"none"}

if [ "${ADAPTIVECPP_COMMIT}" = "none" ]; then
    echo "No AdaptiveCpp commit specified, skipping AdaptiveCpp reinstallation"
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


echo "Installing AdaptiveCpp..."

INSTALL_DIR="/opt/adaptivecpp"
mkdir -p "${INSTALL_DIR}"

TMP_DIR=$(mktemp -d -t adaptivecpp-XXXXXXXXXX)

echo "${TMP_DIR}"
cd "${TMP_DIR}"

SOURCE_DIR="${TMP_DIR}/AdaptiveCpp"
git clone https://github.com/AdaptiveCpp/AdaptiveCpp.git "${SOURCE_DIR}"
cd "${SOURCE_DIR}"
git checkout "${ADAPTIVECPP_COMMIT}"

BUILD_DIR="${TMP_DIR}/build"
mkdir "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DCMAKE_BUILD_TYPE=Release -GNinja -DCMAKE_C_COMPILER=clang-18 -DCMAKE_CXX_COMPILER=clang++-18 -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm -DWITH_OPENCL_BACKEND=ON
cmake --build "${BUILD_DIR}"
cmake --install "${BUILD_DIR}"

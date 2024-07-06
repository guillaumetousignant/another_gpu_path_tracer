FROM mcr.microsoft.com/devcontainers/cpp:0-debian-11

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.29.6"
ARG INSTALL_LLVM_VERSION="18"
ARG INSTALL_ADAPTIVECPP_COMMIT="tags/v24.02.0"
ARG REINSTALL_NINJA_VERSION_FROM_SOURCE="1.12.1"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
    chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends lsb-release wget software-properties-common gnupg libboost-test-dev libboost-fiber-dev libboost-context-dev libpng-dev libtiff-dev libjpeg-dev opencl-headers mesa-opencl-icd doxygen spirv-tools spirv-headers ocl-icd-dev ocl-icd-opencl-dev

COPY ./install-llvm.sh /tmp/

RUN if [ "${INSTALL_LLVM_VERSION}" != "none" ]; then \
    chmod +x /tmp/install-llvm.sh && /tmp/install-llvm.sh ${INSTALL_LLVM_VERSION}; \
    fi \
    && rm -f /tmp/install-llvm.sh

COPY ./install-adaptive-cpp.sh /tmp/

RUN if [ "${INSTALL_ADAPTIVECPP_COMMIT}" != "none" ]; then \
    chmod +x /tmp/install-adaptive-cpp.sh && /tmp/install-adaptive-cpp.sh ${INSTALL_ADAPTIVECPP_COMMIT}; \
    fi \
    && rm -f /tmp/install-adaptive-cpp.sh

COPY ./reinstall-ninja.sh /tmp/

RUN if [ "${REINSTALL_NINJA_VERSION_FROM_SOURCE}" != "none" ]; then \
    chmod +x /tmp/reinstall-ninja.sh && /tmp/reinstall-ninja.sh ${REINSTALL_NINJA_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-ninja.sh

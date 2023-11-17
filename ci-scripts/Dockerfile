#
# build and publish the image with command:
# docker-image-build-publish.sh
#
ARG COMPILER_IMAGE
ARG LIBBOOST_VERSION=1.81

FROM gcc:12 as gcc-12
ENV CXX=g++
ENV CC=gcc
ENV GCC_VERSION=12

RUN apt-get update

FROM gcc:10 as gcc-10
ENV CXX=g++
ENV CC=gcc
ENV GCC_VERSION=10
ENV LIBBOOST_VERSION=1.74

FROM gcc:11 as gcc-11
ENV CXX=g++
ENV CC=gcc
ENV GCC_VERSION=11
ENV LIBBOOST_VERSION=1.74

RUN apt-get update

FROM debian:latest as clang-13
ENV CXX=clang++
ENV CC=clang
ENV CLANG_VERSION=13
ENV LIBBOOST_VERSION=${LIBBOOST_VERSION}

RUN apt-get update && \
    apt-get install -y clang-${CLANG_VERSION} && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${CLANG_VERSION} 1 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${CLANG_VERSION} 1

FROM ${COMPILER_IMAGE} as target-image
ARG CATCH2_REV=v3.2.0
ARG CATCH2_REPO=https://github.com/catchorg/Catch2.git
ARG COMPILER_IMAGE=${COMPILER_IMAGE}

ENV TCHECKER_DOCKER=true
ENV TCHECKER_DOCKER_IMAGE="${COMPILER_IMAGE}"

RUN apt-get update && \
    apt-get install -y \
    git cmake bison flex doxygen \
    libboost${LIBBOOST_VERSION}-dev  \
    valgrind graphviz

RUN if test $(echo  "${LIBBOOST_VERSION}" | tr -d '.') -ge 175; then \
      apt-get install -y libboost-json${LIBBOOST_VERSION}-dev; \
    fi

RUN git clone --branch ${CATCH2_REV} ${CATCH2_REPO} /tmp/catch2

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=${CXX} .. && \
    make -j 4 && \
    make install

ENTRYPOINT [ "/bin/bash", "-s" ]

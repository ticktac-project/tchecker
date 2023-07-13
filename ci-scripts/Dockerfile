#
# build and publish the image with command:
# docker-image-build-publish.sh
#
FROM debian:latest as base
ARG CATCH2_REV=v3.2.0
ARG CATCH2_REPO=https://github.com/catchorg/Catch2.git
ARG LIBBOOST_VERSION=1.81
ARG CXX

RUN apt-get update && \
    apt-get install -y \
    git cmake bison flex doxygen \
    libboost${LIBBOOST_VERSION}-dev  \
    libboost-json${LIBBOOST_VERSION}-dev \
    valgrind graphviz

RUN git clone --branch ${CATCH2_REV} ${CATCH2_REPO} /tmp/catch2

FROM base as gcc-image
RUN apt-get install -y gcc-12 g++-12 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 1 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 1

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=g++ .. && \
    make -j 4 && \
    make install

ENTRYPOINT [ "/bin/bash", "-s" ]

FROM base as clang-image

RUN apt-get install -y clang-13 && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-13 1 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-13 1

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=clang++ .. && \
    make -j 4 && \
    make install

ENTRYPOINT [ "/bin/bash", "-s" ]

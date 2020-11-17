#
# build and publish the image with command:
# docker-image-build-publish.sh
#
ARG IMAGE

FROM ${IMAGE}
ARG CATCH2_REV=v2.13.1
ARG CATCH2_REPO=https://github.com/catchorg/Catch2.git
ARG DEBPKG=""
ARG CXX

RUN apt-get update && \
    apt-get install -y \
       ${DEBPKG} git cmake bison flex doxygen libboost1.67-dev python \
       valgrind graphviz

RUN git clone --branch ${CATCH2_REV} ${CATCH2_REPO} /tmp/catch2

RUN mkdir -p /tmp/catch2/build && \
    cd /tmp/catch2/build && \
    cmake -DCMAKE_CXX_COMPILER=${CXX} .. && \
    make -j 4 && \
    make install

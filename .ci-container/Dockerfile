FROM ubuntu:22.04

WORKDIR /work

RUN apt-get update

RUN apt-get install -y \
    cmake \
    clang-14 \
    clang++-14 \
    git

ENV CC="/usr/bin/clang-14" CXX="/usr/bin/clang++-14"

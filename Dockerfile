FROM ubuntu:latest
LABEL authors="bwilkins"

RUN apt-get -y update && \
    apt-get -y install build-essential rsync && \
    mkdir color-converter

COPY ./apdfl /color-converter/apdfl
COPY ./ColorConverter /color-converter/ColorConverter
COPY ./Makefile /color-converter/Makefile


WORKDIR /color-converter

RUN make clean && make
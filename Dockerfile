FROM ubuntu:latest
LABEL authors="bwilkins"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get -y install build-essential rsync && \
    mkdir color-converter

COPY ./apdfl /color-converter/apdfl
COPY ./ColorConverter /color-converter/ColorConverter
COPY ./Makefile /color-converter/Makefile


WORKDIR /color-converter

RUN make clean && make

COPY fixtures/SVG_Test_input.pdf .

ENV LD_LIBRARY_PATH "/color-converter/ColorConverter/bin/Binaries/:/lib:/lib64:${LD_LIBRARY_PATH}"

RUN ColorConverter/bin/ColorConverter SVG_Test_input.pdf SVG_Test_output.pdf
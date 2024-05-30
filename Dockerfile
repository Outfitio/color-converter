FROM ubuntu:latest as build

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get -y install build-essential rsync && \
    mkdir color-converter

COPY ./apdfl /color-converter/apdfl
COPY ./ColorConverter /color-converter/ColorConverter
COPY ./Makefile /color-converter/Makefile


WORKDIR /color-converter

RUN make clean && make

FROM ubuntu:latest
LABEL authors="Brett Wilkins <brett.wilkins@smartsheet.com>"


COPY --from=build /color-converter/ColorConverter/bin /color-converter/bin
WORKDIR /color-converter

ENV LD_LIBRARY_PATH "/color-converter/bin/Binaries/:/lib:/lib64:${LD_LIBRARY_PATH}"

ENTRYPOINT ["/color-converter/bin/ColorConverter"]
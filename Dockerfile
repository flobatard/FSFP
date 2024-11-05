FROM ubuntu:24.04

RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC 
RUN apt-get update;
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y libboost-all-dev build-essential cmake git openssh-server systemd-coredump pkg-config

WORKDIR /crow
RUN git clone https://github.com/CrowCpp/Crow.git


WORKDIR /lmdb
RUN git clone https://github.com/LMDB/lmdb.git
RUN cd lmdb/libraries/liblmdb; make;

WORKDIR /service
RUN mkdir -p /service/include
RUN mkdir -p /service/lib
RUN mkdir -p /service/obj
RUN mkdir -p /service/bin
RUN mkdir -p /service/data
RUN cp /lmdb/lmdb/libraries/liblmdb/lmdb.h /service/include/lmdb.h
RUN cp /lmdb/lmdb/libraries/liblmdb/liblmdb.a /service/lib/liblmdb.a
RUN cp -r /crow/Crow/include/* /service/include/

WORKDIR /service
COPY . /service


WORKDIR /service
RUN make

EXPOSE 8000 8000

ENTRYPOINT ["bin/main.exe"]
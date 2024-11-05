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
COPY . /service
RUN cp /lmdb/lmdb/libraries/liblmdb/lmdb.h /service/include/lmdb.h
RUN cp /lmdb/lmdb/libraries/liblmdb/liblmdb.a /service/lib/liblmdb.a
RUN cp -r /crow/Crow/include/* /service/include/

WORKDIR /service
RUN ls
RUN ls lib
RUN ls include
RUN make

EXPOSE 8000 8000

ENTRYPOINT ["bin/main.exe"]
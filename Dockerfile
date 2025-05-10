FROM debian:bookworm-slim

RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC 
RUN apt-get update
RUN apt-get upgrade

RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y build-essential
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y cmake
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y git
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y libboost-all-dev openssh-server systemd-coredump pkg-config
RUN apt-get install -y libssl-dev
RUN apt-get install -y git-all


WORKDIR /crow
RUN git clone https://github.com/CrowCpp/Crow.git


WORKDIR /lmdb
RUN git clone https://github.com/LMDB/lmdb.git
RUN cd lmdb/libraries/liblmdb; make;

WORKDIR /jwt-cpp
RUN git clone https://github.com/Thalhammer/jwt-cpp.git
RUN cd jwt-cpp; mkdir build; cd build; cmake ..; make && make install

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

EXPOSE 5000 5000

ENTRYPOINT ["bin/main.exe"]
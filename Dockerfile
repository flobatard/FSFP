FROM debian:stable-slim

# Problem if apt-get update is not run on the same line as apt-get
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get update -y \
    && apt-get install -y build-essential \
    && apt-get install -y git \
    && apt-get install -y cmake \
    && apt-get install -y libboost-all-dev openssh-server systemd-coredump pkg-config \
    && apt-get install -y libssl-dev \
    && apt-get install -y git-all


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
RUN make dirs
RUN make -j4

EXPOSE 5000 5000

ENTRYPOINT ["bin/main.exe"]
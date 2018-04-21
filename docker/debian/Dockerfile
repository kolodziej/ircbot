FROM debian:9.3
MAINTAINER Kacper Kołodziej <kacper@kolodziej.it>

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y build-essential cmake libboost-all-dev doxygen graphviz \
                       devscripts debhelper libprotobuf-dev protobuf-compiler

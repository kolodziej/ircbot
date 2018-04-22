FROM arm32v7/ubuntu:16.04
MAINTAINER Kacper Kołodziej <kacper@kolodziej.it>

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y build-essential cmake libboost-all-dev doxygen graphviz \
                       devscripts debhelper libprotobuf-dev protobuf-compiler \
                       python-dev python3-dev

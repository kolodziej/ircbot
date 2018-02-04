FROM ubuntu:16.04
MAINTAINER Kacper Kołodziej <kacper@kolodziej.it>

VOLUME "/ircbot"
WORKDIR "/ircbot"

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y cmake build-essential libboost-all-dev

ENTRYPOINT mkdir build && cd build && cmake -G 'Unix Makefiles' .. && make

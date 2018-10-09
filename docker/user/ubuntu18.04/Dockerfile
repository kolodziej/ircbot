FROM ubuntu:18.04
MAINTAINER Kacper Kołodziej <kacper@kolodziej.it>

ADD packages/ubuntu/*.deb /packages

RUN dpkg --install /packages/ircbot*amd64.deb
RUN apt-get install -f

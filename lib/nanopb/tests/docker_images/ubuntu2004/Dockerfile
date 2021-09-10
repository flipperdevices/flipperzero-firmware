FROM ubuntu:focal

RUN apt -y update
RUN apt -y upgrade
RUN apt -y dist-upgrade
RUN apt -y autoremove
RUN apt -y install --fix-missing
RUN apt -y install apt-utils

RUN apt -y install git scons build-essential g++
RUN apt -y install protobuf-compiler python3.8 python3-protobuf
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.8 1 && update-alternatives --set python /usr/bin/python3.8

RUN git clone https://github.com/nanopb/nanopb.git
RUN cd nanopb/tests && scons


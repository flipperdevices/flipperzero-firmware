# syntax=docker/dockerfile-upstream:master-labs
FROM python:3.10 as builder

COPY . /build

WORKDIR /build

RUN pip3 install -r requirements-headless.txt

CMD pytest -v --gui=0 --port=/dev/ttyACM0 -m smoke

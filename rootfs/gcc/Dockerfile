# Dockerfile for gcc/g++ environment for Codu Grading system.
# Current Version is 1.0
# To build a docker image, run
#   docker build --tag codu-online-judge-gcc:${VERSION} .

FROM codu-online-judge-base:18.04

RUN apt-get update
RUN apt-get install -y gcc
RUN apt-get install -y swig

# Remove apt storage.
RUN rm -rf /var/lib/apt/lists/*

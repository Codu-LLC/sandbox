# Dockerfile for Java environment for Codu Grading system.
# Current Version is 1.0
# To build a docker image, run
#   docker build --tag codu-online-judge-java:${VERSION} .

FROM codu-online-judge-base:18.04

RUN apt-get update --fix-missing
RUN apt-get install -y default-jdk

# Remove apt storage.
RUN rm -rf /var/lib/apt/lists/*

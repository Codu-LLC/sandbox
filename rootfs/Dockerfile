# Dockerfile for basic environment for Codu Grading system.
# To build a docker image, run
#   docker build --tag codu-online-judge-base:${ubuntu_version} .

FROM ubuntu:18.04

# Install cgroup
RUN apt-get update
RUN apt-get install -y cgroup-bin cgroup-lite libcgroup1

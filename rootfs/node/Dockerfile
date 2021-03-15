# Dockerfile for Node.js environment for Codu Grading system.
# Current Version is 1.0
# To build a docker image, run
#   docker build --tag codu-online-judge-node:${VERSION} .

FROM codu-online-judge-base:18.04

RUN apt-get update
RUN apt-get install -y curl
RUN curl -fsSL https://deb.nodesource.com/setup_15.x | bash -
RUN apt-get install -y nodejs
RUN npm install typescript -g
RUN mkdir sandbox && cd sandbox && npm install @types/node --save-dev

# Remove apt storage.
RUN rm -rf /var/lib/apt/lists/*

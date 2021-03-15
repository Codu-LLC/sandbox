# When updating this version, also update version in extract_rootfs.sh as well.
UBUNTU_VERSION=18.04
GCC_IMAGE_VERSION=1.0
PY_IMAGE_VERSION=1.0
JAVA_IMAGE_VERSION=1.0
NODE_IMAGE_VERSION=1.0

docker build --tag codu-online-judge-base:${UBUNTU_VERSION} .
docker build --tag codu-online-judge-gcc:${GCC_IMAGE_VERSION} -f gcc/Dockerfile .
docker build --tag codu-online-judge-py:${PY_IMAGE_VERSION} -f py/Dockerfile .
docker build --tag codu-online-judge-java:${JAVA_IMAGE_VERSION} -f java/Dockerfile .
docker build --tag codu-online-judge-node:${NODE_IMAGE_VERSION} -f node/Dockerfile .

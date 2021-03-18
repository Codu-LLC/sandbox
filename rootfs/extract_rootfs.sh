#!/bin/bash
ROOT_DIR=/mnt
if [[ $1 ]]; then
	ROOT_DIR=$1
fi
PY_ROOTFS=${ROOT_DIR}/pyrootfs
GCC_ROOTFS=${ROOT_DIR}/gccrootfs
JAVA_ROOTFS=${ROOT_DIR}/javarootfs
NODE_ROOTFS=${ROOT_DIR}/noderootfs

# When updating this version, also update version in generate_images.sh as well.
PY_IMAGE_VERSION=1.0
GCC_IMAGE_VERSION=1.0
JAVA_IMAGE_VERSION=1.0
NODE_IMAGE_VERSION=1.0

mkdir -p ${PY_ROOTFS}/sandbox
mkdir -p ${GCC_ROOTFS}/sandbox
mkdir -p ${JAVA_ROOTFS}/sandbox
mkdir -p ${NODE_ROOTFS}/sandbox

sudo docker export $(sudo docker create codu-online-judge-gcc:${GCC_IMAGE_VERSION}) | tar -C ${GCC_ROOTFS} -xvf -
sudo docker export $(sudo docker create codu-online-judge-py:${PY_IMAGE_VERSION}) | tar -C ${PY_ROOTFS} -xvf -
sudo docker export $(sudo docker create codu-online-judge-java:${JAVA_IMAGE_VERSION}) | tar -C ${JAVA_ROOTFS} -xvf -
sudo docker export $(sudo docker create codu-online-judge-node:${NODE_IMAGE_VERSION}) | tar -C ${NODE_ROOTFS} -xvf -

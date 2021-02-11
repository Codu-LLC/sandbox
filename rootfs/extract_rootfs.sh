PY_ROOTFS=/mnt/pyrootfs
GCC_ROOTFS=/mnt/gccrootfs

# When updating this version, also update version in generate_images.sh as well.
PY_IMAGE_VERSION=1.0
GCC_IMAGE_VERSION=1.0

mkdir -p ${PY_ROOTFS}/sandbox
mkdir -p ${GCC_ROOTFS}/sandbox

sudo docker export $(sudo docker create codu-online-judge-gcc:${GCC_IMAGE_VERSION}) | tar -C ${GCC_ROOTFS} -xvf -
sudo docker export $(sudo docker create codu-online-judge-py:${PY_IMAGE_VERSION}) | tar -C ${PY_ROOTFS} -xvf -

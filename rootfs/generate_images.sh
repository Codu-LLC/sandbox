# When updating this version, also update version in extract_rootfs.sh as well.
UBUNTU_VERSION=18.04
GCC_IMAGE_VERSION=1.0
PY_IMAGE_VERSION=1.0

sudo docker build --tag codu-online-judge-base:${UBUNTU_VERSION} .
sudo docker build --tag codu-online-judge-gcc:${GCC_IMAGE_VERSION} -f gcc/Dockerfile .
sudo docker build --tag codu-online-judge-py:${PY_IMAGE_VERSION} -f py/Dockerfile .

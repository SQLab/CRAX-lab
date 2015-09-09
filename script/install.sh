#!/bin/sh

apt-get install -y build-essential
apt-get install -y subversion
apt-get install -y git
apt-get install -y gettext
apt-get install -y liblua5.1-dev
apt-get install -y libsdl1.2-dev
apt-get install -y libsigc++-2.0-dev
apt-get install -y binutils-dev
apt-get install -y python-docutils
apt-get install -y python-pygments
apt-get install -y nasm

apt-get build-dep llvm-3.0
apt-get build-dep qemu

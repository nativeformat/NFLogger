#!/bin/bash
# Copyright (c) 2018 Spotify AB.
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

# Exit on any non-zero status
set -e
set -x

# Install system dependencies
apt-get -q update
apt-get install -y -q --no-install-recommends apt-utils \
  clang-format \
  clang \
  ninja-build \
  libc++-dev \
  libc++abi-dev \
  cmake \
  python-pip \
  python-dev \
  python3-dev \
  python-virtualenv \
  software-properties-common \
  build-essential \
  git \
  virtualenv

# Update submodules
git submodule sync
git submodule update --init --recursive

export CC=clang
export CXX=clang++

# Install virtualenv
virtualenv --python=$(which python2) nflogger_env
. nflogger_env/bin/activate

# Install Python Packages
pip install six
# pip install --upgrade setuptools pip
pip install -r ${PWD}/ci/requirements.txt

# Execute our python build tools
python ci/linux.py "$@"

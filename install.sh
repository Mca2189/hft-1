#!/bin/bash

yum -y install epel-release
yum install -y python-devel python-pip zlib-devel zlib libconfig libconfig-devel python-devel
yum install -y boost169-python3 boost169-python3-devel
pip install future

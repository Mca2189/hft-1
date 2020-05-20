#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

cd /today
gzip future*.dat

cd /root/huatai/build/
gzip stock*.dat

cd /today/log

gzip data.log
gzip data_night.log

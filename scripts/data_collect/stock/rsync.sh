#!/bin/bash

set -e
date_string=`date  +"%Y-%m-%d" -d  "-0 days"`
if [ $# -gt 0 ];
then
    date_string=$1
fi

echo "rsync"$date_string
ulimit -n 65536
rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@139.196.204.22:/root/huatai/build/stocktick$date_string.dat.gz /home/nick/stock
ssh -i /root/.ssh/ali_key root@139.196.204.22 "rm /root/huatai/build/stocktick$date_string.dat.gz"

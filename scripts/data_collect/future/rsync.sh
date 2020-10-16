#!/bin/bash

set -e
date_string=`date  +"%Y-%m-%d" -d  "-1 days"`
if [ $# -gt 0 ];
then
    date_string=$1
fi
ulimit -n 65536

echo "rsync"$date_string
rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@139.196.204.22:/running/$date_string/future$date_string.dat.gz /home/nick/future
ssh -i /root/.ssh/ali_key root@139.196.204.22 "rm /running/$yes/future$yes.dat.gz"
ssh -i /root/.ssh/ali_key root@139.196.204.22 "rm /running/$yes/log/data.log*"

#!/bin/bash

set -e
date_string=`date  +"%Y-%m-%d" -d  "-1 days"`
if [ $# -gt 0 ];
then
	date_string=$1
fi
ulimit -n 65536

echo "rsync"$date_string
rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/HUOBIcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@54.95.226.129 "rm /running/coin/HUOBIcointick$date_string.dat.gz"


rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/BITMEXcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@54.95.226.129 "rm /running/coin/BITMEXcointick$date_string.dat.gz"

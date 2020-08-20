#!/bin/bash

set -e
date_string=`date  +"%Y-%m-%d" -d  "-1 days"`
ulimit -n 65536
rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@8.210.24.90:/running/coin/OKEXcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@8.210.24.90 "rm /running/coin/OKEXcointick$date_string.dat.gz"

rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@8.210.24.90:/running/coin/BINANCEcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@8.210.24.90 "rm /running/coin/BINANCEcointick$date_string.dat.gz"

rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/HUOBIcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@54.95.226.129 "rm /running/coin/HUOBIcointick$date_string.dat.gz"


rsync -avz -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/BITMEXcointick$date_string.dat.gz /home/nick/coin
ssh -i /root/.ssh/ali_key root@54.95.226.129 "rm /running/coin/BITMEXcointick$date_string.dat.gz"

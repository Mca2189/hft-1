#!/bin/bash                                                      
ntpdate ntp1.aliyun.com > /dev/null 2>&1; /sbin/hwclock -w

cd /running/csv
find . -name "*.csv" | xargs gzip

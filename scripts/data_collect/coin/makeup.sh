#!/bin/sh
if [ $# == 2 ]; then
    datebeg=$1
    dateend=$2
else
    echo "请输入开始时间和结束日期，格式为2017-04-04"
    exit 1
fi

beg_s=`date -d "$datebeg" +%s`
end_s=`date -d "$dateend" +%s`

echo "处理时间范围：$beg_s 至 $end_s"

while [ "$beg_s" -le "$end_s" ];do
    date_string=`date -d @$beg_s +"%Y-%m-%d"`;
    echo "当前日期：$date_string"
    beg_s=$((beg_s+86400));
    ulimit -n 65536
	rsync -avz --ignore-existing -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/OKEXcointick$date_string.dat.gz /home/nick/coin
	rsync -avz --ignore-existing -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/BINANCEcointick$date_string.dat.gz /home/nick/coin
	rsync -avz --ignore-existing -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/BITMEXcointick$date_string.dat.gz /home/nick/coin
	rsync -avz --ignore-existing -e "ssh -i /root/.ssh/ali_key" -r root@54.95.226.129:/running/coin/HUOBIcointick$date_string.dat.gz /home/nick/coin
	ssh -i /root/.ssh/ali_key root@54.95.226.129 "rm /running/coin/HUOBIcointick$date_string.dat.gz; rm /running/coin/OKEXcointick$date_string.dat.gz; rm /running/coin/BINANCEcointick$date_string.dat.gz; rm /running/coin/BITMEXcointick$date_string.dat.gz"
done

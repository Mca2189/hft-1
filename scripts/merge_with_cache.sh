#!/bin/bash

rsync -avz -e "ssh -i /root/.ssh/ali_key -p 18301"  -r xhuang@180.166.2.50:~/quant/tools/read/read_data/ /running/csv/

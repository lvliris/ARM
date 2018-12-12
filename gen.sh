#!/bin/bash
#make
#cp master /home/topeet/minLinux/system/usr/master/
python gen_log.py
rm config/state.json
./master

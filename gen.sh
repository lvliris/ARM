#!/bin/bash
#make
#cp master /home/topeet/minLinux/system/usr/master/
python gen_log.py --num_dev 100 \
 --num_mode 10 --num_habit 4 \
 --mode_len 8 --habit_var 1 \
 --noise_act 0

rm config/state.json
./master 10 4

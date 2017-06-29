#!/bin/bash

## add this to crontab
## * * * * * /home/barnabas/dev/rec.sh

## 
raspistill -t 59000 -tl 1000 -o ./images/%03d.jpg

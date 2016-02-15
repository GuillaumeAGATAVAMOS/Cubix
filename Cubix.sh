#!/bin/bash

export PATH=$PATH:/usr/local/sbin
export PATH=$PATH:/usr/local/bin
export PATH=$PATH:/usr/sbin
export PATH=$PATH:/sbin
export PATH=$PATH:/usr/games

//LoadGW e680
source /data/users/soft/scripts/LoadGw.sh

chmod +x Cubix.txt
chmod 777 Cubix.txt

cd /agataDAS1/AGATA/Experiments/e680/Analysis/Users/Gui/Calibrations/ROOT_CUBE


root -l  Load.cxx StartTools.C

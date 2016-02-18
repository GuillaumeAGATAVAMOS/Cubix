#!/bin/bash

export PATH=$PATH:/usr/local/sbin
export PATH=$PATH:/usr/local/bin
export PATH=$PATH:/usr/sbin
export PATH=$PATH:/sbin
export PATH=$PATH:/usr/games

#LoadGW e680

chmod +x Cubix.C
chmod 777 Cubix.C

cd /agataDAS1/AGATA/Experiments/e680/Analysis/Users/Gui/Calibrations/ROOT_CUBE/Cubix_Software



root -l Cubix.C\(''$1''\) 
#optional StartTools.C

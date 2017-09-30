#!/bin/bash

if hash mencoder 2>/dev/null; then
    
  opt="vbitrate=2160000:mbd=2:keyint=132:v4mv:vqmin=3:lumi_mask=0.07:dark_mask=0.2:mpeg_quant:scplx_mask=0.1:tcplx_mask=0.1:naq"
  mencoder -ovc lavc -lavcopts vcodec=mpeg4:vpass=1:$opt -mf type=png:fps=25 -nosound -o /dev/null mf://\*.png
  mencoder -ovc lavc -lavcopts vcodec=mpeg4:vpass=2:$opt -mf type=png:fps=25 -nosound -o $1 mf://\*.png

else
  echo "mencoder binary not found, aborting ..."
fi

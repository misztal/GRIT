#!/bin/bash

# Script to run all the matlab-scripts producing images

for file in magnostatic_*.m; do
    echo $file;
    matlab -nodisplay -nodesktop -nosplash -r "run('./$file');exit;"
done

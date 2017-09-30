#!/bin/bash

# Usage:
# ./make-gif.sh <demo name>
#   OR
# Example:
# ./make-gif.sh fracture

#2016-02-19 Kenny code review: The "??????" is a little bad design. Why not just use *.svg or use a $1 and $2 then end-user could specify xxx_?????? as input for $1?
#2016-02-19 Tobias: *.svg might produce a gif with frames for multiple demos combining e.g. conforming and fracture, but we could make it up
#                   up to end-user to be intelligent? I dislike adding too many arguments to the script call, it'll end up being almost as long as the command itself.


convert -delay 25 -loop 0 $1_*.svg $1.gif

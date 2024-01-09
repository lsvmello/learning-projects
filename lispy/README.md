# Prerequisites

- Install `editline`: `sudo apt-get install libedit-dev`

# Compiling

`cc -std=c99 -Wall -ledit -lm lispy.c -o lispy`

# Running

- Interactively: `./lispy`
- Running a script: `./lispy stdlib.lspy example.lspy`

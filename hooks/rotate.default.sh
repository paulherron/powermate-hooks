#!/bin/sh

# Set the volume using alsa to the percentage specified in the first argument

# Raspberry Pi
amixer set PCM $1%

# Other Linux (e.g. Manjaro)
#amixer set Master $1%

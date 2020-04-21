#!/bin/sh

# Set the volume using alsa to the percentage specified in the first argument
amixer set Master $1%

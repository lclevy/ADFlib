#!/bin/bash

#
# Quick-build helper script
#
# 1. creates quil3 source archive
# 2. executes automated build  of .deb
#
# !!! Execute in the main directory !!
#

# create source archive
# (make sure no extra files are there!)
util/create_quilt3_archive.sh

# build .deb
debuild -us -uc


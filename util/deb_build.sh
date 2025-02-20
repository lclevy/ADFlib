#!/bin/bash

#
# Helper script to build deb packages
#
# 1. prepares (copies) deb packaging configuration (debian/)
# 2. creates quilt3 source archive
# 3. executes automated build of .deb packages
#
# !!! Execute in the main directory !!
#

if [ "${0}" != "util/deb_build.sh" ]
then
    echo "The script must be executed in the main project directory - aborting..."
    exit 1
fi

if [ -d debian ]
then
   echo "Using packaging configuration from the existing debian/ directory"
else
    if [ ! -d packaging/debian ]
    then
	echo "Missing debian packaging directory/files (packaging/debian) - aborting..."
	exit 1
    fi
    cp -aRpv packaging/debian .
fi

if [ ! -d debian ]
then
    echo "Missing debian packaging directory/files (debian/) - aborting..."
    exit 1
fi

# create source archive
# (make sure no extra files are there!)
util/deb_create_quilt3_archive.sh

# build .deb
debuild -us -uc

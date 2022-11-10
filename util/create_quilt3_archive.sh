#!/bin/bash

#
# Create tar.gz archive (quilt3) from current project
# (usually needed for Debian build tools like debuild)
#
# !!! Execute in the main directory !!
#

# validate if extracted string is a valid version!
# TO ADD
#check_version()
#{
# ...
#}


VERSION_DEBIAN=`grep -e '^[a-z]' debian/changelog \
            | cut -d'(' -f 2 | cut -d')' -f 1`
VERSION=`echo $VERSION_DEBIAN | cut -d'-' -f 1`

SRC_ARCHIVE="../libadf_${VERSION}.orig.tar.gz"
echo -e "\nCreating source archive:\n" \
     "  version: $VERSION\n" \
     "  archive: $SRC_ARCHIVE\n"

#check_version $VERSION

tar cvzf $SRC_ARCHIVE \
    `ls | grep -v "^\.git$" | grep -v debian`
#    .gitlab-ci.yml

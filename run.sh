#!/bin/sh

PWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SFML="SFML/lib/"

export PATH=$PATH:$PWD/SFML/extlibs/libs-osx/Frameworks
export DYLD_LIBRARY_PATH=$PWD/$SFML:$DYLD_LIBRARY_PATH
echo $DYLD_LIBRARY_PATH

./visualishader $*

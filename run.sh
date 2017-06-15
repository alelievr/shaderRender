#!/bin/sh

PWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SFML="SFML/lib/"
FLAC="flac/src/libFLAC/.libs/"
FREETYPE="freetype2/objs/.libs"
OGG="ogg/src/.libs/"

export DYLD_LIBRARY_PATH=$PWD/$SFML:$PWD/$FLAC:$PWD/$FREETYPE:$PWD/$OGG
echo $DYLD_LIBRARY_PATH

./visualishader $*

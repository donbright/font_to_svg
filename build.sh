#/bin/sh

if [ "`command -v clang++`" ]; then
  CC=clang++
else
  CC=g++
fi

WARN="-pedantic -Wall"
FREETYPE_FLAGS=`freetype-config --cflags --libs`
SOURCE_FILES="example1 example2 example3"

for sourcefile in $SOURCE_FILES;
  do $CC $WARN $sourcefile".cpp" -o $sourcefile $FREETYPE_FLAGS
done



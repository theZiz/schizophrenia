#!/bin/sh
# Change the folder to YOUR sparrow3d folder!
PROGRAM="schizophrenia"
VERSION="1.1.0.0"
SOURCE="./testing"
DEST=./build/*
echo "<html>" > index.htm
echo "<head>" >> index.htm
echo "</head>" >> index.htm
echo "<body>" >> index.htm
echo "<h1>$PROGRAM download links:</h1>" >> index.htm
for f in $DEST
do
  if [ -e "$f/$PROGRAM/$PROGRAM" ]; then
    NAME=`echo "$f" | cut -d/ -f3 | cut -d. -f1`
    echo "$NAME:"
    echo "--> Copy temporary folders"
    cp -r "$SOURCE/data" "$f/$PROGRAM"
    cp -r "$SOURCE/level" "$f/$PROGRAM"
    cp -r "$SOURCE/font" "$f/$PROGRAM"
    cp -r "$SOURCE/images" "$f/$PROGRAM"
    cp -r "$SOURCE/sprites" "$f/$PROGRAM"
    cd $f
    echo "--> Create archive"
    if [ $NAME = "pandora" ]; then
      cd $PROGRAM
      ../make_package.sh
      cd ..
		  echo "<a href=$PROGRAM.pnd>$NAME</a></br>" >> ../../index.htm
    else
      if [ $NAME = "i386" ]; then
        tar cfvz "$PROGRAM-$NAME-$VERSION.tar.gz" $PROGRAM > /dev/null
        mv "$PROGRAM-$NAME-$VERSION.tar.gz" ../..
  		  echo "<a href=$PROGRAM-$NAME-$VERSION.tar.gz>$NAME</a></br>" >> ../../index.htm
      else
        zip -r "$PROGRAM-$NAME-$VERSION.zip" $PROGRAM > /dev/null
        mv "$PROGRAM-$NAME-$VERSION.zip" ../..
  		  echo "<a href=$PROGRAM-$NAME-$VERSION.zip>$NAME</a></br>" >> ../../index.htm
      fi
    fi
    echo "--> Remove temporary folders"
    rm -r $PROGRAM/data
    rm -r $PROGRAM/level
    rm -r $PROGRAM/font
    rm -r $PROGRAM/images
    rm -r $PROGRAM/sprites
    cd ..
    cd ..
  fi
done
echo "</body>" >> index.htm
echo "</html>" >> index.htm

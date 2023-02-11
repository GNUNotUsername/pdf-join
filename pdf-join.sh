#!/bin/bash
TMP=($(echo $RANDOM | md5sum));
TEMPDIR=$PWD/$TMP;
mkdir $TEMPDIR;
FAIL=0;
for FILE in $@
do
    cp $FILE $TEMPDIR 2> /dev/null;
    FAIL=$?;
    if [ $FAIL != 0 ] ; then
        cp $FILE.pdf $TEMPDIR 2> /dev/null;
        FAIL=$?;
        if [ $FAIL != 0 ]; then
            break
        fi
    fi
done
if [ $FAIL == 0 ]; then
    ./template $TEMPDIR $TEMPDIR/*.pdf;
    cd $TEMPDIR;
    pdflatex template.tex > /dev/null;
    cp template.pdf ../joined.pdf;
    cd ..;
fi
rm -rf $TEMPDIR;

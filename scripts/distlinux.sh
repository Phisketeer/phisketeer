# !/usr/bin/sh

./scripts/targz.sh $1 $2 $3 $4 $5
DESTDIR=phisketeer
rm -rf $DESTDIR-$1
rm -rf $DESTDIR
mkdir -p $DESTDIR
mv phisketeer-$1.tar.gz $DESTDIR/phisketeer.tar.gz
echo $1 >> $DESTDIR/version
cp scripts/linuxinstaller.sh $DESTDIR/
echo "Creating installer"
cd $DESTDIR
tar -cvf ../$DESTDIR.tar ./*
cd ..
gzip $DESTDIR.tar
cat scripts/linux.run $DESTDIR.tar.gz > phisketeer-$1.bin
chmod +x phisketeer-$1.bin
rm -rf $DESTDIR
rm -f $DESTDIR.tar.gz
echo "Done"
exit 0

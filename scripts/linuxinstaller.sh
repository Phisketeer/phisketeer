# !/usr/bin/sh
TMPDIR=$1
PHIVERSION=$2

echo "Starting Phisketeer $PHIVERSION installer..."
if [ `id -u` -gt 0 ] ; then
  echo "You must be root for installing Phisketeer!"
  echo "Use for example 'sudo ./phisketeer-$PHIVERSION.bin'."
  exit 0
fi
echo "Decompressing Phisketeer $PHIVERSION archive"
tar -zxf phisketeer.tar.gz
cd phisketeer-$PHIVERSION
PHILIBPATH=`pwd`/lib
PHIPLUGINS=`pwd`/plugins
QT_PLUGIN_PATH=$PHIPLUGINS
LD_LIBRARY_PATH=$PHILIBPATH:$LD_LIBRARY_PATH
/usr/bin/xhost si:localuser:root
export LD_LIBRARY_PATH
export QT_PLUGIN_PATH
echo "Starting GUI installer"
./bin/phiinstaller
exit 0

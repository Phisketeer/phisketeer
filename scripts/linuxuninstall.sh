# !/usr/bin/sh
PHIROOT=%phiroot%
PHIVERSION=%phiversion%

echo "Starting PhiSuite $PHIVERSION uninstaller..."
if [ `id -u` -gt 0 ] ; then
  echo "You must be root for uninstalling the PhiSuite!"
  echo "Use for example 'sudo ./phiuninstall.sh'."
  exit 0
fi
read -p "Are you sure to remove the PhiSuite $PHIVERSION from local disk [y|n]? " INPUT
if [ $INPUT != "y" ] ; then 
	echo "PhiSuite uninstall aborted."
	exit 0
fi
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PHIROOT/lib
export LD_LIBRARY_PATH
$PHIROOT/bin/phis -t

rm -f /etc/rc0.d/K10phis
rm -f /etc/rc1.d/K10phis
rm -f /etc/rc2.d/S90phis
rm -f /etc/rc3.d/S90phis
rm -f /etc/rc4.d/S90phis
rm -f /etc/rc5.d/S90phis
rm -f /etc/rc6.d/K10phis
rm -f /etc/init.d/phis
rm -rf /etc/xdg/Phisys
rm -rf $PHIROOT/bin
rm -rf $PHIROOT/lib
rm -rf $PHIROOT/plugins
rm -f $PHIROOT/README.txt
rm -f $PHIROOT/license.txt
rmdir $PHIROOT

echo "PhiSuite successfully removed."
echo "Note: your phis document (data) directory is not touched."
exit 0


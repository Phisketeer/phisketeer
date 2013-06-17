#!/bin/sh -e
### BEGIN INIT INFO
# Provides:          phis
# Required-Start:    $local_fs $remote_fs $network $syslog
# Required-Stop:     $local_fs $remote_fs $network $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# X-Interactive:     true
# Short-Description: Start/stop phis web services
### END INIT INFO

LD_LIBRARY_PATH=%philibpath%:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=%phipluginpath%
export QT_PLUGIN_PATH
# QT_QPA_FONTDIR=/usr/share/fonts
# export QT_QPA_FONTDIR
PHIBINPATH=%phibinpath%

case $1 in
	start)
                $PHIBINPATH/phis -platform minimal
	;;
	stop)
                /usr/bin/killall -q $PHIBINPATH/phis
	;;
	restart)
                /usr/bin/killall -q -w $PHIBINPATH/phis
                $PHIBINPATH/phis -platform minimal
	;;
	reload)
                $PHIBINPATH/phis -c
	;;
	status)
		$PHIBINPATH/phis -v
	;;
	*)
		echo "Usage: /etc/init.d/phis {start|stop|restart|reload|status}"
		exit 1
	;;
esac
exit 0


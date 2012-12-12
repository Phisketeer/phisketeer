This is release V1.4.1 of the Phi(sketeer) software suite.
You find all recent changes at http://wiki.phisketeer.org by clicking on "Change log" in the navigation bar.

Professional installation and compiling support is available through our main sponsor Phisys AG.
Please visit: http://www.phisys.com for further information.

Phi components:
===============
- Artephis is your intuitive editor creating genious Phi pages
- Phis is the stand alone Web service which processes Phi pages into HTML code
- Phisconf is the configuration program for the Web service
- PhiApp is an OS natively running program for displaying Phi content
- libmod_phi is the Phi module for the Apache web server (MPM worker and prefork are supported)
- Amphibia is an open source Web browser including native Phi support

Amphibia notes:
===============
- it is not a replacement for other browser, however it contains a ful WebKit implementation
- the main intention is to test the Phi native mode
- bookmarks are not yet implemented
- searching is not working

Windows notes:
===============
- a 64bit PhiPlugin for any available browser in Windows is currently not supported, however you can still view the HTML output with these browsers
- if you encounter visual problems with the Internet-Explorer displaying Phi content switch between the compatibility modes of the browser
- an Apache module for serving Phi pages is available and highly recommended for production environments

Mac OS X notes:
===============
- requires Mac OS X 10.5 64bit (Leopard) or higher
- due to the restrictions of the AppStore we do not provide multi architecture binaries anymore
- the PhiPlugin on Mac OS X is currently not available, however Safari (on Mac OS X) supports already real 3D transformations, or use Amphibia to get all the power of Phi
- an Apache Phi module for Mac OS X is not available
- Phis is automatically started in the background (usually listening on port 8080) on Artephis launch
- available on the AppStore since version 1.4.0: look out for 'Artephis'...

Linux notes:
===============
- run 'sudo ./phisketeer-1.4.1.bin' to install Phisketeer
   (Add 'DISPLAY XAUTHORITY' to '/etc/sudoers' if you get 'Cannot connect to X server')
- Check the file permissions for the documentroot (default: '/var/phis/default/localhost')
- the PhiPlugin is experimental:
   to enable the PhiPlugin set the MOZ_PLUGIN_PATH in your .bashrc or copy libphiplugin.so.1 to the plug-in path:
   $ export MOZ_PLUGIN_PATH=/opt/phisketeer-1.4.1/lib:$MOZ_PLUGIN_PATH
- for starting the Artephis IDE run:
   $ /opt/phisketeer-1.4.1/bin/artephis (do NOT run Artephis as root)
- for configuring the Phi web service run:
   $ sudo /opt/phisketeer-1.4.1/bin/phisconf
- an Apache module is provided and highly recommended for production environments

Compiling from source:
======================
- Phi(sketeer) sources are freely available on GitHub and Gitorios
- requires a recent Qt 4.8.x or higher installation freely available at http://qt.digia.com
- check out the phi.pro file in the source distribution for additional notes
- run '/path_to_Qt_dir/bin/qmake phi.pro' to start compiling
- Artephis is precompiled and currently not open source due to included proprietary code and icon sets


Please do not hesitate to report bugs and suggestions to bugs@phisketeer.org or feedback@phisketeer.org!
Any contributions are welcome. We are looking for volunteers and developers extending our team.
Donations are also highly appreciated and ensure future development... ;-)

One for all, all for you!
Your Phisketeer team.

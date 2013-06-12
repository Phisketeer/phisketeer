This is release V1.5.0 of the Phi(sketeer) software suite.
You find all recent changes at http://wiki.phisketeer.org by clicking on "Change log" in the navigation bar.

Professional installation and compiling support is available through our main sponsor Phisys AG.
Please visit: http://www.phisys.com for further information.

Phi components:
===============
- Artephis is your intuitive editor creating genius Phi Web pages
- Phis is the stand-alone Web service which processes Phi pages into HTML code
- Phisconf is the configuration program for the Phis Web service
- PhiApp is an OS natively running program for displaying Phi content
- libmod_phi is the Phi module for the Apache Web server (MPM worker and prefork are supported)
- Amphibia is an open source Web browser including native Phi support

Amphibia notes:
===============
- it is not a replacement for other browser, however it contains a full WebKit implementation
- the main intention is to test the Phi native mode
- bookmarks are not yet implemented
- searching is not yet working

Windows notes:
===============
- the PhiPlugin has been removed (all recent browser versions support now perspective transformations)
- if you encounter visual problems with the Internet-Explorer displaying Phi content switch between the compatibility modes of the browser
- an Apache module for serving Phi pages is available and highly recommended for production environments

Mac OS X notes:
===============
- requires Mac OS X 10.7 64bit (Lion) or higher
- due to the restrictions of the AppStore we can not provide multi architecture binaries anymore
- an Apache Phi module (libmod_phi) for Mac OS X is currently not available
- the Phis Web service is automatically started in the background (usually listening on port 8080) on Artephis launch
- available in the AppStore since version 1.4.0: look out for 'Artephis'.

Linux notes:
===============
- run 'sudo ./phisketeer-1.5.0.bin' to install Phisketeer
   (Add 'DISPLAY XAUTHORITY' to '/etc/sudoers' if you get 'Cannot connect to X server')
- Check the file permissions for the documentroot (default: '/var/phis/default/localhost')
- the PhiPlugin is not longer needed
- for starting the Artephis IDE run:
   $ /opt/phisketeer-1.5.0/bin/artephis (do NOT run Artephis as root)
- for configuring the Phis Web service run:
   $ sudo /opt/phisketeer-1.5.0/bin/phisconf
- an Apache module is provided and highly recommended for production environments

Compiling from source:
======================
- Phi(sketeer) sources are freely available on GitHub and Gitorios
- requires a recent Qt 5.0.2 or higher installation freely available at http://qt.digia.com
- check out the phi.pro file in the source distribution for additional notes
- run '/path_to_Qt_dir/bin/qmake phi.pro' to start compiling
- Artephis is precompiled and currently not open source due to included proprietary code and icon sets


Please do not hesitate to report bugs and suggestions to bugs@phisketeer.org or feedback@phisketeer.org!
Any contributions are welcome. We are looking for volunteers and developers extending our team.
Donations are also highly appreciated and ensure future development... ;-)

One for all, all for you!
Your Phisketeer team.

This is release V2.0.1 of the Phi[sketeer] software suite (first official release for V2.0).
You find all recent changes at http://wiki.phisketeer.org by clicking on "Change log" in the navigation bar.

Professional installation and compiling support is available through our main sponsor Phisys AG.
Please visit: http://www.phisys.com for further information.

Note: all Phi libraries and the server parts are released as open-source!

Phi components:
===============
- Phis is the stand-alone Web service which processes Phi pages into HTML code
- PhisConf is the configuration program for the Phis Web service
- PhiApp is an OS natively running program for displaying Phi content
- libmod_phi is the Phisketeer module for the Apache Web server (MPM worker and prefork are supported)
- Amphibia is an open source example Web browser including native Phi support (provided with the source code only)
- Artephis free edition (with friendly permission from Phisys AG) is your intuitive IDE creating brilliant Phi Web pages

Windows notes:
===============
- if you encounter visual problems with the Internet-Explorer displaying Phi content switch between the compatibility modes of the browser
- an Apache module for serving Phi pages is available and highly recommended for production environments
- known issues: outline of graphics text is not shown (reported Qt bug)

Mac OS X notes:
===============
- requires Mac OS X 10.8 64bit (Mountain Lion) or higher
- an Apache Phi module (libmod_phi) for Mac OS X is not available
- the Phis Web service is automatically started in the background (usually listening on port 8080) on IDE launch
- available in the AppStore since version 1.4.0: look out for 'Artephis'. (Does not offer BETA versions.)

Linux notes:
===============
- Check the file permissions for the document-root (default: '/var/phis/default/localhost')
- Running the Phis daemon does not require 'root' anymore
- Artephis -> Settings -> Server provides you a GUI to view the current state of the Phis service
- for starting the Artephis IDE run:
   $ phisketeer-2.0.1/bin/Artephis
- for configuring the Phis Web service run:
   $ phisketeer-2.0.1/bin/phisconf
- the Apache module is highly recommended for production environments

Please do not hesitate to report bugs and suggestions to bugs@phisketeer.org or feedback@phisketeer.org!
Any contributions are welcome. We are looking for volunteers and developers extending our team.
Donations or any other support are also highly appreciated and ensure future development... ;-)

One for all, all for you!
Your Phisketeer team.

This is release V2.0.0 BETA of the Phi(sketeer) software suite.
You find all recent changes at http://wiki.phisketeer.org by clicking on "Change log" in the navigation bar.

Professional installation and compiling support is available through our main sponsor Phisys AG.
Please visit: http://www.phisys.com for further information.

Note: dialog and IDE translations are not yet finished. Please use English.

Phi components:
===============
- Artephis is your intuitive editor creating genius Phi Web pages
- Phis is the stand-alone Web service which processes Phi pages into HTML code
- PhisConf is the configuration program for the Phis Web service
- PhiApp is an OS natively running program for displaying Phi content
- libmod_phi is the Phi module for the Apache Web server (MPM worker and prefork are supported)
- Amphibia is an open source example Web browser including native Phi support (not included)

Windows notes:
===============
- if you encounter visual problems with the Internet-Explorer displaying Phi content switch between the compatibility modes of the browser
- an Apache module for serving Phi pages is available and highly recommended for production environments
- known issues: outline of graphics text is not shown

Mac OS X notes:
===============
- requires Mac OS X 10.8 64bit (Mountain Lion) or higher
- an Apache Phi module (libmod_phi) for Mac OS X is not available
- the Phis Web service is automatically started in the background (usually listening on port 8080) on Artephis launch
- available in the AppStore since version 1.4.0: look out for 'Artephis'. (Does not include BETA versions.)

Linux notes:
===============
- The BETA only comes as a targz-ball and is NOT build for production envrionments! It requries a running X-server.
- Use Artephis in the bin directory to start the IDE.
- Check the file permissions for the document-root (default: '/var/phis/default/localhost')
- Running the Phis daemon does not require root anymore
- Artephis -> Settings -> Server enables you to start the local Phis service
- for starting the Artephis IDE run:
   $ phisketeer-2.0.0/bin/Artephis
- for configuring the Phis Web service run:
   $ phisketeer-2.0.0/bin/phisconf
- an Apache module is provided for testing

Please do not hesitate to report bugs and suggestions to bugs@phisketeer.org or feedback@phisketeer.org!
Any contributions are welcome. We are looking for volunteers and developers extending our team.
Donations are also highly appreciated and ensure future development... ;-)

One for all, all for you!
Your Phisketeer team.

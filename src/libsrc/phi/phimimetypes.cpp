/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phi.h"

const char* PHI::_phiStdMimeTypes="\
# This file controls what Internet media types are sent to the client for\n\
# given file extension(s).  Sending the correct media type to the client\n\
# is important so they know how to handle the content of the file.\n\
# For more information about Internet media types, please read\n\
# RFC 2045, 2046, 2047, 2048, and 2077.  The Internet media type\n\
# registry is at <ftp://ftp.iana.org/assignments/media-types/>.\n\
# MIME type			Extension\n\
application/andrew-inset ez\n\
application/chemtool cht\n\
application/dicom dcm\n\
application/docbook+xml docbook\n\
application/ecmascript es\n\
application/flash-video flv\n\
application/illustrator ai\n\
application/javascript js\n\
application/mac-binhex40\n\
application/mathematica nb\n\
application/msword doc\n\
application/octet-stream bin\n\
application/oda oda\n\
application/ogg ogg\n\
application/pdf pdf\n\
application/pgp pgp\n\
application/pgp-encrypted\n\
application/pgp-encrypted pgp gpg\n\
application/pgp-keys\n\
application/pgp-keys skr pkr\n\
application/pgp-signature\n\
application/pgp-signature sig\n\
application/pkcs7-mime\n\
application/pkcs7-signature p7s\n\
application/postscript ps\n\
application/rtf rtf\n\
application/sdp sdp\n\
application/smil smil smi sml\n\
application/stuffit sit\n\
application/vnd.corel-draw cdr\n\
application/vnd.hp-hpgl hpgl\n\
application/vnd.hp-pcl pcl\n\
application/vnd.lotus-1-2-3 123 wk1 wk3 wk4 wks\n\
application/vnd.mozilla.xul+xml xul\n\
application/vnd.ms-excel xls xlc xll xlm xlw xla xlt xld\n\
application/vnd.ms-powerpoint ppz ppt pps pot\n\
application/vnd.oasis.opendocument.chart odc\n\
application/vnd.oasis.opendocument.database odb\n\
application/vnd.oasis.opendocument.formula odf\n\
application/vnd.oasis.opendocument.graphics odg\n\
application/vnd.oasis.opendocument.graphics-template otg\n\
application/vnd.oasis.opendocument.image odi\n\
application/vnd.oasis.opendocument.presentation odp\n\
application/vnd.oasis.opendocument.presentation-template otp\n\
application/vnd.oasis.opendocument.spreadsheet ods\n\
application/vnd.oasis.opendocument.spreadsheet-template ots\n\
application/vnd.oasis.opendocument.text odt\n\
application/vnd.oasis.opendocument.text-master odm\n\
application/vnd.oasis.opendocument.text-template ott\n\
application/vnd.oasis.opendocument.text-web oth\n\
application/vnd.palm pdb\n\
application/vnd.rn-realmedia\n\
application/vnd.rn-realmedia rm\n\
application/vnd.rn-realmedia-secure rms\n\
application/vnd.rn-realmedia-vbr rmvb\n\
application/vnd.stardivision.calc sdc\n\
application/vnd.stardivision.chart sds\n\
application/vnd.stardivision.draw sda\n\
application/vnd.stardivision.impress sdd sdp\n\
application/vnd.stardivision.mail smd\n\
application/vnd.stardivision.math smf\n\
application/vnd.stardivision.writer sdw vor sgl\n\
application/vnd.sun.xml.calc sxc\n\
application/vnd.sun.xml.calc.template stc\n\
application/vnd.sun.xml.draw sxd\n\
application/vnd.sun.xml.draw.template std\n\
application/vnd.sun.xml.impress sxi\n\
application/vnd.sun.xml.impress.template sti\n\
application/vnd.sun.xml.math sxm\n\
application/vnd.sun.xml.writer sxw\n\
application/vnd.sun.xml.writer.global sxg\n\
application/vnd.sun.xml.writer.template stw\n\
application/vnd.wordperfect wpd\n\
application/x-abiword abw abw.CRASHED abw.gz zabw\n\
application/x-amipro sam\n\
application/x-anjuta-project prj\n\
application/x-applix-spreadsheet as\n\
application/x-applix-word aw\n\
application/x-arc\n\
application/x-archive a\n\
application/x-arj arj\n\
application/x-asax asax\n\
application/x-ascx ascx\n\
application/x-ashx ashx\n\
application/x-asix asix\n\
application/x-asmx asmx\n\
application/x-asp asp\n\
application/x-awk\n\
application/x-axd axd\n\
application/x-bcpio bcpio\n\
application/x-bittorrent torrent\n\
application/x-blender blender blend BLEND\n\
application/x-bzip bz bz2\n\
application/x-bzip bz2 bz\n\
application/x-bzip-compressed-tar tar.bz tar.bz2\n\
application/x-bzip-compressed-tar tar.bz tar.bz2 tbz tbz2\n\
application/x-cd-image iso\n\
application/x-cgi cgi\n\
application/x-chess-pgn pgn\n\
application/x-chm chm\n\
application/x-class-file\n\
application/x-cmbx cmbx\n\
application/x-compress Z\n\
application/x-compressed-tar tar.gz tar.Z tgz taz\n\
application/x-compressed-tar tar.gz tgz\n\
application/x-config config\n\
application/x-core\n\
application/x-cpio cpio\n\
application/x-cpio-compressed cpio.gz\n\
application/x-csh csh\n\
application/x-cue cue\n\
application/x-dbase dbf\n\
application/x-dbm\n\
application/x-dc-rom dc\n\
application/x-deb deb\n\
application/x-designer ui\n\
application/x-desktop desktop kdelnk\n\
application/x-devhelp devhelp\n\
application/x-dia-diagram dia\n\
application/x-disco disco\n\
application/x-dvi dvi\n\
application/x-e-theme etheme\n\
application/x-egon egon\n\
application/x-executable exe\n\
application/x-font-afm afm\n\
application/x-font-bdf bdf\n\
application/x-font-dos\n\
application/x-font-framemaker\n\
application/x-font-libgrx\n\
application/x-font-linux-psf psf\n\
application/x-font-otf\n\
application/x-font-pcf pcf\n\
application/x-font-pcf pcf.gz\n\
application/x-font-speedo spd\n\
application/x-font-sunos-news\n\
application/x-font-tex\n\
application/x-font-tex-tfm\n\
application/x-font-ttf ttc TTC\n\
application/x-font-ttf ttf\n\
application/x-font-type1 pfa pfb gsf pcf.Z\n\
application/x-font-vfont\n\
application/x-frame\n\
application/x-frontline aop\n\
application/x-gameboy-rom gb\n\
application/x-gdbm\n\
application/x-gdesklets-display display\n\
application/x-genesis-rom gen md\n\
application/x-gettext-translation gmo\n\
application/x-glabels glabels\n\
application/x-glade glade\n\
application/x-gmc-link\n\
application/x-gnome-db-connection connection\n\
application/x-gnome-db-database database\n\
application/x-gnome-stones caves\n\
application/x-gnucash gnucash gnc xac\n\
application/x-gnumeric gnumeric\n\
application/x-graphite gra\n\
application/x-gtar gtar\n\
application/x-gtktalog\n\
application/x-gzip gz\n\
application/x-gzpostscript ps.gz\n\
application/x-hdf hdf\n\
application/x-ica ica\n\
application/x-ipod-firmware\n\
application/x-jamin jam\n\
application/x-jar jar\n\
application/x-java class\n\
application/x-java-archive jar ear war\n\
application/x-jbuilder-project jpr jpx\n\
application/x-karbon karbon\n\
application/x-kchart chrt\n\
application/x-kformula kfo\n\
application/x-killustrator kil\n\
application/x-kivio flw\n\
application/x-kontour kon\n\
application/x-kpovmodeler kpm\n\
application/x-kpresenter kpr kpt\n\
application/x-krita kra\n\
application/x-kspread ksp\n\
application/x-kspread-crypt\n\
application/x-ksysv-package\n\
application/x-kugar kud\n\
application/x-kword kwd kwt\n\
application/x-kword-crypt\n\
application/x-lha lha lzh\n\
application/x-lha lzh\n\
application/x-lhz lhz\n\
application/x-linguist ts\n\
application/x-lyx lyx\n\
application/x-lzop lzo\n\
application/x-lzop-compressed-tar tar.lzo tzo\n\
application/x-macbinary\n\
application/x-machine-config\n\
application/x-magicpoint mgp\n\
application/x-master-page master\n\
application/x-matroska mkv\n\
application/x-mdp mdp\n\
application/x-mds mds\n\
application/x-mdsx mdsx\n\
application/x-mergeant mergeant\n\
application/x-mif mif\n\
application/x-mozilla-bookmarks\n\
application/x-mps mps\n\
application/x-ms-dos-executable exe\n\
application/x-mswinurl\n\
application/x-mswrite wri\n\
application/x-msx-rom msx\n\
application/x-n64-rom n64\n\
application/x-nautilus-link\n\
application/x-nes-rom nes\n\
application/x-netcdf cdf nc\n\
application/x-netscape-bookmarks\n\
application/x-object o\n\
application/x-ole-storage\n\
application/x-oleo oleo\n\
application/x-palm-database\n\
application/x-palm-database pdb prc\n\
application/x-par2 PAR2 par2\n\
application/x-pef-executable\n\
application/x-perl pl pm al perl\n\
application/x-phi phi\n\
application/x-phis phis\n\
application/x-php php php3 php4\n\
application/x-pkcs12 p12 pfx\n\
application/x-planner planner mrproject\n\
application/x-planperfect pln\n\
application/x-prjx prjx\n\
application/x-profile\n\
application/x-ptoptimizer-script pto\n\
application/x-pw pw\n\
application/x-python-bytecode pyc pyo\n\
application/x-quattro-pro wb1 wb2 wb3\n\
application/x-quattropro wb1 wb2 wb3\n\
application/x-qw qif\n\
application/x-rar rar\n\
application/x-rar-compressed rar\n\
application/x-rdp rdp\n\
application/x-reject rej\n\
application/x-remoting rem\n\
application/x-resources resources\n\
application/x-resourcesx resx\n\
application/x-rpm rpm\n\
application/x-ruby\n\
application/x-sc\n\
application/x-sc sc\n\
application/x-scribus sla sla.gz scd scd.gz\n\
application/x-shar shar\n\
application/x-shared-library-la la\n\
application/x-sharedlib so\n\
application/x-shellscript sh\n\
application/x-shockwave-flash swf\n\
application/x-siag siag\n\
application/x-slp\n\
application/x-smil kino\n\
application/x-smil smi smil\n\
application/x-sms-rom sms gg\n\
application/x-soap-remoting soap\n\
application/x-streamingmedia ssm\n\
application/x-stuffit\n\
application/x-stuffit bin sit\n\
application/x-sv4cpio sv4cpio\n\
application/x-sv4crc sv4crc\n\
application/x-tar tar\n\
application/x-tarz tar.Z\n\
application/x-tex-gf gf\n\
application/x-tex-pk k\n\
application/x-tgif obj\n\
application/x-theme theme\n\
application/x-toc toc\n\
application/x-toutdoux\n\
application/x-trash   bak old sik\n\
application/x-troff tr roff t\n\
application/x-troff-man man\n\
application/x-troff-man-compressed\n\
application/x-tzo tar.lzo tzo\n\
application/x-ustar ustar\n\
application/x-wais-source src\n\
application/x-web-config\n\
application/x-wpg wpg\n\
application/x-wsdl wsdl\n\
application/x-x509-ca-cert der cer crt cert pem\n\
application/x-xbel xbel\n\
application/x-zerosize\n\
application/x-zoo zoo\n\
application/xhtml+xml xhtml\n\
application/zip zip\n\
audio/ac3 ac3\n\
audio/basic au snd\n\
audio/midi mid midi\n\
audio/mpeg mp3\n\
audio/prs.sid sid psid\n\
audio/vnd.rn-realaudio ra\n\
audio/x-aac aac\n\
audio/x-adpcm\n\
audio/x-aifc\n\
audio/x-aiff aif aiff\n\
audio/x-aiff aiff aif aifc\n\
audio/x-aiffc\n\
audio/x-flac flac\n\
audio/x-it it\n\
audio/x-m4a m4a\n\
audio/x-mod mod ult uni XM m15 mtm 669\n\
audio/x-mp3-playlist\n\
audio/x-mpeg\n\
audio/x-mpegurl m3u\n\
audio/x-ms-asx\n\
audio/x-pn-realaudio ra ram rm\n\
audio/x-pn-realaudio ram rmm\n\
audio/x-riff\n\
audio/x-s3m s3m\n\
audio/x-scpls pls\n\
audio/x-scpls pls xpl\n\
audio/x-stm stm\n\
audio/x-voc voc\n\
audio/x-wav wav\n\
audio/x-xi xi\n\
audio/x-xm xm\n\
image/bmp bmp\n\
image/cgm cgm\n\
image/dpx\n\
image/fax-g3 g3\n\
image/g3fax\n\
image/gif gif\n\
image/ief ief\n\
image/jpeg jpeg jpg jpe\n\
image/jpeg2000 jp2\n\
image/png png\n\
image/rle rle\n\
image/svg+xml svg\n\
image/tiff tif tiff\n\
image/vnd.djvu djvu djv\n\
image/vnd.dwg dwg\n\
image/vnd.dxf dxf\n\
image/x-3ds 3ds\n\
image/x-applix-graphics ag\n\
image/x-cmu-raster ras\n\
image/x-compressed-xcf xcf.gz xcf.bz2\n\
image/x-dcraw bay BAY bmq BMQ cr2 CR2 crw CRW cs1 CS1 dc2 DC2 dcr DCR fff FFF k25 K25 kdc KDC mos MOS mrw MRW nef NEF orf ORF pef PEF raf RAF rdc RDC srf SRF x3f X3F\n\
image/x-dib\n\
image/x-eps eps epsi epsf\n\
image/x-fits fits\n\
image/x-fpx\n\
image/x-icb icb\n\
image/x-ico ico\n\
image/x-iff iff\n\
image/x-ilbm ilbm\n\
image/x-jng jng\n\
image/x-lwo lwo lwob\n\
image/x-lws lws\n\
image/x-msod msod\n\
image/x-niff\n\
image/x-pcx\n\
image/x-photo-cd pcd\n\
image/x-pict pict pict1 pict2\n\
image/x-portable-anymap pnm\n\
image/x-portable-bitmap pbm\n\
image/x-portable-graymap pgm\n\
image/x-portable-pixmap ppm\n\
image/x-psd psd\n\
image/x-rgb rgb\n\
image/x-sgi sgi\n\
image/x-sun-raster sun\n\
image/x-tga tga\n\
image/x-win-bitmap cur\n\
image/x-wmf wmf\n\
image/x-xbitmap xbm\n\
image/x-xcf xcf\n\
image/x-xfig fig\n\
image/x-xpixmap xpm\n\
image/x-xwindowdump xwd\n\
inode/blockdevice\n\
inode/chardevice\n\
inode/directory\n\
inode/fifo\n\
inode/mount-point\n\
inode/socket\n\
inode/symlink\n\
message/delivery-status\n\
message/disposition-notification\n\
message/external-body\n\
message/news\n\
message/partial\n\
message/rfc822\n\
message/x-gnu-rmail\n\
model/vrml wrl\n\
multipart/alternative\n\
multipart/appledouble\n\
multipart/digest\n\
multipart/encrypted\n\
multipart/mixed\n\
multipart/related\n\
multipart/report\n\
multipart/signed\n\
multipart/x-mixed-replace\n\
text/calendar vcs ics\n\
text/css css CSSL\n\
text/directory vcf vct gcrd\n\
text/enriched\n\
text/html html htm\n\
text/htmlh\n\
text/mathml mml\n\
text/plain txt asc\n\
text/rdf rdf\n\
text/rfc822-headers\n\
text/richtext rtx\n\
text/rss rss\n\
text/sgml sgml sgm\n\
text/spreadsheet sylk slk\n\
text/tab-separated-values tsv\n\
text/vnd.rn-realtext rt\n\
text/vnd.wap.wml wml\n\
text/x-adasrc adb ads\n\
text/x-authors\n\
text/x-bibtex bib\n\
text/x-boo boo\n\
text/x-c++hdr hh\n\
text/x-c++src cpp cxx cc C c++\n\
text/x-chdr h h++ hp\n\
text/x-comma-separated-values csv\n\
text/x-copying\n\
text/x-credits\n\
text/x-csharp cs\n\
text/x-csrc c\n\
text/x-dcl dcl\n\
text/x-dsl dsl\n\
text/x-dsrc d\n\
text/x-dtd dtd\n\
text/x-emacs-lisp el\n\
text/x-fortran f\n\
text/x-gettext-translation po\n\
text/x-gettext-translation-template pot\n\
text/x-gtkrc\n\
text/x-haskell hs\n\
text/x-idl idl\n\
text/x-install\n\
text/x-java java\n\
text/x-js js\n\
text/x-ksysv-log\n\
text/x-literate-haskell lhs\n\
text/x-log log\n\
text/x-makefile\n\
text/x-moc moc\n\
text/x-msil il\n\
text/x-nemerle n\n\
text/x-objcsrc m\n\
text/x-pascal p pas\n\
text/x-patch diff patch\n\
text/x-python py\n\
text/x-readme\n\
text/x-rng rng\n\
text/x-scheme scm\n\
text/x-setext etx\n\
text/x-speech\n\
text/x-sql sql\n\
text/x-suse-ymp ymp\n\
text/x-suse-ymu ymu\n\
text/x-tcl tcl tk\n\
text/x-tex tex ltx sty cls\n\
text/x-texinfo texi texinfo\n\
text/x-texmacs tm ts\n\
text/x-troff-me me\n\
text/x-troff-mm mm\n\
text/x-troff-ms ms\n\
text/x-uil uil\n\
text/x-uri uri url\n\
text/x-vb vb\n\
text/x-xds xds\n\
text/x-xmi xmi\n\
text/x-xsl xsl\n\
text/x-xslfo fo xslfo\n\
text/x-xslt xslt xsl\n\
text/xmcd\n\
text/xml xml\n\
video/3gpp 3gp\n\
video/dv dv dif\n\
video/isivideo\n\
video/mpeg mpeg mpg mp2 mpe vob dat\n\
video/quicktime qt mov moov qtvr\n\
video/vivo\n\
video/vnd.rn-realvideo rv\n\
video/wavelet\n\
video/x-3gpp2 3g2\n\
video/x-anim anim[1-9j]\n\
video/x-avi\n\
video/x-flic fli flc\n\
video/x-mng mng\n\
video/x-ms-asf asf asx\n\
video/x-ms-wmv wmv\n\
video/x-msvideo avi\n\
video/x-nsv nsv NSV\n\
video/x-real-video\n\
video/x-sgi-movie movie\n\
application/x-java-jnlp-file jnlp\n\
";

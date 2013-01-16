/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QApplication>
#include "sconfig.h"
#include "phi.h"

int main ( int argc, char **argv )
{
    //QApplication::setDesktopSettingsAware( false );
    QApplication app( argc, argv );
    app.setApplicationName( "Phisconf" );
    app.setApplicationVersion( PHIVERSION );
    PHI::setupApplication( &app );

    SConfig dlg;
    dlg.show();
#ifdef Q_OS_MAC
    // we need to set LSUIElement to true to prevent the icon of the phis daemon in the panel
    // in the Info.plist of the editor. However this doesn't bring SConfig automatically to the front
    dlg.raise();
#endif
    return app.exec();
}

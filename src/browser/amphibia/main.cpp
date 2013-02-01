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
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QApplication>
#include "ampmainwindow.h"
#include "phi.h"
#include "phia.h"

int main ( int argc, char **argv )
{
    QApplication app( argc, argv );

    app.setApplicationName( PHIA::browserName() );
    app.setApplicationVersion( PHIVERSION );
    PHI::setupApplication( &app );
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    QStringList args=app.arguments();
    QString url;
    if ( args.count()>1 ) url=args.at( 1 );

    AMPMainWindow *mw=new AMPMainWindow();
    mw->show();
#ifdef Q_OS_MAC
    mw->raise();
#endif
    if ( !url.isEmpty() ) mw->slotUrlComboActivated( url );
    return app.exec();
}

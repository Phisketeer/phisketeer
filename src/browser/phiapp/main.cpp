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
#include <QObject>
#include "phiapplication.h"
#include "phiaappwindow.h"
int main ( int argc, char **argv )
{
    PHIApplication app( argc, argv, "PhiApp", PHIVERSION );
    QObject::connect( qApp, SIGNAL( lastWindowClosed() ), qApp, SLOT( quit() ) );

    QStringList args=qApp->arguments();
    QString url;
    if ( args.count()>1 ) url=args.at( 1 );

    PHIAAppWindow win;
    win.show();
    if ( !url.isEmpty() ) win.setUrl( QUrl( url ) );
    return app.exec();
}

/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
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

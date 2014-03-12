/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QTextStream>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include "phislogviewer.h"
#include "phiapplication.h"

PHISLogViewer* PHISLogViewer::_instance=0;

PHISLogViewer::PHISLogViewer( QWidget *parent )
    : QWidget( parent ), _lastSize( 0 )
{
    setAttribute( Qt::WA_DeleteOnClose );
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( PHI::defaultString() );
    _filename=s->value( L1( "LogDir" ), phiApp->dataPath()+QDir::toNativeSeparators( L1( "/log" ) ) ).toString();
    s->endGroup();

    _filename.append( QDir::toNativeSeparators( L1( "/error.log" ) ) );
    QFile file( _filename );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Could not open log file '%1'. "
            "If you are using the Apache module examin the Apache logs." ).arg( file.fileName() ), QMessageBox::Ok );
        close();
        return;
    }
    file.close();
    setWindowTitle( _filename );

    _edit=new QPlainTextEdit();
    _edit->setMaximumBlockCount( 5000 );
    _edit->setReadOnly( true );
    _edit->setTextInteractionFlags( Qt::TextSelectableByKeyboard | Qt::TextSelectableByMouse );
    QVBoxLayout *l=new QVBoxLayout();
    l->setContentsMargins( 12, 12, 12, 12 );
    l->addWidget( _edit );
    setLayout( l );

    s=phiApp->settings();
    restoreGeometry( s->value( L1( "LogViewerGeometry" ) ).toByteArray() );
    QTimer *timer=new QTimer( this );
    timer->setInterval( 2000 );
    connect( timer, &QTimer::timeout, this, &PHISLogViewer::refresh );
    timer->start();
    show();
    refresh();
}

PHISLogViewer::~PHISLogViewer()
{
    _instance=0;
    QSettings *s=phiApp->settings();
    s->setValue( L1( "LogViewerGeometry" ), saveGeometry() );
}

PHISLogViewer* PHISLogViewer::instance()
{
    if ( !_instance ) _instance=new PHISLogViewer();
    return _instance;
}

void PHISLogViewer::refresh()
{
    QFile f( _filename );
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) ) return;
    if ( f.size()==_lastSize ) return;
    _lastSize=f.size();
    QTextStream t( &f );
    _edit->setPlainText( t.readAll() );
    QTextCursor cursor=_edit->textCursor();
    cursor.movePosition( QTextCursor::End );
    _edit->setTextCursor( cursor );
    _edit->ensureCursorVisible();
    f.close();
}

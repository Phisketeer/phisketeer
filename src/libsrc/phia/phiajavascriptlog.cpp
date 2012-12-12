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
#include <QVBoxLayout>
#include <QSettings>
#include "phiajavascriptlog.h"

PHIAJavaScriptLog::PHIAJavaScriptLog( QWidget *parent )
    : QDialog( parent )
{
    setSizeGripEnabled( true );
    setWindowTitle( tr( "JavaScript console" ) );
    QVBoxLayout *l=new QVBoxLayout( this );
    _logger=new QPlainTextEdit( this );
    _logger->setReadOnly( true );
    _logger->setMaximumBlockCount( 1000 );
    l->addWidget( _logger );
}

PHIAJavaScriptLog::~PHIAJavaScriptLog()
{
}

void PHIAJavaScriptLog::showEvent( QShowEvent *e )
{
    QSettings s;
    restoreGeometry( s.value( "JavascriptLog" ).toByteArray() );
    QDialog::showEvent( e );
}

void PHIAJavaScriptLog::closeEvent( QCloseEvent *e )
{
    QSettings s;
    s.setValue( "JavascriptLog", saveGeometry() );
    QDialog::closeEvent( e );
}

void PHIAJavaScriptLog::slotJavaScriptConsoleMessage( const QString &msg, int line, const QString &id )
{
    Q_UNUSED( id );
    QString tmp=msg+'('+tr( "Line:" )+' '+QString::number( line )+")\n";
    _logger->appendPlainText( tmp );
}

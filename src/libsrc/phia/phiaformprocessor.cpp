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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUuid>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include "phiaformprocessor.h"

PHIAFormProcessor::PHIAFormProcessor( PHIAWebView *parent )
    : QObject( parent )
{
    qDebug( "PHIAFormProcessor::PHIAFormProcessor()" );
    _tmpFile=new QTemporaryFile( QDir::tempPath()+QDir::separator()+"phiformXXXXXX", this );
    _disposition="Content-Disposition: form-data; name=";
    QUuid uid=QUuid::createUuid();
    _boundary="----------PHI"+uid.toString().toUtf8();
}

PHIAFormProcessor::~PHIAFormProcessor()
{
    qDebug( "PHIAFormProcessor::~PHIAFormProcessor()" );
}

QNetworkReply* PHIAFormProcessor::process( const PHIAPage *page, const QString &buttonid )
{
    QUrl actionUrl( page->action() );
    if ( !_tmpFile->open() || page->action().isEmpty() || !actionUrl.isValid() ) {
        this->deleteLater();
        return 0;
    }
    PHIAWebView *view=qobject_cast<PHIAWebView*>(parent());
    Q_ASSERT( view );

    QHash <QByteArray, PHIBaseItem*> items=page->items();
    PHIBaseItem *it;
    foreach ( it, items ) {
        PHIAItem *pit=qobject_cast<PHIAItem*>(it);
        if ( !PHI::isInputItem( it->wid() ) ) continue;
        if ( it->wid()==PHI::SUBMIT_BUTTON ) {
            if ( it->name()!=buttonid ) continue; // only add submit button which caused the request
        }
        addPHIAItem( pit );
    }
    // Add session, lang
    _tmpFile->write( "--"+_boundary+"\r\n"+_disposition+"\"philang\"\r\n\r\n" );
    _tmpFile->write( page->lang().toAscii() );
    _tmpFile->write( "\r\n" );

    if ( !page->session().isEmpty() ) {
        _tmpFile->write( "--"+_boundary+"\r\n"+_disposition+"\"phisid\"\r\n\r\n" );
        _tmpFile->write( page->session().toUtf8() );
        _tmpFile->write( "\r\n" );
    }

    _tmpFile->write( "--"+_boundary+"--\r\n" );
    if ( _tmpFile->error()!=QFile::NoError ) {
        _tmpFile->close();
        this->deleteLater();
        return 0;
    }
    _tmpFile->reset();

    QNetworkRequest request;
    request.setHeader( QNetworkRequest::ContentTypeHeader,
        "multipart/form-data; boundary="+_boundary );
    request.setRawHeader( "Referer", view->url().toEncoded() );
    request.setUrl( PHI::createUrlForLink( view->url(), page->action() ) );
    request.setHeader( QNetworkRequest::ContentLengthHeader, _tmpFile->size() );

    QNetworkReply *reply=view->networkAccessManager()->post( request, _tmpFile );
    if ( !reply ) {
        _tmpFile->close();
        this->deleteLater();
        return 0;
    }
    connect( reply, SIGNAL( finished() ), this, SLOT( slotReplyFinished() ) );
    return reply;
}

void PHIAFormProcessor::addPHIAItem( const PHIAItem *it )
{
    Q_ASSERT( it );
    QByteArray name, value, filename;
    QFile file;
    value=it->value().toUtf8();

    switch ( it->wid() ) {
    case PHI::CHECK_BOX:
    case PHI::RADIO_BUTTON:
        if ( !it->checked() ) return; // data is not sent for unchecked items
        name=it->parentName().toUtf8();
        if ( name.isEmpty() ) name=it->id();
        break;
    case PHI::FILE_BUTTON: {
        file.setFileName( it->value() );
        QFileInfo info( file.fileName() );
        filename="; filename=\""+info.fileName().toUtf8()
            +"\"\r\nContent-Type: application/octet-stream";
        name=it->id();
        break;
    }
    case PHI::TABLE:
    case PHI::LANG_SELECTOR:
    case PHI::COUNTRY:
    case PHI::COMBO_BOX: {
        QStringList list=it->selectedItems();
        if ( list.isEmpty() ) return;
        name=it->id();
        value=list.first().toUtf8();
        break;
    }
    case PHI::CHECK_LIST:
    case PHI::LIST: {
        QStringList list=it->selectedItems();
        name=it->id();
        QString v;
        foreach ( v, list ) {
            _tmpFile->write( "--"+_boundary+"\r\n"+_disposition+'"'+name+"\"\r\n\r\n" );
            _tmpFile->write( v.toUtf8() );
            _tmpFile->write( "\r\n" );
        }
        return;
    }
    default:
        name=it->id();
    }
    if ( name.isEmpty() ) return;

    _tmpFile->write( "--"+_boundary+"\r\n"+_disposition+'"'+name+'"'+filename+"\r\n\r\n" );
    if ( it->wid()==PHI::FILE_BUTTON ) {
        if ( file.open( QIODevice::ReadOnly ) ) {
            while ( !file.atEnd() ) {
                _tmpFile->write( file.read( static_cast<qint64>(1024*10) ) );
            }
            file.close();
        }
    } else _tmpFile->write( value );
    _tmpFile->write( "\r\n" );
}

void PHIAFormProcessor::slotReplyFinished()
{
    _tmpFile->close();
    this->deleteLater();
}

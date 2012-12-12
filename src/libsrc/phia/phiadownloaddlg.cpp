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
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QProgressBar>
#include <QToolButton>
#include <QDesktopServices>
#include "phiadownloaddlg.h"
#include "phiaconfig.h"

PHIAProgressHandler::PHIAProgressHandler( QObject *parent, PHIADownloadEntry *e, QTreeWidgetItem *it )
    : QObject( parent ), _entry( e ), _it( it )
{
    qDebug( "PHIAProgressHandler::PHIAProgressHandler()" );
    QNetworkReply *reply=e->reply();
    connect( reply, SIGNAL( downloadProgress( qint64, qint64 ) ), this,
        SLOT( slotDownloadProgress( qint64, qint64 ) ) );
    connect( reply, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
    QTreeWidget *tree=it->treeWidget();
    _progressBar=new QProgressBar( tree );
    _progressBar->setMaximum( 100 );
    tree->setItemWidget( it, 2, _progressBar );
    tree->repaint();
}

PHIAProgressHandler::~PHIAProgressHandler()
{
    qDebug( "PHIAProgressHandler::~PHIAProgressHandler()" );
}

void PHIAProgressHandler::slotDownloadProgress( qint64 received, qint64 total )
{
    _entry->setTotalBytes( total );
    _entry->setBytesDownloaded( received );
    _it->setText( 3, QString::number( static_cast<qreal>(total/1024.) ) );
    int prog=1./total*received*100.;
    _progressBar->setValue( prog );
}

void PHIAProgressHandler::slotFinished()
{
    QTreeWidget *tree=_it->treeWidget();
    tree->removeItemWidget( _it, 4 );
    QString text=tr( "Completed" );
    if ( _entry->status()==PHIADownloadEntry::Error ) text=tr( "Error" );
    if ( _entry->status()==PHIADownloadEntry::Canceled ) text=tr( "Aborted" );
    _it->setText( 4, text );
    qDebug( "Reply finished %d", _entry->status() );
}

void PHIAProgressHandler::slotAbortClicked()
{
    _entry->abort();
}

PHIADownloadDlg* PHIADownloadDlg::_instance=0;

PHIADownloadDlg::PHIADownloadDlg( QWidget *parent )
    : QDialog( parent ), _id( 0 )
{
    qDebug( "PHIADownloadDlg::PHIADownloadDlg()" );
    setupUi( this );
    setWindowTitle( tr( "Downloads" ) );
    _tree->setIndentation( 0 );
}

PHIADownloadDlg::~PHIADownloadDlg()
{
    qDebug( "PHIADownloadDlg::~PHIADownloadDlg()" );
}

PHIADownloadDlg* PHIADownloadDlg::instance( QWidget *parent )
{
    if ( _instance ) return _instance;
    _instance=new PHIADownloadDlg( parent );
    return _instance;
}

void PHIADownloadDlg::addDownload( PHIADownloadEntry *entry )
{
    QProgressBar *bar=0;
    QTreeWidgetItem *it=new QTreeWidgetItem();
    it->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    it->setData( 0, Qt::UserRole, _id );
    _entries.insert( _id++, entry );
    QFileInfo info( entry->file() );
    it->setText( 0, info.fileName() );
    it->setText( 1, entry->started().toString() );
    it->setText( 3, QString::number( static_cast<qreal>(entry->totalBytes()/1024.) ) );
    _tree->insertTopLevelItem( 0, it );
    if ( entry->status()==PHIADownloadEntry::Canceled
         || entry->status()==PHIADownloadEntry::Error ) {
        bar=new QProgressBar( _tree );
        _tree->setItemWidget( it, 2, bar );
        bar->setMaximum( 100 );
        bar->setValue( 1./entry->totalBytes()*entry->bytesDownloaded()*100 );
        if ( entry->status()==PHIADownloadEntry::Canceled )
            it->setText( 4, tr( "Aborted" ) );
        else it->setText( 4, tr( "Error" ) );
    } else if ( entry->status()==PHIADownloadEntry::Completed ) {
        bar=new QProgressBar( _tree );
        _tree->setItemWidget( it, 2, bar );
        bar->setMaximum( 100 );
        bar->setValue( 100 );
        it->setText( 4, tr( "Completed" ) );
    } else {
        PHIAProgressHandler *handler=new PHIAProgressHandler( this, entry, it );
        QToolButton *but=new QToolButton( _tree );
        but->setFixedWidth( 24 );
        but->setIcon( QPixmap( ":/gnome/cancel" ) );
        connect( but, SIGNAL( clicked() ), handler, SLOT( slotAbortClicked() ) );
        _tree->setItemWidget( it, 4, but );
    }
    _tree->resizeColumnToContents( 0 );
    _tree->resizeColumnToContents( 3 );
}

void PHIADownloadDlg::on__clear_clicked()
{
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Clear list" ),
        tr( "This will also stop your current downloads. Are you sure?" ),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
    if ( res==QMessageBox::No ) return;
    emit clearClicked();
    _tree->clear();
    _entries.clear();
    _id=0;
}

void PHIADownloadDlg::on__tree_itemDoubleClicked( QTreeWidgetItem *item, int column )
{
    Q_UNUSED( column );
    if ( !item ) return;
    qint32 id=item->data( 0, Qt::UserRole ).value<qint32>();
    PHIADownloadEntry *entry=_entries.value( id );
    if ( entry->status()!=PHIADownloadEntry::Completed ) {
        QMessageBox::information( this, tr( "Open file" ),
            tr( "Only completely downloaded files can be opened."),
            QMessageBox::Ok );
        return;
    }
    QUrl url( entry->file() );
    url.setScheme( "file" );
    QDesktopServices::openUrl( url );
    qDebug( "entry file=%s", qPrintable( entry->file() ) );
}

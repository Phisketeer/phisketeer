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
#ifndef PHIADOWNLOADDLG_H
#define PHIADOWNLOADDLG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QNetworkReply>
#include <QProgressBar>
#include "ui_downloaddlg.h"

class PHIADownloadEntry;

class PHIAProgressHandler : public QObject
{
    Q_OBJECT

public:
    PHIAProgressHandler( QObject *parent, PHIADownloadEntry *e, QTreeWidgetItem *it );
    virtual ~PHIAProgressHandler();

public slots:
    void slotAbortClicked();

protected slots:
    void slotDownloadProgress( qint64 received, qint64 total );
    void slotFinished();

private:
    QProgressBar *_progressBar;
    PHIADownloadEntry *_entry;
    QTreeWidgetItem *_it;
};

class PHIADownloadDlg : public QDialog, private Ui::DownloadDlg
{
    Q_OBJECT

public:
    static PHIADownloadDlg* instance( QWidget *parent=0 );
    void addDownload( PHIADownloadEntry *entry );

protected:
    PHIADownloadDlg( QWidget *parent );
    virtual ~PHIADownloadDlg();

protected slots:
    void on__clear_clicked();
    void on__tree_itemDoubleClicked( QTreeWidgetItem *item, int column );

signals:
    void clearClicked();

private:
    qint32 _id;
    static PHIADownloadDlg *_instance;
    QHash <qint32, PHIADownloadEntry*> _entries;
};

#endif // PHIADOWNLOADDLG_H

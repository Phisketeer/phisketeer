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
#ifndef AMPTOOLS_H
#define AMPTOOLS_H

#include <QComboBox>
#include <QTabWidget>
#include <QDialog>
#include <QMutex>
#include <QMutexLocker>
#include <QIcon>
#include <QUrl>
#include "ampwebview.h"
#include "phiawebview.h"

class AMPUrlCombo : public QComboBox
{
    Q_OBJECT

public:
    AMPUrlCombo( QWidget *parent=0 );
    virtual ~AMPUrlCombo();

public slots:
    void slotHistoryChanged();
    void slotSetCurrentUrl( const QUrl& );

protected:
    QString _urlString;
};

class AMPTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    AMPTabWidget( QWidget *parent=0 );
    virtual ~AMPTabWidget();
    void newTabRequested( PHIAAbstractWebView *view=0, int index=-1 );

public slots:
    void slotTabCloseRequested( int );

protected:
    int tabIdForView( PHIAAbstractWebView* );
    void connectSignals( PHIAAbstractWebView* );
    void downloadUnsupportedContent( QNetworkReply *reply );

protected slots:
    void slotCurrentTabChanged( int );
    void slotTitleChanged( PHIAAbstractWebView* );
    void slotIconChanged( PHIAAbstractWebView* );
    void slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* );
};

#endif // AMPTOOLS_H

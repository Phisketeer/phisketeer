/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
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

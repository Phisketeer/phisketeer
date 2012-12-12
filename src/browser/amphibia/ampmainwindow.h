/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
#ifndef AMPMAINWINDOW_H
#define AMPMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QProgressBar>
#include <QToolButton>
#include <QSslConfiguration>
#include "ampwebview.h"
#include "amptools.h"
#include "phianetmanager.h"
#include "phiawebview.h"
#include "phiajavascriptlog.h"
#include "ui_mainwindow.h"

class AMPMainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    AMPMainWindow( QWidget *parent=0 );
    virtual ~AMPMainWindow();
    void setCurrentView( PHIAAbstractWebView *view );
    inline PHIAAbstractWebView* currentView() const { return _currentView; }
    inline AMPTabWidget* tabWidget() const { return _tabWidget; }
    inline QProgressBar* progressBar() const { return _progressBar; }
    inline QNetworkAccessManager* networkAccessManager() const { return _netManager->networkAccessManager(); }

public slots:
    void slotUrlComboActivated( const QString& );

protected slots:
    void on_actionSaveAs_triggered();
    void on_actionSettings_triggered();
    void on_actionClose_triggered();
    void on_actionOpenFile_triggered();
    void on_actionNewTab_triggered();
    void on_actionQuit_triggered();
    void on_actionBack_triggered();
    void on_actionForward_triggered();
    void on_actionReload_triggered();
    void on_actionStop_triggered();
    void on_actionSource_triggered();
    void on_actionNewWindow_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomNormal_triggered();
    void on_actionPrint_triggered();
    void on_actionPrintPreview_triggered();
    void on_actionSendLink_triggered();
    void on_actionOpenUrl_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionHelp_triggered();
    void on_actionAbout_triggered();
    void on_actionHome_triggered();
    void on_actionFullScreen_triggered();
    void on_actionCookies_triggered();
    void on_actionDownloads_triggered();
    void on_actionConsole_triggered();

    void slotInit();
    void slotJavaScriptConsoleMessage( const QString&, int, const QString& );
    void slotPageLoading( bool );
    void slotLinkHovered( const QString&, const QString&, const QString& );
    void slotSslToolClicked();
    void slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth );

protected:
    virtual void closeEvent( QCloseEvent* );

private:
    AMPTabWidget *_tabWidget;
    PHIAAbstractWebView *_currentView;
    PHIANetManager *_netManager;
    AMPUrlCombo *_urlCombo;
    QProgressBar *_progressBar;
    QToolButton *_sslTool;
    PHIAJavaScriptLog *_jsLog;
};

#endif // AMPMAINWINDOW_H

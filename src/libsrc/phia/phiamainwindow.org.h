/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
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
#ifndef PHIAMAINWINDOW_H
#define PHIAMAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QToolButton>
#include <QNetworkProxy>
#include <QAuthenticator>
#include "phiaabstractwebview.h"
#include "ui_mainwindow.h"
#include "phia.h"
#include "phiajavascriptlog.h"

class PHIAEXPORT PHIAMainWindow : public QMainWindow, private Ui::MainWindow
{    
    Q_OBJECT

public:
    PHIAMainWindow( const QString &url, QWidget *parent=0 );
    virtual ~PHIAMainWindow();
    inline QMenu* phiMenu() const { return menuPhi; }
    static PHIAMainWindow* createWindow( const QString &url, const QUrl &referer,
        const QString &target, const QString &options );

public slots:
    void slotSetUrl( const QString &url );
    void slotSetUrl( const QUrl &url );

protected slots:
    void slotTitleChanged( PHIAAbstractWebView* );
    void slotIconChanged( PHIAAbstractWebView* );
    void slotProgressMaximum( int );
    void slotLoadProgress( int );
    void slotLoading( bool );
    void slotStatusBarMessage( const QString&, int timeout=0 );
    void slotWindowCloseRequested();
    void slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* );
    void slotSslToolClicked();
    void slotUpdateGeometry( const QRect &geom );
    void slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth );
    void slotShowStatusBar( bool );
    void slotShowToolBar( bool );
    void slotShowMenu( bool );
    void slotShowScrollBars( bool );
    void slotResizable( bool );
    void slotJavaScriptConsoleMessage( const QString&, int, const QString& );

    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSettings_triggered();
    void on_actionQuit_triggered();
    void on_actionPrint_triggered();
    void on_actionPrintPreview_triggered();
    void on_actionZoom_in_triggered();
    void on_actionZoom_out_triggered();
    void on_actionZoom_normal_triggered();
    void on_actionCookies_triggered();
    void on_actionDownloads_triggered();
    void on_actionConsole_triggered();

protected:
    virtual void closeEvent( QCloseEvent *e );
    void setWebView( PHIAAbstractWebView* );
    PHIAMainWindow();

protected slots:
/*
    void iconChanged( PHIAAbstractWebView* );
    void titleChanged( PHIAAbstractWebView* );
    void unsupportedContent( PHIAAbstractWebView*, QNetworkReply* );
    void unsupportedFile( PHIAAbstractWebView*, const QByteArray& );
    void urlChanged( const QUrl& );
    void loadProgress( int );
    void progressMaximum( int );
    void windowCloseRequested();
    void loading( bool );
    void linkHovered( const QString &link, const QString &title, const QString &textContent );
    void backAvailable( bool );
    void forwardAvailable( bool );
    void homeRequested();
    void menuBarVisibilityChangeRequested( bool visible );
    void microFocusChanged();
    void scrollRequested( int dx, int dy, const QRect & rectToScroll );
    void selectionChanged();
    void statusBarVisibilityChangeRequested( bool visible );
    void toolBarVisibilityChangeRequested( bool visible );
    void javaScriptConsoleMessage( const QString &msg, int lineNumber, const QString& sourceId );
    void geometryChangeRequested( const QRect &geom );
*/

private:
    PHIAAbstractWebView *_view;
    QProgressBar *_progressBar;
    QToolButton *_sslTool;
    bool _saveGeometry;
    PHIAJavaScriptLog *_jsLog;
    static QHash <QString, PHIAMainWindow*> _activeWindows;
};

#endif // PHIAMAINWINDOW_H

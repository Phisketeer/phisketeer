/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIAAPPWINDOW_H
#define PHIAAPPWINDOW_H
#include <QMainWindow>
#include "phiaabstractwebview.h"
#include "phia.h"

class QProgressBar;
class QPlainTextEdit;

class PHIAEXPORT PHIAAppWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PHIAAppWindow( QWidget *parent=0 );
    virtual ~PHIAAppWindow();
    void setUrl( const QUrl &url );

protected slots:
    void slotFaviconChanged( PHIAAbstractWebView *view );
    void slotTitleChanged( PHIAAbstractWebView *view );
    void slotUnsupportedContent( PHIAAbstractWebView *view, const QUrl &url );
    void slotShowStatusBar( bool b );
    void slotGeometryChangeRequested( const QRect &geom );
    void slotWindowCloseRequested();
    void slotOpenWindowRequested( const QString &link, const QUrl &ref, const QString &target, const QString &opts );
    void slotIsLoading( bool b );
    void slotLoadProgress( qint64 progress, qint64 max );
    void slotStatusBarMessage( const QString &s, int timeout );
    void slotShowJSConsole();
    void slotJSMessage( const QString &error, int linenumber, const QString &id );

protected:
    void connectSignals( PHIAAbstractWebView *view );

private:
    PHIAAbstractWebView *_view;
    QProgressBar *_progress;
    QWidget *_jsWidget;
    QPlainTextEdit *_jsConsole;
};

#endif // PHIAAPPWINDOW_H

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
#ifndef PHIAWEBVIEW_H
#define PHIAWEBVIEW_H
#include "phiaabstractwebview.h"
#include "phia.h"

class PHIGraphicsView;
class PHIAGraphicsScene;
class QScriptEngine;
class QScriptValue;

class PHIAEXPORT PHIAWebView : public PHIAAbstractWebView
{
    Q_OBJECT

public:
    explicit PHIAWebView( QWidget *parent=0 );
    PHIAGraphicsScene* scene() const;
    void throwJavaScriptError( const QScriptValue &err );

    virtual void setScrollBarsEnabled( bool enable );
    virtual QString title() const;
    virtual QIcon icon() const;
    virtual QString source() const;
    virtual void renderDocument( QPainter* );
    virtual QUrl url() const;
    virtual void setUrl( const QUrl &url );
    virtual QScriptEngine* scriptEngine() const;
    virtual QSslConfiguration sslConfiguration() const;

public slots:
    void slotCreateWindow( const QString &url, const QString &target, const QString &opts );
    void slotLinkRequested( const QUrl &url );

    virtual void slotBack();
    virtual void slotForward();
    virtual void slotStop();
    virtual void slotReload();
    virtual void slotZoomIn();
    virtual void slotZoomOut();
    virtual void slotZoomNormal();
    virtual void slotSetZoomFactor( qreal );
    virtual void slotPrint( QPrinter *printer );

protected slots:
    void slotIconChanged( const QIcon &icon );
    void slotTitleChanged( const QString &title );
    void slotPageSizeChanged();

private:
    PHIGraphicsView *_view;
    qreal _zoomFactor;
};

#endif // PHIAWEBVIEW_H

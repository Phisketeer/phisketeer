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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiaappwindow.h"
#include "phiawebview.h"

PHIAAppWindow::PHIAAppWindow( QWidget *parent )
    : QMainWindow( parent )
{
    _view=new PHIAWebView( this );
    setCentralWidget( _view );
    connectSignals( _view );
}

void PHIAAppWindow::connectSignals( PHIAAbstractWebView *view )
{
    connect( view, &PHIAAbstractWebView::iconChanged, this, &PHIAAppWindow::slotFaviconChanged );
    connect( view, &PHIAAbstractWebView::titleChanged, this, &PHIAAppWindow::slotTitleChanged );
}

void PHIAAppWindow::setUrl( const QUrl &url )
{
    _view->setUrl( url );
}

void PHIAAppWindow::slotFaviconChanged( PHIAAbstractWebView *view )
{
    setWindowIcon( view->icon() );
}

void PHIAAppWindow::slotTitleChanged(PHIAAbstractWebView *view )
{
    setWindowTitle( view->title() );
}

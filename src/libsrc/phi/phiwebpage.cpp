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
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QWebFrame>
#include <QWebSettings>
#include "phiwebpage.h"
#include "phinetmanager.h"

PHIWebPage::PHIWebPage( QObject *parent )
    : QWebPage( parent ), _loading( false )
{
    setNetworkAccessManager( PHINetManager::instance()->defaultNetworkAccessManager() );
    mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
    mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    connect( this, &QWebPage::loadStarted, this, &PHIWebPage::slotLoadingStart );
    connect( this, &QWebPage::loadFinished, this, &PHIWebPage::slotLoadingFinished );
}

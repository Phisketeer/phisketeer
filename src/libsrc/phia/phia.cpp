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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phia.h"
#include "phi.h"

const QString& PHIA::libVersion()
{
    static QString ver( L1( PHIAVERSION ) );
    return ver;
}

qreal PHIA::zoomFactor()
{
    return static_cast<qreal>(.02);
}

/*
QString PHIA::browserName()
{
    return QString( "Amphibia" );
}

QString PHIA::viewerName()
{
    return QString( "PhiApp" );
}

QString PHIA::phiaDir()
{
    return QString( "phia" );
}

QString PHIA::configName( PHIA::Config config )
{
    QString s;
    switch ( config ) {
    case CacheDirectory: s="CacheDirectory"; break;
    case MaxCacheSize: s="MaxCacheSize"; break;
    case PrintBackgrounds: s="PrintBackgrounds"; break;
    case AutoLoadImages: s="AutoLoadImages"; break;
    case EnableJava: s="EnableJava"; break;
    case EnableJavascript: s="EnableJavascript"; break;
    case Geometry: s="Geometry"; break;
    case DockWidgets: s="DockWidgets"; break;
    case MaxHistoryItems: s="MaxHistoryItems"; break;
    case JSOpenWindow: s="JSOpenWindow"; break;
    case ProxyHost: s="ProxyHost"; break;
    case ProxyPort: s="ProxyPort"; break;
    case DownloadDirectory: s="DownloadDirectory"; break;
    case ConfigFile: s="ConfigFile"; break;
    case LastUpdateCheck: s="LastUpdateCheck"; break;
    case EnablePlugins: s="EnablePlugins"; break;
    default: s="Unknown";
    }
    return s;
}

void PHIA::setContainerLineEdit( const PHIAItem *it, int row, const QString &text )
{
    if ( !PHI::isWidgetContainer( static_cast<PHI::Widget>(it->wid()) ) ) return;
    // get the layout
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(it->graphicsItem());
    Q_ASSERT( l );
    QList <QLineEdit*> children=l->lineEditItems();
    if ( row>=children.count() ) return;
    children.at( row )->setText( text );
}

QLineEdit* PHIA::containerLineEdit( const PHIAItem *it, int row )
{
    if ( !PHI::isWidgetContainer( static_cast<PHI::Widget>(it->wid()) ) ) return 0;
    // get the layout
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(it->graphicsItem());
    Q_ASSERT( l );
    QList <QLineEdit*> children=l->lineEditItems();
    if ( row>=children.count() ) return 0;
    return children.at( row );
}

void PHIA::setContainerComboBox( const PHIAItem *it, int row, const QString &text )
{
    if ( !PHI::isWidgetContainer( static_cast<PHI::Widget>(it->wid()) ) ) return;
    // get the layout
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(it->graphicsItem());
    Q_ASSERT( l );
    QList <QComboBox*> children=l->comboBoxItems();
    if ( row>=children.count() ) return;
    QComboBox *combo=children.at( row );
    Q_ASSERT( combo );
    int i=combo->findText( text, Qt::MatchFixedString );
    if ( i==-1 ) combo->setCurrentIndex( 0 );
    else combo->setCurrentIndex( i );
}

void PHIA::setContainerComboBoxData( const PHIAItem *it, int row, const QString &text )
{
    if ( !PHI::isWidgetContainer( static_cast<PHI::Widget>(it->wid()) ) ) return;
    // get the layout
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(it->graphicsItem());
    Q_ASSERT( l );
    QList <QComboBox*> children=l->comboBoxItems();
    if ( row>=children.count() ) return;
    QComboBox *combo=children.at( row );
    Q_ASSERT( combo );
    int i=combo->findData( text, Qt::UserRole, Qt::MatchFixedString );
    for ( int c=0; c<combo->count(); c++ ) qDebug( "COMBO %s",
        qPrintable( combo->itemData( c, Qt::UserRole ).toString() ) );
    if ( i==-1 ) combo->setCurrentIndex( 0 );
    else combo->setCurrentIndex( i );
}

QComboBox* PHIA::containerComboBox( const PHIAItem *it, int row )
{
    if ( !PHI::isWidgetContainer( static_cast<PHI::Widget>(it->wid()) ) ) return 0;
    // get the layout
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(it->graphicsItem());
    Q_ASSERT( l );
    QList <QComboBox*> children=l->comboBoxItems();
    if ( row>=children.count() ) return 0;
    return children.at( row );
}
*/

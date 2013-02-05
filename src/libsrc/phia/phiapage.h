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
#ifndef PHIAPAGE_H
#define PHIAPAGE_H

#include <QDataStream>
#include <QIcon>
#include <QImage>
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phia.h"

class PHIAPage : public PHIBasePage
{    
    Q_OBJECT
    Q_DISABLE_COPY( PHIAPage )

    Q_PROPERTY( QString title WRITE setTitle READ title )
    Q_PROPERTY( QString author READ author )
    Q_PROPERTY( QString company READ company )
    Q_PROPERTY( QString version READ version )
    Q_PROPERTY( QString action READ action )
    Q_PROPERTY( QString copyright READ copyright )
    Q_PROPERTY( QString javascript READ javascript )
    Q_PROPERTY( QString styleSheet READ styleSheet )
    Q_PROPERTY( QString bgColor READ bgColor )
    Q_PROPERTY( QString keywords READ keywords )
    Q_PROPERTY( QString sessionRedirect READ sessionRedirect )
    Q_PROPERTY( QString templatePage READ templatePage )
    Q_PROPERTY( QString id READ id )
    Q_PROPERTY( quint16 itemCount READ itemCount )
    Q_PROPERTY( QString session READ session )
    Q_PROPERTY( quint32 width READ width )
    Q_PROPERTY( quint32 height READ height )
    Q_PROPERTY( QString fontFamily READ fontFamily )
    Q_PROPERTY( QStringList itemIds READ itemIds )
    Q_PROPERTY( QStringList properties READ properties )
    // provided languages by current page
    Q_PROPERTY( QStringList languages READ languages )
    // default lang (initialized with philang or first matching lang from accepted languages of the browser)
    Q_PROPERTY( QString lang READ lang )

    friend PHIAEXPORT QDataStream& operator<<( QDataStream&, const PHIAPage* );
    friend PHIAEXPORT QDataStream& operator>>( QDataStream&, PHIAPage* );

public:
    PHIAPage( QObject *parent=0 );
    virtual ~PHIAPage();
    QIcon icon() const;
    void clearItems();
    inline QList <PHIPageMenuEntry> menuEntries() const { return _menuEntries; }
    inline virtual PHIBaseItem* createElementById( quint16, const QString&,
         qreal, qreal, qreal=0, qreal=0 ) { return 0; } // disable
    inline virtual void removeElementById( const QString& ) { return; } // disable
    QList <PHIBaseItem*> childrenOf( const QString &parent ) const;

public slots:
    inline virtual PHIBaseItem* getElementById( const QString &id ) const
        { return _items.value( id.toUtf8() ); }
    virtual void setTitle( const QString& );

signals:
    void titleChanged();
    void iconChanged();
};

PHIAEXPORT QDataStream& operator<<( QDataStream&, const PHIAPage* );
PHIAEXPORT QDataStream& operator>>( QDataStream&, PHIAPage* );

#endif // PHIAPAGE_H

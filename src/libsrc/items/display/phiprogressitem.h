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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIPROGRESSITEM_H
#define PHIPROGRESSITEM_H
#include "phibaseitem.h"

class PHIProgressItem : public PHIBaseItem
{
    Q_OBJECT
    Q_PROPERTY( quint8 _progress READ realProgress WRITE setProgress SCRIPTABLE false )

public:
    enum ItemData { DProgress=1 };
    enum Wid { Progress=59 };
    explicit PHIProgressItem( const PHIBaseItemPrivate &p ) : PHIBaseItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIProgressItem( const PHIProgressItem &it ) : PHIBaseItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIProgressItem() {}

    virtual PHIWID wid() const { return Progress; }
    virtual QString listName() const { return tr( "Progress bar" ); }
    virtual QString description() const { return tr( "Displays a progress bar." ); }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/lineedit" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &headerOut ) const;

    inline quint8 realProgress() const { return data( DProgress ).value<quint8>(); }
    inline void setProgress( quint8 p ) { setData( DProgress, qBound( static_cast<quint8>(0), p, static_cast<quint8>(100) ) ); updateWidget(); }

public slots:
    QScriptValue progress( const QScriptValue &v=QScriptValue() );
    inline QScriptValue val( const QScriptValue &v=QScriptValue() ) { return progress( v ); }

protected:
    void ideInit();
    QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void updateWidget();

private:
    void initWidget();
};

#endif // PHIPROGRESSITEM_H

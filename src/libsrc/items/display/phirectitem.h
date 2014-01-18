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
#ifndef PHIRECTITEM_H
#define PHIRECTITEM_H
#include "phiabstractitems.h"
#include "phicolorconfig.h"
#include "phidatasources.h"

class QLabel;
class QSpinBox;

class PHIRectConfig : public PHIColorConfig
{
    Q_OBJECT

public:
    explicit PHIRectConfig( PHIBaseItem *it, QWidget *parent=0 );
    virtual ~PHIRectConfig();
    virtual bool storeData();
    virtual PHIConfigData originalData() const;

protected slots:
    void radiusSpinChanged( int );
    void radiusToolClicked();

private:
    QLabel *_radiusLabel;
    QSpinBox *_radiusSpin;
    QToolButton *_radiusTool;
    PHIIntData *_oldRadiusData;
};

class PHIRectItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( qint16 _borderRadius READ realBorderRadius WRITE setBorderRadius SCRIPTABLE false )

public:
    enum ItemData { DBorderRadius=1 };
    enum Wid { Rect=14 };
    explicit PHIRectItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ) {}
    PHIRectItem( const PHIRectItem &it ) : PHIAbstractShapeItem( it ), _radiusData( it._radiusData ) {}
    virtual ~PHIRectItem() {}

    inline virtual PHIWID wid() const { return Rect; }
    inline PHIIntData* radiusData() { return &_radiusData; }
    virtual QString listName() const { return tr( "Rect" ); }
    virtual QString description() const { return tr( "Draws a box or rect with optional rounded courners." ); }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/rect" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    inline virtual qint16 realBorderRadius() const { return data( DBorderRadius, 0 ).value<qint16>(); }
    inline virtual void setBorderRadius( qint16 r ) { setData( DBorderRadius, r ); update(); }

public slots:
    QScriptValue borderRadius( const QScriptValue &r=QScriptValue() );

protected:
    virtual PHIConfigWidget* ideConfigWidget();
    virtual void drawShape( QPainter *painter, const QRectF &r );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void squeeze();
    virtual void ideUpdateData();
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void phisCreateData( const PHIDataParser &parser);

private:
    virtual PHIIntData* intData_1() { return &_radiusData; }

private:
    PHIIntData _radiusData;
};

class PHIRoundedRectItem : public PHIRectItem
{
    Q_OBJECT

public:
    enum Wid { RoundedRect=25 };
    explicit PHIRoundedRectItem( const PHIBaseItemPrivate &p ) : PHIRectItem( p ) {}
    PHIRoundedRectItem( const PHIRoundedRectItem &it ) : PHIRectItem( it ) {}
    virtual ~PHIRoundedRectItem() {}

    inline virtual PHIWID wid() const { return Rect; }
    virtual QString listName() const { return tr( "Rounded rect" ); }
    virtual QString description() const { return tr( "Draws a rect with rounded courners." ); }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/roundedrect" ) ); }
    virtual bool isPrivateItem() const { return true; }
};

#endif // PHIRECTITEM_H

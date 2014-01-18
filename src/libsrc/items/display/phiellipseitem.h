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
#ifndef PHIELLIPSEITEM_H
#define PHIELLIPSEITEM_H
#include "phiabstractitems.h"
#include "phicolorconfig.h"
#include "phidatasources.h"

class QLabel;
class QSpinBox;
class QToolButton;

class PHIEllipseConfig : public PHIColorConfig
{
    Q_OBJECT

public:
    explicit PHIEllipseConfig( PHIBaseItem *it, QWidget *parent=0 );
    virtual ~PHIEllipseConfig();
    virtual bool storeData();
    virtual PHIConfigData originalData() const;

protected slots:
    void startSpinChanged( int );
    void spanSpinChanged( int );
    void startToolClicked();
    void spanToolClicked();

private:
    QLabel *_startLabel, *_spanLabel;
    QSpinBox *_startSpin, *_spanSpin;
    QToolButton *_startTool, *_spanTool;
    PHIIntData *_oldStartData, *_oldSpanData;
};

class PHIEllipseItem : public PHIAbstractShapeItem
{
    Q_OBJECT
    Q_PROPERTY( int _startAngle READ realStartAngle WRITE setStartAngle SCRIPTABLE false )
    Q_PROPERTY( int _spanAngle READ realSpanAngle WRITE setSpanAngle SCRIPTABLE false )

public:
    enum ItemData { DStartAngle=1, DSpanAngle=2 };
    enum Wid { Ellipse=13 };
    explicit PHIEllipseItem( const PHIBaseItemPrivate &p ) : PHIAbstractShapeItem( p ) {}
    PHIEllipseItem( const PHIEllipseItem &it ) : PHIAbstractShapeItem( it ),
        _startData( it._startData ), _spanData( it._spanData ) {}
    virtual ~PHIEllipseItem() {}

    inline virtual PHIWID wid() const { return Ellipse; }
    virtual QString listName() const { return tr( "Ellipse" ); }
    virtual QString description() const { return tr( "Draws an ellipse, pie or circle" ); }
    virtual QPixmap pixmap() const { return QPixmap( L1( ":/items/ellipse" ) ); }
    inline PHIIntData* startData() { return &_startData; }
    inline PHIIntData* spanData() { return &_spanData; }
    virtual PHIIntData* intData_1() { return &_startData; }
    virtual PHIIntData* intData_2() { return &_spanData; }
    virtual void ideInit();

    inline void setStartAngle( int a ) { setData( DStartAngle, a ); update(); }
    inline void setSpanAngle( int a ) { setData( DSpanAngle, a ); update(); }
    inline int realStartAngle() const { return data( DStartAngle, 0 ).toInt(); }
    inline int realSpanAngle() const { return data( DSpanAngle, 5760 ).toInt(); }

protected:
    virtual void ideUpdateData();
    virtual void saveItemData( QDataStream &out, int version );
    virtual void loadItemData( QDataStream &in, int version );
    virtual void squeeze();
    virtual PHIConfigWidget* ideConfigWidget();
    virtual void drawShape( QPainter *painter, const QRectF &r );
    virtual QRectF boundingRect() const;
    virtual void phisCreateData( const PHIDataParser &parser );
    virtual void phisParseData( const PHIDataParser &parser );

signals:
    void startAngleChanged( int );
    void spanAngleChanged( int );

private:
    PHIIntData _startData, _spanData;
};

#endif // PHIELLIPSEITEM_H

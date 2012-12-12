/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef PHISITEM_H
#define PHISITEM_H

#include <QObject>
#include "phiitem.h"
#include "phidatasources.h"
#include "phisitemdata.h"

class PHISItem : public QObject, public PHIItem
{
    Q_OBJECT

public:
    PHISItem( QObject *parent=0 );
    virtual ~PHISItem();
    virtual void loadFromByteArray( quint16 wid, const QByteArray &id, const QByteArray &arr );
    inline const PHIImageData* imageData() const { return _d.imageData(); }
    inline const PHITextData* textData() const { return _d.textData(); }
    inline const PHITextData* stylesheetData() const { return _d.styleSheetData(); }
    inline const PHITextData* toolTipData() const { return _d.toolTipData(); }
    inline const PHIBooleanData* disabledData() const { return _d.disabledData(); }
    inline const PHIBooleanData* visibleData() const { return _d.visibleData(); }
    inline const PHIBooleanData* checkableData() const { return _d.checkedData(); }
    inline const PHIBooleanData* readOnlyData() const { return _d.readOnlyData(); }
    inline const PHIIntData* startAngleData() const { return _d.startAngleData(); }
    inline const PHIIntData* spanAngleData() const { return _d.spanAngleData(); }
    inline const PHIImageBookData* imageBookData() const { return _d.imageBookData(); }

    inline QString id() const { return QString::fromUtf8( _id ); }
    inline QString parent() const { return QString::fromUtf8( _parent ); }
    inline const QByteArray effectData() const { return _effectData; }

    inline void setTemplateItem( bool b ) { b ? _attributes |= PHIItem::ATemplateItem :
        _attributes &= ~PHIItem::ATemplateItem; }
    inline void setTemplatePageId( const QByteArray &id ) { _templatePageId=id; }
    inline QByteArray templatePageId() const { return _templatePageId; }
    inline bool isTemplateItem() const { return _attributes & PHIItem::ATemplateItem; }
    bool isDragDrop() const;
    /*
    inline QColor color() const { return _d.color(); }
    inline QColor outlineColor() const { return _d.outlineColor(); }
    inline QFont font() const { return _d.font(); }
    inline qreal penWidth() const { return _d.penWidth(); }
    inline quint8 line() const { return _d.line(); }
    inline quint8 pattern() const { return _d.pattern(); }
    inline quint8 alignment() const { return _d.alignment; }
    */
    //inline QPalette palette() const { return _d.palette(); }

private:
    PHISItemData _d;
    QByteArray _templatePageId;
};

#endif // PHISITEM_H

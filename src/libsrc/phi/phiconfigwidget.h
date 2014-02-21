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
#ifndef PHICONFIGWIDGET_H
#define PHICONFIGWIDGET_H
#include <QWidget>
#include <QList>
#include <QByteArray>
#include "phi.h"
#include "phipalette.h"

class PHIBaseItem;
class QToolBar;
class PHIData;
class PHIImageData;
class PHIImageBookData;

typedef QHash<QByteArray, QVariant> PHIConfigProperties;
typedef QHash<PHIPalette::ItemRole, QPair<PHIPalette::ColorRole, QColor> > PHIConfigColors;
typedef QHash<PHIData*, PHIData*> PHIConfigData;

class PHIEXPORT PHIConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PHIConfigWidget( PHIBaseItem *it, QWidget *parent=0 ) : QWidget( parent ), _item( it ) {;}
    virtual QToolBar* toolBar() const { return 0; }
    virtual bool storeData()=0; // return false if data is unchanged
    // properties which are needed to be updated in undo stack and are NOT stored in _variants:
    virtual PHIConfigProperties originalProperties() const { return PHIConfigProperties(); }
    virtual PHIConfigColors originalColors() const { return PHIConfigColors(); }
    // PHIConfData: QHash <PHIData *ref, PHIData *orgData>
    virtual PHIConfigData originalData() const { return PHIConfigData(); }

signals:
    void requestTextConfig( PHIData* );
    void requestImageConfig( PHIImageData* );
    void requestImageBookConfig( PHIImageBookData* );

protected:
    inline PHIBaseItem* item() const { return _item; }

private:
    PHIBaseItem *_item;
};

#endif // PHICONFIGWIDGET_H

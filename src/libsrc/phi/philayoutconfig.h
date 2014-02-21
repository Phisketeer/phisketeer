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
#ifndef PHILAYOUTCONFIG_H
#define PHILAYOUTCONFIG_H
#include <QWidget>
#include "phicolorconfig.h"
#include "phi.h"
#include "ui_layoutconfig.h"

class PHIAbstractLayoutItem;

class PHIEXPORT PHILayoutWidget : public QWidget, Ui::LayoutConfig
{
    Q_OBJECT

public:
    explicit PHILayoutWidget( PHIAbstractLayoutItem *item, QWidget *parent=0 );
    virtual void storeData();

private slots:
    void init();

signals:
    void enableHeaderChanged( bool );

private:
    PHIAbstractLayoutItem *_item;
};

class PHIEXPORT PHILayoutConfig : public PHIColorConfig
{
    Q_OBJECT

public:
    explicit PHILayoutConfig( PHIAbstractLayoutItem *item, QWidget *parent=0 );
    virtual bool storeData();

signals:
    void enableHeaderChanged( bool );

private:
    PHILayoutWidget *_layoutWidget;
};

#endif // PHILAYOUTCONFIG_H

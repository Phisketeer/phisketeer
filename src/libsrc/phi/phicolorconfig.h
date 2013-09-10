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
#ifndef PHICOLORCONFIG_H
#define PHICOLORCONFIG_H
#include <QWidget>
#include "phiconfigwidget.h"
#include "phipalette.h"
#include "ui_colorconfig.h"

class PHIBaseItem;
class QPaintEvent;
class QPlainTextEdit;
class QGroupBox;
class QLabel;

class PHIColorPreview : public QWidget
{
    Q_OBJECT

public:
    explicit PHIColorPreview( PHIBaseItem *it, QWidget *parent=0 );

protected:
    void paintEvent( QPaintEvent *e );

private:
    PHIBaseItem *_item;
    QPlainTextEdit *_edit;
};

class PHIColorConfig : public PHIConfigWidget, protected Ui::ColorConfig
{
    Q_OBJECT

public:
    explicit PHIColorConfig( PHIBaseItem *item, QWidget *parent=0 );
    virtual ~PHIColorConfig();
    virtual bool storeData();
    virtual PHIConfigColors oldColors() const;

protected slots:
    void on__colorButton_clicked();
    void on__outlineColorButton_clicked();

protected:
    QList <PHIPalette::ItemRole> _itemRoles;
    QHash <PHIPalette::ItemRole, QColor> _oldColors;
    QHash <PHIPalette::ItemRole, PHIPalette::ColorRole> _oldRoles;
};

#endif // PHICOLORCONFIG_H

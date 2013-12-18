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
#ifndef PHICOMBOBOX_H
#define PHICOMBOBOX_H
#include <QComboBox>

class QEvent;
class QKeyEvent;
class QMenu;
class PHIBaseItem;

class PHIComboBox : public QComboBox
{
    Q_OBJECT

public:
    PHIComboBox( PHIBaseItem *it, QWidget *parent=0 );
    virtual ~PHIComboBox();

protected:
    virtual bool event( QEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void showPopup();


private:
    PHIBaseItem *_it;
    QMenu *_menu;
};

#endif // PHICOMBOBOX_H

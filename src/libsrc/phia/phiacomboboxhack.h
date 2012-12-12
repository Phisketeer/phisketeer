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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIACOMBOBOXHACK_H
#define PHIACOMBOBOXHACK_H

#include <QComboBox>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QEvent>

class PHIAItem;

class PHIAComboButton : public QToolButton
{
    Q_OBJECT

public:
    PHIAComboButton( QWidget *parent=0 );
    virtual ~PHIAComboButton();

public slots:
    void setItems( const QStringList &list, int selected );

signals:
    void selectedAction( QAction *act );

private:
    QMenu *_popup;
};


class PHIAComboBox : public QComboBox
{
    Q_OBJECT

public:
    PHIAComboBox( const PHIAItem*, QWidget *parent=0 );
    virtual ~PHIAComboBox();

protected:
    virtual bool event( QEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    void showPopup();

protected slots:
    void updateItem( QAction* );

private:
    PHIAComboButton *_toolButton;
    const PHIAItem *_item;
};

#endif // PHIACOMBOBOXHACK_H

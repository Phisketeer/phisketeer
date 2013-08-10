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
#include <QLineEdit>
#include <QPlainTextEdit>
#include "phiinputitems.h"

PHILineEditItem::PHILineEditItem( Type type, PHIBasePage *page )
    : PHIAbstractInputItem( type, page )
{
    QLineEdit *edit=new QLineEdit();
    setWidget( edit );
}

void PHILineEditItem::setWidgetText( const QString &s )
{
    QLineEdit *edit=qobject_cast<QLineEdit*>(widget());
    Q_ASSERT( edit );
    edit->setText( s );
}

PHITextAreaItem::PHITextAreaItem( Type type, PHIBasePage *page )
    : PHIAbstractInputItem( type, page )
{
    QPlainTextEdit *edit=new QPlainTextEdit();
    if ( isIdeItem() ) edit->setReadOnly( true );
#ifdef Q_OS_MAC
    edit->setForegroundRole( QPalette::Button );
    edit->setFrameStyle( QFrame::Box );
#endif
    setWidget( edit );
}

void PHITextAreaItem::setWidgetText( const QString &t )
{
    QPlainTextEdit *edit=qobject_cast<QPlainTextEdit*>(widget());
    Q_ASSERT( edit );
    edit->setPlainText( t );
}

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
#ifndef PHIINPUTITEMS_H
#define PHIINPUTITEMS_H
#include "phiabstractinputitem.h"

class PHILineEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { LineEdit=1 };
    explicit PHILineEditItem();
    virtual QString listName() const { return tr( "Line edit" ); }
    virtual QString description() const { return tr( "Line edit with input type <text>" ); }
    virtual PHIWID wid() const { return LineEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

class PHITextAreaItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { TextArea=2 };
    explicit PHITextAreaItem();
    virtual QString listName() const { return tr( "Textarea" ); }
    virtual QString description() const { return tr( "Input type <textarea>" ); }
    virtual PHIWID wid() const { return TextArea; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/textarea" ) ); }

protected:
    virtual bool isSingleLine() const { return false; }
    virtual void setWidgetText( const QString &t );
};

class PHIPasswordItem : public PHILineEditItem
{
    Q_OBJECT

public:
    enum Wid { Password=3 };
    explicit PHIPasswordItem();
    virtual QString listName() const { return tr( "Password" ); }
    virtual QString description() const { return tr( "Line edit with input type <password>" ); }
    virtual PHIWID wid() const { return Password; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/password" ) ); }
};

class PHINumberEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { NumberEdit=52 };
    explicit PHINumberEditItem();
    virtual QString listName() const { return tr( "Decimal number" ); }
    virtual QString description() const { return tr( "Decimal number edit with input type <number>" ); }
    virtual PHIWID wid() const { return NumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

class PHIRealNumberEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { RealNumberEdit=53 };
    explicit PHIRealNumberEditItem();
    virtual QString listName() const { return tr( "Real number" ); }
    virtual QString description() const { return tr( "Real number edit with input type <number>" ); }
    virtual PHIWID wid() const { return RealNumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

class PHISubmitButtonItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { SubmitButton=7 };
    explicit PHISubmitButtonItem();
    virtual QString listName() const { return tr( "Submit button" ); }
    virtual QString description() const { return tr( "Submit button with input type <submit>" ); }
    virtual PHIWID wid() const { return SubmitButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/submit" ) ); }

protected:
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
};

class PHIResetButtonItem : public PHISubmitButtonItem
{
    Q_OBJECT

public:
    enum Wid { ResetButton=8 };
    explicit PHIResetButtonItem();
    virtual QString listName() const { return tr( "Reset button" ); }
    virtual QString description() const { return tr( "Reset button with input type <reset>" ); }
    virtual PHIWID wid() const { return ResetButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/reset" ) ); }
};

class PHIButtonItem : public PHISubmitButtonItem
{
    Q_OBJECT
    Q_PROPERTY( QString url READ url WRITE setUrl )

public:
    enum Wid { Button=6 };
    enum ItemData { DUrl=1 };
    explicit PHIButtonItem();
    virtual QString listName() const { return tr( "Button" ); }
    virtual QString description() const { return tr( "Native button with input type <button>" ); }
    virtual PHIWID wid() const { return Button; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/button" ) ); }
    virtual bool hasUrl() const { return true; }

public slots:
    QString url() const { return QString::fromUtf8( data( DUrl, QString() ).toByteArray() ); }
    void setUrl( const QString &url ) { setData( DUrl, url.toUtf8() ); }
};

#endif // PHIINPUTITEMS_H

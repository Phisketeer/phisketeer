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
#include "phiabstractitems.h"

class PHILineEditItem : public PHIAbstractInputItem
{
    Q_OBJECT
    Q_PROPERTY( QString _placeholder READ realPlaceholder WRITE setPlaceholder SCRIPTABLE false )

public:
    enum Wid { LineEdit=1 };
    enum ItemData { DPlaceholder=1 };
    explicit PHILineEditItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHILineEditItem( const PHILineEditItem &it ) : PHIAbstractInputItem( it ), _placeholderData( it._placeholderData ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHILineEditItem() {}

    virtual QString listName() const { return tr( "Line edit" ); }
    virtual QString description() const { return tr( "Line edit with input type <text>" ); }
    virtual PHIWID wid() const { return LineEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void ideInit();
    virtual PHITextData* placeholderData() { return &_placeholderData; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    void setPlaceholder( const QString &t );
    QString realPlaceholder() const { return QString::fromUtf8( data( DPlaceholder ).toByteArray() ); }

public slots:
    QScriptValue placeholder( const QScriptValue &v=QScriptValue() );

protected:
    virtual void setReadOnly( bool b );
    virtual void initWidget();
    virtual void setWidgetText( const QString &s );
    virtual void ideUpdateData();
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void loadItemData( QDataStream &in, int version );
    virtual void saveItemData( QDataStream &out, int version );
    virtual void squeeze();
    virtual void phisParseData( const PHIDataParser &parser );
    virtual void phisCreateData( const PHIDataParser &parser );
    void genHtml( const QByteArray &type, const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private:
    PHITextData _placeholderData;
};

class PHIEmailItem : public PHILineEditItem
{
    Q_OBJECT

public:
    enum Wid { Email=51 };
    explicit PHIEmailItem( const PHIBaseItemPrivate &p ) : PHILineEditItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIEmailItem( const PHIEmailItem &it ) : PHILineEditItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIEmailItem() {}

    virtual QString listName() const { return tr( "Email address" ); }
    virtual QString description() const { return tr( "Line edit with input type <email>" ); }
    virtual PHIWID wid() const { return Email; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private:
    virtual void initWidget();
};

class PHIPhoneItem : public PHILineEditItem
{
    Q_OBJECT

public:
    enum Wid { Phone=54 };
    explicit PHIPhoneItem( const PHIBaseItemPrivate &p ) : PHILineEditItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIPhoneItem( const PHIPhoneItem &it ) : PHILineEditItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIPhoneItem() {}

    virtual QString listName() const { return tr( "Phone number" ); }
    virtual QString description() const { return tr( "Line edit with input type <phone>" ); }
    virtual PHIWID wid() const { return Phone; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private:
    virtual void initWidget();
};

class PHISearchItem : public PHILineEditItem
{
    Q_OBJECT

public:
    enum Wid { Search=50 };
    explicit PHISearchItem( const PHIBaseItemPrivate &p ) : PHILineEditItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHISearchItem( const PHIPhoneItem &it ) : PHILineEditItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISearchItem() {}

    virtual QString listName() const { return tr( "Search" ); }
    virtual QString description() const { return tr( "Line edit with input type <search>" ); }
    virtual PHIWID wid() const { return Search; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private:
    virtual void initWidget();
};

class PHITextAreaItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { TextArea=2 };
    explicit PHITextAreaItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHITextAreaItem( const PHITextAreaItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHITextAreaItem() {}

    virtual QString listName() const { return tr( "Textarea" ); }
    virtual QString description() const { return tr( "Input type <textarea>" ); }
    virtual PHIWID wid() const { return TextArea; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/textarea" ) ); }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual bool isSingleLine() const { return false; }
    virtual void setWidgetText( const QString &t );
    virtual void setReadOnly( bool b );

private:
    void initWidget();
};

class PHIPasswordItem : public PHILineEditItem
{
    Q_OBJECT

public:
    enum Wid { Password=3 };
    explicit PHIPasswordItem( const PHIBaseItemPrivate &p ) : PHILineEditItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIPasswordItem( const PHIPasswordItem &it ) : PHILineEditItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIPasswordItem() {}

    virtual QString listName() const { return tr( "Password" ); }
    virtual QString description() const { return tr( "Line edit with input type <password>" ); }
    virtual PHIWID wid() const { return Password; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/password" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

private:
    virtual void initWidget();
};

class PHINumberEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { NumberEdit=52 };
    explicit PHINumberEditItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHINumberEditItem( const PHINumberEditItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHINumberEditItem() {}

    virtual QString listName() const { return tr( "Number" ); }
    virtual QString description() const { return tr( "Decimal number edit with input type <number>" ); }
    virtual PHIWID wid() const { return NumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void ideInit();

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setReadOnly( bool b );

private:
    void initWidget();
};

class PHIRealNumberEditItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { RealNumberEdit=53 };
    explicit PHIRealNumberEditItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIRealNumberEditItem( const PHIRealNumberEditItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIRealNumberEditItem() {}

    virtual QString listName() const { return tr( "Real number" ); }
    virtual QString description() const { return tr( "Real number edit with input type <number>" ); }
    virtual PHIWID wid() const { return RealNumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void ideInit();

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setReadOnly( bool b );

private:
    void initWidget();
};

class PHISubmitButtonItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { SubmitButton=7 };
    explicit PHISubmitButtonItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHISubmitButtonItem( const PHISubmitButtonItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHISubmitButtonItem() {}

    virtual QString listName() const { return tr( "Submit button" ); }
    virtual QString description() const { return tr( "Submit button with input type <submit>" ); }
    virtual PHIWID wid() const { return SubmitButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/submit" ) ); }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual void initWidget();
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual PHIBooleanData* readOnlyData() { return 0; }
    virtual void click( const QGraphicsSceneMouseEvent *e );
};

class PHIResetButtonItem : public PHISubmitButtonItem
{
    Q_OBJECT

public:
    enum Wid { ResetButton=8 };
    explicit PHIResetButtonItem( const PHIBaseItemPrivate &p ) : PHISubmitButtonItem( p ) {}
    PHIResetButtonItem( const PHIResetButtonItem &it ) : PHISubmitButtonItem( it ) {}
    virtual ~PHIResetButtonItem() {}

    virtual QString listName() const { return tr( "Reset button" ); }
    virtual QString description() const { return tr( "Reset button with input type <reset>" ); }
    virtual PHIWID wid() const { return ResetButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/reset" ) ); }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
};

class PHIButtonItem : public PHISubmitButtonItem
{
    Q_OBJECT
    Q_PROPERTY( QString _url READ realUrl WRITE setUrl SCRIPTABLE false )

public:
    enum Wid { Button=6 };
    enum ItemData { DUrl=1 };
    explicit PHIButtonItem( const PHIBaseItemPrivate &p ) : PHISubmitButtonItem( p ) {}
    PHIButtonItem( const PHIButtonItem &it ) : PHISubmitButtonItem( it ) {}
    virtual ~PHIButtonItem() {}

    virtual QString listName() const { return tr( "Button" ); }
    virtual QString description() const { return tr( "Native button with input type <button>" ); }
    virtual PHIWID wid() const { return Button; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/button" ) ); }
    virtual bool hasUrl() const { return true; }
    virtual void ideInit();
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

    QString realUrl() const { return QString::fromUtf8( data( DUrl, QString() ).toByteArray() ); }
    void setUrl( const QString &url ) { setData( DUrl, url.toUtf8() ); }

protected:
    virtual void squeeze();
};

#endif // PHIINPUTITEMS_H

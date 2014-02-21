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
#ifndef PHIINPUTITEMS_H
#define PHIINPUTITEMS_H
#include "phiabstractitems.h"

class PHILineEditItem : public PHIAbstractInputItem
{
    Q_OBJECT
    Q_PROPERTY( QString _placeholder READ realPlaceholder WRITE setPlaceholder SCRIPTABLE false )
    Q_PROPERTY( qint32 _maxLength WRITE setMaxLength READ realMaxLength SCRIPTABLE false )

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
    virtual QString realValue() const;
    void setMaxLength( qint32 l );
    void setPlaceholder( const QString &t );
    inline QString realPlaceholder() const { return QString::fromUtf8( data( DPlaceholder ).toByteArray() ); }
    inline qint32 realMaxLength() const { return data( DMaxLength, 100 ).toInt(); }

public slots:
    QScriptValue placeholder( const QScriptValue &v=QScriptValue() );
    QScriptValue maxLength( const QScriptValue &l=QScriptValue() );

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
    virtual void clientInitData();
    void genHtml( const QByteArray &type, const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected slots:
    virtual void slotChanged();

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
    Q_PROPERTY( qint32 _maxLength WRITE setMaxLength READ realMaxLength SCRIPTABLE false )

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
    void setMaxLength( qint32 l );
    inline qint32 realMaxLength() const { return data( DMaxLength, 5000 ).toInt(); }

public slots:
    QScriptValue maxLength( const QScriptValue &l=QScriptValue() );

protected:
    virtual bool isSingleLine() const { return false; }
    virtual void squeeze();
    virtual void setWidgetText( const QString &t );
    virtual void setReadOnly( bool b );

protected slots:
    void slotChanged();

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
    enum Wid { NumberEdit=52, RealNumberEdit=53 };
    explicit PHINumberEditItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHINumberEditItem( const PHINumberEditItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHINumberEditItem() {}

    virtual QString listName() const { return tr( "Number" ); }
    virtual QString description() const { return tr( "Decimal number edit with input type <number>" ); }
    virtual PHIWID wid() const { return NumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void ideInit();
    virtual void setValue( const QString &v );
    virtual QString realValue() const;
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &headerOut ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    virtual QScriptValue min( const QScriptValue &m=QScriptValue() );
    virtual QScriptValue max( const QScriptValue &m=QScriptValue() );
    virtual QScriptValue step( const QScriptValue &m=QScriptValue() );

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void setReadOnly( bool b );
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    virtual bool isButton() const { return true; }

protected slots:
    virtual void slotChanged();

private:
    virtual void initWidget();
};

class PHIRealNumberEditItem : public PHINumberEditItem
{
    Q_OBJECT

public:
    explicit PHIRealNumberEditItem( const PHIBaseItemPrivate &p ) : PHINumberEditItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIRealNumberEditItem( const PHIRealNumberEditItem &it ) : PHINumberEditItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIRealNumberEditItem() {}

    virtual QString listName() const { return tr( "Real number" ); }
    virtual QString description() const { return tr( "Real number edit with input type <number>" ); }
    virtual PHIWID wid() const { return RealNumberEdit; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/lineedit" ) ); }
    virtual void setValue( const QString &v );
    virtual QString realValue() const;
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

public slots:
    virtual QScriptValue min( const QScriptValue &m=QScriptValue() );
    virtual QScriptValue max( const QScriptValue &m=QScriptValue() );
    virtual QScriptValue step( const QScriptValue &m=QScriptValue() );

protected:
    virtual void setWidgetText( const QString &s );
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

protected slots:
    virtual void slotChanged();

private:
    virtual void initWidget();
};

#endif // PHIINPUTITEMS_H

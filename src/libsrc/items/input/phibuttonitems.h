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
#ifndef PHIBUTTONITEMS_H
#define PHIBUTTONITEMS_H
#include "phiabstractitems.h"

class QLineEdit;
class QToolButton;

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

public slots:
    virtual QScriptValue readOnly( const QScriptValue &r=QScriptValue() ) { Q_UNUSED( r ) return QScriptValue( QScriptValue::UndefinedValue ); }

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual PHIBooleanData* readOnlyData() { return 0; }
    virtual void paint( QPainter *painter, const QRectF &exposed );
    virtual void initWidget();
    virtual void setWidgetText( const QString &t );
    virtual void setColor( PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &col );
    virtual void click( const QGraphicsSceneMouseEvent *e );
    virtual void clientPostData( QHttpMultiPart *multiPart ) const { Q_UNUSED( multiPart ); }
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

protected:
    virtual void click( const QGraphicsSceneMouseEvent *e );
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

public slots:
    QScriptValue url( const QScriptValue &u=QScriptValue() );

protected:
    virtual void squeeze();
    virtual void click( const QGraphicsSceneMouseEvent *e );
};

class PHIFileButtonItem : public PHIAbstractInputItem
{
    Q_OBJECT

public:
    enum Wid { FileButton=9 };
    explicit PHIFileButtonItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ), _edit( 0 ), _tool( 0 ) { if ( isGuiItem() ) initWidget(); }
    PHIFileButtonItem( const PHIFileButtonItem &it ) : PHIAbstractInputItem( it ), _edit( 0 ), _tool( 0 ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIFileButtonItem() {}

    virtual QString listName() const { return tr( "File upload" ); }
    virtual QString description() const { return tr( "Native file upload button with input type <file>" ); }
    virtual PHIWID wid() const { return FileButton; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/upload" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;

protected:
    virtual void ideInit();
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;
    virtual void setWidgetText( const QString &t ) { Q_UNUSED( t ) }
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;

private slots:
    void slotToolClicked();

private:
    void initWidget();

private:
    QLineEdit *_edit;
    QToolButton *_tool;
};

#endif // PHIBUTTONITEMS_H

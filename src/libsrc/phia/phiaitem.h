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
#ifndef PHIAITEM_H
#define PHIAITEM_H

#include <QGraphicsItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QUrl>
#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include "phibaseitem.h"
#include "gitems.h"

class PHIAWebView;
class PHIAScriptEvent;

class PHIAItem : public PHIBaseItem
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAItem )

    friend QDataStream& operator<<( QDataStream&, const PHIAItem* );
    friend QDataStream& operator>>( QDataStream&, PHIAItem* );
    friend class PHIAScriptItem;
    friend class PHIAAnimation;
    friend class PHIAFaderAnimation;

    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString parentName READ parentName )
    Q_PROPERTY( QString id READ name )
    //Q_PROPERTY( quint16 type READ type )
    //Q_PROPERTY( quint32 attributes READ attributes )
    //Q_PROPERTY( qint32 fontSize WRITE setFontSize READ fontSize )
    //Q_PROPERTY( QString originalValue READ originalValue )
    Q_PROPERTY( QString styleSheet WRITE setStyleSheet READ styleSheet )
    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( QString value WRITE setValue READ value )
    Q_PROPERTY( QString title WRITE setToolTip READ toolTip )
    Q_PROPERTY( QString imageId WRITE setImageId READ imageId )
    Q_PROPERTY( QString label WRITE setLabel READ label )
    Q_PROPERTY( QString url WRITE setUrl READ url )
    Q_PROPERTY( QString accessKey WRITE setAccessKey READ accessKey )
    Q_PROPERTY( QString delimiter WRITE setDelimiter READ delimiter )
    Q_PROPERTY( quint16 maxLength WRITE setMaxLength READ maxLength )
    Q_PROPERTY( bool disabled WRITE setDisabled READ disabled )
    Q_PROPERTY( bool checked WRITE setChecked READ checked )
    Q_PROPERTY( bool readOnly WRITE setReadOnly READ readOnly )
    Q_PROPERTY( quint16 tabIndex WRITE setTabIndex READ tabIndex )

    // Animation framework
    Q_PROPERTY( qreal width WRITE setWidth READ width SCRIPTABLE false )
    Q_PROPERTY( qreal height WRITE setHeight READ height SCRIPTABLE false )
    Q_PROPERTY( qreal left WRITE setX READ x SCRIPTABLE false )
    Q_PROPERTY( qreal top WRITE setY READ y SCRIPTABLE false )
    Q_PROPERTY( qreal opacity WRITE setOpacity READ opacity SCRIPTABLE false )
    Q_PROPERTY( QPointF pos WRITE setPos READ pos SCRIPTABLE false )
    //Q_PROPERTY( QRectF rect WRITE setRect READ rect SCRIPTABLE false )
    //Q_PROPERTY( QPointF rotateXY WRITE setRotateXY READ rotateXY SCRIPTABLE false )
    Q_PROPERTY( qreal xRotation WRITE setXRotation READ xRotation SCRIPTABLE false )
    Q_PROPERTY( qreal yRotation WRITE setYRotation READ yRotation SCRIPTABLE false )
    Q_PROPERTY( qreal zRotation WRITE setZRotation READ zRotation SCRIPTABLE false )

    // not useable by scripting:
public:
    enum EffectProperty { EPNone=0x0, EPOpacity=0x1, EPRotateX=0x2, EPRotateY=0x4,
        EPRotateZ=0x8, EPMoveX=0x10, EPMoveY=0x20, EPMoveW=0x40, EPMoveH=0x80,
        EPSlideH=0x100 };
    Q_DECLARE_FLAGS( EffectProperties, EffectProperty )

    enum Event { ENone=0, EClick=1, EDblClick, EMouseOver, EMouseMove, EMouseUp, EMouseDown,
        EMouseOut, EChange, EFocus, EBlur, EKeyPress, EKeyDown, EKeyUp, EDrop,
        ECustom, ESubmit, ESelect, ELoad };

    explicit PHIAItem( PHIAWebView *parent );
    virtual ~PHIAItem();
    PHIAWebView* view() const;

    virtual void addChild( const PHIAItem* );
    virtual void setPalette( const QPalette& );
    virtual void resetItem();
    virtual void activate(); // layout container
    virtual void setHeight( qreal h );
    virtual void setWidth( qreal w );
    virtual void setVisible( bool );

    virtual void clearEffects();
    virtual void stopAnimations();
    virtual void setFadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease );
    virtual void setFadeOut( qint32 start, qint32 duration, qreal minOpac, const QString &ease );
    virtual void setShadow( const QColor &c, qreal xOff, qreal yOff, qreal radius );
    virtual void setBlur( qreal radius );
    virtual void setSurface( qreal yOff, qreal size );
    virtual void setColorize( const QColor &c, qreal strength );
    virtual void setMoveTo( qint32 start, qint32 duration, qint32 left, qint32 top, const QString &ease );
    virtual void setMoveBy( qint32 start, qint32 duration, qint32 x, qint32 y,
        qint32 w, qint32 h, const QString &ease );
    virtual void setRotateIn( quint8 axis, qint32 start, qint32 duration, const QString &ease );
    virtual void setRotateOut( quint8 axis, qint32 start, qint32 duration, const QString &ease );
    virtual void setRotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ, const QString &ease );
    inline virtual void setPos( const QPointF &f ){ _x=f.x(); _y=f.y(); _git->setPos( _x, _y ); }
    inline virtual QPointF pos() const { return QPointF( _x, _y ); }
    //inline virtual void setRotateXY( const QPointF &f ) { _xRotate=f.x(); _yRotate=f.y(); updateTransformation(); }
    //inline virtual QPointF rotateXY() const { return QPointF( _xRotate, _yRotate ); }
    //inline virtual QRectF rect() const { return QRectF( _x, _y, width(), height() ); }
    //inline virtual void setRect( const QRectF &r ) { _x=r.x(); _y=r.y(); _git->setPos( _x, _y );
    //    setWidth( r.width() ); setHeight( r.height() ); }
    virtual void setSlideUp( qint32 start, qint32 duration, const QString &ease );
    virtual void setSlideDown( qint32 start, qint32 duration, const QString &ease );

    virtual QStringList selectedItems() const;
    virtual QUrl createLocalUrl( const QUrl &url );

    inline virtual bool setupFinished() const { return _setupFinished; }
    inline QGraphicsItem* graphicsItem() const { return _git; }
    inline QGraphicsWidget* graphicsWidget() const { return qgraphicsitem_cast<QGraphicsWidget*>(_git); }
    void checkForDragInMousePressEvent( QGraphicsSceneMouseEvent *e ) const;
    void checkForDragInMouseMoveEvent( QGraphicsSceneMouseEvent *e ) const;
    void checkDragEnterEvent( QGraphicsSceneDragDropEvent *e ) const;
    void checkDragMoveEvent( QGraphicsSceneDragDropEvent *e ) const;
    void checkDropEvent( QGraphicsSceneDragDropEvent *e ) const;    
    virtual void setCursor( const QByteArray &c );

    //useable by scripting
public slots:
    virtual void setValue( const QString &v );
    virtual QString value() const;
    virtual void setDisabled( bool );
    virtual bool disabled() const;
    virtual void setChecked( bool );
    virtual bool checked() const;
    virtual void setReadOnly( bool );
    virtual bool readOnly() const;
    virtual void setLabel( const QString &l );
    virtual QString url() const;
    virtual void setUrl( const QString &u );
    virtual QString label() const;
    virtual void setDelimiter( const QString &d );
    virtual QString delimiter() const;
    virtual void setAccessKey( const QString &k );
    virtual QString accessKey() const;
    virtual void setTabIndex( quint16 i );
    virtual quint16 tabIndex() const;
    virtual void setToolTip( const QString &s );
    virtual QString toolTip() const;
    virtual void setMaxLength( quint16 l );
    virtual quint16 maxLength() const;

    virtual void focus();
    virtual void blur();
    virtual void click();

    //inline virtual QString originalValue() const { return PHIBaseItem::value(); }
    inline virtual QStringList properties() const { return PHI::properties( this ); }

// event handler
protected slots:
    virtual void slotOnClick( QGraphicsSceneMouseEvent *e );
    virtual void slotOnClick();
    virtual void slotOnDblClick( QGraphicsSceneMouseEvent *e );
    virtual void slotOnFocus( QFocusEvent *e );
    virtual void slotOnBlur( QFocusEvent *e );
    virtual void slotOnMouseDown( QGraphicsSceneMouseEvent *e );
    virtual void slotOnMouseUp( QGraphicsSceneMouseEvent *e );
    virtual void slotOnMouseOver( QGraphicsSceneHoverEvent *e );
    virtual void slotOnMouseMove( QGraphicsSceneHoverEvent *e );
    virtual void slotOnMouseOut( QGraphicsSceneHoverEvent *e );
    virtual void slotOnKeyPress( QKeyEvent *e );
    virtual void slotOnKeyDown( QKeyEvent *e );
    virtual void slotOnKeyUp( QKeyEvent *e );
    virtual void slotOnChange();

protected:
    virtual bool visible() const;
    virtual void setGraphicsEffect();

    virtual void setFont( const QFont &f );
    virtual void setFontSize( qint32 );
    virtual void setColor( const QColor& );
    virtual void setOutlineColor( const QColor& );
    virtual void setPattern( quint8 pattern );
    virtual void setStyleSheet( const QString& );
    virtual void setAlignment( quint8 align );
    virtual void setRolloverTextColor( const QColor& );
    virtual void setRolloverBackgroundColor( const QColor& );
    virtual void setBorderRadius( qint16 r );
    virtual void setFadeTime( qint16 ft );
    virtual void setFadeInterval( qint16 fp );
    virtual void setStartAngle( qint16 a );
    virtual void setSpanAngle( qint16 a );
    virtual void setOnHandler( const QString &e, const QScriptValue &fn );
    virtual void setOffHandler( const QString &e, const QScriptValue &fn );

    virtual QFont font() const;
    virtual qint32 fontSize() const;
    virtual QPalette palette() const;
    virtual QColor color() const;
    virtual QColor outlineColor() const;
    virtual quint8 alignment() const;
    virtual QString styleSheet() const;
    virtual QColor rolloverTextColor() const;
    virtual QColor rolloverBackgroundColor() const;

    inline virtual void setX( qreal x ) { _x=x; _git->setPos( _x, _y ); }
    inline virtual void setY( qreal y ) { _y=y; _git->setPos( _x, _y ); }
    inline virtual void setZValue( qint16 z ) { _zValue=z; _git->setZValue( static_cast<qreal>(z) ); }
    inline virtual void setTransform( const QTransform &t ) { _transform=t; _git->setTransform( t ); }
    inline virtual void setHShear( qreal sh ) { _sh=sh; updateTransformation(); }
    inline virtual void setVShear( qreal sv ) { _sv=sv; updateTransformation(); }
    inline virtual void setXTranslation( qreal xt ) { _xTranslate=xt; updateTransformation(); }
    inline virtual void setYTranslation( qreal yt ) { _yTranslate=yt; updateTransformation(); }
    inline virtual void setXRotation( qreal xr ) { _xRotate=xr; updateTransformation(); }
    inline virtual void setYRotation( qreal yr ) { _yRotate=yr; updateTransformation(); }
    inline virtual void setZRotation( qreal zr ) { _zRotate=zr; updateTransformation(); }
    inline virtual void setOpacity( qreal o ) { _opacity=o; _git->setOpacity( o ); }
    inline virtual void setDropable( bool b ) { PHIBaseItem::setDropable( b ); _git->setAcceptDrops( b ); }

protected slots:
    void slotRequestForImageFinished();
    void slotLinkActivated( const QString& );
    void slotLinkActivated( const QUrl& );
    void slotLinkHovered( const QString& );
    void slotCheckStateToggled( bool );
    void slotClearRotateOut();
    void slotUpdateColumnsHack();

protected:
    void updateTransformation();
    bool isShapeItem() const;
    void requestImage( int i=-1 );
    void updateSize();
    void generateItem();
    QPixmap loadingPixmap() const;
    PHIAScriptEvent* getEvent( Event, bool cancelable=false ) const;
    void callOnEvents( Event, const QScriptValueList &args ) const;
    bool hasOnEvents( Event ) const;
    bool valueChecked( const QString &v ) const;
    void setValueChecked( const QString &v, bool c );
    void triggerEvent( const QString &e, const QScriptValueList &args );

signals:
    void setupDone( const PHIAItem* );
    void linkRequested( const QUrl& );
    void linkHovered( const QString&, const QString&, const QString& );
    void submitForm( const QString& );
    void resetForm();
    void testId( int );

protected:
    QGraphicsItem *_git;
    bool _setupFinished;
    QNetworkReply *_reply;
    QHash <QNetworkReply*, int> _replyHash;
    QMultiHash <EffectProperty, QPropertyAnimation*> _animations;
    QMultiHash <QString, QScriptValue> _eventFunctions;
    mutable QHash <quint8, PHIAScriptEvent*> _scriptEvents;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIAItem::EffectProperties )

QDataStream& operator<<( QDataStream&, const PHIAItem* );
QDataStream& operator>>( QDataStream&, PHIAItem* );

#endif // PHIAITEM_H

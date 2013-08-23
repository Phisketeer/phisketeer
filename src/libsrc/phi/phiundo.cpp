/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
*/
#include <QObject>
#include "phiundo.h"
#include "phibaseitem.h"
#include "phibasepage.h"
#include "phigraphicsscene.h"
#include "phiitemfactory.h"
//#include "phieffects.h"

#define UT(s) QString( L1( " <%1>" ) ).arg( s )

PHIUndoCommand::PHIUndoCommand( PHIBaseItem *it )
    : QUndoCommand( 0 ), _it( it )
{
}

void PHIUndoCommand::undo()
{
    //item()->setFocus();
}

void PHIUndoCommand::redo()
{
    //item()->setFocus();
}

PHIUndoMove::PHIUndoMove( PHIBaseItem *it, const QPointF &oldPos )
    : PHIUndoCommand( it ), _oldPos( oldPos )
{
    qDebug( "PHIUndoMove::PHIUndoMove()" );
    setText( QObject::tr( "Move" )+UT( it->name() ) );
    _newPos=it->pos();
}

bool PHIUndoMove::mergeWith( const QUndoCommand *other )
{
    const PHIUndoMove *o=dynamic_cast<const PHIUndoMove*>(other);
    Q_ASSERT( o );
    if ( item()!=o->item() ) return false;
    _newPos=o->_newPos;
    return true;
}

void PHIUndoMove::undo()
{
    item()->setPos( _oldPos );
    PHIUndoCommand::undo();
}

void PHIUndoMove::redo()
{
    item()->setPos( _newPos );
    PHIUndoCommand::redo();
}

PHIUndoColor::PHIUndoColor( PHIBaseItem *it, PHIPalette::ItemRole ir, PHIPalette::ColorRole cr, const QColor &newCol )
    : PHIUndoCommand( it ), _itemRole( ir ), _newColorRole( cr ), _newCol( newCol )
{
    qDebug( "PHIUndoColor::PHIUndoColor()" );
    switch ( ir ) {
    case PHIPalette::Background:
        setText( QObject::tr( "Outline color" )+UT( it->name() ) ); break;
    case PHIPalette::WidgetBase:
        setText( QObject::tr( "BG color" )+UT( it->name() ) ); break;
    case PHIPalette::Foreground:
        setText( QObject::tr( "Pattern color" )+UT( it->name() ) ); break;
    case PHIPalette::WidgetText:
        setText( QObject::tr( "Text color" )+UT( it->name() ) ); break;
    default:
        setText( QObject::tr( "Color" )+UT( it->name() ) ); break;
    }
    _oldCol=it->color( ir );
    _oldColorRole=it->colorRole( ir );
}

bool PHIUndoColor::mergeWith( const QUndoCommand *other )
{
    const PHIUndoColor *o=dynamic_cast<const PHIUndoColor*>(other);
    Q_ASSERT( o );
    if ( item()!=o->item() ) return false;
    if ( _itemRole!=o->_itemRole ) return false;
    _newCol=o->_newCol;
    _newColorRole=o->_newColorRole;
    return true;
}

void PHIUndoColor::undo()
{
    item()->setColor( _itemRole, _oldColorRole, _oldCol );
    PHIUndoCommand::undo();
}

void PHIUndoColor::redo()
{
    item()->setColor( _itemRole, _newColorRole, _newCol );
    PHIUndoCommand::redo();
}

PHIUndoAdd::PHIUndoAdd( PHIBaseItem *it, PHIGraphicsScene *scene )
    : PHIUndoCommand( it ), _scene( scene )
{
    qDebug( "PHIUndoAdd::PHIUndoAdd()" );
    setText( QObject::tr( "Add" )+UT( it->name() ) );
}

PHIUndoAdd::~PHIUndoAdd()
{
    qDebug( "PHIUndoAdd::~PHIUndoAdd()" );
}

void PHIUndoAdd::undo()
{
    item()->setParent( _scene ); // reparent to scene
    _scene->removeItem( item()->graphicsWidget() );
    /*
    bool layoutChanged=false;
    if ( PHI::isLayoutContainer( item()->wid() ) ) {
        SAbstractLayoutItem *l=qobject_cast<SAbstractLayoutItem*>(item());
        Q_ASSERT( l );
        l->breakLayout();
        PHIBaseItem *it;
        foreach ( it, _childItems ) {
            QRectF p=_childRects.value( it );
            it->setPos( p.x(), p.y() );
            it->setWidth( p.width() );
            it->setHeight( p.height() );
            //it->setMoving( false );
        }
        layoutChanged=true;
    }
    item()->scene()->removePHIBaseItem( item() );
    item()->setDeleted( true );
    if ( layoutChanged ) item()->scene()->updateItemIds( layoutChanged );
    */
}

void PHIUndoAdd::redo()
{
    item()->setParent( _scene->page() );
    _scene->addItem( item()->graphicsWidget() );
    PHIUndoCommand::redo();
    /*
    item()->setDeleted( false );
    item()->scene()->addPHIBaseItem( item() );
    bool layoutChanged=false;
    if ( PHI::isLayoutContainer( item()->wid() ) ) {
        SAbstractLayoutItem *l=qobject_cast<SAbstractLayoutItem*>(item());
        Q_ASSERT( l );
        PHIBaseItem *it;
        //qDebug( "layout %s children %d", qPrintable( item()->id() ), _childItems.count() );
        foreach ( it, _childItems ) {
            //it->setMoving( false );
            _childRects.insert( it, QRectF( it->x(), it->y(), it->width(), it->height() ) );
            //qDebug( "add PHIBaseItem %s", qPrintable( it->id() ) );
            l->addPHIBaseItem( it );
            it->setDeleted( false );
        }
        l->activate();
        layoutChanged=true;
    }
    // item()->setSelected( true );
    qDebug( "ADD %s %s", qPrintable( item()->id() ), qPrintable( item()->parent() ) );
    //item()->setFocusItem();
    if ( layoutChanged ) item()->scene()->updateItemIds( layoutChanged );
    */
}

PHIUndoDelete::PHIUndoDelete( PHIBaseItem* it, PHIGraphicsScene *scene )
    : PHIUndoCommand( it ), _scene( scene )
{
    qDebug( "PHIUndoDelete::PHIUndoDelete()" );
    setText( QObject::tr( "Delete" )+UT( it->name() ) );
    /*
    if ( PHI::isLayoutContainer( item()->wid() ) ) {
        SAbstractLayoutItem *l=qobject_cast<SAbstractLayoutItem*>(item());
        Q_ASSERT( l );
        _childItems=l->children();
    }
    */
}

void PHIUndoDelete::undo()
{
    item()->setParent( _scene->page() );
    _scene->addItem( item()->graphicsWidget() );
    PHIUndoCommand::undo();
    /*
    item()->setDeleted( false );
    item()->scene()->addPHIBaseItem( item() );
    bool layoutChanged=false;
    if ( PHI::isLayoutContainer( item()->wid() ) ) {
        SAbstractLayoutItem *l=qobject_cast<SAbstractLayoutItem*>(item());
        Q_ASSERT( l );
        //l->setMoving( false );
        PHIBaseItem *it;
        foreach ( it, _childItems ) {
            //it->setMoving( false );
            l->addPHIBaseItem( it );
            it->setDeleted( false );
        }
        l->activate();
        //l->setPos( _containerRect.x(), _containerRect.y() );
        l->setWidth( _containerRect.width() );
        l->setHeight( _containerRect.height() );
        layoutChanged=true;
    }
    // item()->setSelected( true );
    item()->setFocusItem();
    item()->scene()->updateItemIds( layoutChanged );
    */
}

void PHIUndoDelete::redo()
{
    item()->setParent( _scene );
    _scene->removeItem( item()->graphicsWidget() );
    /*
    bool layoutChanged=false;
    if ( PHI::isLayoutContainer( item()->wid() ) ) {
        SAbstractLayoutItem *l=qobject_cast<SAbstractLayoutItem*>(item());
        Q_ASSERT( l );
        _containerRect=QRectF( l->x(), l->y(), l->width(), l->height() );
        //l->setMoving( false );
        _childItems=l->children();
        PHIBaseItem *it;
        foreach ( it, _childItems ) {
            //it->setMoving( false );
        }
        l->breakLayout();
        layoutChanged=true;
    }
    item()->scene()->removePHIBaseItem( item() );
    item()->setDeleted( true );
    item()->scene()->updateItemIds( layoutChanged );
    */
}

PHIUndoSize::PHIUndoSize( PHIBaseItem *it, const QSizeF &oldSize )
    : PHIUndoCommand( it ), _oldSize( oldSize )
{
    qDebug( "PHIUndoSize::PHIUndoSize()" );
    setText( QObject::tr( "Size" )+UT( it->name() ) );
    _newSize=it->size();
}

bool PHIUndoSize::mergeWith( const QUndoCommand *other )
{
    const PHIUndoSize *o=dynamic_cast<const PHIUndoSize*>(other);
    Q_ASSERT( o );
    if ( item()!=o->item() ) return false;
    _newSize=o->item()->size();
    return true;
}

void PHIUndoSize::undo()
{
    item()->resize( _oldSize );
    PHIUndoCommand::undo();
}

void PHIUndoSize::redo()
{
    item()->resize( _newSize );
    PHIUndoCommand::redo();
}

/*
PHIUndoUrl::PHIUndoUrl( PHIBaseItem *it, const QString &oldUrl, const QString &newUrl )
    : PHIUndoCommand( it ), _oldUrl( oldUrl ), _newUrl( newUrl )
{
    qDebug( "PHIUndoUrl::PHIUndoUrl()" );
}

PHIUndoUrl::~PHIUndoUrl()
{
    qDebug( "PHIUndoUrl::PHIUndoUrl()" );
}

bool PHIUndoUrl::mergeWith( const QUndoCommand* )
{
    return false;
}

void PHIUndoUrl::redo()
{
    QGraphicsProxyWidget *w=qgraphicsitem_cast<QGraphicsProxyWidget*>(item()->graphicsItem());
    if ( w ) {
        QWebView *view=qobject_cast<QWebView*>(w->widget());
        if ( view ) {
            if ( !_newUrl.isEmpty() ) view->load( QUrl( _newUrl ) );
            else view->setContent( PHI::emptyHtmlDoc() );
        }
    }
}

void PHIUndoUrl::undo()
{
    QGraphicsProxyWidget *w=qgraphicsitem_cast<QGraphicsProxyWidget*>(item()->graphicsItem());
    if ( w ) {
        QWebView *view=qobject_cast<QWebView*>(w->widget());
        if ( view ) {
            if ( !_oldUrl.isEmpty() ) view->load( QUrl( _oldUrl ) );
            else view->setContent( PHI::emptyHtmlDoc() );
        }
    }
}
*/

PHIUndoProperty::PHIUndoProperty( PHIBaseItem *it, PHIUndoCommand::Id id, const QVariant &newProp )
    : PHIUndoCommand( it ), _id( id ), _newProp( newProp )
{
    createText();
}

PHIUndoProperty::PHIUndoProperty( PHIBaseItem *it, const char *prop, const QVariant &newProp )
    : PHIUndoCommand( it ), _newProp( newProp )
{
    const QString p=L1( prop );
    if ( p==L1( "opacity" ) ) _id=Opacity;
    if ( p==L1( "font" ) ) _id=Font;
    if ( p==L1( "penWidth" ) ) _id=PenWidth;
    if ( p==L1( "pattern" ) ) _id=Pattern;
    if ( p==L1( "line" ) ) _id=Line;
    if ( p==L1( "zIndex" ) ) _id=ZIndex;
    if ( p==L1( "transformPos" ) ) _id=TransformPos;
    createText();
}

void PHIUndoProperty::createText()
{
    QString text;
    switch ( _id ) {
    case Opacity: text=QObject::tr( "Opacity" );
        _oldProp.setValue( item()->opacity() ); break;
    case Font: text=QObject::tr( "Font" );
        _oldProp.setValue( item()->font() ); break;
    case Line: text=QObject::tr( "Outline" );
        _oldProp.setValue( item()->property( "line").value<quint8>() ); break;
    case PenWidth: text=QObject::tr( "Line width" );
        _oldProp.setValue( item()->property( "penWidth" ).toReal() ); break;
    case Pattern: text=QObject::tr( "Pattern" );
        _oldProp.setValue( item()->property( "pattern" ).value<quint8>() ); break;
    case ZIndex: text=QObject::tr( "zIndex" );
        _oldProp.setValue( item()->zIndex() ); break;
    case TransformPos: text=QObject::tr( "Transform origin" );
        _oldProp.setValue( item()->transformPos() ); break;
/*
    case Alignment: text=QObject::tr( "Alignment" ); _oldProp.setValue( it->alignment() ); break;
    case Text: text=QObject::tr( "Text" ); _oldProp.setValue( it->text() ); break;
    case Data: text=QObject::tr( "Data" ); _oldProp.setValue( *(it->itemData()) ); break;
    case Pixmap: text=QObject::tr( "Image" ); _oldProp.setValue( it->pixmap() ); break;
    case Value: text=QObject::tr( "Value" ); _oldProp.setValue( it->value() ); break;
    case Label: text=QObject::tr( "Label" ); _oldProp.setValue( it->label() ); break;
    case ItemId: text=QObject::tr( "Id" ); _oldProp.setValue( it->id() ); break;
    case Parent: text=QObject::tr( "Parent" ); _oldProp.setValue( it->parent() ); break;
    case TabOrder: text=QObject::tr( "Tab index" ); _oldProp.setValue( it->tabOrder() ); break;
    case Check: text=QObject::tr( "Check value" ); _oldProp.setValue( it->checked() ); break;
    case ToolTip: text=QObject::tr( "Title" ); _oldProp.setValue( it->toolTip() ); break;
    case Visibility: text=QObject::tr( "Visibility" ); _oldProp.setValue( it->visible() ); break;
    case Attribute: text=QObject::tr( "Attribute" );
        _oldProp.setValue(static_cast<qint32>(it->attributes())); break;
*/
    default: qFatal( "Unknown id in PHIUndoProperty" );
    }
    setText( text+UT( item()->name() ) );
}

bool PHIUndoProperty::mergeWith( const QUndoCommand *other )
{
    const PHIUndoProperty *o=dynamic_cast<const PHIUndoProperty*>(other);
    Q_ASSERT( o );
    if ( item()!=o->item() ) return false;
    if ( _id!=o->_id ) return false;
    //if ( _id==Visibility ) return false;
    //if ( _id==Check ) return false;
    _newProp=o->_newProp;
    return true;
}

void PHIUndoProperty::redo()
{
    switch ( _id ) {
    case Opacity: item()->setOpacity( _newProp.value<qreal>() ); break;
    case Font: item()->setFont( _newProp.value<QFont>() ); break;
    case Line: item()->setProperty( "line", _newProp.value<quint8>() ); break;
    case PenWidth: item()->setProperty( "penWidth", _newProp.toReal() ); break;
    case Pattern: item()->setProperty( "pattern", _newProp.value<quint8>() ); break;
    case ZIndex: item()->setZIndex( _newProp.value<qint16>() ); break;
    case TransformPos: item()->setTransformPos( _newProp.value<quint8>() ); break;
    /*
    case Alignment: item()->setAlignment( static_cast<PHI::Alignment>(_newProp.toUInt()) ); break;
    case Text: {
        item()->setText( _newProp.toString() );
        PHIBaseItem *it=item()->scene()->findPHIBaseItem( item()->parent() );
        if ( !it ) break;
        if ( PHI::isLayoutContainer( it->wid() ) ) {
            item()->graphicsWidget()->adjustSize();
            item()->graphicsLayoutItem()->updateGeometry();
        }
        break;
    }
    case Data:
        *(item()->itemData())=_newProp.value<PHIPHIBaseItemData>();
        QTimer::singleShot( 50, item(), SLOT( updateGradient() ) );
        break;
    case Pixmap: item()->setPixmap( _newProp.value<QPixmap>() ); break;
    case TransformPos: item()->setTransformPos( _newProp.toInt() ); break;
    case ZValue:
        item()->setZValue( _newProp.toInt() );
        item()->scene()->mw()->setZValue( _newProp.toInt(), item()->id() ); break;
    case Value: item()->setValue( _newProp.toString() ); break;
    //case Label: item()->setLabel( _newProp.toString() ); break;
    case ItemId: item()->setId( _newProp.toString() );
        _newProp.setValue( item()->id() ); // could be changed if id already exist
        item()->scene()->itemIdChanged( _oldProp.toString(), _newProp.toString() ); break;
    case Parent: item()->setParent( _newProp.toString() ); break;
    case TabOrder:
        item()->setTabOrder( _newProp.value<quint16>() );
        item()->scene()->update(); break;
    case Check: item()->setChecked( _newProp.toBool() ); break;
    case ToolTip: item()->setToolTip( _newProp.toString() ); break;
    case Visibility: item()->setVisible( _newProp.toBool() ); break;
    case Attribute: item()->setAttributes( static_cast<PHIItem::Attributes>(_newProp.value<qint32>()) ); break;
    */
    default: qFatal( "Unknown id in PHIUndoProperty" );
    }
    PHIUndoCommand::redo();
}

void PHIUndoProperty::undo()
{
    switch ( _id ) {
    case Opacity: item()->setOpacity( _oldProp.value<qreal>() ); break;
    case Font: item()->setFont( _oldProp.value<QFont>() ); break;
    case Line: item()->setProperty( "line", _oldProp.value<quint8>() ); break;
    case PenWidth: item()->setProperty( "penWidth", _oldProp.toReal() ); break;
    case Pattern: item()->setProperty( "pattern", _oldProp.value<quint8>() ); break;
    case ZIndex: item()->setZIndex( _oldProp.value<qint16>() ); break;
    case TransformPos: item()->setTransformPos( _oldProp.value<quint8>() ); break;
/*
    case Alignment: item()->setAlignment( static_cast<PHI::Alignment>(_oldProp.toUInt()) ); break;
    case Text: {
        item()->setText( _oldProp.toString() );
        PHIBaseItem *it=item()->scene()->findPHIBaseItem( item()->parent() );
        if ( !it ) break;
        if ( PHI::isLayoutContainer( it->wid() ) ) {
            item()->graphicsWidget()->adjustSize();
            item()->graphicsLayoutItem()->updateGeometry();
        }
        break;
    }
    case Data:
        *(item()->itemData())=_oldProp.value<PHIPHIBaseItemData>();
        QTimer::singleShot( 50, item(), SLOT( updateGradient() ) );
        break;
    case Pixmap: item()->setPixmap( _oldProp.value<QPixmap>() ); break;
    case TransformPos: item()->setTransformPos( _oldProp.toInt() ); break;
    case ZValue:
        item()->setZValue( _oldProp.toInt() );
        item()->scene()->mw()->setZValue( _oldProp.toInt(), item()->id() ); break;
    case Value: item()->setValue( _oldProp.toString() ); break;
    //case Label: item()->setLabel( _oldProp.toString() ); break;
    case ItemId: item()->setId( _oldProp.toString() );
        item()->scene()->itemIdChanged( _newProp.toString(), _oldProp.toString() ); break;
    case Parent: item()->setParent( _oldProp.toString() ); break;
    case TabOrder:
        item()->setTabOrder( _oldProp.value<quint16>() );
        item()->scene()->update();
        item()->scene()->subTabIndex(); break;
    case Check: item()->setChecked( _oldProp.toBool() ); break;
    case ToolTip: item()->setToolTip( _oldProp.toString() ); break;
    case Visibility: item()->setVisible( _oldProp.toBool() ); break;
    case Attribute: item()->setAttributes( static_cast<PHIItem::Attributes>(_oldProp.value<qint32>()) ); break;
    */
    default: qFatal( "Unknown id in PHIUndoProperty" );
    }
    PHIUndoCommand::undo();
}

PHIUndoTransform::PHIUndoTransform( PHIBaseItem *it, qreal sh, qreal sv, qreal x, qreal y, qreal z )
    : PHIUndoCommand( it ), _oldSH( sh ), _oldSV( sv ), _oldX( x ), _oldY( y ), _oldZ( z )
{
    qDebug( "PHIUndoTransform::PHIUndoTransform()" );
    _newX=it->xRotation();
    _newY=it->yRotation();
    _newZ=it->zRotation();
    _newSH=it->hSkew();
    _newSV=it->vSkew();
    if ( _newX==0 && _newY==00 && _newZ==0 && _newSH==0 && _newSV==0 )
        setText( QObject::tr( "Reset" )+UT( it->name() ) );
    else setText( QObject::tr( "Transform" )+UT( it->name() ) );
}

bool PHIUndoTransform::mergeWith( const QUndoCommand *other )
{
    const PHIUndoTransform *o=dynamic_cast<const PHIUndoTransform*>(other);
    Q_ASSERT( o );
    if ( item()!=o->item() ) return false;
    _newX=o->_newX;
    _newY=o->_newY;
    _newZ=o->_newZ;
    _newSH=o->_newSH;
    _newSV=o->_newSV;
    return true;
}

void PHIUndoTransform::redo()
{
    item()->setTransformation( _newSH, _newSV, _newX, _newY, _newZ );
    PHIUndoCommand::redo();
}

void PHIUndoTransform::undo()
{
    item()->setTransformation( _oldSH, _oldSV, _oldX, _oldY, _oldZ );
    PHIUndoCommand::undo();
}

PHIUndoPage::PHIUndoPage( PHIGraphicsScene *scene, const PHIBasePage &newPage )
    : PHIUndoCommand( 0 ), _scene( scene ), _oldPage( *(scene->page()) ), _newPage( newPage )
{
    qDebug( "PHIUndoPage::PHIUndoPage()" );
    Q_ASSERT( _scene );
    setText( QObject::tr( "Page" )+UT( newPage.id() ) );
}

PHIUndoPage::~PHIUndoPage()
{
    qDebug( "PHIUndoPage::~PHIUndoPage()" );
}

bool PHIUndoPage::mergeWith( const QUndoCommand *other )
{
    const PHIUndoPage *o=dynamic_cast<const PHIUndoPage*>(other);
    Q_ASSERT( o );
    if ( id()!=o->id() ) return false;
    _newPage=o->_newPage;
    return true;
}

void PHIUndoPage::redo()
{
    const PHIBasePage page=*(_scene->page());
    *(_scene->page())=_newPage;
    if ( page.languages()!=_newPage.languages() ||
            page.defaultLanguage()!=_newPage.defaultLanguage() )
        emit _scene->languagesChanged( _newPage.languages() );
    if ( page.bgColor()!=_newPage.bgColor() )
        _scene->setBackgroundBrush( QColor( _newPage.bgColor() ) );
    if ( page.size()!=_newPage.size() ) emit _scene->page()->documentSizeChanged();
}

void PHIUndoPage::undo()
{
    const PHIBasePage page=*(_scene->page());
    *(_scene->page())=_oldPage;
    if ( page.languages()!=_oldPage.languages() ||
            page.defaultLanguage()!=_oldPage.defaultLanguage() )
        emit _scene->languagesChanged( _oldPage.languages() );
    if ( page.bgColor()!=_oldPage.bgColor() )
        _scene->setBackgroundBrush( QColor( _oldPage.bgColor() ) );
    if ( page.size()!=_oldPage.size() ) emit _scene->page()->documentSizeChanged();
}

/*
PHIUndoEffect::PHIUndoEffect( PHIBaseItem *it, const PHIEffect &newEffect )
    : PHIUndoCommand( it ), _newEffect( newEffect )
{
    qDebug( "PHIUndoEffect::PHIUndoEffect()" );
    setText( QObject::tr( "Effect" )+UT( it->id() ) );
    _oldEffect=*(it->effect());
}

PHIUndoEffect::~PHIUndoEffect()
{
    qDebug( "PHIUndoEffect::~PHIUndoEffect()" );
}

bool PHIUndoEffect::mergeWith( const QUndoCommand *other )
{
    const PHIUndoEffect *o=dynamic_cast<const PHIUndoEffect*>(other);
    Q_ASSERT( o );
    if ( id()!=o->id() ) return false;
    _newEffect=o->_newEffect;
    return true;
}

void PHIUndoEffect::redo()
{
    *(item()->effect())=_newEffect;
    PHI::setEffect( item()->graphicsItem(), _newEffect );
}

void PHIUndoEffect::undo()
{
    *(item()->effect())=_oldEffect;
    PHI::setEffect( item()->graphicsItem(), _oldEffect );
}

PHIUndoTextData::PHIUndoTextData( PHIBaseItem *it, const PHITextData &newText )
    : PHIUndoCommand( it )
{
    qDebug( "PHIUndoTextData::PHIUndoTextData()" );
    Q_ASSERT( it->textChangeable() );
    setText( QObject::tr( "Text" )+UT( it->id() ) );
    _oldText=*(it->textData());
    _newText=newText;
}

PHIUndoTextData::~PHIUndoTextData()
{
    qDebug( "PHIUndoTextData::~PHIUndoTextData()" );
}

bool PHIUndoTextData::mergeWith( const QUndoCommand *other )
{
    const PHIUndoTextData *o=dynamic_cast<const PHIUndoTextData*>(other);
    //Q_ASSERT( o );
    if ( id()!=o->id() ) return false;
    //_newText=o->_newText;
    //return true;
    return false;
}

void PHIUndoTextData::redo()
{
    qDebug( "new text (%s): %s", qPrintable( item()->id() ), qPrintable( _newText.text() ) );
    *(item()->textData())=_newText;
    item()->setText( _newText.text() );
}

void PHIUndoTextData::undo()
{
    qDebug( "old text (%s): %s", qPrintable( item()->id() ), qPrintable( _oldText.text() ) );
    *(item()->textData())=_oldText;
    item()->setText( _oldText.text() );
}

PHIUndoStyleSheet::PHIUndoStyleSheet( PHIBaseItem *it, const PHITextData &newCss )
    : PHIUndoCommand( it )
{
    qDebug( "PHIUndoStyleSheet::PHIUndoStyleSheet()" );
    Q_ASSERT( it->styleSheetChangeable() );
    setText( QObject::tr( "Style sheet" )+UT( it->id() ) );
    _oldCss=*(it->styleSheetData());
    _newCss=newCss;
}

PHIUndoStyleSheet::~PHIUndoStyleSheet()
{
    qDebug( "PHIUndoStyleSheet::~PHIUndoStyleSheet()" );
}

bool PHIUndoStyleSheet::mergeWith( const QUndoCommand *other )
{
    const PHIUndoStyleSheet *o=dynamic_cast<const PHIUndoStyleSheet*>(other);
    //Q_ASSERT( o );
    if ( id()!=o->id() ) return false;
    //_newText=o->_newText;
    //return true;
    return false;
}

void PHIUndoStyleSheet::redo()
{
    *(item()->styleSheetData())=_newCss;
    item()->setStyleSheet( _newCss.text() );
}

void PHIUndoStyleSheet::undo()
{
    *(item()->styleSheetData())=_oldCss;
    item()->setStyleSheet( _oldCss.text() );
}
*/

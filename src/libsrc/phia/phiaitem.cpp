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
#include <QtGui>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <QScriptEngine>
#include <QScriptValue>
#include <QDesktopServices>
#include "phia.h"
#include "phiaitem.h"
#include "phiawebview.h"
#include "gitems.h"
#include "gwidgetitems.h"
#include "glayoutitems.h"
#include "gwebitems.h"
#include "phiaanimation.h"

#define PHIGIT GWidgetItem *git=qgraphicsitem_cast<GWidgetItem*>(_git);

PHIAItem::PHIAItem( PHIAWebView *view )
    : PHIBaseItem( view ), _git( 0 ), _setupFinished( false ), _reply( 0 )
{
    connect( this, SIGNAL( setupDone( const PHIAItem* ) ), view,
        SLOT( slotItemSetupDone( const PHIAItem* ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        view, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ) );
    connect( this, SIGNAL( linkRequested( const QUrl& ) ), view,
        SLOT( slotLinkRequested( const QUrl& ) ), Qt::QueuedConnection );
    connect( this, SIGNAL( resetForm() ), view,
        SLOT( slotResetForm() ), Qt::QueuedConnection );
    connect( this, SIGNAL( submitForm( const QString& ) ), view,
        SLOT( slotSubmitFormRequested( const QString& ) ), Qt::QueuedConnection );
}

PHIAItem::~PHIAItem()
{
    QPropertyAnimation *anim;
    foreach ( anim, _animations ) {
        anim->stop();
        delete anim;
    }
    //qDebug( "---deleting %s", _id.data() );
    delete _git;
    //qDebug( "PHIAItem::~PHIAItem()" );
}

PHIAWebView* PHIAItem::view() const
{
    PHIAWebView *webView=qobject_cast<PHIAWebView*>(parent());
    Q_ASSERT( webView );
    return webView;
}

void PHIAItem::setCursor( const QByteArray &c )
{
    PHIBaseItem::setCursor( c );
    if ( c=="auto" ) {
        PHIGIT
        if ( git ) git->widget()->unsetCursor();
        if ( _git->hasCursor() ) _git->unsetCursor();
        return;
    }
    PHIGIT
    if ( git ) git->widget()->setCursor( PHI::toCursorShape( c ) );
    _git->setCursor( PHI::toCursorShape( c ) );
    qDebug( "setCursor: %s", c.data() );
}

QFont PHIAItem::font() const
{
    return _variants.value( DFont, view()->page()->font() ).value<QFont>();
}

qint32 PHIAItem::fontSize() const
{
    return static_cast<qint32>(font().pointSize());
}

QPalette PHIAItem::palette() const
{
    PHIGIT
    if ( git ) return git->palette();
    return _variants.value( DPalette, view()->page()->palette() ).value<QPalette>();
}

void PHIAItem::addChild( const PHIAItem *it )
{
    GAbstractLayoutItem *layout=qgraphicsitem_cast<GAbstractLayoutItem*>(_git);
    GWidgetItem *w=qgraphicsitem_cast<GWidgetItem*>(it->graphicsItem());
    if ( !layout || !w ) return;
    layout->insertChild( it );
}

void PHIAItem::activate()
{
    GAbstractLayoutItem *layout=qgraphicsitem_cast<GAbstractLayoutItem*>(_git);
    if ( layout ) layout->activate();
}

void PHIAItem::setWidth( qreal w )
{
    _width=w;
    updateSize();
}

void PHIAItem::setHeight( qreal h )
{
    _height=h;
    updateSize();
}

void PHIAItem::updateSize()
{
    GAbstractLayoutItem *l=qgraphicsitem_cast<GAbstractLayoutItem*>(_git);
    if ( l ) return l->resize( width(), height() );
    GExternalView *ev=qgraphicsitem_cast<GExternalView*>(_git);
    if ( ev ) {
        ev->setWidth( width() );
        ev->setHeight( height() );
        return;
    }
    PHIGIT
    if ( git ) {
        git->setWidth( width() );
        git->setHeight( height() );
        return;
    }
    GEllipseItem *e=qgraphicsitem_cast<GEllipseItem*>(_git);
    if ( e ) return e->setRect( 0, 0, width(), height() );
    GRectItem *r=qgraphicsitem_cast<GRectItem*>(_git);
    if ( r ) return r->setRect( 0, 0, width(), height() );
    GRoundedRectItem *rr=qgraphicsitem_cast<GRoundedRectItem*>(_git);
    if ( rr ) return rr->setRect( 0, 0, width(), height() );
    GPixmapItem *pix=qgraphicsitem_cast<GPixmapItem*>(_git);
    if ( pix ) return pix->setRect( 0, 0, width(), height() );
    GSvgItem *svg=qgraphicsitem_cast<GSvgItem*>(_git);
    if ( svg ) {
        qDebug() << "svg" << width() << height() << svg->size() << svg->boundingRect();
        svg->setSize( QSizeF( width(), height() ) );
        return;
    }
    GTextItem *t=qgraphicsitem_cast<GTextItem*>(_git);
    if ( t ) return t->setTextWidth( width() );
}

void PHIAItem::setChecked( bool check )
{
    PHIGIT
    if ( git ) return git->setChecked( check );
}

bool PHIAItem::checked() const
{
    PHIGIT
    if ( git ) return git->checked();
    return PHIBaseItem::checked();
}

void PHIAItem::setValueChecked( const QString &v, bool c )
{
    PHIGIT
    if ( git ) return git->setValueChecked( v, c );
}

bool PHIAItem::valueChecked( const QString &v ) const
{
    PHIGIT
    if ( git ) return git->valueChecked( v );
    return false;
}

void PHIAItem::setReadOnly( bool ro )
{
    PHIGIT
    if ( git ) return git->setReadOnly( ro );
}

bool PHIAItem::readOnly() const
{
    PHIGIT
    if ( git ) return git->readOnly();
    return PHIBaseItem::readOnly();
}

void PHIAItem::setDisabled( bool disable )
{
    _git->setEnabled( !disable );
}

bool PHIAItem::disabled() const
{
    return !_git->isEnabled();
}

void PHIAItem::setVisible( bool visible )
{
    _git->setVisible( visible );
}

bool PHIAItem::visible() const
{
    return _git->isVisible();
}

void PHIAItem::focus()
{
    _git->setFocus();
}

void PHIAItem::blur()
{
    _git->clearFocus();
}

void PHIAItem::click()
{
    slotOnClick();
}

void PHIAItem::setAccessKey( const QString &k )
{
    PHIBaseItem::setAccessKey( k.left(1) );
    GExternalView *ev=qgraphicsitem_cast<GExternalView*>(_git);
    if ( ev ) return ev->setAccessKey( k );
    PHIGIT
    if ( git ) return git->setAccessKey( k.left(1) );
}

QString PHIAItem::accessKey() const
{
    return PHIBaseItem::accessKey();
}

void PHIAItem::setTabIndex( quint16 i )
{
    if ( i==tabIndex() ) return;
    PHIBaseItem::setTabIndex( i );
    view()->updateTabOrdering();
}

quint16 PHIAItem::tabIndex() const
{
    return PHIBaseItem::tabIndex();
}

void PHIAItem::setAlignment( quint8 align )
{
    PHIGIT
    if ( git ) return git->setAlignment( align );
    GTextItem *tit=qgraphicsitem_cast<GTextItem*>(_git);
    if ( tit ) {
        QTextDocument *doc=tit->document();
        QTextOption opt=doc->defaultTextOption();
        opt.setAlignment( PHI::toQtAlignment( align ) );
        doc->setDefaultTextOption( opt );
        PHIBaseItem::setAlignment( align );
        return;
    }
}

quint8 PHIAItem::alignment() const
{
    PHIGIT
    if ( git ) return git->alignment();
    return PHIBaseItem::alignment();
}

void PHIAItem::setValue( const QString &v )
{
    GGraphicsTextItem *sit=qgraphicsitem_cast<GGraphicsTextItem*>(_git);
    if ( sit ) {
        sit->setText( v );
        // Adjust new text size (respects font and language)
        const QRectF brect = sit->boundingRect();
        if ( transformPosition()>0 ) {
            qreal xFactor=.0;
            qreal yFactor=.0;
            switch( transformPosition() ) {
            case 1: break;
            case 2: xFactor=.5; yFactor=0; break;
            case 3: xFactor=1; yFactor=0; break;
            case 4: xFactor=0; yFactor=.5; break;
            case 5: xFactor=.5; yFactor=.5; break;
            case 6: xFactor=1; yFactor=.5; break;
            case 7: xFactor=0; yFactor=1; break;
            case 8: xFactor=.5; yFactor=1; break;
            case 9: xFactor=1; yFactor=1; break;
            }
            qreal xOff=xFactor*( width()-brect.width() );
            qreal yOff=yFactor*( height()-brect.height() );
            //qDebug() << "width: " << width() << "height:" << height();
            //qDebug() << "bwidth:" << brect.width() << "bheight" << brect.height();
            //qDebug( "%d xOff %f yOff %f", transformPosition(), xOff, yOff );
            setX( x()+xOff );
            setY( y()+yOff );
            _width=brect.width();
            _height=brect.height();
            setXTranslation( xTranslation()-xOff );
            setYTranslation( yTranslation()-yOff );
        }
        return;
    }
    GTextItem *tit=qgraphicsitem_cast<GTextItem*>(_git);
    if ( tit ) return tit->setValue( v );
    GSvgItem *vit=qgraphicsitem_cast<GSvgItem*>(_git);
    if ( vit ) return vit->setValue( v );
    GExternalView *ev=qgraphicsitem_cast<GExternalView*>(_git);
    if ( ev ) return ev->setValue( v );
    PHIGIT
    if ( git ) return git->setValue( v );
}

QString PHIAItem::value() const
{
    GGraphicsTextItem *sit=qgraphicsitem_cast<GGraphicsTextItem*>(_git);
    if ( sit ) return sit->text();
    GTextItem *tit=qgraphicsitem_cast<GTextItem*>(_git);
    if ( tit ) return tit->value();
    GSvgItem *vit=qgraphicsitem_cast<GSvgItem*>(_git);
    if ( vit ) return vit->value();
    GExternalView *ev=qgraphicsitem_cast<GExternalView*>(_git);
    if ( ev ) return ev->value();
    PHIGIT
    if ( git ) return git->value();
    return PHIBaseItem::value();
}

void PHIAItem::setLabel( const QString &l )
{
    PHIGIT
    if ( git ) git->setLabel( l );
}

QString PHIAItem::label() const
{
    PHIGIT
    if ( git ) return git->label();
    return PHIBaseItem::label();
}

void PHIAItem::setUrl( const QString &u )
{
    PHIGIT
    if ( git ) return git->setUrl( u );
    //GWebItem *w=qgraphicsitem_cast<GWebItem*>(_git);
    //if ( w ) return w->setUrl( u );
}

QString PHIAItem::url() const
{
    PHIGIT
    if ( git ) return git->url();
    //GWebItem *w=qgraphicsitem_cast<GWebItem*>(_git);
    //if ( w ) return w->url();
    return PHIBaseItem::url();
}

void PHIAItem::setStyleSheet( const QString &css )
{
    PHIGIT
    if ( git ) return git->setStyleSheet( css );
    GTextItem *tit=qgraphicsitem_cast<GTextItem*>(_git);
    if ( tit ) return tit->setStyleSheet( css );
}

QString PHIAItem::styleSheet() const
{
    PHIGIT
    if ( git ) return git->styleSheet();
    return PHIBaseItem::styleSheet();
}

void PHIAItem::setToolTip( const QString &s )
{
    GDiaShowItem *dia=qgraphicsitem_cast<GDiaShowItem*>(_git);
    if ( dia ) return dia->setToolTipList( s );
    if ( _git ) _git->setToolTip( s );
}

QString PHIAItem::toolTip() const
{
    GDiaShowItem *dia=qgraphicsitem_cast<GDiaShowItem*>(_git);
    if ( dia ) return dia->toolTipList();
    if ( _git ) return _git->toolTip();
    return QString();
}

void PHIAItem::setMaxLength( quint16 max )
{
    PHIGIT
    if ( git ) return git->setMaxLength( max );
    PHIBaseItem::setMaxLength( max );
}

quint16 PHIAItem::maxLength() const
{
    PHIGIT
    if ( git ) return git->maxLength();
    return PHIBaseItem::maxLength();
}

void PHIAItem::setDelimiter( const QString &d )
{
    PHIGIT
    if ( git ) git->setDelimiter( d );
}

QString PHIAItem::delimiter() const
{
    PHIGIT
    if ( git ) return git->delimiter();
    return PHIBaseItem::delimiter();
}

QStringList PHIAItem::selectedItems() const
{
    PHIGIT
    if ( git ) return git->selectedItems();
    return QStringList();
}

void PHIAItem::setFont( const QFont &font )
{
    QFont f=font;
    _variants.insert( DFont, f );

#ifdef PHIMACFONT
    f.setPointSizeF( font.pointSizeF()*PHI::fontSizeFactor() );
#endif

    PHIGIT
    if ( git ) {
        qDebug( "FONT size %d", f.pointSize() );
        return git->setFont( f );
    }    
    GGraphicsTextItem *sit=qgraphicsitem_cast<GGraphicsTextItem*>(_git);
    if ( sit ) {
        sit->setFont( f );
        setValue( value() ); // update geometry for translated text
        return;
    }
    GTextItem *tit=qgraphicsitem_cast<GTextItem*>(_git);
    if ( tit ) {
        tit->setFont( f );
        return;
    }
}

void PHIAItem::setFontSize( qint32 size )
{
    QFont f=font();
    f.setPointSize( size );
    setFont( f );
}

void PHIAItem::setPalette( const QPalette &p )
{
    //qDebug( "button col=%s", qPrintable( p.button().color().name() ) );
    GTextItem *t=qgraphicsitem_cast<GTextItem*>(_git);
    if ( t ) {
        if ( !PHIBaseItem::color().isValid() )
            t->setDefaultTextColor( p.color( QPalette::WindowText ) );
        else t->setDefaultTextColor( PHIBaseItem::color() );
        if ( PHIBaseItem::styleSheet().isEmpty() ) { // use palette to color links
            setStyleSheet( PHI::linkStyleSheet( p,
                !(view()->page()->attributes() & PHIPage::ANoUnderlineLinks) ) );
        }
        return;
    }
    PHIGIT
    if ( git ) {
        git->setPalette( p );
        QLineEdit *edit=qobject_cast<QLineEdit*>(git->widget());
        if ( edit ) edit->setPalette( p );

        if ( PHIBaseItem::color().isValid() ) setColor( PHIBaseItem::color() );
        else qDebug( "id %s color is not valid", id().data() );
        if ( PHIBaseItem::outlineColor().isValid() ) setOutlineColor( PHIBaseItem::outlineColor() );
        else qDebug( "id %s outcolor is not valid", id().data() );
        if ( wid()==PHI::CALENDAR ) {
            QPalette calpal=p;
            calpal.setColor( QPalette::Window, qApp->palette().color( QPalette::Window ) );
            git->setPalette( calpal );
        } else if ( wid()==PHI::RICH_TEXT ) {
            QPalette pal=p;
            if ( PHIBaseItem::outlineColor().isValid() )
                pal.setColor( QPalette::Base, PHIBaseItem::outlineColor() );
            else pal.setColor( QPalette::Base, QColor( Qt::transparent ) );
            if ( PHIBaseItem::color().isValid() )
                pal.setColor( QPalette::Text, PHIBaseItem::color() );
            else pal.setColor( QPalette::Text, p.color( QPalette::WindowText ) );
            git->setPalette( pal );
            if ( PHIBaseItem::styleSheet().isEmpty() ) {
                setStyleSheet( PHI::linkStyleSheet( p,
                    !(view()->page()->attributes() & PHIPage::ANoUnderlineLinks) ) );
            }
        } else if ( wid()==PHI::DATEEDIT || wid()==PHI::FILE_BUTTON ) {
            git->overwritePalette( p );
        }
        return;
    }
}

QPixmap PHIAItem::loadingPixmap() const
{
    QString alt=PHIBaseItem::toolTip();
    if ( alt.isEmpty() ) alt=">>> "+name();
    QImage img( static_cast<int>(width()), static_cast<int>(height()),
        QImage::Format_ARGB32_Premultiplied );
    QPainter p;
    p.begin( &img );
    p.setCompositionMode( QPainter::CompositionMode_Clear );
    p.fillRect( QRectF( 0., 0., width(), height() ), QColor( 0, 0, 0, 0 ) );
    p.setCompositionMode( QPainter::CompositionMode_Source );
    p.drawText( QRectF( 0., 0., width(), height() ), Qt::AlignCenter | Qt::TextWordWrap, alt );
    p.end();
    return QPixmap::fromImage( img );
}

void PHIAItem::slotLinkActivated( const QUrl &u )
{
    QUrl url=createLocalUrl( u );
    qDebug( "slotLinkActivated url=%s", qPrintable( u.toString() ) );
    emit linkRequested( url );
}

void PHIAItem::slotLinkActivated( const QString &link )
{
    if ( link.contains( "javascript:", Qt::CaseInsensitive ) ) {
        QString script=link;
        script.replace( QRegExp( ".*javascript:", Qt::CaseInsensitive ), "" );
        //qWarning( "SCRIPT %s", qPrintable( script ) );
        view()->runScript( script );
        return;
    } else if ( link.contains( "mailto:", Qt::CaseSensitive ) || link.contains( "file:", Qt::CaseSensitive ) ) {
        QDesktopServices::openUrl( QUrl( link ) );
        return;
    }
    QUrl url=PHI::createUrlForLink( view()->url(), link );
    if ( url.isValid() ) emit linkRequested( createLocalUrl( url ) );
}

void PHIAItem::slotLinkHovered( const QString &s )
{
    if ( s.isEmpty() ) {
        emit linkHovered( QString(), QString(), QString() );
        setCursor( PHIBaseItem::cursor() );
        //_git->setCursor( QCursor( Qt::ArrowCursor ) );
    } else {
        if ( s.contains( "javascript:", Qt::CaseInsensitive ) || s.contains( "mailto:", Qt::CaseSensitive )
                || s.contains( "file:", Qt::CaseSensitive ) )
            emit linkHovered( s, QString(), QString() );
        else {
            QUrl url=PHI::createUrlForLink( view()->url(), s );
            emit linkHovered( createLocalUrl( url ).toString(), QString(), QString() );
        }
        _git->setCursor( QCursor( Qt::PointingHandCursor ) );
    }
}

QUrl PHIAItem::createLocalUrl( const QUrl &link )
{
    QUrl url=link;
    //if ( view()->url().host()==link.host() ) { // same host so add session and lang
        QString sid=view()->page()->session();
        QString philang=view()->page()->lang();
        if ( !sid.isEmpty() )
            if ( !url.hasQueryItem( "phisid" ) ) url.addQueryItem( "phisid", sid );
        if ( !philang.isEmpty() )
            if ( !url.hasQueryItem( "philang" ) ) url.addQueryItem( "philang", philang );
    //}
    return url;
}

void PHIAItem::setStartAngle( qint16 a )
{
    GEllipseItem *it=qgraphicsitem_cast<GEllipseItem*>(_git);
    if ( it ) {
        it->setStartAngle( static_cast<int>(a) );
        PHIBaseItem::setStartAngle( a );
    }
}

void PHIAItem::setSpanAngle( qint16 a )
{
    GEllipseItem *it=qgraphicsitem_cast<GEllipseItem*>(_git);
    if ( it ) {
        it->setSpanAngle( static_cast<int>(a) );
        PHIBaseItem::setSpanAngle( a );
    }
}

void PHIAItem::setBorderRadius( qint16 r )
{
    GRoundedRectItem *it=qgraphicsitem_cast<GRoundedRectItem*>(_git);
    if ( it ) {
        it->setRadius( static_cast<int>(r) );
        PHIBaseItem::setBorderRadius( r );
    }
}

void PHIAItem::setFadeTime( qint16 ft )
{
    GDiaShowItem *it=qgraphicsitem_cast<GDiaShowItem*>(_git);
    if ( it ) {
        it->setFadeTime( ft );
        PHIBaseItem::setFadeTime( ft );
    }
}

void PHIAItem::setFadeInterval( qint16 fp )
{
    GDiaShowItem *it=qgraphicsitem_cast<GDiaShowItem*>(_git);
    if ( it ) {
        it->setFadeInterval( fp );
        PHIBaseItem::setFadeInterval( fp );
    }
}

void PHIAItem::slotCheckStateToggled( bool b )
{
    qDebug( "slotCheckStateToggled %s (%d)", qPrintable( name() ), b );
    slotOnChange();
    if ( wid()!=PHI::RADIO_BUTTON ) return;
    if ( !b ) return;
    // unselect other radio buttons
    QHash <QByteArray, PHIBaseItem*> items=view()->page()->items();
    PHIBaseItem *it;
    foreach ( it, items.values() ) {
        if ( it->wid()==PHI::RADIO_BUTTON ) {
            if ( it==this ) continue;
            if ( it->parentName()!=parentName() ) continue;
            it->setChecked( false );
            qobject_cast<PHIAItem*>(it)->slotOnChange();
        }
    }
}

void PHIAItem::slotRequestForImageFinished()
{
    int number=-1;
    QByteArray arr;
    if ( _reply ) {
        _setupFinished=true;
        emit setupDone( this );
        if ( _reply->error()!=QNetworkReply::NoError ) {
            _reply->deleteLater();
            _reply=0;
            return;
        }
        arr=_reply->readAll();
        _reply->deleteLater();
        _reply=0;
    } else {
        QNetworkReply *reply=0;
        bool found=false;
        foreach ( reply, _replyHash.keys() ) {
            number=reply->request().attribute( QNetworkRequest::User ).toInt();
            if ( reply->isFinished() && number==_replyHash.value( reply ) ) {
                found=true;
                break;
            }
        }
        if ( found && reply ) {
            _replyHash.remove( reply );
            if ( _replyHash.isEmpty() ) {
                _setupFinished=true;
                emit setupDone( this );
            }
            if ( reply->error()!=QNetworkReply::NoError ) {
                reply->deleteLater();
                return;
            }
            arr=reply->readAll();
            reply->deleteLater();
        } else return;
    }

    QPixmap pix;
    pix.loadFromData( arr );
    if ( pix.isNull() ) qDebug( "ERROR pix %d", arr.size() );
    else qDebug( "pix is valid %d", arr.size() );
    switch ( wid() ) {
    case PHI::IMAGE: {
        GPixmapItem *pit=qgraphicsitem_cast<GPixmapItem*>(_git);
        Q_ASSERT( pit );
        return pit->setPixmap( pix );
    }
    case PHI::DIA_SHOW: {
        qDebug( "TYPE %d", _git->type() );
        GDiaShowItem *dia=qgraphicsitem_cast<GDiaShowItem*>(_git);
        Q_ASSERT( dia );
        return dia->setPixmap( pix, number );
    }
    default:
        PHIGIT
        if ( git ) return git->setPixmap( pix, number );
    }
}

void PHIAItem::generateItem()
{
    //PHIItem::Properties p=this->itemProperties();
    _setupFinished=true;
    qDebug( "WID=%d (%s) %d", static_cast<int>(wid()), id().data(), checked() );

    switch ( wid() ) {
    case PHI::LINE_EDIT: _git=new GLineEditItem( width(), height(), this ); break;
    case PHI::SEARCH: _git=new GSearchItem( width(), height(), this ); break;
    case PHI::PHONE: _git=new GPhoneNumberItem( width(), height(), this ); break;
    case PHI::EMAIL: _git=new GEmailItem( width(), height(), this ); break;
    case PHI::PASSWD: _git=new GPasswordItem( width(), height(), this ); break;
    case PHI::BUTTON: _git=new GButtonItem( width(), height(), this ); break;
    case PHI::SUBMIT_BUTTON: _git=new GSubmitButtonItem( width(), height(), this ); break;
    case PHI::RESET_BUTTON: _git=new GResetButtonItem( width(), height(), this ); break;
    case PHI::RADIO_BUTTON: _git=
        new GRadioButtonItem( PHIBaseItem::label(), width(), height(), this ); break;
    case PHI::IMAGE_BUTTON: _git=new GImageButtonItem( loadingPixmap(), width(), height(), this );
        requestImage(); break;
    case PHI::CHECK_BOX: _git=
        new GCheckBoxItem( PHIBaseItem::label(), width(), height(), this ); break;
    case PHI::MULTI_LINE_EDIT: _git=new GMultiLineEditItem( width(), height(), this ); break;
    case PHI::LABEL: _git=new GLabelItem( width(), height(), this ); break;
    case PHI::PHISYS_LINK:
    case PHI::ROLLOVER_BUTTON: _git=new GRolloverButton( width(), height(), this ); break;
    case PHI::LINK: _git=new GLinkItem( width(), height(), this ); break;
    case PHI::HIDDEN: _git=new GHiddenItem( width(), height(), this ); break;
    case PHI::FILE_BUTTON: _git=new GFileItem( width(), height(), this ); break;
    case PHI::RECT: _git=new GRectItem( width(), height(), this ); break;
    case PHI::ROUNDED_RECT: _git=new GRoundedRectItem( width(), height(), this ); break;
    case PHI::ELLIPSE: _git=
        new GEllipseItem( width(), height(), PHIBaseItem::startAngle(), PHIBaseItem::spanAngle(), this ); break;
    case PHI::GRAPH_TEXT: _git=new GGraphicsTextItem( PHIBaseItem::value(), PHIBaseItem::font(), this ); break;
    case PHI::TEXT: _git=new GTextItem( PHIBaseItem::value(), PHIBaseItem::font(), width(), this ); break;
    case PHI::RICH_TEXT: _git=new GTextBrowserItem( width(), height(), this ); break;
    case PHI::DECIMAL: _git=new GDecimalNumberItem( width(), height(), this ); break;
    case PHI::REALNUMBER: _git=new GRealNumberItem( width(), height(), this ); break;

    case PHI::COUNTRY:
    case PHI::COMBO_BOX: _git=new GComboBoxItem( PHIBaseItem::value(), width(), height(), this ); break;
    case PHI::LIST: _git=new GListBoxItem( PHIBaseItem::value(), width(), height(), this ); break;
    case PHI::TABLE: _git=new GTableItem( PHIBaseItem::value(), width(), height(), this ); break;
    case PHI::IMAGE: _git=new GPixmapItem( loadingPixmap(), this ); requestImage(); break;
    case PHI::CALENDAR: _git=new GCalendarItem( width(), height(), this ); break;
    case PHI::DATEEDIT: _git=new GDateEditItem( width(), height(), this ); break;
    case PHI::SVG: _git=new GSvgItem( this, width(), height() ); break;
    case PHI::LANG_SELECTOR: _git=new GLangSelectorItem( PHIBaseItem::value(), width(), height(), this ); break;
    case PHI::CHECK_LIST: _git=new GCheckListItem( PHIBaseItem::value(), width(), height(), this ); break;
    case PHI::HTML_DOC: _git=new GWebItem( width(), height(), this ); break;
    case PHI::GOOGLE_STATIC_MAP: _git=new GStaticGoogleMapItem( width(), height(), this ); break;
    case PHI::TWITTER: _git=new GTwitterItem( width(), height(), this ); break;
    case PHI::YOUTUBE: _git=new GYouTubeItem( width(), height(), this ); break;
    case PHI::FACEBOOK_LIKE: _git=new GFBLikeItem( width(), height(), this ); break;
    case PHI::GOOGLE_PLUS: _git=new GGooglePlusItem( width(), height(), this ); break;
    case PHI::DIA_SHOW: _git=new GDiaShowItem( width(), height(), this ); break;
    case PHI::PROGRESSBAR: _git=new GProgressBarItem( width(), height(), this ); break;

    case PHI::HSPACE: _git=new GHSpacerItem( width(), height(), this ); break;
    case PHI::VSPACE: _git=new GVSpacerItem( width(), height(), this ); break;
    case PHI::LAYOUT_HBOX: _git=new GLinearLayoutItem( width(), height(), this, childIds(), true ); break;
    case PHI::LAYOUT_VBOX: _git=new GLinearLayoutItem( width(), height(), this, childIds(), false ); break;

    case PHI::LAYOUT_FORM:
    case PHI::LAYOUT_GRID:
    case PHI::LAYOUT_ADDRESS:
    case PHI::LAYOUT_DELIVERY:
    case PHI::LAYOUT_CREDIT_CARD:
    case PHI::LAYOUT_CONTACT:
    case PHI::LAYOUT_PERIOD:
    case PHI::LAYOUT_REGISTER:
    case PHI::LAYOUT_LOGIN: _git=new GGridLayoutItem( width(), height(), this, gridLayoutInfo() ); break;

    default:
        _git=new GGraphicsTextItem( QString(), font(), this );
    }
    _git->setPos( PHIBaseItem::x(), PHIBaseItem::y() );
    setDelimiter( PHIBaseItem::delimiter() );
    if ( !PHIBaseItem::styleSheet().isEmpty() ) setStyleSheet( PHIBaseItem::styleSheet() );
    if ( !PHIBaseItem::toolTip().isEmpty() ) setToolTip( PHIBaseItem::toolTip() );
    if ( PHIBaseItem::color().isValid() ) setColor( PHIBaseItem::color() );
    else setColor( QColor( Qt::black ) );
    if ( PHIBaseItem::outlineColor().isValid() ) setOutlineColor( PHIBaseItem::outlineColor() );
    else setOutlineColor( QColor( Qt::black ) );
    if ( PHIBaseItem::pattern()==15 ) setPattern( PHIBaseItem::pattern() );
    setUrl( PHIBaseItem::url() );
    setValue( PHIBaseItem::value() );
    setReadOnly( PHIBaseItem::readOnly() );
    setDisabled( PHIBaseItem::disabled() );
    setVisible( PHIBaseItem::visible() );
    setChecked( PHIBaseItem::checked() );
    setAlignment( PHIBaseItem::alignment() );
    setOpacity( PHIBaseItem::opacity() );
    setZValue( PHIBaseItem::zValue() );
    setMaxLength( PHIBaseItem::maxLength() );
    setBorderRadius( PHIBaseItem::borderRadius() );
    setFadeTime( PHIBaseItem::fadeTime() );
    setFadeInterval( PHIBaseItem::fadeInterval() );
    setDropable( PHIBaseItem::droppable() );
    //qDebug( "FONT %d", PHIBaseItem::font( view()->page()->font() ).pointSize() );
    setFont( PHIBaseItem::font( view()->page()->font() ) );
    setRolloverTextColor( PHIBaseItem::rolloverTextColor() );
    setRolloverBackgroundColor( PHIBaseItem::rolloverBackgroundColor() );
    //qDebug( "color - %s", qPrintable( PHIBaseItem::color().name() ) );
    //qDebug( "color - %s", qPrintable( PHIBaseItem::outlineColor().name() ) );
    for ( int i=0; i<PHIBaseItem::pictureBookIds().count(); i++ ) {
        requestImage( i );
    }

    if ( attributes() & PHIItem::AEffectData ) {
        qint32 start, duration;
        quint8 axis, ease;
        if ( _effect->effects() & PHIEffect::EFadeIn ) {
            qreal maxOpac;
            _effect->fadeIn( start, duration, maxOpac, ease );
            setFadeIn( start, duration, maxOpac, PHI::toEasingCurveString( ease ) );
        }
        if ( _effect->effects() & PHIEffect::EFadeOut ) {
            qreal minOpac;
            _effect->fadeOut( start, duration, minOpac, ease );
            setFadeOut( start, duration, minOpac, PHI::toEasingCurveString( ease ) );
        }
        if ( _effect->effects() & PHIEffect::EGraphics ) setGraphicsEffect();
        if ( _effect->effects() & PHIEffect::EMoveTo ) {
            qint32 left, top;
            _effect->moveTo( start, duration, left, top, ease );
            setMoveTo( start, duration, left, top, PHI::toEasingCurveString( ease ) );
        }
        if ( _effect->effects() & PHIEffect::EMoveBy ) {
            qint32 x, y, w, h;
            _effect->moveBy( start, duration, x, y, w, h, ease );
            setMoveBy( start, duration, x, y, w, h, PHI::toEasingCurveString( ease ) );
        }
        if ( _effect->effects() & PHIEffect::ERotateIn ) {
            _effect->rotateIn( axis, start, duration );
            setRotateIn( axis, start, duration, PHI::defaultEasingCurve() );
        }
        if ( _effect->effects() & PHIEffect::ERotateOut ) {
            _effect->rotateOut( axis, start, duration );
            setRotateOut( axis, start, duration, PHI::defaultEasingCurve() );
        }
        if ( _effect->effects() & PHIEffect::ERotate ) {
            qreal stepX, stepY, stepZ;
            _effect->rotate( axis, stepX, stepY, stepZ );
            setRotate( axis, stepX, stepY, stepZ, "linear" );
        }
    }
    updateTransformation();
    view()->scene()->addItem( _git );
    if ( wid()==PHI::TABLE || wid()==PHI::CHECK_LIST )
        QTimer::singleShot( 0, this, SLOT( slotUpdateColumnsHack() ) );

    if ( _setupFinished ) emit setupDone( this );
}

void PHIAItem::slotUpdateColumnsHack()
{
    setValue( PHIBaseItem::value() );
}

QColor PHIAItem::color() const
{
    QGraphicsTextItem *t=qgraphicsitem_cast<QGraphicsTextItem*>(_git);
    if ( t ) return t->defaultTextColor();
    if ( isShapeItem() ) {
        QAbstractGraphicsShapeItem *s=qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_git);
        Q_ASSERT( s );
        return s->brush().color();
    }
    PHIGIT
    if ( git ) return git->color();
    return PHIBaseItem::color();
}

void PHIAItem::setColor( const QColor &c )
{
    if ( !c.isValid() ) return;
    QGraphicsTextItem *t=qgraphicsitem_cast<QGraphicsTextItem*>(_git);
    if ( t ) {
        qDebug( "------------------ %s", qPrintable( c.name() ) );
        t->setDefaultTextColor( c );
        t->setHtml( value() );
        return;
    }
    if ( isShapeItem() ) {
        QAbstractGraphicsShapeItem *s=qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_git);
        Q_ASSERT( s );
        QBrush brush( c );
        qDebug( "setColor brush %s %d", qPrintable( c.name() ), pattern() );
        if ( pattern()==15 ) s->setBrush( QBrush( gradient() ) );
        else {
            brush.setStyle( static_cast<Qt::BrushStyle>(pattern()) );
            s->setBrush( brush );
        }
        return;
    }
    PHIGIT
    if ( git ) return git->setColor( c );
}

void PHIAItem::setPattern( quint8 p )
{
    if ( p > PHI::maxPatternStyle() ) return;
    if ( isShapeItem() ) {
        QAbstractGraphicsShapeItem *s=qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_git);
        Q_ASSERT( s );
        QBrush brush( color() );
        if ( p==15 ) s->setBrush( QBrush( gradient() ) );
        else {
            brush.setStyle( static_cast<Qt::BrushStyle>(pattern()) );
            s->setBrush( brush );
        }
        qDebug( "setPattern %d", p );
        return;
    }
    PHIBaseItem::setPattern( p );
}

QColor PHIAItem::outlineColor() const
{
    if ( isShapeItem() ) {
        QAbstractGraphicsShapeItem *s=qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_git);
        Q_ASSERT( s );
        return s->pen().color();
    }
    PHIGIT
    if ( git ) return git->outlineColor();
    return PHIBaseItem::outlineColor();
}

void PHIAItem::setOutlineColor( const QColor &c )
{
    if ( !c.isValid() ) return;
    if ( isShapeItem() ) {
        QAbstractGraphicsShapeItem *s=qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_git);
        Q_ASSERT( s );
        QPen pen( c );
        pen.setWidthF( penWidth() );
        pen.setStyle( static_cast<Qt::PenStyle>(line()) );
        pen.setJoinStyle( Qt::MiterJoin );
        s->setPen( pen );
        return;
    }
    PHIGIT
    if ( git ) return git->setOutlineColor( c );
}

void PHIAItem::setRolloverTextColor( const QColor &c )
{
    PHIGIT
    if ( git ) return git->setRolloverTextColor( c );
}

QColor PHIAItem::rolloverTextColor() const
{
    PHIGIT
    if ( git ) return git->rolloverTextColor();
    return PHIBaseItem::rolloverTextColor();
}

void PHIAItem::setRolloverBackgroundColor( const QColor &c )
{
    PHIGIT
    if ( git ) return git->setRolloverBackgroundColor( c );
}

QColor PHIAItem::rolloverBackgroundColor() const
{
    PHIGIT
    if ( git ) return git->rolloverBackgroundColor();
    return PHIBaseItem::rolloverBackgroundColor();
}

bool PHIAItem::isShapeItem() const
{
    if ( qgraphicsitem_cast<GEllipseItem*>(_git) ) return true;
    if ( qgraphicsitem_cast<GRectItem*>(_git) ) return true;
    if ( qgraphicsitem_cast<GRoundedRectItem*>(_git) ) return true;
    if ( qgraphicsitem_cast<GGraphicsTextItem*>(_git) ) return true;
    return false;
}

void PHIAItem::setShadow( const QColor &c, qreal xOff, qreal yOff, qreal radius )
{
    _effect->setShadow( c, xOff, yOff, radius );
    setGraphicsEffect();
}

void PHIAItem::setBlur( qreal radius )
{
    _effect->setBlur( radius );
    setGraphicsEffect();
}

void PHIAItem::setSurface( qreal yOff, qreal size )
{
    _effect->setSurface( yOff, size );
    setGraphicsEffect();
}

void PHIAItem::setColorize( const QColor &c, qreal strength )
{
    _effect->setColorize( c, strength );
    setGraphicsEffect();
}

void PHIAItem::setGraphicsEffect()
{
    PHI::setEffect( _git, *_effect );
}

void PHIAItem::resetItem()
{
    setValue( PHIBaseItem::value() );
    setChecked( PHIBaseItem::checked() );
    setReadOnly( PHIBaseItem::readOnly() );
    setDisabled( PHIBaseItem::disabled() );
    setLabel( PHIBaseItem::label() );
    setVisible( PHIBaseItem::visible() );
    //setDelimiter( PHIBaseItem::delimiter() );
    setAlignment( PHIBaseItem::alignment() );
}

void PHIAItem::requestImage( int number )
{
    _setupFinished=false;
    QUrl url=view()->url();
    url.setEncodedPath( "phi.phis" );
    //url.setScheme( QString( "http" ) ); // use unsecure connection

    QList <QPair <QByteArray, QByteArray> > list;
    QPair <QByteArray, QByteArray> pair;
    pair.first="phiimg";
    if ( number==-1 ) {
        QUuid uid( imageId() );
        pair.second=imageId().toUtf8();
        list << pair;
        if ( imageId().startsWith( "phi" ) || !uid.isNull() ) {
            pair.first="phitmp";
            pair.second="1";
            list << pair;
        }
    } else {
        QString pbid=pictureBookIds().at( number );
        qDebug( "REQUEST %s (%s#%d)", qPrintable( this->name() ), qPrintable( pbid ), number );
        QUuid uid( pbid );
        pair.second=pbid.toUtf8();
        list << pair;
        if ( pbid.startsWith( "phi" ) || !uid.isNull() ) {
            pair.first="phitmp";
            pair.second="1";
            list << pair;
        }
    }
    url.setEncodedQueryItems( list );

    QNetworkRequest request;
    request.setUrl( url );
    request.setRawHeader( "Accept", "image/png,image/*;q=0.7" );
    request.setRawHeader( "Referer", view()->url().toEncoded() );
    request.setAttribute( QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache );

    if ( number==-1 ) {
        qDebug( "REQUEST IMAGE %s", qPrintable( url.toString() ) );
        _reply=view()->networkAccessManager()->get( request );
        if ( !_reply ) {
            slotRequestForImageFinished();
            return;
        }
        connect( _reply, SIGNAL ( finished() ), this, SLOT( slotRequestForImageFinished() ) );
    } else {
        qDebug( "REQUEST IMAGEBOOK %s", qPrintable( url.toString() ) );
        request.setAttribute( QNetworkRequest::User, number );
        QNetworkReply *reply=view()->networkAccessManager()->get( request );
        if ( !reply ) return;
        connect( reply, SIGNAL( finished() ), this, SLOT( slotRequestForImageFinished() ) );
        _replyHash.insert( reply, number );
    }
}

void PHIAItem::updateTransformation()
{
    /*
    QTransform xrot;
    xrot.rotate( xRotation(), Qt::XAxis );
    _git->setTransform( xrot, true );
    */
    //qreal adjust=0;
    // suppress visual artefacts on 180 and 360 degree rotation:
    //if ( _currentEffects!=CENone ) adjust=0.0001;
    QTransform trans, shear, xRot, yRot, zRot;
    xRot.rotate( xRotation(), Qt::XAxis );
    yRot.rotate( yRotation(), Qt::YAxis );
    zRot.rotate( zRotation(), Qt::ZAxis );
    trans.translate( -xTranslation(), -yTranslation() );
    shear.shear( hShear(), vShear() );
    _transform=trans*shear*zRot*yRot*xRot*trans.inverted();
    //_transform=shear*zRot*yRot*xRot;
    _git->setTransform( _transform );

}

void PHIAItem::clearEffects()
{
    _git->setGraphicsEffect( 0 );
    //QPropertyAnimation *anim;
    //foreach ( anim, _animations ) anim->stop();
}

void PHIAItem::stopAnimations()
{
    QPropertyAnimation *anim;
    foreach ( anim, _animations ) {
        anim->stop();
        delete anim;
    }
    _animations.clear();
}

void PHIAItem::setFadeIn( qint32 start, qint32 duration, qreal maxOpac, const QString &ease )
{
    setOpacity( 0 );
    PHIBaseItem::setFadeIn( start, duration, maxOpac, ease );
    new PHIAFaderAnimation( this, PHIAFaderAnimation::FIn, maxOpac, start, duration, ease );
}

void PHIAItem::setFadeOut( qint32 start, qint32 duration, qreal minOpac, const QString &ease )
{
    setOpacity( 1 );
    PHIBaseItem::setFadeOut( start, duration, minOpac, ease );
    new PHIAFaderAnimation( this, PHIAFaderAnimation::FOut, minOpac, start, duration, ease );
}

void PHIAItem::setMoveTo( qint32 start, qint32 duration, qint32 left, qint32 top, const QString &ease )
{
    qDebug( "%s move To %d %d", id().data(), left, top );
    PHIBaseItem::setMoveTo( start, duration, left, top, ease );
    new PHIAAnimation( this, EPMoveX, left, start, duration, ease );
    new PHIAAnimation( this, EPMoveY, top, start, duration, ease );
}

void PHIAItem::setMoveBy( qint32 start, qint32 duration, qint32 dx, qint32 dy, qint32 dw, qint32 dh,
        const QString &ease )
{
    qDebug( "%s move By %d %d %d %d", id().data(), dx, dy, dw, dh );
    PHIBaseItem::setMoveBy( start, duration, dx, dy, dw, dh, ease );
    if ( dx ) new PHIAAnimation( this, EPMoveX, dx, start, duration, ease, true );
    if ( dy ) new PHIAAnimation( this, EPMoveY, dy, start, duration, ease, true );
    if ( dw ) new PHIAAnimation( this, EPMoveW, dw, start, duration, ease, true );
    if ( dh ) new PHIAAnimation( this, EPMoveH, dh, start, duration, ease, true );
}

void PHIAItem::setSlideUp( qint32 start, qint32 duration, const QString &ease )
{
    qDebug( "%s slide up %d %d", id().data(), start, duration );
    new PHIAVSlideAnimation( this, PHIAVSlideAnimation::SUp, start, duration, ease );
}

void PHIAItem::setSlideDown( qint32 start, qint32 duration, const QString &ease )
{
    qDebug( "%s slide down %d %d", id().data(), start, duration );
    new PHIAVSlideAnimation( this, PHIAVSlideAnimation::SDown, start, duration, ease );
}

void PHIAItem::setRotateIn( quint8 axis, qint32 start, qint32 duration, const QString &ease )
{
    PHIBaseItem::setRotateIn( axis, start, duration, ease );
    if ( axis & 0x1 ) {
        new PHIAAnimation( this, EPRotateX, 0., start, duration, ease );
        setXRotation( 90. );
        setVisible( true );
    }
    if ( axis & 0x2 ) {
        new PHIAAnimation( this, EPRotateY, 0., start, duration, ease );
        setYRotation( 90. );
        setVisible( true );
    }
}

void PHIAItem::setRotateOut( quint8 axis, qint32 start, qint32 duration, const QString &ease )
{
    PHIBaseItem::setRotateOut( axis, start, duration, ease );
    if ( axis & 0x1 ) new PHIAAnimation( this, EPRotateX, 90., start, duration, ease );
    if ( axis & 0x2 ) new PHIAAnimation( this, EPRotateY, 90., start, duration, ease );
    QTimer::singleShot( start+duration, this, SLOT( slotClearRotateOut() ) );
}

void PHIAItem::slotClearRotateOut()
{
    setVisible( false );
    setXRotation( 0. );
    setYRotation( 0. );
}

void PHIAItem::setRotate( quint8 axis, qreal stepX, qreal stepY, qreal stepZ, const QString &ease )
{
    PHIBaseItem::setRotate( axis, stepX, stepY, stepZ, ease );
    QPropertyAnimation *anim;
    qreal endVal;
    QList <QPropertyAnimation*> stoplist;
    if ( axis & 0x1 ) {
        if ( stepX==0 ) {
            stoplist=_animations.values( EPRotateX );
            foreach ( anim, stoplist ) {
                anim->stop();
            }
        } else {
            endVal=stepX>0 ? 360. : -360.;
            setXRotation( 0 );
            new PHIAAnimation( this, EPRotateX, endVal, 0, 9000/qAbs(stepX), ease, false, true );
        }
    }
    if ( axis & 0x2 ) {
        if ( stepY==0 ) {
            stoplist=_animations.values( EPRotateY );
            foreach ( anim, stoplist ) {
                anim->stop();
            }
        } else {
            endVal=stepY>0 ? 360. : -360.;
            setYRotation( 0 );
            new PHIAAnimation( this, EPRotateY, endVal, 0, 9000/qAbs(stepY), ease, false, true );
        }
    }
    if ( axis & 0x4 ) {
        if ( stepZ==0 ) {
            stoplist=_animations.values( EPRotateZ );
            foreach ( anim, stoplist ) {
                anim->stop();
            }
        } else {
            endVal=stepZ>0 ? 360. : -360.;
            setZRotation( 0 );
            new PHIAAnimation( this, EPRotateZ, endVal, 0, 9000/qAbs(stepZ), ease, false, true );
        }
    }
}

QDataStream& operator<<( QDataStream &out, const PHIAItem *it )
{
    out << dynamic_cast<const PHIBaseItem*>(it);
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIAItem *it )
{
    in >> dynamic_cast<PHIBaseItem*>(it);
    it->generateItem();
    return in;
}

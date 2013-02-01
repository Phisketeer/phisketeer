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
#include <QRegExpValidator>
#include <QStandardPaths>
#include "gwidgetitems.h"
#include "phi.h"
#include "phia.h"
#include "phiaitem.h"
#include "phiawebview.h"

GLabelItem::GLabelItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GLabelItem::GLabelItem()" );
    _l=new QLabel();
    _l->setTextInteractionFlags( Qt::TextSelectableByMouse );
    _l->setWordWrap( false );
    _l->setAutoFillBackground( false );
    setWidget( _l );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::Label );
    setPreferredSize( w, h );
    setMinimumSize( 0, 0 );
    //setMinimumSize( PHI::minimumSizeForPix() );
    //setPreferredHeight( PHI::defaultHeight() );
    //setMinimumHeight( PHI::minimumHeight() );
    //setMinimumWidth( PHI::minimumWidth() );

    resize( w, h );
    /*
    connect( _l, SIGNAL( linkActivated( const QString& ) ), it,
        SLOT( slotLinkActivated( const QString ) ) );
    connect( _l, SIGNAL( linkHovered( const QString& ) ), it,
        SLOT( slotLinkHovered( const QString& ) ) );
    */
    _alignment=0;
}

GLabelItem::~GLabelItem()
{
    qDebug( "GLabelItem::~GLabelItem()" );
}

void GLabelItem::setAlignment( quint8 align )
{
    _alignment=align;
    _l->setAlignment( PHI::toQtAlignment( align ) );
    //_l->setAlignment( Qt::AlignVCenter );
}

GRolloverButton::GRolloverButton( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLabelItem( w, h, it, parent )
{
    qDebug( "GRolloverButton::GRolloverButton()" );
    setWidget( 0 );
    delete _l; // delete QLabel
    _pixLabel=new PHIPixLabel();
    setWidget( _pixLabel );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::Label );
    setPreferredSize( w, h );
    setMinimumSize( 0, 0 );
    resize( w, h );
}

GRolloverButton::~GRolloverButton()
{
    qDebug( "GRolloverButton::~GRolloverButton()" );
}

void GRolloverButton::setAlignment( quint8 align )
{
    _alignment=align;
    _pixLabel->setAlignment( PHI::toQtAlignment( align ) );
    //_l->setAlignment( Qt::AlignVCenter );
}

void GRolloverButton::setPixmap( const QPixmap &pix, int number )
{
    _images.insert( number, pix );
    if ( number==0 ) _pixLabel->setPixmap( pix );
    update();
}

void GRolloverButton::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    GLabelItem::mouseReleaseEvent( e );
}

void GRolloverButton::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    GLabelItem::mousePressEvent( e );
}

void GRolloverButton::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverEnterEvent( e );
    if ( _images.count()>1 ) _pixLabel->setPixmap( _images.value( 1 ) );
    QPalette pal=palette();
    if ( _rolloverTextColor.isValid() ) {
        _color=pal.windowText().color();
        pal.setColor( QPalette::WindowText, _rolloverTextColor );
    }
    if ( _rolloverBackgroundColor.isValid() ) {
        _outlineColor=pal.window().color();
        pal.setColor( QPalette::Window, _rolloverBackgroundColor );
    }
    setPalette( pal );
    setCursor( QCursor( Qt::PointingHandCursor ) );
    update();
}

void GRolloverButton::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverMoveEvent( e );
    setCursor( QCursor( Qt::PointingHandCursor ) );
}

void GRolloverButton::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverLeaveEvent( e );
    if ( _images.count() ) _pixLabel->setPixmap( _images.value( 0 ) );
    QPalette pal=palette();
    if ( _rolloverTextColor.isValid() ) pal.setColor( QPalette::WindowText, _color );
    if ( _rolloverBackgroundColor.isValid() ) pal.setColor( QPalette::Window, _outlineColor );
    setPalette( pal );
    unsetCursor();
    update();
}

GLinkItem::GLinkItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLabelItem( w, h, it, parent )
{
    qDebug( "GLinkItem::GLinkItem()" );
    Q_ASSERT( _l );
    resize( w, h );
}

GLinkItem::~GLinkItem()
{
    qDebug( "GLinkItem::~GLinkItem()" );
}

void GLinkItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    GLabelItem::mouseReleaseEvent( e );
}

void GLinkItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    GLabelItem::mousePressEvent( e );
}

void GLinkItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverEnterEvent( e );
    QPalette pal=palette();
    if ( _rolloverTextColor.isValid() ) {
        _color=pal.windowText().color();
        pal.setColor( QPalette::WindowText, _rolloverTextColor );
    }
    if ( _rolloverBackgroundColor.isValid() ) {
        _outlineColor=pal.window().color();
        pal.setColor( QPalette::Window, _rolloverBackgroundColor );
    }
    setPalette( pal );
    update();
}

void GLinkItem::hoverMoveEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverMoveEvent( e );
}

void GLinkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *e )
{
    GLabelItem::hoverLeaveEvent( e );
    QPalette pal=palette();
    if ( _rolloverTextColor.isValid() ) pal.setColor( QPalette::WindowText, _color );
    if ( _rolloverBackgroundColor.isValid() ) pal.setColor( QPalette::Window, _outlineColor );
    setPalette( pal );
    update();
}

void GLinkItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->setRenderHint( QPainter::TextAntialiasing, true );
    GLabelItem::paint( painter, option, widget );
}

GHiddenItem::GHiddenItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLabelItem( w, h, it, parent )
{
    qDebug( "GHiddenItem::GHiddenItem()" );
    setFlag( ItemHasNoContents, true );
    setVisible( false );
}

GHiddenItem::~GHiddenItem()
{
    qDebug( "GHiddenItem::~GHiddenItem()" );
}

GButtonItem::GButtonItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GButtonItem::GButtonItem()" );
    _b=new QPushButton();
    setWidget( _b );
    setMinimumSize( 1, 1 );
    //setMinimumHeight( PHI::minimumHeight() );
    //setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::PushButton );
    resize( w, h );
}

GButtonItem::~GButtonItem()
{
    qDebug( "GButtonItem::~GButtonItem()" );
}

QColor GButtonItem::color() const
{
    return palette().buttonText().color();
}

QColor GButtonItem::outlineColor() const
{
    return palette().button().color();
}

void GButtonItem::setColor( const QColor &c )
{
    QPalette p=palette();
    if ( p.buttonText().color()==c ) return;
    p.setColor( QPalette::ButtonText, c );
    setPalette( p );
}

void GButtonItem::setOutlineColor( const QColor &c )
{
    QPalette p=palette();
    if ( p.button().color()==c ) return;
    p.setColor( QPalette::Button, c );
    setPalette( p );
}

GImageButtonItem::GImageButtonItem( const QPixmap &pix, qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent) : GButtonItem( w, h, it, parent )
{
    qDebug( "GImageButtonItem::GImageButtonItem()" );
    _b->setIconSize( QSize( static_cast<int>(w), static_cast<int>(h) ) );
    _b->setIcon( QIcon( pix ) );
    setMinimumSize( PHI::minimumWidth(), PHI::minimumHeightForImageButton() );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum, QSizePolicy::PushButton );
    resize( w, h );
}

GImageButtonItem::~GImageButtonItem()
{
    qDebug( "GImageButtonItem::~GImageButtonItem()" );
}

void GImageButtonItem::setPixmap( const QPixmap &pix, int )
{
    _pix=pix.scaled( QSize( height()/1.5, height()/1.5 ) );
    _b->setIconSize( _pix.size() );
    _b->setIcon( QIcon( _pix ) );
    _pix=pix;
}

void GImageButtonItem::setHeight( qreal h )
{
    GButtonItem::setHeight( h );
    QPixmap p=_pix.scaled( QSize( h/1.5, h/1.5 ) );
    _b->setIconSize( p.size() );
    _b->setIcon( QIcon( p ) );
}

GSubmitButtonItem::GSubmitButtonItem( qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent ) : GButtonItem( w, h, it, parent )
{
    qDebug( "GSubmitButtonItem::GSubmitButtonItem()" );
}

GSubmitButtonItem::~GSubmitButtonItem()
{
    qDebug( "GSubmitButtonItem::~GSubmitButtonItem()" );
}

GResetButtonItem::GResetButtonItem( qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent ) : GButtonItem( w, h, it, parent )
{
    qDebug( "GResetButtonItem::GResetButtonItem()" );
}

GResetButtonItem::~GResetButtonItem()
{
    qDebug( "GResetButtonItem::~GResetButtonItem()" );
}

GDecimalNumberItem::GDecimalNumberItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GDecimalNumberItem::GDecimalNumberItem()" );
    _spin=new QSpinBox();
    setWidget( _spin );
    _spin->setMinimum( 0 );
    _spin->setMaximum( 100 );
    _spin->setSingleStep( 1 );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit );
    setMinimumHeight( PHI::minimumHeight() );
    resize( w, h );
    setCacheMode( QGraphicsItem::NoCache );
    connect( _spin, SIGNAL( valueChanged( int ) ), this, SLOT( slotNumberChanged( int ) ) );
}

GDecimalNumberItem::~GDecimalNumberItem()
{
    qDebug( "GDecimalNumberItem::~GDecimalNumberItem()" );
}

void GDecimalNumberItem::slotNumberChanged( int )
{
    update();
    emit onChange();
}

void GDecimalNumberItem::setValue( const QString &v )
{
    QStringList list=v.split( ':', QString::KeepEmptyParts );
    if ( list.count()>1 ) _spin->setMinimum( list.at(1).toInt() );
    if ( list.count()>2 ) _spin->setMaximum( list.at(2).toInt() );
    if ( list.count()>3 ) _spin->setSingleStep( list.at(3).toInt() );
    if ( list.count()>0 ) _spin->setValue( list.at(0).toInt() );
    else _spin->setValue( 0 );
    _value=v;
}

GRealNumberItem::GRealNumberItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GRealNumberItem::GRealNumberItem()" );
    _spin=new QDoubleSpinBox();
    setWidget( _spin );
    _spin->setMinimum( 0 );
    _spin->setMaximum( 10 );
    _spin->setSingleStep( .5 );
    _spin->setDecimals( 1 );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit );
    setMinimumHeight( PHI::minimumHeight() );
    resize( w, h );
    setCacheMode( QGraphicsItem::NoCache );
    connect( _spin, SIGNAL( valueChanged( double ) ), this, SLOT( slotNumberChanged( double ) ) );
}

GRealNumberItem::~GRealNumberItem()
{
    qDebug( "GRealNumberItem::~GRealNumberItem()" );
}

void GRealNumberItem::slotNumberChanged( double )
{
    update();
    emit onChange();
}

void GRealNumberItem::setValue( const QString &v )
{
    QStringList list=v.split( ':', QString::KeepEmptyParts );
    if ( list.count()>1 ) _spin->setMinimum( list.at(1).toDouble() );
    if ( list.count()>2 ) _spin->setMaximum( list.at(2).toDouble() );
    if ( list.count()>3 ) _spin->setSingleStep( list.at(3).toDouble() );
    if ( list.count()>4 ) _spin->setDecimals( list.at(4).toInt() );
    if ( list.count()>0 ) _spin->setValue( list.at(0).toDouble() );
    else _spin->setValue( 0 );
    _value=v;
}

GLineEditItem::GLineEditItem( qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent ) : GWidgetItem( w, h, it, parent )
{
    qDebug( "GLineEditItem::GLineEditItem()" );
    _e=new QLineEdit();
    setWidget( _e );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit );
    setMinimumHeight( PHI::minimumHeight() );
    resize( w, h );
    setCacheMode( QGraphicsItem::NoCache );
    connect( _e, SIGNAL( textChanged( const QString& ) ), this,
        SLOT( slotTextChanged( const QString& ) ) );
}

GLineEditItem::~GLineEditItem()
{
    qDebug( "GLineEditItem::~GLineEditItem()" );
}

void GLineEditItem::slotTextChanged( const QString & )
{
    update();
    emit onChange();
}

GEmailItem::GEmailItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLineEditItem( w, h, it, parent )
{
    qDebug( "GEmailItem::~GEmailItem()" );
    _e->setValidator( new QRegExpValidator(
        QRegExp( QString::fromLatin1( PHI::emailRegExp() ), Qt::CaseInsensitive ), this ) );
}

GEmailItem::~GEmailItem()
{
    qDebug( "GEmailItem::~GEmailItem()" );
}

GPhoneNumberItem::GPhoneNumberItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLineEditItem( w, h, it, parent )
{
    qDebug( "GPhoneNumberItem::~GPhoneNumberItem()" );
    _e->setValidator( new QRegExpValidator(
        QRegExp( QString::fromLatin1( PHI::phoneNumberRegExp() ), Qt::CaseSensitive ), this ) );
}

GPhoneNumberItem::~GPhoneNumberItem()
{
    qDebug( "GPhoneNumberItem::~GPhoneNumberItem()" );
}

GSearchItem::GSearchItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLineEditItem( w, h, it, parent )
{
    qDebug( "GSearchItem::~GSearchItem()" );
}

GSearchItem::~GSearchItem()
{
    qDebug( "GSearchItem::~GSearchItem()" );
}

GPasswordItem::GPasswordItem( qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent ) : GLineEditItem( w, h, it, parent )
{
    qDebug( "GPasswordItem::GPasswordItem()" );
    Q_ASSERT(_e);
    _e->setEchoMode( QLineEdit::Password );
}

GPasswordItem::~GPasswordItem()
{
    qDebug( "GPasswordItem::~GPasswordItem()" );
}

GRadioButtonItem::GRadioButtonItem( const QString &l, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GRadioButtonItem::GRadioButtonItem()" );
    _r=new QRadioButton( l );
    _r->setAutoExclusive( false );
    setWidget( _r );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum, QSizePolicy::RadioButton );
    resize( w, h );
    connect( this, SIGNAL( updateCheckState( bool ) ), it, SLOT( slotCheckStateToggled( bool ) ) );
    connect( _r, SIGNAL( clicked( bool ) ), this, SLOT( slotCheckStateToggled(bool) ) );
}

GRadioButtonItem::~GRadioButtonItem()
{
    qDebug( "GRadioButtonItem::~GRadioButtonItem()" );
}

void GRadioButtonItem::slotCheckStateToggled( bool b )
{
    if ( b ) emit updateCheckState( true );
    else _r->setChecked( true );
}

GCheckBoxItem::GCheckBoxItem( const QString &l, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GCheckBoxItem::GCheckBoxItem()" );
    _cb=new QCheckBox( l );
    setWidget( _cb );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum, QSizePolicy::CheckBox );
    resize( w, h );
    connect( _cb, SIGNAL( toggled( bool ) ), it, SLOT( slotCheckStateToggled( bool ) ) );
}

GCheckBoxItem::~GCheckBoxItem()
{
    qDebug( "GCheckBoxItem::~GCheckBoxItem()" );
}

GTextBrowserItem::GTextBrowserItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GTextBrowserItem::GTextBrowserItem()" );
    _b=new QTextBrowser();
    _b->setTextInteractionFlags( (PHITEXTINTERACTIONFLAGS) & ~Qt::TextSelectableByKeyboard );
    _b->setOpenLinks( false );
    _b->setFrameStyle( QFrame::NoFrame );
    //_b->setBackgroundRole( QPalette::Window );
    //_b->setForegroundRole( QPalette::Window );
    setWidget( _b );
    //Palette will be overwritten on loading by PHIAWebView:
    //QPalette pal=palette();
    //pal.setColor( QPalette::Base, QColor( 0, 0, 0, 0 ) );
    //setPalette( pal );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding, QSizePolicy::Frame );
    setMinimumSize( PHI::minimumWidth(), PHI::minimumHeight() );
    resize( w, h );
    connect( _b, SIGNAL( anchorClicked( const QUrl& ) ),
        this, SLOT( slotAnchorClicked( const QUrl& ) ) );
    connect( this, SIGNAL( linkActivated( const QString& ) ),
        it, SLOT( slotLinkActivated( const QString& ) ) );
    connect( _b, SIGNAL( highlighted( const QString& ) ),
        it, SLOT( slotLinkHovered( const QString& ) ) );
}

GTextBrowserItem::~GTextBrowserItem()
{
    qDebug( "GTextBrowserItem::~GTextBrowserItem()" );
}

void GTextBrowserItem::setColor( const QColor &c )
{
    QPalette pal=palette();
    pal.setColor( QPalette::Text, c );
    setPalette( pal );
}

QColor GTextBrowserItem::color() const
{
    return palette().text().color();
}

void GTextBrowserItem::setOutlineColor( const QColor &c )
{
    QPalette pal=palette();
    pal.setColor( QPalette::Base, c );
    setPalette( pal );
}

QColor GTextBrowserItem::outlineColor() const
{
    return palette().base().color();
}

void GTextBrowserItem::setAlignment( quint8 align )
{
    _alignment=align;
    QTextDocument *doc=_b->document();
    QTextOption opt=doc->defaultTextOption();
    opt.setAlignment( PHI::toQtAlignment( align ) );
    doc->setDefaultTextOption( opt );
}

void GTextBrowserItem::slotAnchorClicked( const QUrl &url )
{
    emit linkActivated( url.toString() );
}

void GTextBrowserItem::setValue( const QString &v )
{
    _b->setHtml( v );
    _value=v;
}

void GTextBrowserItem::setStyleSheet( const QString &s )
{
    qDebug( "%s", qPrintable(s));
    _b->document()->setDefaultStyleSheet( s );
    _b->setHtml( _value );
}

QString GTextBrowserItem::styleSheet() const
{
    return _b->document()->defaultStyleSheet();
}

void GTextBrowserItem::hoverEnterEvent( QGraphicsSceneHoverEvent *e )
{
    GWidgetItem::hoverEnterEvent( e );
    qDebug( "TextBrowser hover enter %s", _item->cursor().data() );
    if ( _item->cursor()=="auto" ) {
        qDebug( "unset cursor" );
        if ( hasCursor() ) unsetCursor();
    } else setCursor( PHI::toCursorShape( _item->cursor() ) );
}

GMultiLineEditItem::GMultiLineEditItem( qreal w, qreal h,
    const PHIAItem *it, QGraphicsItem *parent ) : GWidgetItem( w, h, it, parent )
{
    qDebug( "GMultiLineEditItem::GMultiLineEditItem()" );
    _e=new QPlainTextEdit();
    connect( _e, SIGNAL( textChanged() ), this, SLOT( slotTextChanged() ) );
    setWidget( _e );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setMinimumSize( PHI::minimumSizeForBox() );
    resize( w, h );
}

GMultiLineEditItem::~GMultiLineEditItem()
{
    qDebug( "GMultiLineEditItem::~GMultiLineEditItem()" );
}

void GMultiLineEditItem::slotTextChanged()
{
    emit onChange();
}

GComboBoxItem::GComboBoxItem( const QString &v, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GComboBoxItem::GComboBoxItem()" );
#ifdef Q_OS_WIN
    _cb=new QComboBox();
#else
    _cb=new PHIAComboBox( it );
#endif
    connect( _cb, SIGNAL( currentIndexChanged( int ) ), this,
        SLOT( slotCurrentIndexChanged( int ) ) );
    setWidget( _cb );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::ComboBox );
    setMinimumHeight( PHI::minimumHeight() );
    resize( w, h );
    _value=v;
}

GComboBoxItem::~GComboBoxItem()
{
    qDebug( "GComboBoxItem::~GComboBoxItem()" );
}

void GComboBoxItem::slotCurrentIndexChanged( int )
{
    emit onChange();
}

void GComboBoxItem::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
    //qreal z=zValue();
    //bring the item to the top
    //setZValue( static_cast<qreal>(PHI::maxZValue()+1) );
    //qDebug( "setting z %f %f", z, zValue() );
    //qApp->processEvents();
    GWidgetItem::mousePressEvent( e );
    //setZValue( z );
}

void GComboBoxItem::setValue( const QString &v )
{
    _cb->blockSignals( true );
    _cb->clear();
    QStringList list=v.split( _delimiter, QString::SkipEmptyParts );
    QStringList items;
    QString option, sendValue;
    bool isSelected;
    int selected=-1;
    for( int z=0; z<list.count(); z++ ) {
        option=list.at( z );
        PHI::getItemCheckData( option, sendValue, isSelected );
        if ( isSelected && selected==-1 ) selected=z;
         _cb->insertItem( z, option, sendValue );
         items << option;
    }
    if ( selected > -1 ) _cb->setCurrentIndex( selected );
    _cb->blockSignals( false );
    _value=v;
}

QString GComboBoxItem::value() const
{
    return _cb->itemData( _cb->currentIndex() ).toString();
}

QStringList GComboBoxItem::selectedItems() const
{
    QString item=_cb->itemData( _cb->currentIndex() ).toString();
    QStringList list;
    list << item;
    return list;
}

bool GComboBoxItem::valueChecked( const QString &v ) const
{
    QString item=_cb->itemData( _cb->currentIndex() ).toString();
    return item==v;
}

void GComboBoxItem::setValueChecked( const QString &v, bool )
{
    for ( int i=0; i<_cb->count(); i++ ) {
        if ( _cb->itemData( i ).toString()==v ) return _cb->setCurrentIndex( i );
    }
}

GLangSelectorItem::GLangSelectorItem( const QString &v, qreal w, qreal h, const PHIAItem *it,
    QGraphicsItem *parent ) : GComboBoxItem( v, w, h, it, parent )
{
    qDebug( "GLangSelectortem::GLangSelectorItem()" );
    _delimiter=":";
    QString curLang=it->view()->page()->lang();
    QString curLangLocale=curLang;
    // de-de -> de_DE
    if ( curLangLocale.size()==5 ) curLangLocale=curLang.left( 2 )+'_'+curLang.right( 2 ).toUpper();
    QLocale curLocale( curLangLocale );
    QStringList languages=it->view()->page()->languages();
    QString lang, value;
    foreach( lang, languages ) {
        if ( lang==curLang ) {
            value+=tr( curLocale.languageToString( curLocale.language() ).toUtf8().constData() );
            if ( lang.size()==5 ) value+=" ("
                +tr( curLocale.countryToString( curLocale.country() ).toUtf8().constData() )+')';
            value+='['+lang+"][1]:";
        } else {
            curLangLocale=lang;
            if ( curLangLocale.size()==5 ) curLangLocale=lang.left( 2 )+'_'+lang.right( 2 ).toUpper();
            QLocale loc( curLangLocale );
            value+=tr( curLocale.languageToString( loc.language() ).toUtf8().constData() );
            if ( lang.size()==5 ) value+=" ("
                +tr( curLocale.countryToString( loc.country() ).toUtf8().constData() )+')';
            value+='['+lang+"]:";
        }
    }
    value.chop( 1 ); // remove trailing ':'
    GComboBoxItem::setValue( value );
    connect( this, SIGNAL( langActivated( const QString& ) ), it,
        SLOT( slotLinkActivated( const QString& ) ) );
}

GLangSelectorItem::~GLangSelectorItem()
{
    qDebug( "GLangSelectortem::GLangSelectorItem()" );
}

QString GLangSelectorItem::value() const
{
    return GComboBoxItem::value();
}

QStringList GLangSelectorItem::selectedItems() const
{
    return GComboBoxItem::selectedItems();
}

void GLangSelectorItem::slotCurrentIndexChanged( int index )
{
    if ( _item->id()!="philangselector" ) {
        emit onChange();
        return;
    }
    QUrl url=_item->view()->url();
    QUrlQuery query( url );
    query.removeQueryItem( "philang" );
    query.addQueryItem( "philang", _cb->itemData( index ).toString() );
    url.setQuery( query );
    emit langActivated( url.toString() );
}

GListBoxItem::GListBoxItem( const QString &v, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GListBoxItem::GListBoxItem()" );
    _lw=new QListWidget();
    connect( _lw, SIGNAL( itemSelectionChanged() ), this, SIGNAL( onChange() ) );
    _lw->setSelectionMode( QAbstractItemView::ExtendedSelection );
    _lw->setResizeMode( QListWidget::Adjust );
    _lw->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
#ifdef Q_OS_MAC
    _lw->setForegroundRole( QPalette::Button );
    _lw->setFrameStyle( QFrame::Box );
#endif
    setWidget( _lw );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setMinimumSize( PHI::minimumSizeForBox() );
    resize( w, h );
    connect( _lw, SIGNAL( currentRowChanged( int ) ), this, SLOT( slotCurrentRowChanged( int ) ) );
    _value=v;
}

GListBoxItem::~GListBoxItem()
{
    qDebug( "GListBoxItem::~GListBoxItem()" );
}

void GListBoxItem::slotCurrentRowChanged( int )
{
    emit onChange();
}

void GListBoxItem::setValue( const QString &v )
{
    _lw->blockSignals( true );
    _lw->clear();
    QStringList list=v.split( _delimiter, QString::SkipEmptyParts );
    QString option, sendValue;
    bool isChecked;
    for( int z=0; z<list.count(); z++ ) {
        option=list.at( z );
        PHI::getItemCheckData( option, sendValue, isChecked );
        qDebug( "OPTION %s %s %d", qPrintable( option ), qPrintable( sendValue ), isChecked );
        QListWidgetItem *item=new QListWidgetItem( option );
        //item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
        item->setData( Qt::UserRole, sendValue );
        _lw->insertItem( z, item );
        item->setSelected( isChecked );
    }
    _lw->blockSignals( false );
    _value=v;
}

QString GListBoxItem::value() const
{
    QListWidgetItem *it=_lw->currentItem();
    if ( !it ) return QString();
    //qDebug( "DATA %s", qPrintable( it->data( Qt::UserRole ).toString() ) );
    return it->data( Qt::UserRole ).toString();
}

QStringList GListBoxItem::selectedItems() const
{
    QList <QListWidgetItem*> selectedItems=_lw->selectedItems();
    QListWidgetItem *item;
    QStringList list;
    foreach ( item, selectedItems ) {
        list.append( item->data( Qt::UserRole ).toString() );
    }
    return list;
}

bool GListBoxItem::valueChecked( const QString &v ) const
{
    QListWidgetItem *it;
    for ( int i=0; i<_lw->count(); i++ ) {
        it=_lw->item( i );
        if ( it->data( Qt::UserRole ).toString()==v ) {
            return it->isSelected();
        }
    }
    return false;
}

void GListBoxItem::setValueChecked( const QString &v, bool b )
{
    QListWidgetItem *it;
    for ( int i=0; i<_lw->count(); i++ ) {
        it=_lw->item( i );
        if ( it->data( Qt::UserRole ).toString()==v ) {
            return it->setSelected( b );
        }
    }
}

GCheckListItem::GCheckListItem( const QString &v, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    qDebug( "GCheckListItem::GCheckListItem()" );
    _tree=new QTreeWidget();
    _tree->setFrameStyle( QFrame::NoFrame );
    _tree->setIndentation( 0 );
    _tree->setRootIsDecorated( false );
    QPalette pal=_tree->palette();
    pal.setColor( QPalette::Base, Qt::transparent );
    _tree->setPalette( pal );
    connect( _tree, SIGNAL( itemSelectionChanged() ), this, SIGNAL( onChange() ) );

    setWidget( _tree );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    setMinimumSize( PHI::minimumSizeForBox() );
    resize( w, h );
    _value=v;
}

GCheckListItem::~GCheckListItem()
{
    qDebug( "GCheckListItem::~GCheckListItem()" );
}

void GCheckListItem::setReadOnly( bool ro )
{
    _readOnly=ro;
    QTreeWidgetItem *it;
    for ( int i=0; i<_tree->topLevelItemCount(); i++ ) {
        it=_tree->topLevelItem( i );
        if ( ro ) it->setFlags( Qt::ItemIsEnabled );
        else it->setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    }
}

void GCheckListItem::setValue( const QString &v )
{
    _tree->setSortingEnabled( false );
    _tree->clear();
    QStringList rows=v.split( _delimiter, QString::SkipEmptyParts );
    QString row;
    int colscount=0;
    for ( int r=0; r<rows.count(); r++ ) {
        row=rows.at( r );
        QStringList cols=row.split( "|", QString::KeepEmptyParts );
        if ( r==0 ) {
            _tree->setColumnCount( cols.count() );
            _tree->setHeaderLabels( cols );
            colscount=cols.count();
            continue;
        }
        QTreeWidgetItem *item=new QTreeWidgetItem();
        item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        item->setCheckState( 0, Qt::Unchecked );
        for( int c=0; c<cols.count(); c++ ) {
            QString option, sendValue;
            bool isChecked;
            option=cols.at( c );
            if ( c==0 ) {
                PHI::getItemCheckData( option, sendValue, isChecked );
                item->setData( 0, Qt::UserRole, sendValue );
                item->setText( 0, option );
                if ( isChecked ) item->setCheckState( 0, Qt::Checked );
            } else {
                //item->setData( 0, Qt::UserRole, option );
                item->setText( c, option );
            }
            _tree->addTopLevelItem( item );
            //_tree->resizeColumnToContents( c );
        }
    }
    _tree->setSortingEnabled( true );
    _value=v;
    //qApp->processEvents();
    for ( int i=0; i<colscount-1; i++ ) {
        _tree->resizeColumnToContents( i );
        //int w=_tree->columnWidth( i ); // fixed in Qt 4.7.3
        //if ( i==0 ) w+=20;
        //_tree->setColumnWidth( i, w+10 );
    }
}

QString GCheckListItem::value() const
{
    QTreeWidgetItem *it=_tree->currentItem();
    if ( !it ) return QString();
    return it->data( 0, Qt::UserRole ).toString();
}

bool GCheckListItem::valueChecked( const QString &v ) const
{
    QTreeWidgetItem *it;
    for ( int i=0; i < _tree->topLevelItemCount(); i++ ) {
        it=_tree->topLevelItem( i );
        if ( it->data( 0, Qt::UserRole ).toString()==v ) return (it->checkState( 0 )==Qt::Checked);
    }
    return false;
}

void GCheckListItem::setValueChecked( const QString &v, bool c )
{
    QTreeWidgetItem *it;
    for ( int i=0; i < _tree->topLevelItemCount(); i++ ) {
        it=_tree->topLevelItem( i );
        if ( it->data( 0, Qt::UserRole ).toString()==v ) {
            it->setCheckState( 0, c ? Qt::Checked : Qt::Unchecked );
            return;
        }
    }
}

QStringList GCheckListItem::selectedItems() const
{
    QStringList checkedItems;
    QTreeWidgetItem *it;
    for ( int i=0; i < _tree->topLevelItemCount(); i++ ) {
        it=_tree->topLevelItem( i );
        if ( it->checkState( 0 )==Qt::Checked ) {
            checkedItems.append( it->data( 0, Qt::UserRole ).toString() );
        }
    }
    return checkedItems;
}

GTableItem::GTableItem( const QString &v, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GCheckListItem( v, w, h, it, parent )
{
    qDebug( "GTableItem::GTableItem()" );
    Q_ASSERT( _tree );
}

GTableItem::~GTableItem()
{
    qDebug( "GTableItem::~GTableItem()" );
}

void GTableItem::setReadOnly( bool ro )
{
    _readOnly=ro;
    QTreeWidgetItem *it;
    for ( int i=0; i<_tree->topLevelItemCount(); i++ ) {
        it=_tree->topLevelItem( i );
        if ( ro ) it->setFlags( Qt::ItemIsEnabled );
        else it->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
    }
}

QString GTableItem::value() const
{
    QTreeWidgetItem *it=_tree->currentItem();
    if ( !it ) return QString();
    return it->data( 0, Qt::UserRole ).toString();
}

QStringList GTableItem::selectedItems() const
{
    QStringList list;
    list << value();
    return list;
}

void GTableItem::setValue( const QString &v )
{
    _tree->setSortingEnabled( false );
    _tree->clear();
    QStringList rows=v.split( _delimiter, QString::SkipEmptyParts );
    QString row;
    int colscount=0;
    QTreeWidgetItem *currentItem( 0 );
    for ( int r=0; r<rows.count(); r++ ) {
        row=rows.at( r );
        QStringList cols=row.split( "|", QString::KeepEmptyParts );
        if ( r==0 ) {
            _tree->setColumnCount( cols.count() );
            _tree->setHeaderLabels( cols );
            colscount=cols.count();
            continue;
        }
        QTreeWidgetItem *item=new QTreeWidgetItem();
        if ( readOnly() ) item->setFlags( Qt::ItemIsEnabled );
        else item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
        for( int c=0; c<cols.count(); c++ ) {
            QString option, sendValue;
            option=cols.at( c );
            if ( c==0 ) {
                bool isSelected;
                PHI::getItemCheckData( option, sendValue, isSelected );
                item->setData( 0, Qt::UserRole, sendValue );
                if ( isSelected ) currentItem=item;
            }
            item->setText( c, option );
            _tree->addTopLevelItem( item );
            //_tree->resizeColumnToContents( c );
        }
    }
    _value=v;
    if ( currentItem ) {
        _tree->setCurrentItem( currentItem );
    }
    //qApp->processEvents();
    for ( int i=0; i<colscount-1; i++ ) {
        _tree->resizeColumnToContents( i );
        //int w=_tree->columnWidth( i ); // fixed in Qt 4.7.3
        //_tree->setColumnWidth( i, w+10 );
    }
    _tree->setSortingEnabled( true );
}

GFileItem::GFileItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GLineEditItem( w, h, it, parent )
{
    qDebug( "GFileItem::GFileItem()" );
    Q_ASSERT( _e );
    _e->setReadOnly( true );
    _e->setMinimumHeight( PHI::minimumHeight() );
    _e->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _b=new QToolButton();
    _b->setArrowType( Qt::RightArrow );
    _b->setMinimumHeight( PHI::minimumHeightForFileUpload() );
    _b->setFixedWidth( static_cast<int>(PHI::minimumWidth()) );
    _b->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
    QWidget *wid=new QWidget();
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( _e );
    l->addWidget( _b );
    wid->setLayout( l );
    setWidget( wid );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, QSizePolicy::LineEdit );
    setMinimumWidth( PHI::minimumWidthForFileUpload() );
    setMinimumHeight( PHI::minimumHeightForFileUpload() );
    resize( w, h );
    connect( _b, SIGNAL( clicked() ), this, SLOT( slotButtonClicked() ) );
}

GFileItem::~GFileItem()
{
    qDebug( "GFileItem::~GFileItem()" );
}

void GFileItem::slotButtonClicked()
{
    QString oldText=_e->text();
    QString dir=QStandardPaths::writableLocation( QStandardPaths::HomeLocation );
    QString f=QFileDialog::getOpenFileName( view(), tr( "File to send" ), dir,
        tr( "Images (*.png *.jpg *.gif *.tif);;Text (*.txt *.doc *.html *.xml *.svg);; All (*.*)" ) );
    if ( oldText==f ) return;
    _e->setText( f );
    emit onChange();
}

void GFileItem::overwritePalette( const QPalette &p )
{
    QPalette pal=p;
    pal.setColor( QPalette::Window, QColor( Qt::transparent ) );
    _e->setPalette( pal );
    setPalette( pal );
}

/*
void GFileItem::setOutlineColor( const QColor &c )
{
    GWidgetItem::setOutlineColor( c );
    QPalette pal=palette();
    pal.setColor( QPalette::Base, c );
    pal.setColor( QPalette::Window, QColor( Qt::transparent ) );
    _e->setPalette( pal );
    setPalette( pal );
}
*/

GDateEditItem::GDateEditItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent ), _init( true )
{
    qDebug( "GDateEditItem::GDateEditItem()" );
    _dateEdit=new QDateEdit();
    _dateEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    _dateEdit->setMinimumHeight( PHI::minimumHeight() );
    _dateEdit->setMinimumWidth( PHI::minimumWidth() );
    _dateEdit->setDisplayFormat( QLocale( PHI::toLocale( it->view()->page()->lang() ) )
        .dateFormat( QLocale::ShortFormat ) );
    //_dateEdit->setMaximumWidth( 90 );
    _toolButton=new QToolButton();
    _toolButton->setArrowType( Qt::DownArrow );
    //_toolButton->setMaximumHeight( _dateEdit->sizeHint().height() );
    _toolButton->setMinimumHeight( PHI::minimumHeight() );
    _toolButton->setFixedWidth( static_cast<int>(PHI::minimumWidth()) );
    _toolButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
    QWidget *wid=new QWidget();
    QHBoxLayout *l=new QHBoxLayout();
    l->setContentsMargins( 0, 0, 0, 0 );
    l->addWidget( _dateEdit );
    l->addWidget( _toolButton );
    wid->setLayout( l );
    setWidget( wid );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit );
    setMinimumWidth( PHI::minimumWidthForDateEdit() );
    setMinimumHeight( PHI::minimumHeight() );
    adjustSize();
    resize( w, h );
    connect( _dateEdit, SIGNAL( dateChanged( const QDate& ) ),
        this, SLOT( slotDateChanged( const QDate& ) ) );
    connect( _toolButton, SIGNAL( clicked() ), this, SLOT( slotToolButtonClicked() ) );
}

GDateEditItem::~GDateEditItem()
{
    qDebug( "GDateEditItem::~GDateEditItem()" );
}

void GDateEditItem::setValue( const QString &v )
{
    QStringList dates=v.split( ':', QString::SkipEmptyParts );
    QDate now, start, end;
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], PHI::isoDateFormat() );
    if ( !now.isValid() ) now=QDate::currentDate();
    _dateEdit->setDate( now );
    if ( _init ) {
        _init=false;
        if ( dates.count()>1 ) start=QDate::fromString( dates[1], PHI::isoDateFormat() );
        if ( !start.isValid() ) start=now;
        if ( dates.count()>2 ) end=QDate::fromString( dates[2], PHI::isoDateFormat() );
        if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
        _dateEdit->setDateRange( start, end );
    } else emit onChange();
}

void GDateEditItem::overwritePalette( const QPalette &p )
{
    QPalette pal=p;
    pal.setColor( QPalette::Window, QColor( Qt::transparent ) );
    _dateEdit->setPalette( pal );
    setPalette( pal );
}

/*
void GDateEditItem::setOutlineColor( const QColor &c )
{
    GWidgetItem::setOutlineColor( c );
    QPalette pal=palette();
    pal.setColor( QPalette::Base, c );
    pal.setColor( QPalette::Window, QColor( Qt::transparent ) );
    _dateEdit->setPalette( pal );
    setPalette( pal );
}
*/

QString GDateEditItem::value() const
{
    return _dateEdit->date().toString( PHI::isoDateFormat() );
}

void GDateEditItem::slotDateChanged( const QDate& )
{
    emit onChange();
}

void GDateEditItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
    _pos=e->screenPos();
    _pos.setX( _pos.x()-static_cast<int>(width()) );
    GWidgetItem::mouseReleaseEvent( e );
}

void GDateEditItem::slotToolButtonClicked()
{
    QDialog dlg( view(), Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint );
    dlg.move( _pos );
    dlg.setSizeGripEnabled( true );
    dlg.setWindowTitle( " " );
    QVBoxLayout *vbox=new QVBoxLayout();
    vbox->setContentsMargins( 0, 0, 0, 0 );
    QCalendarWidget *cal=new QCalendarWidget();
    cal->setDateRange( _dateEdit->minimumDate(), _dateEdit->maximumDate() );
    cal->setSelectedDate( _dateEdit->date() );
    //cal->setFirstDayOfWeek( Qt::Monday );
    cal->setLocale( QLocale( PHI::toLocale( _item->view()->page()->lang() ) ) );
    connect( cal, SIGNAL( selectionChanged() ), &dlg, SLOT( reject() ) );
    vbox->addWidget( cal );
    dlg.setLayout( vbox );
    dlg.exec();
    if ( cal->selectedDate()==_dateEdit->date() ) return;
    _dateEdit->setDate( cal->selectedDate() );
    emit onChange();
}

GCalendarItem::GCalendarItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent ), _init( true )
{
    qDebug( "GCalendarItem::GCalendarItem()" );
    _calendar=new QCalendarWidget();
    //_calendar->setFirstDayOfWeek( Qt::Monday );
    _calendar->setLocale( QLocale( PHI::toLocale( it->view()->page()->lang() ) ) );
    setWidget( _calendar );
    setMinimumSize( PHI::minimumSizeForCalendar() );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    resize( w, h );
}

GCalendarItem::~GCalendarItem()
{
    qDebug( "GCalendarItem::~GCalendarItem()" );
}

void GCalendarItem::setValue( const QString &v )
{
    QStringList dates=v.split( ':', QString::SkipEmptyParts );
    QDate now, start, end;
    if ( dates.count()>0 ) now=QDate::fromString( dates[0], PHI::isoDateFormat() );
    if ( !now.isValid() ) now=QDate::currentDate();
    _calendar->setSelectedDate( now );
    if ( _init ) {
        _init=false;
        if ( dates.count()>1 ) start=QDate::fromString( dates[1], PHI::isoDateFormat() );
        if ( !start.isValid() ) start=now;
        if ( dates.count()>2 ) end=QDate::fromString( dates[2], PHI::isoDateFormat() );
        if ( !end.isValid() ) end=QDate( 9999, 12, 31 );
        _calendar->setDateRange( start, end );
    } else emit onChange();
}

QString GCalendarItem::value() const
{
    return _calendar->selectedDate().toString( PHI::isoDateFormat() );
}

GProgressBarItem::GProgressBarItem( qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent )
    : GWidgetItem( w, h, it, parent )
{
    _bar=new QProgressBar();
    _bar->setRange( 0, 100 );
    _bar->setTextVisible( false );
    setWidget( _bar );
    setMinimumWidth( PHI::minimumWidth() );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    resize( w, h );
}

GProgressBarItem::~GProgressBarItem()
{
}

void GProgressBarItem::setValue( const QString &v )
{
    _bar->setValue( qBound( 0, v.toInt(), 100 ) );
}

QString GProgressBarItem::value() const
{
    return QString::number( _bar->value() );
}

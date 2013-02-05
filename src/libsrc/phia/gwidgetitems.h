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
#ifndef GWIDGETITEMS_H
#define GWIDGETITEMS_H

#include "gwidgetitem.h"
#include "phiacomboboxhack.h"
#include "phipixlabel.h"

class GLabelItem : public GWidgetItem
{
public:
    GLabelItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GLabelItem();
    inline virtual void setValue( const QString &v ) { _l->setText( v ); _value=v; }
    inline virtual quint8 alignment() const { return _alignment; }
    virtual void setAlignment( quint8 align );

protected:
    QLabel *_l;
    quint8 _alignment;
};

class GRolloverButton : public GLabelItem
{
public:
    GRolloverButton( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GRolloverButton();
    virtual void setPixmap( const QPixmap &pix, int number=-1 );
    virtual void setAlignment( quint8 align );
    inline virtual void setRolloverTextColor( const QColor &c ) { _rolloverTextColor=c; }
    inline virtual void setRolloverBackgroundColor( const QColor &c ) { _rolloverBackgroundColor=c; }
    inline virtual QColor rolloverTextColor() const { return _rolloverTextColor; }
    inline virtual QColor rolloverBackgroundColor() const { return _rolloverBackgroundColor; }
    inline virtual void setUrl( const QString &u ) { _url=u; }
    inline virtual QString url() const { return _url; }
    inline virtual void setValue( const QString &v ) { _pixLabel->setText( v ); _value=v; }

protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );

protected:
    QColor _rolloverTextColor, _rolloverBackgroundColor, _color, _outlineColor;
    QString _url;
    QHash <int, QPixmap> _images;
    PHIPixLabel *_pixLabel;
};

class GLinkItem : public GLabelItem
{
public:
    GLinkItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GLinkItem();
    inline virtual QString value() const { return _value; }
    inline virtual void setUrl( const QString &u ) { _url=u; }
    inline virtual QString url() const { return _url; }
    inline virtual void setRolloverTextColor( const QColor &c ) { _rolloverTextColor=c; }
    inline virtual void setRolloverBackgroundColor( const QColor &c ) { _rolloverBackgroundColor=c; }
    inline virtual QColor rolloverTextColor() const { return _rolloverTextColor; }
    inline virtual QColor rolloverBackgroundColor() const { return _rolloverBackgroundColor; }

protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *e );
    virtual void hoverMoveEvent( QGraphicsSceneHoverEvent *e );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

protected:
    QColor _rolloverTextColor, _rolloverBackgroundColor, _color, _outlineColor;
    QString _url;
};

class GHiddenItem : public GLabelItem
{
public:
    GHiddenItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GHiddenItem();
};

class GButtonItem : public GWidgetItem
{
public:
    GButtonItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GButtonItem();
    inline virtual void setValue( const QString &v ) { _b->setText( v ); }
    inline virtual QString value() const { return _b->text(); }
    inline virtual void setUrl( const QString &u ) { _url=u; }
    inline virtual QString url() const { return _url; }
    virtual void setColor( const QColor &c );
    virtual void setOutlineColor( const QColor &c );
    virtual QColor color() const;
    virtual QColor outlineColor() const;

protected:
    QPushButton *_b;
    QString _url;
};

class GImageButtonItem : public GButtonItem
{
public:
    GImageButtonItem( const QPixmap &pix, qreal w, qreal h, const PHIAItem *it, QGraphicsItem *parent=0 );
    virtual ~GImageButtonItem();
    virtual void setPixmap( const QPixmap &pix, int number=-1 );
    virtual void setHeight( qreal h );

private:
    QPixmap _pix;
};

class GSubmitButtonItem : public GButtonItem
{
public:
    GSubmitButtonItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GSubmitButtonItem();
};

class GResetButtonItem : public GButtonItem
{
public:
    GResetButtonItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GResetButtonItem();
};

class GDecimalNumberItem : public GWidgetItem
{
    Q_OBJECT

public:
    GDecimalNumberItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GDecimalNumberItem();
    virtual void setValue( const QString &v );
    inline virtual QString value() const { return QString::number( _spin->value() ); }
    inline virtual void setReadOnly( bool ro ) { _spin->setReadOnly( ro ); }
    inline virtual bool readOnly() const { return _spin->isReadOnly(); }

protected slots:
    void slotNumberChanged( int );

protected:
    QSpinBox *_spin;
};

class GRealNumberItem : public GWidgetItem
{
    Q_OBJECT

public:
    GRealNumberItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GRealNumberItem();
    virtual void setValue( const QString &v );
    inline virtual QString value() const { return QString::number( _spin->value() ); }
    inline virtual void setReadOnly( bool ro ) { _spin->setReadOnly( ro ); }
    inline virtual bool readOnly() const { return _spin->isReadOnly(); }

protected slots:
    void slotNumberChanged( double );

protected:
    QDoubleSpinBox *_spin;
};

class GLineEditItem : public GWidgetItem
{
    Q_OBJECT

public:
    GLineEditItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GLineEditItem();
    inline virtual void setValue( const QString &v ) { _e->setText( v ); }
    inline virtual QString value() const { return _e->text(); }
    inline virtual bool readOnly() const { return _e->isReadOnly(); }
    inline virtual void setReadOnly( bool ro ) { _e->setReadOnly( ro ); }
    inline virtual quint16 maxLength() const { return static_cast<quint16>(_e->maxLength()); }
    inline virtual void setMaxLength( quint16 max ) { _e->setMaxLength( static_cast<int>(max) ); }

protected slots:
    void slotTextChanged( const QString& );

protected:
    QLineEdit *_e;
};

class GEmailItem : public GLineEditItem
{
    Q_OBJECT

public:
    GEmailItem( qreal w, qreal h, const PHIAItem* , QGraphicsItem *parent=0 );
    virtual ~GEmailItem();
};

class GPhoneNumberItem : public GLineEditItem
{
    Q_OBJECT

public:
    GPhoneNumberItem( qreal w, qreal h, const PHIAItem* , QGraphicsItem *parent=0 );
    virtual ~GPhoneNumberItem();
};

class GSearchItem : public GLineEditItem
{
    Q_OBJECT

public:
    GSearchItem( qreal w, qreal h, const PHIAItem* , QGraphicsItem *parent=0 );
    virtual ~GSearchItem();
};

class GPasswordItem : public GLineEditItem
{
public:
    GPasswordItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GPasswordItem();
};

class GRadioButtonItem : public GWidgetItem
{
    Q_OBJECT

public:
    GRadioButtonItem( const QString &l, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GRadioButtonItem();
    inline virtual void setLabel( const QString &l ) { _r->setText( l ); }
    inline virtual QString label() const { return _r->text(); }
    inline virtual void setChecked( bool c ) { _r->setChecked( c ); }
    inline virtual bool checked() const { return _r->isChecked(); }
    //inline virtual bool readOnly() const { return _readOnly; }
    //inline virtual void setReadOnly( bool ro ) { _readOnly=ro; }

protected slots:
    void slotCheckStateToggled( bool );

signals:
    void updateCheckState( bool );

protected:
    QRadioButton *_r;
};

class GCheckBoxItem : public GWidgetItem
{
public:
    GCheckBoxItem( const QString &l, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GCheckBoxItem();
    inline virtual void setLabel( const QString &l ) { _cb->setText( l ); }
    inline virtual QString label() const { return _cb->text(); }
    inline virtual bool checked() const { return _cb->isChecked(); }
    inline virtual void setChecked( bool c ) { _cb->setChecked( c ); }

protected:
    QCheckBox *_cb;
};

class GMultiLineEditItem : public GWidgetItem
{
    Q_OBJECT

public:
    GMultiLineEditItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GMultiLineEditItem();
    inline virtual void setValue( const QString &v ) { _e->setPlainText( v ); }
    inline virtual QString value() const { return _e->toPlainText(); }
    inline virtual bool readOnly() const { return _e->isReadOnly(); }
    inline virtual void setReadOnly( bool ro ) { _e->setReadOnly( ro ); }

protected slots:
    void slotTextChanged();

protected:
    QPlainTextEdit *_e;
};

class GTextBrowserItem : public GWidgetItem
{
    Q_OBJECT

public:
    GTextBrowserItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GTextBrowserItem();
    inline virtual bool readOnly() const { return true; }
    inline virtual quint8 alignment() const { return _alignment; }
    virtual void setStyleSheet( const QString &s );
    virtual QString styleSheet() const;
    virtual QColor color() const;
    virtual void setColor( const QColor &c );
    virtual QColor outlineColor() const;
    virtual void setOutlineColor( const QColor &c );
    virtual void setAlignment( quint8 align );
    virtual void setValue( const QString &v );

protected slots:
    void slotAnchorClicked( const QUrl& );

protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *e );

signals:
    void linkActivated( const QString& );

protected:
    QTextBrowser *_b;
    quint8 _alignment;
};

class GComboBoxItem : public GWidgetItem
{
    Q_OBJECT

public:
    GComboBoxItem( const QString &v, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GComboBoxItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;
    virtual QStringList selectedItems() const;
    virtual void setValueChecked( const QString &, bool );
    virtual bool valueChecked( const QString & ) const;

protected slots:
    virtual void slotCurrentIndexChanged( int index );

protected:
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *e );

protected:
#ifdef Q_OS_WIN
    QComboBox *_cb;
#else
    PHIAComboBox *_cb;
#endif
};

class GLangSelectorItem : public GComboBoxItem
{
    Q_OBJECT

public:
    GLangSelectorItem( const QString &v, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GLangSelectorItem();
    virtual void setValue( const QString &v ) { _value=v; }
    virtual void setDelimiter( const QString& ) { _delimiter=":"; }
    virtual QString value() const;
    virtual QStringList selectedItems() const;

protected slots:
    virtual void slotCurrentIndexChanged( int index );

signals:
    void langActivated( const QString &url );
};

class GListBoxItem : public GWidgetItem
{
    Q_OBJECT

public:
    GListBoxItem( const QString &v, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GListBoxItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;
    virtual QStringList selectedItems() const;
    virtual void setValueChecked( const QString &, bool );
    virtual bool valueChecked( const QString & ) const;

protected slots:
    virtual void slotCurrentRowChanged( int index );

protected:
    QListWidget *_lw;
};


class GCheckListItem : public GWidgetItem
{
    Q_OBJECT

public:
    GCheckListItem( const QString &v, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GCheckListItem();
    virtual void setValue( const QString &v );
    virtual void setReadOnly( bool ro );
    virtual QString value() const;
    virtual QStringList selectedItems() const;
    virtual bool valueChecked( const QString &v ) const;
    virtual void setValueChecked( const QString &v, bool c );

protected:
    QTreeWidget *_tree;
};

class GTableItem : public GCheckListItem
{
    Q_OBJECT

public:
    GTableItem( const QString &v, qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GTableItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;
    virtual QStringList selectedItems() const;
    virtual void setReadOnly( bool ro );
};

class GFileItem : public GLineEditItem
{
    Q_OBJECT

public:
    GFileItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GFileItem();
    inline virtual bool readOnly() const { return true; }
    inline virtual void setReadOnly( bool ) { ; }
    inline virtual void setValue( const QString &v ) { GWidgetItem::setValue( v ); }
    //virtual void setOutlineColor( const QColor &c );
    virtual void overwritePalette( const QPalette &p );

protected slots:
    void slotButtonClicked();

protected:
    QToolButton *_b;
};

class GDateEditItem : public GWidgetItem
{
    Q_OBJECT

public:
    GDateEditItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GDateEditItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;
    //virtual void setOutlineColor( const QColor &c );
    virtual void overwritePalette( const QPalette &p );

protected slots:
    void slotToolButtonClicked();
    void slotDateChanged( const QDate &date );

protected:
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );

protected:
   QToolButton *_toolButton;
   QDateEdit *_dateEdit;
   QPoint _pos;
   bool _init;
};

class GCalendarItem : public GWidgetItem
{
    Q_OBJECT

public:
    GCalendarItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GCalendarItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;

protected:
   QCalendarWidget *_calendar;
   bool _init;
};

class GProgressBarItem : public GWidgetItem
{
    Q_OBJECT

public:
    GProgressBarItem( qreal w, qreal h, const PHIAItem*, QGraphicsItem *parent=0 );
    virtual ~GProgressBarItem();
    virtual void setValue( const QString &v );
    virtual QString value() const;

protected:
    QProgressBar *_bar;
};

#endif // GWIDGETITEMS_H

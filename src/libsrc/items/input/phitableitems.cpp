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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHttpMultiPart>
#include <QHttpPart>
#include "phitableitems.h"
#include "phirequest.h"

class PHITableItem : public QTableWidgetItem
{
public:
    PHITableItem( const QString &t ) : QTableWidgetItem( t, QTableWidgetItem::UserType ),
        _cellType( PHIDecoratedTableItem::CString ) {}
    inline void setCellType( PHIDecoratedTableItem::CellType t ) { _cellType=t; }
    inline PHIDecoratedTableItem::CellType cellType() const { return _cellType; }
    virtual QTableWidgetItem* clone() const;
    virtual bool operator< ( const QTableWidgetItem &it ) const;
    virtual void setData( int role, const QVariant &value );

    static void setDateDisplayFormat( const QString &s ) { _dateDisplayFormat=s; }
    static QString dateToDisplayFormat( const QDateTime &d );
    static void setLang( const QString &l ) { _lang=l; }

private:
    static QString _dateDisplayFormat, _lang;
    PHIDecoratedTableItem::CellType _cellType;
};

QString PHITableItem::_dateDisplayFormat=L1( "yyyy-mm-dd" );
QString PHITableItem::_lang=L1( "en" );

QString PHITableItem::dateToDisplayFormat( const QDateTime &d )
{
    return QLocale( _lang ).toString( d, _dateDisplayFormat );
}

QTableWidgetItem* PHITableItem::clone() const
{
    PHITableItem *c=new PHITableItem( QString() );
    *c=*this;
    return c;
}

void PHITableItem::setData( int role, const QVariant &value )
{
    if ( role==Qt::DisplayRole ) {
        switch ( _cellType ) {
        case PHIDecoratedTableItem::CString: return QTableWidgetItem::setData( role, value.toString() ); break;
        case PHIDecoratedTableItem::CNumber: return QTableWidgetItem::setData( role, value.toInt() ); break;
        case PHIDecoratedTableItem::CFloat: return QTableWidgetItem::setData( role, value.toDouble() ); break;
        case PHIDecoratedTableItem::CCurrency: return QTableWidgetItem::setData( role, value.toString() ); break;
        case PHIDecoratedTableItem::CDate: {
            QDateTime d;
            if ( value.type()==QVariant::DateTime ) d=value.toDateTime();
            else d=QDateTime::fromString( value.toString(), Qt::ISODate );
            QTableWidgetItem::setData( role, dateToDisplayFormat( d ) );
            QTableWidgetItem::setData( Qt::UserRole+2, d );
            return;
        }
        default:;
        }
    }
    QTableWidgetItem::setData( role, value );
}

bool PHITableItem::operator< ( const QTableWidgetItem &it ) const
{
    if ( text().endsWith( L1( "%" ) ) && it.text().endsWith( L1( "%" ) ) ) {
        QString s1=text(), s2=it.text();
        s1.chop( 1 ); s2.chop( 1 );
        return s1.toDouble()<s2.toDouble();
    }
    switch( _cellType ) {
    case PHIDecoratedTableItem::CString: return QTableWidgetItem::operator <( it );
    case PHIDecoratedTableItem::CNumber: return data( Qt::DisplayRole ).toInt()<it.data( Qt::DisplayRole ).toInt();
    case PHIDecoratedTableItem::CFloat: return data( Qt::DisplayRole ).toDouble()<it.data( Qt::DisplayRole ).toDouble();
    case PHIDecoratedTableItem::CCurrency: {
        QString s1=text(), s2=it.text();
        if ( s1[0].isLetter() && s2[0].isLetter() ) {
            s1.remove( 0, 1 );
            s2.remove( 0, 2 );
            return s1.trimmed().toDouble()<s2.trimmed().toDouble();
        }
        break;
    }
    case PHIDecoratedTableItem::CDate: {
        QDateTime d1=data( Qt::UserRole+2 ).toDateTime();
        QDateTime d2=it.data( Qt::UserRole+2 ).toDateTime();
        return d1<d2;
    }
    default:;
    }
    return QTableWidgetItem::operator <( it );
}

PHITableConfig::PHITableConfig( PHIBaseItem *it, QWidget *parent )
    : PHIConfigWidget( it, parent )
{
    setupUi( this );
    _add->setIcon( QIcon( L1( ":/misc/add" ) ) );
    _del->setIcon( QIcon( L1( ":/misc/del" ) ) );
    QStringList header;
    header << tr( "Width") << tr( "Type" ) << tr( "Alignment" ) << tr( "Sortable" );
    _colsDef->setRowCount( header.count() );
    _colsDef->setVerticalHeaderLabels( header );
    PHIDecoratedTableItem *table=qobject_cast<PHIDecoratedTableItem*>(it);
    Q_ASSERT( table );
    _options=table->options();
    _altRowColors->setChecked( _options & PHIDecoratedTableItem::AltRowColors );
    _rowIndex->setChecked( _options & PHIDecoratedTableItem::ShowRowIndex );
    _hideGrid->setChecked( _options & PHIDecoratedTableItem::HideGrid );
    _hideBorder->setChecked( _options & PHIDecoratedTableItem::HideBorder );
    _selectMulti->setChecked( _options & PHIDecoratedTableItem::MultiSelection );
    _selectSingle->setChecked( _options & PHIDecoratedTableItem::SingleSelection );
    _selectNone->setChecked( !_selectMulti->isChecked() && !_selectSingle->isChecked() );
    _selectMode->setEnabled( false );

    QComboBox *cb;
    QSpinBox *sp;
    QCheckBox *chk;
    for ( int i=0; i<table->colTypes().count(); i++ ) {
        on__add_clicked();
        cb=qobject_cast<QComboBox*>(_colsDef->cellWidget( Type, i ));
        cb->setCurrentIndex( table->colTypes().at( i ) );
        cb=qobject_cast<QComboBox*>(_colsDef->cellWidget( Alignment, i ));
        cb->setCurrentIndex( table->colAlignments().at( i ) & ~PHIDecoratedTableItem::ASortable );
        sp=qobject_cast<QSpinBox*>(_colsDef->cellWidget( Width, i ));
        sp->setValue( table->colWidths().at( i ) );
        chk=qobject_cast<QCheckBox*>(_colsDef->cellWidget( Sortable, i ));
        chk->setChecked( table->colAlignments().at( i ) & PHIDecoratedTableItem::ASortable );
    }
    _changed=false;
}

void PHITableConfig::on__add_clicked()
{
    _changed=true;
    _colsDef->insertColumn( _colsDef->columnCount() );
    int col=_colsDef->columnCount()-1;

    QSpinBox *spin=new QSpinBox();
    spin->setRange( 0, 1000 );
    spin->setSpecialValueText( tr( "auto" ) );
    spin->setWrapping( true );
    spin->setSuffix( L1( " px" ) );
    spin->setSingleStep( 20 );
    _colsDef->setCellWidget( Width, col, spin );
    QComboBox *cb=new QComboBox();
    cb->addItems( QStringList() << tr( "String" ) << tr( "Number" ) << tr( "Float" ) << tr( "Date" ) << tr( "Currency" ) );
    _colsDef->setCellWidget( Type, col, cb );
    cb=new QComboBox();
    cb->addItems( QStringList() << tr( "Left" ) << tr( "Right" ) << tr( "Center" ) );
    _colsDef->setCellWidget( Alignment, col, cb );

    QCheckBox *chk=new QCheckBox();
    _colsDef->setCellWidget( Sortable, col, chk );
    QStringList headers;
    for ( int i=0; i<col+1; i++ ) headers << QString::number( i );
    _colsDef->setHorizontalHeaderLabels( headers );
}

void PHITableConfig::on__del_clicked()
{
    if ( _colsDef->columnCount()==0 ) return;
    _colsDef->removeColumn( _colsDef->currentColumn() );
    _changed=true;
}

bool PHITableConfig::storeData()
{
    PHIDecoratedTableItem *table=qobject_cast<PHIDecoratedTableItem*>(item());
    quint8 opts=0;
    if ( _altRowColors->isChecked() ) opts |= PHIDecoratedTableItem::AltRowColors;
    if ( _selectSingle->isChecked() ) opts |= PHIDecoratedTableItem::SingleSelection;
    if ( _selectMulti->isChecked() ) opts |= PHIDecoratedTableItem::MultiSelection;
    if ( _rowIndex->isChecked() ) opts |= PHIDecoratedTableItem::ShowRowIndex;
    if ( _hideGrid->isChecked() ) opts |= PHIDecoratedTableItem::HideGrid;
    if ( _hideBorder->isChecked() ) opts |= PHIDecoratedTableItem::HideBorder;
    if ( opts!=_options ) {
        _changed=true;
        table->setOptions( opts );
    }
    PHIListChar types, alignments;
    PHIListWord widths;
    for ( int i=0; i<_colsDef->columnCount(); i++ ) {
        int width=qobject_cast<QSpinBox*>(_colsDef->cellWidget( Width, i ))->value();
        int type=qobject_cast<QComboBox*>(_colsDef->cellWidget( Type, i ))->currentIndex();
        int align=qobject_cast<QComboBox*>(_colsDef->cellWidget( Alignment, i ))->currentIndex();
        if ( qobject_cast<QCheckBox*>(_colsDef->cellWidget( Sortable, i ))->isChecked() ) align |= PHIDecoratedTableItem::ASortable;
        types.append( static_cast<quint8>(type) );
        alignments.append( static_cast<quint8>(align) );
        widths.append( static_cast<quint16>(width) );
    }
    table->setColTypes( types );
    table->setColAlignments( alignments );
    table->setColWidths( widths );
    item()->update();
    return _changed;
}

void PHIDecoratedTableItem::ideInit()
{
    setColor( PHIPalette::WidgetText, PHIPalette::Text, page()->phiPalette().color( PHIPalette::Text ) );
    setColor( PHIPalette::WidgetBase, PHIPalette::Window, page()->phiPalette().color( PHIPalette::Window ) );
    textData()->setText( L1( "header1|header2|header3...\ncol1[submitdata]|col2" ) );
    setOption( SingleSelection );
}

void PHIDecoratedTableItem::initWidget()
{
    QTableWidget *t=new QTableWidget();
    setWidget( t );
    PHITableItem *it=new PHITableItem( QString() );
    t->setItemPrototype( it );
    t->setFrameShape( QFrame::Box );
    t->setSelectionBehavior( QAbstractItemView::SelectRows );
    t->horizontalHeader()->setHighlightSections( false );
    t->setSortingEnabled( false );
    QPalette pal=t->palette();
    pal.setColor( QPalette::Window, Qt::transparent );
    t->setPalette( pal );
    if ( !isClientItem() ) return;
    connect( t->horizontalHeader(), &QHeaderView::sectionClicked, this, &PHIDecoratedTableItem::slotSectionClicked );
    connect( t, &QTableWidget::itemSelectionChanged, this, &PHIDecoratedTableItem::slotItemSelectionChanged );
}

void PHIDecoratedTableItem::setWidgetText( const QString &t )
{
    PHITableItem::setDateDisplayFormat( QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ) );
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    if ( !table ) return;
    table->clear();
    int off=0;
    if ( wid()==CheckList ) off=1;

    QStringList list=t.split( realDelimiter() );
    table->setVerticalHeaderLabels( QStringList() );
    if ( list.count()>0 ) {
        QStringList headers=list.at( 0 ).split( L1( "|" ) );
        if ( off ) headers.prepend( QString() );
        table->setColumnCount( headers.count() );
        table->setHorizontalHeaderLabels( headers );
        list.removeAt( 0 );
        table->setRowCount( list.count() );
        for ( int row=0; row<list.count(); row++ ) {
            QString line=list.at( row );
            QString val, checked=L1( "0" );
            int start=line.indexOf( L1( "[" ) );
            int end=line.indexOf( L1( "]" ) );
            if ( start!=-1 && end !=-1 ) {
                val=line.mid( start+1, end-start-1 );
                line.replace( start, end-start+1, QString() );
            }
            start=line.indexOf( L1( "[" ) );
            end=line.indexOf( L1( "]" ) );
            if ( start!=-1 && end !=-1 ) {
                checked=line.mid( start+1, end-start-1 );
                line.replace( start, end-start+1, QString() );
                if ( checked.toLower()==L1( "true" ) ) checked=L1( "1" );
                else if ( checked.toInt() ) checked=L1( "1" );
            }
            QStringList cols=line.split( L1( "|" ) );
            for ( int col=0; col<cols.count(); col++ ) {
                PHITableItem *it=new PHITableItem( cols.at( col ) );
                // @todo: implement real HTML subset for cell
                QString html=cols.at( col );
                if ( html.contains( QLatin1Char( '<' ) ) ) {
                    it->setData( Qt::UserRole+3, html );
                    QFont f=it->font();
                    if ( html.contains( L1( "<b>" ) ) ) f.setBold( true );
                    else f.setBold( false );
                    if ( html.contains( L1( "<i>" ) ) ) f.setItalic( true );
                    else f.setItalic( false );
                    it->setFont( f );
                    html.replace( QRegExp( L1( "<[^>]*>") ), QString() );
                    it->setData( Qt::DisplayRole, html );
                }
                table->setItem( row, col+off, it );
                if ( col==0 ) {
                    if ( val.isEmpty() ) val=cols.at( 0 );
                    it->setData( Qt::UserRole, val );
                    it->setData( Qt::UserRole+1, row );
                }
            }
            // fill empty cells:
            for ( int col=cols.count()+off; col<table->columnCount(); col++ ) {
                PHITableItem *it=new PHITableItem( QString() );
                Q_ASSERT( it );
                table->setItem( row, col, it );            }
            if ( off ) {
                QTableWidgetItem *it=new QTableWidgetItem();
                it->setData( Qt::CheckStateRole, checked.toInt() ? Qt::Checked : Qt::Unchecked );
                table->setItem( row, 0, it );
            }
        }
    }
    updateWidget();
}

void PHIDecoratedTableItem::updateWidget()
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    if ( !table ) return;
    table->verticalHeader()->setVisible( options() & PHIDecoratedTableItem::ShowRowIndex );
    table->setAlternatingRowColors( options() & PHIDecoratedTableItem::AltRowColors );
    table->setShowGrid( !(options() & PHIDecoratedTableItem::HideGrid) );
    QAbstractItemView::SelectionMode mode=QAbstractItemView::NoSelection;
    if ( options() & PHIDecoratedTableItem::SingleSelection ) mode=QAbstractItemView::SingleSelection;
    else if ( options() & PHIDecoratedTableItem::MultiSelection ) mode=QAbstractItemView::MultiSelection;
    table->setSelectionMode( mode );
    if ( options() & HideBorder ) table->setFrameStyle( QFrame::NoFrame );
    else table->setFrameStyle( QFrame::Box );
    int off=0;
    if ( wid()==CheckList ) {
        table->setColumnWidth( 0, 28 );
        off=1;
    }
    for ( int col=0; col<colTypes().count(); col++ ) {
        quint8 align=colAlignments().at( col );
        quint8 type=colTypes().at( col );
        quint16 w=colWidths().at( col );
        if ( w>0 ) table->setColumnWidth( col+off, static_cast<int>(w) );
        else table->resizeColumnToContents( col+off );
        for ( int row=0; row<table->rowCount(); row++ ) {
            PHITableItem *it=dynamic_cast<PHITableItem*>(table->item( row, col ));
            if ( it ) {
                if ( align & ARight ) it->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter  );
                else if ( align & ACenter ) it->setTextAlignment( Qt::AlignCenter | Qt::AlignVCenter );
                QVariant tmp=it->data( Qt::DisplayRole );
                it->setCellType( static_cast<PHIDecoratedTableItem::CellType>(type) );
                it->setData( Qt::DisplayRole, tmp );
            }
        }
    }
    table->resizeRowsToContents();
}

void PHIDecoratedTableItem::squeeze()
{
    PHIAbstractInputItem::squeeze();
    if ( options()==SingleSelection ) removeData( DOptions );
    if ( realDelimiter()==L1( "\n" ) ) removeData( DDelimiter );
    if ( colTypes().count()==0 ) {
        removeData( DColType );
        removeData( DColAlignment );
        removeData( DColWidth );
        removeData( DColSortable );
        removeData( DColCheckable );
    }
    removeData( DDateFormat );
}

QSizeF PHIDecoratedTableItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::PreferredSize ) return QSizeF( 300., 150. );
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

int PHIDecoratedTableItem::logicRow( int row ) const
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    int col=0;
    if ( wid()==CheckList ) col++;
    for ( int i=0; i<table->rowCount(); i++ ) {
        if ( table->item( i, col )->data( Qt::UserRole+1 ).toInt()==row ) {
            return i;
        }
    }
    return -1;
}

QScriptValue PHIDecoratedTableItem::delimiter( const QScriptValue &d )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !d.isValid() ) return realDelimiter();
    setDelimiter( d.toString() );
    return self();
}

QScriptValue PHIDecoratedTableItem::data( const QScriptValue &d )
{
    if ( !isServerItem() ) return scriptEngine()->undefinedValue();
    if ( !d.isValid() ) return realText();
    setText( d.toString() );
    return self();
}

QScriptValue PHIDecoratedTableItem::rowCount() const
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    return table->rowCount();
}

QScriptValue PHIDecoratedTableItem::hideRow( int row, bool b )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    table->setRowHidden( logicRow( row ), b );
    return self();
}

QScriptValue PHIDecoratedTableItem::hideCol( int col, bool b )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    table->setColumnHidden( col, b );
    return self();
}

QScriptValue PHIDecoratedTableItem::label( int col, const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    if ( !v.isValid() ) return table->horizontalHeaderItem( col )->text();
    table->horizontalHeaderItem( col )->setText( v.toString() );
    return self();
}

QScriptValue PHIDecoratedTableItem::cellHtml( int row, int col, const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    if ( !v.isValid() ) {
        if ( row>=table->rowCount() || col>=table->columnCount() ) return false;
        PHITableItem *it=dynamic_cast<PHITableItem*>(table->item( logicRow( row ), col ) );
        if ( !it ) return false;
        if ( it->data( Qt::UserRole+3 ).isValid() ) return it->data( Qt::UserRole+3 ).toString();
        return it->text();
    }
    if ( row>=table->rowCount() || col>=table->columnCount() ) return self();
    QTableWidgetItem *it=table->item( logicRow( row ), col );
    Q_ASSERT( it );
    it->setData( Qt::UserRole+3, v.toString() );
    // @todo: implement real HTML subset for cell
    QString html=v.toString();
    QFont f=it->font();
    if ( html.contains( L1( "<b>" ) ) ) f.setBold( true );
    else f.setBold( false );
    if ( html.contains( L1( "<i>" ) ) ) f.setItalic( true );
    else f.setItalic( false );
    it->setFont( f );
    html.replace( QRegExp( L1( "<[^>]*>") ), QString() );
    it->setData( Qt::DisplayRole, html );
    return self();
}

QScriptValue PHIDecoratedTableItem::cellText( int row, int col, const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    if ( !v.isValid() ) {
        if ( row>=table->rowCount() || col>=table->columnCount() ) return false;
        PHITableItem *it=dynamic_cast<PHITableItem*>(table->item( logicRow( row ), col ) );
        if ( !it ) return false;
        return it->text();
    }
    if ( row>=table->rowCount() || col>=table->columnCount() ) return self();
    QTableWidgetItem *it=table->item( logicRow( row ), col );
    QFont f=it->font();
    f.setBold( false );
    f.setItalic( false );
    it->setFont( f );
    it->setData( Qt::DisplayRole, v.toString() );
    return self();
}

QScriptValue PHIDecoratedTableItem::cellColor( int row, int col, const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    /* - write only -
    if ( !v.isValid() ) {
        if ( row>=table->rowCount() || col>=table->columnCount() ) return false;
        return PHI::colorToString( table->item( logicRow( row ), col )->textColor() );
    }
    */
    if ( row>=table->rowCount() || col>=table->columnCount() ) return self();
    Q_ASSERT( table->item( logicRow( row ), col ) );
    table->item( logicRow( row ), col )->setTextColor( PHI::colorFromString( v.toString() ) );
    return self();
}

QScriptValue PHIDecoratedTableItem::cellBgColor( int row, int col, const QScriptValue &v )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( wid()==CheckList ) col++;
    /* - write only -
    if ( !v.isValid() ) {
        if ( row>=table->rowCount() || col>=table->columnCount() ) return false;
        return PHI::colorToString( table->item( logicRow( row ), col )->backgroundColor() );
    }
    */
    if ( row>=table->rowCount() || col>=table->columnCount() ) return self();
    Q_ASSERT( table->item( logicRow( row ), col ) );
    table->item( logicRow( row ), col )->setBackgroundColor( PHI::colorFromString( v.toString() ));
    return self();
}

QScriptValue PHIDecoratedTableItem::checked( const QScriptValue &r, const QScriptValue &v )
{
    if ( !isClientItem() || wid()==DecoratedTable ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    if ( !v.isValid() ) {
        if ( r.isString() ) {
            QString val=r.toString();
            for ( int i=0; i<table->rowCount(); i++ ) {
                if ( val==table->item( i, 1 )->data( Qt::UserRole ).toString() )
                    return table->item( i, 0 )->data( Qt::CheckStateRole ).toBool();
            }
            return false;
        }
        if ( r.toInt32()>=table->rowCount() ) return false;
        return table->item( logicRow( r.toInt32() ), 0 )->data( Qt::CheckStateRole ).toBool();
    }
    if ( r.isString() ) {
        QString val=r.toString();
        for ( int i=0; i<table->rowCount(); i++ ) {
            if ( val==table->item( i, 1 )->data( Qt::UserRole ).toString() ) {
               table->item( i, 0 )->setData( Qt::CheckStateRole, v.toBool() ? Qt::Checked : Qt::Unchecked );
               return self();
            }
        }
        return self();
    }
    if ( r.toInt32()>=table->rowCount() ) return self();
    table->item( logicRow( r.toInt32() ), 0 )->setData( Qt::CheckStateRole, v.toBool() ? Qt::Checked : Qt::Unchecked );
    return self();
}

QScriptValue PHIDecoratedTableItem::dateFormat( const QScriptValue &df )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    if ( !df.isValid() ) return realDateFormat();
    setDateFormat( df.toString() );
    return self();
}

QScriptValue PHIDecoratedTableItem::selected( const QScriptValue &row )
{
    if ( !isClientItem() || wid()==CheckList ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    int r;
    if ( !row.isValid() ) return logicRow( table->currentRow() );
    if ( row.isString() ) {
        QString val=row.toString();
        for ( int i=0; i<table->rowCount(); i++ ) {
            if ( table->item( i, 0 )->data( Qt::UserRole ).toString()==val ) {
                table->selectRow( i );
                return self();
            }
        }
    }
    r=row.toInt32();
    for ( int i=0; i<table->rowCount(); i++ ) {
        if ( table->item( i, 0 )->data( Qt::UserRole+1 ).toInt()==r ) {
            table->selectRow( i );
            break;
        }
    }
    return self();
}

QScriptValue PHIDecoratedTableItem::addRow( const QScriptValue &v, const QScriptValue &pos )
{
    if ( !isClientItem() ) return scriptEngine()->undefinedValue();
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    int row=table->rowCount();
    if ( pos.isValid() ) {
        row=pos.toInt32();
        if ( row>table->rowCount() ) row=table->rowCount();
    }
    table->insertRow( row );
    int col=0;
    if ( wid()==CheckList ) {
        QTableWidgetItem *it=new QTableWidgetItem();
        it->setData( Qt::CheckStateRole, Qt::Unchecked );
        table->setItem( row, 0, it );
        col++;
    }
    for ( int c=col; c<table->columnCount(); c++ ) {
        PHITableItem *it=new PHITableItem( QString() );
        table->setItem( row, c, it );
        if ( c==col ) {
            if ( v.isValid() ) it->setData( Qt::UserRole, v.toString() );
            it->setData( Qt::UserRole+1, table->rowCount()-1 );
        }
    }
    table->resizeRowToContents( row );
    return table->rowCount()-1;
}

void PHIDecoratedTableItem::slotSectionClicked( int col )
{
    bool isSortable=false;
    if ( wid()==CheckList ) col--;
    if ( col>-1 && col<colAlignments().count() ) {
        quint8 align=colAlignments().at( col );
        if ( align & ASortable ) isSortable=true;
    }
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    table->horizontalHeader()->setSortIndicatorShown( isSortable );
    if ( !isSortable ) return;
    if ( table->currentRow()>-1 ) {
        QTableWidgetItem *it=table->item( table->currentRow(), 0 );
        if ( it ) {
            trigger( L1( "selectionChanged" ), QScriptValue( logicRow( it->row() ) ) );
            trigger( L1( "changed" ), QScriptValue( logicRow( it->row() ) ) );
        }
    }
    if ( wid()==CheckList ) col++;
    table->sortByColumn( col );
}

void PHIDecoratedTableItem::slotItemSelectionChanged()
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    int row=table->currentRow();
    trigger( L1( "selectionChanged" ), QScriptValue( logicRow( row ) ) );
    trigger( L1( "changed" ), QScriptValue( logicRow( row ) ) );
}

void PHIDecoratedTableItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    Q_UNUSED( req )
    QByteArray col, head, high, sel, highTxt;
    col=cssColor( colorForRole( PHIPalette::WidgetText ) );
    head=cssColor( page()->phiPalette().color( PHIPalette::ButtonText ) );
    high=cssColor( page()->phiPalette().color( PHIPalette::Highlight ) );
    // @todo: choose appropriate hover color
    sel=cssColor( page()->phiPalette().color( PHIPalette::AlternateBase ) );
    highTxt=cssColor( page()->phiPalette().color( PHIPalette::HighlightText) );
    out+='#'+id()+BL( " .ui-jqgrid{background-color:transparent !important;background:none !important}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv td{color:" )+col+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid-labels .ui-th-column{color:" )+head+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv .ui-state-highlight{border-color:#cccccc;background:none;background-color:" )+sel+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv .ui-state-hover{background-color:" )+high+BL( ";color:" )+highTxt+BL( "}\n" );
    out+='#'+id()+BL( " .ui-corner-all{border-radius:0px}\n" );
    if ( options() & HideGrid ) {
        out+='#'+id()+BL( " .ui-jqgrid tr.ui-row-ltr td{border-color:transparent}\n" );
        out+='#'+id()+BL( " .ui-jqgrid tr.ui-row-rtl td{border-color:transparent}\n" );
    }
    if ( options() & HideBorder ) {
        out+='#'+id()+BL( " .ui-jqgrid{border-color:transparent}\n" );
    }
    QFont f=font();
    if ( f!=page()->font() ) {
        out+='#'+id()+BL( " .ui-jqgrid tr.jqgrow td{font-family:'" )+f.family().toUtf8();
        if ( !f.lastResortFamily().isEmpty() ) {
            out+=BL( "','" )+f.lastResortFamily().toUtf8();
        }
        out+=BL( "';font-size:" )+QByteArray::number( f.pointSize() )+BL( "pt;" );
        if ( f.bold() ) out+=BL( "font-weight:bold;" );
        if ( f.italic() ) out+=BL( "font-style:italic;" );
        if ( f.underline() ) out+=BL( "text-decoration:underline;" );
        out+=BL( "}\n" );
    }
}

void PHIDecoratedTableItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"><table id=\"" )+id()+BL( "_phit\"></table>" );
    if ( wid()==DecoratedTable ) {
        out+=BL( "<input type=\"hidden\" id=\"" )+id()+BL( "_phi\" name=\"" )+id()+BL( "\">" );
    }
    out+=BL( "</div>\n" );
    PHIByteArrayList rows;
    if ( realDelimiter().length()>1 ) {
        // expensive utf8->string->utf8 conversion:
        QStringList list=realText().split( realDelimiter() );
        QString s;
        foreach ( s, list ) rows.append( s.toUtf8() );
    } else rows=PHIBaseItem::data( DText ).toByteArray().split( realDelimiter().toUtf8()[0] );
    if ( rows.count()>0 ) {
        PHIByteArrayList cols=rows.first().split( '|' );
        QByteArray def=BL( "[" ), labels, row, values=BL( "['" );
        QByteArray data=BL( "\n\"{'p':'1','t':'1','r':'" )+QByteArray::number( rows.count()-1 )+BL( "','d':[\"\n" );
        QByteArray colspec, checks=BL( "[" );
        for ( int i=0; i<cols.count(); i++ ) {
            if ( i < colTypes().count() ) {
                colspec.clear();
                quint8 align=colAlignments().at( i );
                quint8 type=colTypes().at( i );
                quint16 w=colWidths().at( i );
                colspec+=BL( ",sortable:" );
                if ( align & ASortable ) colspec+=BL( "true," );
                else colspec+=BL( "false," );
                if ( align & ARight ) colspec+=BL( "align:'right'," );
                else if ( align & ACenter ) colspec+=BL( "align:'center'," );
                colspec+=BL( "sorttype:" );
                if ( type==CString ) colspec+=BL( "'text'" );
                else if ( type==CNumber ) colspec+=BL( "'int',formatter:'integer'" );
                else if ( type==CFloat ) colspec+=BL( "'float',formatter:'number'" );
                else if ( type==CDate ) colspec+=BL( "'date',formatter:function(a,b,c){return $('" )+id()+BL( "')._dFm(a,b,c);}" );
                else if ( type==CCurrency ) colspec+=BL( "'currency',formatter:'currency'" );
                else colspec+=BL( "'text'" );
                if ( w>0 ) colspec+=BL( ",width:'" )+QByteArray::number( w )+BL( "px'" );
            }
            labels+=BL( ".label(" )+QByteArray::number( i )+BL( ",'" )+cols[i].replace( '\'', BL( "\\'" ) )+BL( "')" );
            def+=BL( "{name:'" )+QByteArray::number( i )+BL( "c'" )+colspec+BL( "}," );
        }
        for ( int i=1; i<rows.count(); i++ ) {
            row=rows.at( i );
            // @todo: replace json with xml to prevent string replacement
            if ( row.indexOf( '\'' )>-1 ) row.replace( '\'', BL( "\\\\'" ) );
            if ( row.indexOf( '"' )>-1 ) row.replace( '"', BL( "\\\"") );
            int start=row.indexOf( '[' );
            int end=row.indexOf( ']' );
            QByteArray v;
            if ( start!=-1 && end !=-1 ) {
                v=row.mid( start+1, end-start-1 );
                row.replace( start, end-start+1, QByteArray() );
                if ( wid()==CheckList ) {
                    start=row.indexOf( '[' );
                    end=row.indexOf( ']' );
                    if ( start!=-1 && end !=-1 ) { // check or uncheck value
                        QByteArray chk=row.mid( start+1, end-start-1 );
                        row.replace( start, end-start+1, QByteArray() );
                        if ( chk.toLower()==BL( "true" ) || chk.toInt() ) checks+=BL( "1," );
                        else checks+=BL( "0," );
                    } else checks+=BL( "0," );
                }
            } else {
                start=row.indexOf( '|' );
                v=row.mid( 0, start );
                if ( wid()==CheckList ) checks+=BL( "0," );
            }
            values+=v+BL( "','" );
            if ( realDelimiter()!=L1( "\n" ) ) row.replace( '\n', BL( "<br>" ) );
            data+=BL( "+\"{'i':'" )+QByteArray::number( i-1 )+BL( "r',c:['");
            //if ( options() & PHIDecoratedTableItem::ShowRowIndex ) data+=QByteArray::number( i )+BL( "','" );
            data+=row.replace( '|', BL( "','" ) )+BL( "']},\"\n" );
        }
        if ( rows.count()==1 ) {
            data+=BL( "+\",\"\n" ); // handle empty rows
            values+=BL( "','" );
        }
        data.chop( 3 ); // remove last ",\"\n"
        def.chop( 1 ); // remove last ","
        values.chop( 2 ); // remove ",'"
        if ( checks.size()>1 ) checks.chop( 1 ); // remove ","
        labels+=BL( ";\n" );
        data+=BL( "]}\"" );
        values+=']';
        checks+=']';
        script+=BL( "$$table('" )+id()+BL( "'," )+def+BL( "]," )+data+',';
        if ( options() & ShowRowIndex ) script+=BL( "true,\n" );
        else script+=BL( "false,\n" );
        script+=values+','+QByteArray::number(-25)+',';
        if ( wid()==CheckList ) script+=BL( "true," )+checks;
        else script+=BL( "false" );
        script+=BL( ")\n" )+labels;
    }
    if ( options() & HideBorder ) setAdjustedRect( rect().adjusted( -1, -1, -1, -24 ) );
    else setAdjustedRect( rect().adjusted( 0, 0, -2, -25 ) );
    htmlInitItem( script, false );
    script+=BL( ".dateFormat('" )+QLocale( page()->lang() ).dateFormat( QLocale::ShortFormat ).toLatin1().toLower()+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetText )==PHIPalette::Custom ) )
        script+=BL( ".color('" )+cssColor( realColor() )+BL( "')" );
    if ( Q_UNLIKELY( colorRole( PHIPalette::WidgetBase )==PHIPalette::Custom ) )
        script+=BL( ".bgColor('" )+cssColor( realBackgroundColor() )+BL( "')" );
    script+=BL( ";\n" );
}

PHIWID PHIDecoratedTableItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const
{
    header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-jqgrid-en\"></script>\n\t" );
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) )
        header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-jqgridiefix\"></script>\n" );
    else header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-jqgrid\"></script>\n" );
    return static_cast<PHIWID>(DecoratedTable);
}

PHIWID PHIDecoratedTableItem::htmlScriptExtension( const PHIRequest *req, QByteArray &script ) const
{
    Q_UNUSED( req )
    script+=BL( "jQuery.jgrid.no_legacy_api=true;\n" );
    return static_cast<PHIWID>(DecoratedTable);
}

void PHIDecoratedTableItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    QList <QTableWidgetItem*> list=table->selectedItems();
    if ( !list.count() ) return;
    QTableWidgetItem *it=list.first();
    QHttpPart hp;
    hp.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
    hp.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"" ) );
    hp.setBody( it->data( Qt::UserRole ).toString().toUtf8() );
    multiPart->append( hp );
}

void PHIDecoratedTableItem::setDateFormat( const QString &s )
{
    setData( DDateFormat, s );
    QString fmt=s;
    fmt.replace( L1( "H" ), L1( "h" ) );
    fmt.replace( L1( "M" ), L1( "§%" ) );
    fmt.replace( L1( "m" ), L1( "M" ) );
    fmt.replace( L1( "§%" ), L1( "m" ) );
    fmt.replace( L1( "tt" ), L1( "ap" ) );
    fmt.replace( L1( "TT" ), L1( "AP" ) );
    fmt.replace( L1( "l" ), L1( "zzz" ) );
    PHITableItem::setDateDisplayFormat( fmt );
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    if ( !table || !table->rowCount() ) return;
    int col=0;
    if ( wid()==CheckList ) col++;
    while ( col<table->columnCount() ) {
        PHITableItem *it;
        it=dynamic_cast<PHITableItem*>(table->item( 0, col ) );
        ++col;
        if ( it->cellType()!=CDate ) continue;
        for ( int row=0; row<table->rowCount(); row++ ) {
            it=dynamic_cast<PHITableItem*>(table->item( row, col-1 ) );
            QDateTime d=it->data( Qt::UserRole+2 ).toDateTime();
            it->setData( Qt::DisplayRole, d );
        }
    }
}

QScriptValue PHIDecoratedTableItem::clear()
{
    if ( isClientItem() ) {
        QTableWidget *table=qobject_cast<QTableWidget*>(widget());
        Q_ASSERT( table );
        table->clearContents();
        table->setRowCount( 0 );
        return self();
    }
    setText( QString() );
    return self();
}

PHIConfigWidget* PHIDecoratedTableItem::ideConfigWidget()
{
    return new PHITableConfig( this );
}

void PHICheckListItem::clientPostData( QHttpMultiPart *multiPart ) const
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    Q_ASSERT( table );
    for ( int i=0; i<table->rowCount(); i++ ) {
        if ( table->item( i, 0 )->data( Qt::CheckStateRole ).toBool() ) {
            QHttpPart hp;
            hp.setHeader( QNetworkRequest::ContentTypeHeader, BL( "text/plain" ) );
            hp.setHeader( QNetworkRequest::ContentDispositionHeader, BL( "form-data; name=\"" )+id()+BL( "\"" ) );
            hp.setBody( table->item( i, 1 )->data( Qt::UserRole ).toString().toUtf8() );
            multiPart->append( hp );
        }
    }
}

void PHICheckListItem::ideInit()
{
    PHIDecoratedTableItem::ideInit();
    setOption( SingleSelection, false );
    setOption( MultiSelection );
}

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

private:
    PHIDecoratedTableItem::CellType _cellType;
};

QTableWidgetItem* PHITableItem::clone() const
{
    PHITableItem *c=new PHITableItem( QString() );
    *c=*this;
    return c;
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
    case PHIDecoratedTableItem::CNumber: return text().toInt()<it.text().toInt();
    case PHIDecoratedTableItem::CFloat: return text().toDouble()<it.text().toDouble();
    case PHIDecoratedTableItem::CCurrency: {
        QString s1=text(), s2=it.text();
        if ( s1[0].isLetter() && s2[0].isLetter() ) {
            s1.remove( 0, 1 );
            s2.remove( 0, 2 );
            return s1.trimmed().toDouble()<s2.trimmed().toDouble();
        }
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
}

void PHIDecoratedTableItem::setWidgetText( const QString &t )
{
    QTableWidget *table=qobject_cast<QTableWidget*>(widget());
    if ( !table ) return;
    table->clear();
    table->setSortingEnabled( true );
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
                Q_ASSERT( it );
                table->setItem( row, col+off, it );
                if ( col==0 ) {
                    if ( val.isEmpty() ) val=cols.at( 0 );
                    it->setData( Qt::UserRole, val );
                }
            }
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
    table->horizontalHeader()->setSortIndicatorShown( true );
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
                it->setCellType( static_cast<PHIDecoratedTableItem::CellType>(type) );
            }
        }
    }
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
}

QSizeF PHIDecoratedTableItem::sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const
{
    if ( isChild() ) return realSize();
    if ( which==Qt::PreferredSize ) return QSizeF( 300., 150. );
    return PHIAbstractInputItem::sizeHint( which, constraint );
}

QScriptValue PHIDecoratedTableItem::delimiter( const QScriptValue &d )
{
    if ( !d.isValid() ) return realDelimiter();
    setDelimiter( d.toString() );
    return self();
}

void PHIDecoratedTableItem::cssStatic( const PHIRequest *req, QByteArray &out ) const
{
    QByteArray col, bgCol, head, high, sel, highTxt;
    if ( Q_LIKELY( req->agentFeatures() & PHIRequest::RGBA ) ) {
        col=cssRgba( color( PHIPalette::WidgetText ) );
        bgCol=cssRgba( color( PHIPalette::WidgetBase ) );
        if ( backgroundColorRole()==PHIPalette::Window ) bgCol=BL( "transparent" );
        head=cssRgba( page()->phiPalette().color( PHIPalette::ButtonText ) );
        high=cssRgba( page()->phiPalette().color( PHIPalette::Highlight ) );
        // @todo: choose appropriate hover color
        sel=cssRgba( page()->phiPalette().color( PHIPalette::Error ) );
        highTxt=cssRgba( page()->phiPalette().color( PHIPalette::HighlightText) );
    } else {
        col=color( PHIPalette::PHIPalette::WidgetText ).name().toLatin1();
        bgCol=color( PHIPalette::PHIPalette::WidgetBase ).name().toLatin1();
        if ( backgroundColorRole()==PHIPalette::Window ) bgCol=BL( "" );
        head=page()->phiPalette().color( PHIPalette::ButtonText ).name().toLatin1();
        high=page()->phiPalette().color( PHIPalette::Highlight ).name().toLatin1();
        sel=page()->phiPalette().color( PHIPalette::Error ).name().toLatin1();
        highTxt=page()->phiPalette().color( PHIPalette::HighlightText).name().toLatin1();
    }
    out+='#'+id()+BL( " .ui-jqgrid{background-color:" )+bgCol+BL( " !important;background-image:none !important}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv td{color:" )+col+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid-labels .ui-th-column{color:" )+head+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv .ui-state-highlight{border-color:#cccccc;background:none;background-color:" )+sel+BL( "}\n" );
    out+='#'+id()+BL( " .ui-jqgrid .ui-jqgrid-bdiv .ui-state-hover{background-color:" )+high+BL( ";color:" )+highTxt+BL( "}\n" );
    if ( options() & HideGrid ) {
        out+='#'+id()+BL( " .ui-jqgrid tr.ui-row-ltr td{border-color:transparent}\n" );
        out+='#'+id()+BL( " .ui-jqgrid tr.ui-row-rtl td{border-color:transparent}\n" );
    }
    if ( options() & HideBorder ) {
        out+='#'+id()+BL( " .ui-jqgrid{border-color:transparent}\n" );
    }
}

void PHIDecoratedTableItem::html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const
{
    out+=indent+BL( "<div" );
    htmlBase( req, out, script );
    out+=BL( "\"><table id=\"" )+id()+BL( "_phit\"></table><input type=\"hidden\" id=\"" )
        +id()+BL( "_phi\" name=\"" )+id()+BL( "\"></div>\n" );
    PHIByteArrayList rows;
    if ( realDelimiter().length()>1 ) {
        // expensive utf8->string->utf8 conversion:
        QStringList list=realText().split( realDelimiter() );
        QString s;
        foreach ( s, list ) rows.append( s.toUtf8() );
    } else rows=data( DText ).toByteArray().split( realDelimiter().toUtf8()[0] );
    if ( rows.count()>0 ) {
        PHIByteArrayList cols=rows.first().split( '|' );
        QByteArray def=BL( "[" ), labels, row, values=BL( "['" );
        QByteArray data=BL( "\n\"{'p':'1','t':'1','r':'" )+QByteArray::number( rows.count()-1 )+BL( "','d':[\"\n" );
        QByteArray colspec;
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
                else if ( type==CDate ) colspec+=BL( "'date',formatter:'date'" );
                else if ( type==CCurrency ) colspec+=BL( "'currency',formatter:'currency'" );
                else colspec+=BL( "'text'" );
                if ( w>0 ) colspec+=BL( ",width:" )+QByteArray::number( w );
            }
            labels+=BL( ".label(" )+QByteArray::number( i )+BL( ",'" )+cols.at( i )+BL( "')" );
            def+=BL( "{name:'" )+QByteArray::number( i )+'\''+colspec+BL( "}," );
        }
        for ( int i=1; i<rows.count(); i++ ) {
            row=rows.at( i );
            // @todo: replace json with xml to prevent string replacement
            row.replace( '\'', BL( "\\\\'" ) );
            row.replace( '"', BL( "\\\"") );
            int start=row.indexOf( '[' );
            int end=row.indexOf( ']' );
            QByteArray v;
            if ( start!=-1 && end !=-1 ) {
                v=row.mid( start+1, end-start-1 );
                row.replace( start, end-start+1, QByteArray() );
            } else {
                start=row.indexOf( '|' );
                v=row.mid( 0, start );
            }
            values+=v+BL( "','" );
            if ( realDelimiter()!=L1( "\n" ) ) row.replace( '\n', BL( "<br>" ) );
            data+=BL( "+\"{'i':'" )+QByteArray::number( i-1 )+BL( "',c:['");
            //if ( options() & PHIDecoratedTableItem::ShowRowIndex ) data+=QByteArray::number( i )+BL( "','" );
            data+=row.replace( '|', BL( "','" ) )+BL( "']},\"\n" );
        }
        data.chop( 3 ); // remove last ",\"\n"
        def.chop( 1 ); // remove last ","
        values.chop( 2 ); // remove ",'"
        labels+=BL( ";\n" );
        data+=BL( "]}\"" );
        values+=']';
        script+=BL( "$.grid('" )+id()+BL( "'," )+def+BL( "]," )+data+',';
        if ( options() & PHIDecoratedTableItem::ShowRowIndex ) script+=BL( "true,\n" );
        else script+=BL( "false,\n" );
        script+=values+','+QByteArray::number(-25)+',';
        if ( wid()==CheckList ) script+=BL( "true" );
        else script+=BL( "false" );
        script+=')'+labels;
    }
}

PHIWID PHIDecoratedTableItem::htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const
{
    if ( Q_UNLIKELY( req->agentFeatures() & PHIRequest::IE678 ) )
        header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-jqgridiefix\"></script>\n" );
    else header+=BL( "<script type=\"text/javascript\" src=\"phi.phis?j=ui-jqgrid\"></script>\n" );
    return static_cast<PHIWID>(DecoratedTable);
}

PHIConfigWidget* PHIDecoratedTableItem::ideConfigWidget()
{
    return new PHITableConfig( this );
}

void PHICheckListItem::ideInit()
{
    PHIDecoratedTableItem::ideInit();
    setOption( SingleSelection, false );
    setOption( MultiSelection );
}

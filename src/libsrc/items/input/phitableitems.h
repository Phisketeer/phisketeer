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
#ifndef PHITABLEITEMS_H
#define PHITABLEITEMS_H
#include <QVector>
#include "phiabstractitems.h"
#include "phiconfigwidget.h"
#include "ui_tableconfig.h"

class PHITableConfig : public PHIConfigWidget, private Ui::TableConfig
{
    Q_OBJECT

public:
    enum ColType { Width=0, Type=1, Alignment=2, Sortable=3, Checkable=4 };
    explicit PHITableConfig( PHIBaseItem *it, QWidget *parent=0 );
    bool storeData();

protected slots:
    void on__add_clicked();
    void on__del_clicked();

private:
    bool _changed;
    quint16 _options;
};

class PHIDecoratedTableItem : public PHIAbstractInputItem
{
    Q_OBJECT
    Q_PROPERTY( QString _delimiter READ realDelimiter WRITE setDelimiter SCRIPTABLE false )

public:
    enum Wid { DecoratedTable=46, CheckList=43 };
    enum DataType { DOptions=1, DColWidth=2, DColType=3, DColAlignment=4, DColSortable=5, DColCheckable=6, DDelimiter=7, DDateFormat=8 };
    enum CellType { CString=0, CNumber=1, CFloat=2, CDate=3, CCurrency=4 };
    enum CellAlign { ALeft=0, ARight=1, ACenter=2, ASortable=8 };
    enum Option { None=0x0, SingleSelection=0x1, MultiSelection=0x2, AltRowColors=0x4, ShowRowIndex=0x8,
        HideGrid=0x10, HideBorder=0x20 };
    explicit PHIDecoratedTableItem( const PHIBaseItemPrivate &p ) : PHIAbstractInputItem( p ) { if ( isGuiItem() ) initWidget(); }
    PHIDecoratedTableItem( const PHIDecoratedTableItem &it ) : PHIAbstractInputItem( it ) { if ( isGuiItem() ) initWidget(); }
    virtual ~PHIDecoratedTableItem() {}

    virtual QString listName() const { return tr( "Table" ); }
    virtual QString description() const { return tr( "Decorated table with sortable columns." ); }
    virtual PHIWID wid() const { return DecoratedTable; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/table" ) ); }
    virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
    virtual PHIConfigWidget* ideConfigWidget();
    virtual PHIWID htmlHeaderExtension( const PHIRequest *req, QByteArray &header ) const;
    virtual PHIWID htmlScriptExtension( const PHIRequest *req, QByteArray &script ) const;
    virtual bool hasHtmlExtension() const { return true; }
    virtual bool hasDelimiter() const { return true; }
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;

    inline void setOptions( quint16 o ) { setData( DOptions, o ); updateWidget(); }
    inline quint16 options() const { return PHIBaseItem::data( DOptions, SingleSelection ).value<quint16>(); }
    inline void setOption( quint8 o, bool b=true ) { quint8 opts=options(); b ? opts |= o : opts &= ~o; setData( DOptions, opts ); updateWidget(); }
    inline PHIListChar colTypes() const { return PHIBaseItem::data( DColType ).value<PHIListChar>(); }
    inline void setColTypes( const PHIListChar &l ) { setData( DColType, qVariantFromValue( l ) ); }
    inline PHIListChar colAlignments() const { return PHIBaseItem::data( DColAlignment ).value<PHIListChar>(); }
    inline void setColAlignments( const PHIListChar &l ) { setData( DColAlignment, qVariantFromValue( l ) ); }
    inline PHIListWord colWidths() const { return PHIBaseItem::data( DColWidth ).value<PHIListWord>(); }
    inline void setColWidths( const PHIListWord &l ) { setData( DColWidth, qVariantFromValue( l ) ); }
    inline void setDelimiter( const QString &d ) { setData( DDelimiter, d.toUtf8() ); }
    inline QString realDelimiter() const { return QString::fromUtf8( PHIBaseItem::data( DDelimiter, BL( "\n" ) ).toByteArray() ); }
    inline QString realDateFormat() const { return PHIBaseItem::data( DDateFormat ).toString(); }
    void setDateFormat( const QString &s );

public slots:
    virtual QScriptValue val( const QScriptValue &v=QScriptValue() ) { Q_UNUSED( v ) return QScriptValue( QScriptValue::UndefinedValue ); }
    QScriptValue clear();
    QScriptValue delimiter( const QScriptValue &d=QScriptValue() );
    QScriptValue data( const QScriptValue &d=QScriptValue() );
    QScriptValue rowCount() const;
    QScriptValue hideRow( int row, bool b=true );
    QScriptValue hideCol( int col, bool b=true );
    QScriptValue label( int col, const QScriptValue &v=QScriptValue() );
    QScriptValue cellText( int row, int col, const QScriptValue &v=QScriptValue() );
    QScriptValue cellHtml( int row, int col, const QScriptValue &v=QScriptValue() );
    QScriptValue cellColor( int row, int col, const QScriptValue &v=QScriptValue() );
    QScriptValue cellBgColor( int row, int col, const QScriptValue &v=QScriptValue() );
    QScriptValue checked( const QScriptValue &r, const QScriptValue &v=QScriptValue() );
    QScriptValue dateFormat( const QScriptValue &df=QScriptValue() );
    QScriptValue selected( const QScriptValue &row=QScriptValue() );
    QScriptValue addRow( const QScriptValue &v=QScriptValue(), const QScriptValue &pos=QScriptValue() );

protected slots:
    void slotSectionClicked( int );
    void slotItemSelectionChanged();

protected:
    virtual QSizeF sizeHint( Qt::SizeHint which, const QSizeF &constraint ) const;
    virtual void squeeze();
    virtual void setWidgetText( const QString &t );
    virtual void ideInit();
    virtual void initWidget();
    virtual void updateWidget();
    virtual void cssStatic( const PHIRequest *req, QByteArray &out ) const;
    int logicRow( int row ) const;
};

class PHICheckListItem : public PHIDecoratedTableItem
{
    Q_OBJECT

public:
    explicit PHICheckListItem( const PHIBaseItemPrivate &p ) : PHIDecoratedTableItem( p ) {}
    PHICheckListItem( const PHICheckListItem &it ) : PHIDecoratedTableItem( it ) {}
    virtual ~PHICheckListItem() {}

    virtual QString listName() const { return tr( "Check list" ); }
    virtual QString description() const { return tr( "Checkable list with multiselection." ); }
    virtual PHIWID wid() const { return CheckList; }
    virtual QPixmap pixmap() const { return QPixmap( QLatin1String( ":/items/checklist" ) ); }
    virtual void clientPostData( QHttpMultiPart *multiPart ) const;

protected:
    virtual void ideInit();
    //virtual void html( const PHIRequest *req, QByteArray &out, QByteArray &script, const QByteArray &indent ) const;
};

#endif // PHITABLEITEMS_H

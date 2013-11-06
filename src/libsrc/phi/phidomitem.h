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
#ifndef PHIDOMITEM_H
#define PHIDOMITEM_H
#include <QObject>
#include "phibaseitem.h"
#include "phieffect.h"
#include "phiitemstylecss.h"

class PHIDomItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString accessKey READ accessKey WRITE setAccessKey )
    Q_PROPERTY( bool checked READ checked WRITE setChecked )
    Q_PROPERTY( QString delimiter READ delimiter WRITE setDelimiter )
    Q_PROPERTY( int fadeInterval READ fadeInterval WRITE setFadeInterval )
    Q_PROPERTY( int fadeTime READ fadeTime WRITE setFadeTime )
    Q_PROPERTY( QString id READ name )
    Q_PROPERTY( QString imageId READ imageId WRITE setImageId )
    Q_PROPERTY( QString label READ label WRITE setLabel )
    Q_PROPERTY( int maxLength READ maxLength WRITE setMaxLength )
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString parentName READ parentName WRITE setParentName )
    Q_PROPERTY( QStringList pictureBookIds READ pictureBookIds WRITE setPictureBookIds )
    Q_PROPERTY( QStringList properties READ properties )
    Q_PROPERTY( bool readOnly READ readOnly WRITE setReadOnly )
    Q_PROPERTY( QString styleSheet READ styleSheet WRITE setStyleSheet )
    Q_PROPERTY( int tabIndex READ tabIndex WRITE setTabIndex )
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( QString type READ type )
    Q_PROPERTY( QString url READ url WRITE setUrl )
    Q_PROPERTY( int wid READ wid )

public:
    explicit PHIDomItem( PHIBaseItem *it ) : QObject( it ), _it( it )
        { new PHIDomEffect( this ); new PHIItemStyleCSS( this ); }
    virtual ~PHIDomItem() {}
    inline PHIBaseItem* baseItem() const { return _it; }

// scriptable:
public slots:
    inline QString accessKey() const { return _it->property( "_accessKey" ).toString(); }
    inline void setAccessKey( const QString &a ) { _it->setProperty( "_accessKey", a ); }
    inline bool checked() const { return _it->realChecked(); }
    inline void setChecked( bool b ) { _it->setChecked( b ); }
    inline bool disabled() const { return _it->realDisabled(); }
    inline void setDisabled( bool b ) { _it->setDisabled( b ); }
    inline bool readOnly() const { return _it->realReadOnly(); }
    inline void setReadOnly( bool b ) { _it->setReadOnly( b ); }
    inline QString delimiter() const { return _it->property( "_delimiter" ).toString(); }
    inline void setDelimiter( const QString &d ) { _it->setProperty( "_delimiter", d ); }
    inline QString label() const { return _it->property( "_label" ).toString(); }
    inline void setLabel( const QString &l ) { _it->setProperty( "_label", l ); }
    inline QString url() const { return _it->property( "_url" ).toString(); }
    inline void setUrl( const QString &d ) { _it->setProperty( "_url", d ); }
    inline QString name() const { return _it->name(); }
    inline int wid() const { return static_cast<int>(_it->wid()); }
    inline QString type() const { return _it->property( "_type" ).toString(); }
    inline int fadeTime() const { return _it->property( "_fadeTime" ).toInt(); }
    inline void setFadeTime( int ft ) { _it->setProperty( "_fadeTime", ft ); }
    inline int fadeInterval() const { return _it->property( "_fadeInterval" ).toInt(); }
    inline void setFadeInterval( int fi ) { _it->setProperty( "_fadeInterval", fi ); }
    inline int maxLength() const { return _it->property( "_maxLength" ).toInt(); }
    inline void setMaxLength( int ml ) { _it->setProperty( "_maxLength", ml ); }
    inline QString imageId() const { return QString::fromUtf8( _it->imagePath() ); }
    inline void setImageId( const QString &p ) { _it->setImagePath( p.toUtf8() ); }
    inline QString styleSheet() const { return _it->styleSheet(); }
    inline void setStyleSheet( const QString &s ) { _it->setStyleSheet( s ); }
    inline QStringList properties() const { return PHI::properties( this ); }
    inline QString title() const { return _it->realTitle(); }
    inline void setTitle( const QString &t ) { _it->setTitle( t ); }
    inline qint16 tabIndex() const { return _it->realTabIndex(); }
    inline void setTabIndex( int i ) { _it->setTabIndex( static_cast<qint16>(i) ); }
    inline QString parentName() const { return QString::fromLatin1( _it->parentId() ); }
    inline void setParentName( const QString &p ) { _it->setParentId( p ); }
    QStringList pictureBookIds() const;
    void setPictureBookIds( const QStringList &l );

    // @todo: implement event handlers
    // void blur();
    // void click();
    // void focus();

private:
    PHIBaseItem *_it;
};

inline QStringList PHIDomItem::pictureBookIds() const
{
    QStringList list;
    foreach( const QByteArray arr, _it->imagePathes() ) list.append( QString::fromUtf8( arr ) );
    return list;
}

inline void PHIDomItem::setPictureBookIds( const QStringList &l )
{
    PHIByteArrayList ids;
    foreach( QString id, l ) ids.append( id.toUtf8() );
    _it->setImagePathes( ids );
}

#endif // PHIDOMITEM_H

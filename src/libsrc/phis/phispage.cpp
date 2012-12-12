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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phispage.h"
#include "phi.h"

PHISPageMenuEntry::PHISPageMenuEntry()
    : PHIPageMenuEntry(), _textData( 0 )
{
    qDebug( "PHISPageMenuEntry::PHISPageMenuEntry()" );
    _textData=new PHITextData();
}

PHISPageMenuEntry::PHISPageMenuEntry( const QString &id, const QString &parent,
    const QPixmap &pix, Options options, const PHITextData *data )
    : PHIPageMenuEntry(), _textData( 0 )
{
    qDebug( "PHISPageMenuEntry::PHISPageMenuEntry()" );
    _textData=new PHITextData();
    _id=id.toUtf8();
    _parent=parent.toUtf8();
    _img=pix.toImage();
    _options=options;
    *_textData=*data;
}

PHISPageMenuEntry::PHISPageMenuEntry( const PHISPageMenuEntry &e )
    : PHIPageMenuEntry(), _textData( 0 )
{
    _textData=new PHITextData();
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _options=e._options;
    *_textData=*e._textData;
}

PHISPageMenuEntry& PHISPageMenuEntry::operator =( const PHISPageMenuEntry &e )
{
    *_textData=*e._textData;
    _id=e._id;
    _parent=e._parent;
    _img=e._img;
    _options=e._options;
    return *this;
}

PHISPageMenuEntry::~PHISPageMenuEntry()
{
    delete _textData;
    qDebug( "PHISPageMenuEntry::~PHISPageMenuEntry()" );
}

QDataStream& operator<<( QDataStream &out, const PHISPageMenuEntry &m )
{
    out << m._id << m._parent << m._textData << m._img << static_cast<quint8>(m._options);
    return out;
}

QDataStream& operator>>( QDataStream &in , PHISPageMenuEntry &m )
{
    quint8 opt;
    in >> m._id >> m._parent >> m._textData >> m._img >> opt;
    m._options=static_cast<PHIPageMenuEntry::Options>(opt);
    return in;
}

PHISPage::PHISPage() : PHIPage()
{
    qDebug( "PHISPage::PHISPage()" );
    _title=new PHITextData();
    _styleSheet=new PHITextData();
    _author=new PHITextData();
    _version=new PHITextData();
    _company=new PHITextData();
    _copyright=new PHITextData();
    _template=new PHITextData();
    _javascript=new PHITextData();
    _serverscript=new PHITextData();
    _action=new PHITextData();
    _keys=new PHITextData();
    _sessionRedirect=new PHITextData();
    _description=new PHITextData();
    _opengraph=new PHITextData();
    _languages << QString( "C" );
    _sessionTimeout=60;
    _geometry=PHIPage::G4_3;
    _dbPort=3306;
}

PHISPage::~PHISPage()
{
    delete _title;
    delete _styleSheet;
    delete _author;
    delete _version;
    delete _company;
    delete _copyright;
    delete _template;
    delete _javascript;
    delete _serverscript;
    delete _action;
    delete _keys;
    delete _sessionRedirect;
    delete _description;
    delete _opengraph;
    qDebug( "PHISPage::~PHISPage()" );
}

PHISPage::PHISPage( const PHISPage &p )
    : PHIPage( p )
{
    qDebug( "PHISPage::PHISPage( const PHISPage& )" );
    _title=new PHITextData();
    _styleSheet=new PHITextData();
    _author=new PHITextData();
    _version=new PHITextData();
    _company=new PHITextData();
    _copyright=new PHITextData();
    _template=new PHITextData();
    _javascript=new PHITextData();
    _serverscript=new PHITextData();
    _action=new PHITextData();
    _keys=new PHITextData();
    _sessionRedirect=new PHITextData();
    _description=new PHITextData();
    _opengraph=new PHITextData();

    *_title=*p._title;
    *_styleSheet=*p._styleSheet;
    *_author=*p._author;
    *_version=*p._version;
    *_company=*p._company;
    *_copyright=*p._copyright;
    *_template=*p._template;
    *_javascript=*p._javascript;
    *_serverscript=*p._serverscript;
    *_action=*p._action;
    *_keys=*p._keys;
    *_sessionRedirect=*p._sessionRedirect;
    *_description=*p._description;
    *_opengraph=*p._opengraph;

    _languages=p._languages;
    _sessionTimeout=p._sessionTimeout;
    _geometry=p._geometry;
    _dbName=p._dbName;
    _dbHost=p._dbHost;
    _dbUser=p._dbUser;
    _dbPasswd=p._dbPasswd;
    _dbOptions=p._dbOptions;
    _dbDriver=p._dbDriver;
    _dbPort=p._dbPort;
    _dbFileName=p._dbFileName;

    _dynamicData=p._dynamicData;
    _editorData=p._editorData;
    _eventData=p._eventData;
    _ids=p._ids;
    _colHash=p._colHash;
    _newItemCount=p._newItemCount;
    _menuEntries=p._menuEntries;
}

PHISPage& PHISPage::operator=( const PHISPage &p )
{
    qDebug( "PHISPage::operator=( const PHISPage& )" );
    PHIPage::operator=( p );
    *_title=*p._title;
    *_styleSheet=*p._styleSheet;
    *_author=*p._author;
    *_version=*p._version;
    *_company=*p._company;
    *_copyright=*p._copyright;
    *_template=*p._template;
    *_javascript=*p._javascript;
    *_serverscript=*p._serverscript;
    *_action=*p._action;
    *_keys=*p._keys;
    *_sessionRedirect=*p._sessionRedirect;
    *_description=*p._description;
    *_opengraph=*p._opengraph;

    _languages=p._languages;
    _sessionTimeout=p._sessionTimeout;
    _geometry=p._geometry;
    _dbName=p._dbName;
    _dbHost=p._dbHost;
    _dbUser=p._dbUser;
    _dbPasswd=p._dbPasswd;
    _dbOptions=p._dbOptions;
    _dbDriver=p._dbDriver;
    _dbPort=p._dbPort;
    _dbFileName=p._dbFileName;

    _dynamicData=p._dynamicData;
    _editorData=p._editorData;
    _eventData=p._eventData;
    _ids=p._ids;
    _colHash=p._colHash;
    _newItemCount=p._newItemCount;
    _menuEntries=p._menuEntries;
    return *this;
}

void PHISPage::setDatabase( const PHISDatabase &db )
{
    _dbName=db._name;
    _dbHost=db._host;
    _dbUser=db._user;
    _dbPasswd=db._passwd;
    _dbOptions=db._connectOptions;
    _dbDriver=db._driver;
    _dbPort=db._port;
}

QDataStream& PHISPage::save( QDataStream &out ) const
{
    QByteArray reserved;
    out << dynamic_cast<const PHIPage*>(this);

    _dynamicData.clear();
    QDataStream dsd( &_dynamicData, QIODevice::WriteOnly );
    dsd.setVersion( PHI_DSV );
    dynamicData( dsd );
    out << _dynamicData << _eventData << reserved;

    _editorData.clear();
    QDataStream dse( &_editorData, QIODevice::WriteOnly );
    dse.setVersion( PHI_DSV );
    editorData( dse );
    out << _editorData << reserved << reserved;
    return out;
}

QDataStream& PHISPage::load( QDataStream &in, bool processEditorData )
{
    QByteArray reserved;
    in >> dynamic_cast<PHIPage*>(this) >> _dynamicData >> _eventData >> reserved
       >> _editorData >> reserved >> reserved;

    QDataStream dsd( &_dynamicData, QIODevice::ReadOnly );
    dsd.setVersion( PHI_DSV );
    setDynamicData( dsd );

    if ( processEditorData ) {
        QDataStream dse( &_editorData, QIODevice::ReadOnly );
        dse.setVersion( PHI_DSV );
        setEditorData( dse );
    } else _editorData.clear();
    return in;
}

QDataStream& PHISPage::dynamicData( QDataStream &out ) const
{
    PHITextData txtData;
    if ( _attributes & PHIPage::ADatabase ) {
        if ( _attributes & PHIPage::ADbSettingsFromFile ) out << _dbFileName;
        else out << _dbName << _dbHost << _dbPasswd << _dbUser << _dbDriver << _dbOptions << _dbPort;
    }
    out << _title << _styleSheet << _author << _version << _company << _copyright
        << _template << _javascript << _serverscript << _action << _sessionTimeout
        << _keys << _sessionRedirect << _description << _opengraph << &txtData << &txtData
        << _languages;
    if ( _attributes & PHIPage::AApplication ) out << _menuEntries;
    return out;
}

QDataStream& PHISPage::setDynamicData( QDataStream &in )
{
    PHITextData txtData;
    if ( _attributes & PHIPage::ADatabase ) {
        if ( _attributes & PHIPage::ADbSettingsFromFile ) in >> _dbFileName;
        else in >> _dbName >> _dbHost >> _dbPasswd >> _dbUser >> _dbDriver >> _dbOptions >> _dbPort;
    }
    in >> _title >> _styleSheet >> _author >> _version >> _company >> _copyright
       >> _template >> _javascript >> _serverscript >> _action >> _sessionTimeout
       >> _keys >> _sessionRedirect >> _description >> _opengraph >> &txtData >> &txtData
       >> _languages;
    if ( _attributes & PHIPage::AApplication ) in >> _menuEntries;
    return in;
}

QDataStream& PHISPage::editorData( QDataStream &out ) const
{
    out << static_cast<quint8>(_geometry);
    for ( quint8 i=2; i<28; i=i+5 ) { // 100% colors
        out << _colHash.value( i );
        qDebug( "Storing col ( %d, %s )", i, qPrintable( _colHash.value( i ).name() ) );
    }
    return out;
}

QDataStream& PHISPage::setEditorData( QDataStream &in )
{
    quint8 geom;
    in >> geom;
    _geometry=static_cast<Geometry>(geom);
    QColor c;
    _colHash.clear();
    for ( quint8 i=2; i<28; i=i+5 ) { // get 100% colors
        in >> c;
        _colHash.insert( i, c );
        //qDebug( "Inserting col ( %d, %s )", i, qPrintable( c.name() ) );
    }
    return in;
}

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
#
#    Note: The provided classes are based on the Qt example for Context2D.
*/
#include <QVariant>
#include <QScriptContext>
#include "phidomimage.h"

//QScriptValue PHIDomImage::_self;

/*
static QScriptValue Image( QScriptContext *context, QScriptEngine *env )
{
    QScriptValue val = context->thisObject();
    PHIDomImage *image=new PHIDomImage();
    QScriptValue sv=env->newVariant( QVariant::fromValue( image ) );
    sv.setPrototype( PHIDomImage::_self );
    return sv;
}

static QScriptValue width( QScriptContext *context, QScriptEngine* )
{
    QScriptValue val=context->thisObject();
    PHIDomImage *image=qvariant_cast<PHIDomImage*>(val.toVariant());
    if ( image ) return image->width();
    return 0;
}

static QScriptValue height( QScriptContext *context, QScriptEngine* )
{
    QScriptValue val=context->thisObject();
    PHIDomImage *image=qvariant_cast<PHIDomImage*>(val.toVariant());
    if ( image ) return image->height();
    return 0;
}

static QScriptValue setSrc( QScriptContext *context, QScriptEngine *env )
{
    QScriptValue val=context->thisObject();
    QString src=context->argument( 0 ).toString();
    PHIDomImage *image=qvariant_cast<PHIDomImage*>(val.toVariant());
    if ( image ) image->setSrc( src );
    return env->undefinedValue();
}

static QScriptValue name( QScriptContext *context, QScriptEngine* )
{
    QScriptValue val=context->thisObject();
    PHIDomImage *image=qvariant_cast<PHIDomImage*>(val.toVariant());
    if ( image ) return image->name();
    return QString();
}

void PHIDomImage::setup( QScriptEngine *e )
{
    qRegisterMetaType<PHIDomImage>();
    e->globalObject().setProperty( "Image",e->newFunction(::Image, 0) );
    _self = e->newObject();
    _self.setProperty( "setSrc", e->newFunction( &::setSrc, 1) );
    _self.setProperty( "width", e->newFunction( &::width ) );
    _self.setProperty( "height", e->newFunction( &::height ) );
    _self.setProperty( "name", e->newFunction( &::name ) );
    e->setDefaultPrototype( qMetaTypeId<PHIDomImage>(), _self );
}
*/

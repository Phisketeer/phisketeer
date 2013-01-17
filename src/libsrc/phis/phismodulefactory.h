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
#ifndef PHISMODULEFACTORY_H
#define PHISMODULEFACTORY_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QReadWriteLock>

class PHISInterface;
class PHISModule;

class PHISModuleFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHISModuleFactory )

public:
    static PHISModuleFactory* instance( QObject *parent=0 );
    inline QStringList keys() const { return _keys; }
    inline void lock() { _lock.lockForRead(); }
    inline void unlock() { _lock.unlock(); }
    inline PHISModule* module( const QString &key  ) const { return _modules.value( key, 0 ); }
    void invalidate();

protected:
    explicit PHISModuleFactory( QObject *parent=0 );
    virtual ~PHISModuleFactory();
    
private:
    static PHISModuleFactory *_instance;
    QStringList _keys;
    QHash <QString, PHISModule*> _modules;
    QReadWriteLock _lock;
};

inline PHISModuleFactory* PHISModuleFactory::instance( QObject *parent )
{
    if ( _instance ) return _instance;
    return _instance=new PHISModuleFactory( parent );
}

#endif // PHISMODULEFACTORY_H

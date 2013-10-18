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
#ifndef PHISPARENT_H
#define PHISPARENT_H

#include <QObject>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QHash>
#include "phis.h"
#include "phi.h"

class PHISEXPORT PHISParent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHISParent )

public:
    static PHISParent* instance( QObject *parent=0, const QString &name=PHI::defaultString() );
    virtual ~PHISParent();
    QString tempDir( const QString &domain );
    QString documentRoot( const QString &domain );
    void invalidate( const QString &domain=QString() );
    QVariant value( const QString &key ) const;
    inline QStringList loadedModules() const { QReadLocker l( &_lock ); return _loadedModules; }
    inline QStringList moduleLoadErrors() const { QReadLocker l( &_lock ); return _moduleLoadErrors; }
    inline QString index() const { QReadLocker lock( &_lock ); return _index; }
    inline qint32 keepAlive() const { QReadLocker lock( &_lock ); return _keepAlive; }
    inline QString admin() const { QReadLocker lock( &_lock ); return _admin; }
    inline const QString& name() const { return _name; }

protected:
    PHISParent( QObject *parent, const QString &name );
    void clearTmpDir( const QString &tmp );

protected slots:
    void initListener();

private:
    static PHISParent *_instance;
    mutable QReadWriteLock _lock;
    const QString _name;
    QHash <QString, QString> _tmpDirs;
    QHash <QString, QString> _docRootDirs;
    QDateTime _invalidateTouch;
    QStringList _loadedModules, _moduleLoadErrors;
    QString _admin, _index;
    qint32 _keepAlive;
};

inline PHISParent* PHISParent::instance( QObject *parent, const QString &name )
{
    if ( _instance ) return _instance;
    Q_ASSERT( parent );
    Q_ASSERT( !name.isEmpty() );
    _instance=new PHISParent( parent, name );
    return _instance;
}

#endif // PHISPARENT_H

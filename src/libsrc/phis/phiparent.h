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
#ifndef PHIPARENT_H
#define PHIPARENT_H

#include <QObject>
#include <QApplication>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QDir>
#include <QHash>
#include <QDateTime>
#include <QVector>
//#include "philicense.h"
#include "phis.h"

/** Parent class of all classes with long term lifetime over threads.
  * Use this class as parent for other classes and protect them by a mutex for thread safety. */
class PHISEXPORT PHIParent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIParent )

public:
    static PHIParent* instance( QObject *parent=0 );
    virtual ~PHIParent();
    inline bool isApacheModule() const { return _internalApp; }
    QString tempDir( const QString &domain, const QString &def=QString( "default" ) );

    //void database( QString &db, QString &name, QString &host, QString &user,
    //    QString &pwd, QString &connOpts, int &port, const QString &domain );

    void invalidate( const QString &domain=QString() );
/*
    void readLicenseFile( const QString &documentRoot, const QString &domain );

    inline bool hasValidLicense( const QString &domain ) const
        { QReadLocker l( &_lock ); return _validLicenses.value( domain, false ); }
    inline const PHILicense* license( const QString &domain ) const
        { QReadLocker l( &_lock ); return _licenses.value( domain, 0 ); }
*/

protected:
    PHIParent( QObject *parent=0 );
    void clearTmpDir( const QString &tmp );

    class DBSettings {
    public:
        QString _db, _host, _name, _user, _passwd, _opts;
        int _port;
    };


private:
    static PHIParent *_instance;
    mutable QReadWriteLock _lock;
    QApplication *_app;
    bool _internalApp;
    //QHash <QString, bool> _validLicenses;
    QHash <QString, QString> _tmpDirs;
    QHash <QString, DBSettings> _dbSettings;
    QDateTime _invalidateTouch;
    //QHash <QString, PHILicense*> _licenses;
/*
    QStringList _argList;
    int _argc;
    QVector<char *> _argv;
    QList<QByteArray> _argvData;
*/
};

#endif // PHIPARENT_H

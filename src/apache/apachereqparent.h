/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef APACHEREQPARENT_H
#define APACHEREQPARENT_H
#include <QObject>
#include <QReadWriteLock>
#include <QStringList>
#include <QHash>

class ApacheReqParent : public QObject
{
    Q_OBJECT

public:
    virtual ~ApacheReqParent();
    static ApacheReqParent* instance();
    inline QStringList initErrors() const { return _errors; }
    QStringList moduleLoadErrors() const;
    QStringList loadedModules() const;
    QString tempDir( const QString &domain );

protected:
    explicit ApacheReqParent( QObject *parent=0 );

private:
    static ApacheReqParent *_instance;
    mutable QReadWriteLock _lock;
    QHash <QString, QString> _tmpDirs;
    QStringList _errors;
};

inline ApacheReqParent* ApacheReqParent::instance()
{
    if ( !_instance ) _instance=new ApacheReqParent();
    return _instance;
}

class ApacheReqChild : public QObject
{
    Q_OBJECT

public:
    virtual ~ApacheReqChild();
    static ApacheReqChild* instance();
    inline QStringList initErrors() const { return _errors; }

protected:
    explicit ApacheReqChild( QObject *parent=0 );

private:
    static ApacheReqChild *_instance;
    QStringList _errors;
};

inline ApacheReqChild* ApacheReqChild::instance()
{
    if ( Q_UNLIKELY( !_instance ) ) _instance=new ApacheReqChild();
    return _instance;
}

#endif // APACHEREQPARENT_H

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
#ifndef PHISMODULE_H
#define PHISMODULE_H
#include <QObject>
#include <QScriptEngine>
#include <QStringList>

class QSqlDatabase;
class PHISRequest;

#ifdef PHISLIB
#define PHISEXPORT Q_DECL_EXPORT
#else
#define PHISEXPORT Q_DECL_IMPORT
#endif

class PHISEXPORT PHISInterface
{
    friend class PHIProcessor;

protected:
    explicit PHISInterface( PHISRequest*, QScriptEngine* );
    virtual ~PHISInterface();

private:
    PHISRequest *_req;
    QScriptEngine *_engine;

public:
    inline QScriptEngine* scriptEngine() const { return _engine; }
};

class PHISModuleIF
{
public:
    virtual QObject* create( const QString &key, PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

Q_DECLARE_INTERFACE( PHISModuleIF, "org.phisketeer.phis.module/1.0" )

class PHISEXPORT PHISModule : public QObject, PHISModuleIF
{
    Q_OBJECT
    Q_INTERFACES( PHISModuleIF )

public:
    virtual QObject* create( const QString &key, PHISInterface* ) const=0;
    virtual QStringList keys() const=0;
};

#endif // PHISMODULE_H

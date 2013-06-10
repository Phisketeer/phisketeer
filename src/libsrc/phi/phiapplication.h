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
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIAPPLICATION_H
#define PHIAPPLICATION_H

#include <QApplication>
#include <QSettings>
#include "phi.h"

class PHIPrivateApplication : public QApplication
{
    Q_OBJECT

public:
    explicit PHIPrivateApplication( int &argc, char **argv );
    //virtual ~PHIPrivateApplication();

signals:
    void openFileRequest( const QString& );

protected:
    virtual bool event( QEvent* );
};

class PHIEXPORT PHIApplication : public QObject
{
    Q_OBJECT

public:
    explicit PHIApplication( int &argc, char **argv, const char *name, const char *version );
    virtual ~PHIApplication();
    inline const QString& pluginsPath() const { return _pluginsPath; }
    inline const QString& rootPath() const { return _rootPath; }
    inline const QString& binPath() const { return _binPath; }
    inline const QString& libPath() const { return _libPath; }
    inline const QString& modulesPath() const { return _modulesPath; }
    inline const QString& itemsPath() const { return _itemsPath; }
    inline const QString& tsPath() const { return _tsPath; }
    inline const QString& serverBin() const { return _serverBin; }
    inline QSettings* settings() const { return _settings; }
    inline QGuiApplication* qapp() const { return _app; }
    inline int exec() { return _app->exec(); }
    bool stopPhisService();
    bool startPhisService();
    int checkPhisService();
    bool clearPhisServiceCache();
    static PHIApplication* instance() { return _instance; }

public slots:
    void loadTranslations();
    inline void quit() { _app->quit(); }

signals:
    void openFileRequest( const QString& );

private:
    static PHIApplication *_instance;
    QGuiApplication *_app;
    QSettings *_settings;
    QString _pluginsPath, _rootPath, _binPath, _libPath, _modulesPath;
    QString _itemsPath, _tsPath, _serverBin;
};

#endif // PHIAPPLICATION_H

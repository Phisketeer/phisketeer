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

#define phiApp PHIApplication::instance()

class PHIPrivateApplication : public QApplication
{
    Q_OBJECT

public:
    explicit PHIPrivateApplication( int &argc, char **argv );
    virtual ~PHIPrivateApplication();

signals:
    void openFileRequest( QString );

protected:
    virtual bool event( QEvent* );
};

class PHIEXPORT PHIApplication : public QObject
{
    Q_OBJECT

public:
    enum Type { App, ApacheModule, Service };
    explicit PHIApplication( int &argc, char **argv, const char *name, const char *version, Type type=App );
    virtual ~PHIApplication();

    inline const QString& pluginsPath() const { return _pluginsPath; }
    inline const QString& rootPath() const { return _rootPath; }
    inline const QString& binPath() const { return _binPath; }
    inline const QString& libPath() const { return _libPath; }
    inline const QString& modulesPath() const { return _modulesPath; }
    inline const QString& itemsPath() const { return _itemsPath; }
    inline const QString& tsPath() const { return _tsPath; }
    inline const QString& serverBin() const { return _serverBin; }
    inline const QString& appBin() const { return _appBin; }
    inline const QString& tmpPath() const { return _tmpPath; }
    inline const QString& cachePath() const { return _cachePath; }
    inline const QString& appLanguage() const { return _lang; }
    inline const QString& userDocPath() const { return _usrDocPath; }
    inline const QString& dataPath() const { return _dataPath; }
    inline QSettings* serverSettings() const { return _serverSettings; }
    inline QSettings* settings() const { return _settings; }
    inline int exec() { return _app->exec(); }
    inline QString applicationName() const { return _app->applicationName(); }
    bool stopPhisService();
    bool startPhisService();
    int checkPhisService();
    bool clearPhisServiceCache();
    void setupApplication( QGuiApplication *app, const QString &domain, const QString &org, const char *version );
    static PHIApplication* instance() { return _instance; }

public slots:
    void loadTranslations();
    inline void quit() { _app->quit(); }
    void invalidate();

signals:
    void openFileRequest( QString );

private:
    static PHIApplication *_instance;
    QGuiApplication *_app;
    QSettings *_settings, *_serverSettings;
    QString _pluginsPath, _rootPath, _binPath, _libPath, _modulesPath, _lang;
    QString _itemsPath, _tsPath, _serverBin, _cachePath, _tmpPath, _appBin;
    QString _usrDocPath, _dataPath;
};

#endif // PHIAPPLICATION_H

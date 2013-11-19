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
#ifndef PHISPROCESSOR_H
#define PHISPROCESSOR_H
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QFileInfo>
#include <QDir>
#include "phirequest.h"
#include "phis.h"

class PHIBasePage;
class PHIBaseItem;
class QFile;

class PHISEXPORT PHISProcessor
{
    Q_DISABLE_COPY( PHISProcessor )
    Q_DECLARE_TR_FUNCTIONS( PHISProcessor )

public:
    explicit PHISProcessor( const PHIRequest *req, int dbConnId )
        : _req( req ), _dbConnId( dbConnId ) {}
    void run();
    QString resolveRelativeFile( const QString &file ) const;

protected:
    PHIBasePage* loadPage( QFile &file );
    void initDb( const PHIBasePage *page );
    void parseMaster( PHIBasePage *master, PHIBasePage *page ) const;
    // returns true if user has set own content during script execution:
    bool runScript( const PHIBasePage *master, PHIBasePage *page ) const;
    void createJS( const QString &name ) const;
    void createUiCSS() const;
    void genSysItem() const;
    void genImage() const;
    void genCSS() const;
    void genJS() const;
    void genScripts() const;

private:
    const PHIRequest *_req;
    const int _dbConnId;
    QSqlDatabase _db;
};

inline QString PHISProcessor::resolveRelativeFile( const QString &file ) const
{
    if ( Q_UNLIKELY( file.isEmpty() ) ) return file;
    QString tmp=QDir::fromNativeSeparators( file );
    if ( Q_LIKELY( tmp.startsWith( QLatin1Char( '/' ) ) ) )
        return _req->documentRoot()+file;
    QFileInfo fi( _req->canonicalFilename() );
    return fi.canonicalPath()+QDir::separator()+file;
}

#endif // PHISPROCESSOR_H

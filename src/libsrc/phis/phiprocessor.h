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
#ifndef PHIPROCESSOR_H
#define PHIPROCESSOR_H

#include <QByteArray>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTransform>
#include <QImage>
#include <QSqlDatabase>
#include "phiresponserec.h"
#include "phispage.h"
#include "phibasepage.h"
#include "phibaseitem.h"
#include "phisrequest.h"

class PHISEXPORT PHIProcessor
{
    Q_DISABLE_COPY( PHIProcessor )

public:
    explicit PHIProcessor( const PHISRequest *req, int connId=0 );
    ~PHIProcessor();
    void run();

protected:
    QByteArray parse( const PHISPage* const );
    void loadPage( PHISPage *&p, QFile *f, bool isMasterPage );
    bool runScriptEngine( PHIBasePage *page, const QString &script );
    void genImage() const;
    PHISPage* loadMasterPage( const QString &filename );
    void createSystemCSS( const PHIBasePage* const ) const;
    void createJS( const QString &name ) const;
    QByteArray createjQueryTheme( const PHIBasePage* const ) const;
    void genSysItem() const;
    void genCSS() const;
    void genJS() const;
    void genLicense() const;

private:
    const PHISRequest *_req;
    PHIResponseRec *_resp;
    QSqlDatabase _db;
    int _connectionId;
};

#endif // PHIPROCESSOR_H

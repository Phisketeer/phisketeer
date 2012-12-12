/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
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
#ifndef PHIAFORMPROCESSOR_H
#define PHIAFORMPROCESSOR_H

#include <QObject>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QByteArray>
#include "phiawebview.h"
#include "phiapage.h"
#include "phiaitem.h"

class PHIAFormProcessor : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAFormProcessor )

public:
    explicit PHIAFormProcessor( PHIAWebView *parent );
    virtual ~PHIAFormProcessor();
    QNetworkReply* process( const PHIAPage*, const QString &submitbuttonid );

protected slots:
    void slotReplyFinished();

protected:
    void addPHIAItem( const PHIAItem* );

private:
    QTemporaryFile *_tmpFile;
    QByteArray _disposition, _boundary;
};

#endif // PHIAFORMPROCESSOR_H

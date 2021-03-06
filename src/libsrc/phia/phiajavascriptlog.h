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
#ifndef PHIAJAVASCRIPTLOG_H
#define PHIAJAVASCRIPTLOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include "phia.h"

class PHIAEXPORT PHIAJavaScriptLog : public QDialog
{
    Q_OBJECT

public:
    explicit PHIAJavaScriptLog( QWidget *parent=0 );
    virtual ~PHIAJavaScriptLog();
        
public slots:
    void slotJavaScriptConsoleMessage( const QString &msg, int line, const QString &id );

protected:
    void showEvent( QShowEvent *e );
    void closeEvent( QCloseEvent *e );

private:
    QPlainTextEdit *_logger;
};

#endif // PHIAJAVASCRIPTLOG_H

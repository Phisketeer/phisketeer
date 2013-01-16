/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SCONFIG_H
#define SCONFIG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "ui_sconfigdlg.h"

class SConfig : public QDialog, private Ui::SConfigDlg
{
    Q_OBJECT

public:
    explicit SConfig( QWidget *parent=0 );
    virtual ~SConfig();

protected slots:
    virtual void accept();
    void slotInit();
    void on__start_clicked();
    void on__clearCache_clicked();
    void on__stop_clicked();
    void on__baseDirTool_clicked();
    void on__mimeTool_clicked();
    void on__virtualTree_itemClicked( QTreeWidgetItem*, int col );
    void on__addTool_clicked();
    void on__delTool_clicked();
    void on__logDirTool_clicked();
    void on__sslKeyTool_clicked();
    void on__sslCertTool_clicked();
    void on__tempTool_clicked();
    void on__rootTool_clicked();
    void slotCheckService();

protected:
    void save();

private:
    QString _bin, _base;
};

#endif // SCONFIG_H

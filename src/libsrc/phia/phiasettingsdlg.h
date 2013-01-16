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
#ifndef PHIASETTINGSDLG_H
#define PHIASETTINGSDLG_H

#include <QDialog>
#include "ui_settingsdlg.h"
#include "phiaconfig.h"
#include "phia.h"

class PHIAEXPORT PHIASettingsDlg : public QDialog, private Ui::SettingsDlg
{
    Q_OBJECT

public:
    explicit PHIASettingsDlg( QWidget *parent=0 );
    virtual ~PHIASettingsDlg();

protected slots:
    void on__addLang_clicked();
    void on__delLang_clicked();
    void on__clearCache_clicked();
    void on__loadCacheDir_clicked();
    void on__passwd_textEdited( const QString& );
    void on__verify_textEdited( const QString& );
    void on__addAddress_clicked();
    void on__delAddress_clicked();
    void on__chgAddress_clicked();
    void on__addContact_clicked();
    void on__delContact_clicked();
    void on__chgContact_clicked();
    void on__addCard_clicked();
    void on__delCard_clicked();
    void on__chgCard_clicked();
    void on__loadDownloadDir_clicked();
    void on__checkNow_clicked();
    virtual void accept();

protected:
    PHIAAddress itemToAddress( const QTreeWidgetItem *it );
    PHIAContact itemToContact( const QTreeWidgetItem *it );
    PHIACreditCard itemToCreditCard( const QTreeWidgetItem *it );
    void setupItem( QTreeWidgetItem *it, const PHIAAddress &a );
    void setupItem( QTreeWidgetItem *it, const PHIAContact &c );
    void setupItem( QTreeWidgetItem *it, const PHIACreditCard &c );

protected:
    bool _passwdChanged;
};

#endif // PHIASETTINGSDLG_H

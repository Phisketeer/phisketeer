/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#ifndef PHIWIZARD_H
#define PHIWIZARD_H

#include <QWizard>
#include <QWidget>
#include "ui_wizardstart.h"
#include "ui_wizardlicense.h"
#include "ui_wizardroot.h"
#include "ui_wizardcomponents.h"
#include "ui_wizarddata.h"
#include "ui_wizardphis.h"
#include "ui_wizardapache.h"
#include "ui_wizardcommit.h"
#include "ui_wizardinstall.h"

class PHIWizardStart : public QWizardPage, private Ui::WizardStart
{
    Q_OBJECT

public:
    explicit PHIWizardStart( QWidget *parent=0 );
    virtual ~PHIWizardStart();
    virtual void initializePage();
};

class PHIWizardLicense : public QWizardPage, private Ui::WizardLicense
{
    Q_OBJECT

public:
    explicit PHIWizardLicense( QWidget *parent=0 );
    virtual ~PHIWizardLicense();
    virtual void initializePage();
};

class PHIWizardRoot : public QWizardPage, private Ui::WizardRoot
{
    Q_OBJECT

public:
    explicit PHIWizardRoot( QWidget *parent=0 );
    virtual ~PHIWizardRoot();
    virtual bool validatePage();

protected slots:
    void on__browse_clicked();
};

class PHIWizardComponents : public QWizardPage, private Ui::WizardComponents
{
    Q_OBJECT

public:
    explicit PHIWizardComponents( QWidget *parent=0 );
    virtual ~PHIWizardComponents();
    virtual int nextId() const;
};

class PHIWizardData : public QWizardPage, private Ui::WizardData
{
    Q_OBJECT

public:
    explicit PHIWizardData( QWidget *parent=0 );
    virtual ~PHIWizardData();
    virtual int nextId() const;
    virtual bool validatePage();

protected slots:
    void on__browse_clicked();
};

class PHIWizardPhis : public QWizardPage, private Ui::WizardPhis
{
    Q_OBJECT

public:
    explicit PHIWizardPhis( QWidget *parent=0 );
    virtual ~PHIWizardPhis();
    virtual int nextId() const;
};

class PHIWizardApache : public QWizardPage, private Ui::WizardApache
{
    Q_OBJECT

public:
    explicit PHIWizardApache( QWidget *parent=0 );
    virtual ~PHIWizardApache();
    virtual int nextId() const;

protected slots:
    void on__browse_clicked();
};

class PHIWizardCommit : public QWizardPage, private Ui::WizardCommit
{
    Q_OBJECT

public:
    explicit PHIWizardCommit( QWidget *parent=0 );
    virtual ~PHIWizardCommit();
    virtual void initializePage();
};

class PHIWizardInstall : public QWizardPage, private Ui::WizardInstall
{
    Q_OBJECT

public:
    explicit PHIWizardInstall( QWidget *parent=0 );
    virtual ~PHIWizardInstall();
    virtual void initializePage();
    virtual bool validatePage();

protected slots:
    void init();

protected:
    void install();
    void finish( bool error=false );
};

class PHIWizard : public QWizard
{
    Q_OBJECT

public:
    enum Page { Start=0, License=1, Root=2, Components=3, Data=4, Phis=5, Apache=6,
        Commit=7, Install=8, Final=9 };
    explicit PHIWizard( QWidget *parent=0 );
    virtual ~PHIWizard();

protected slots:
    virtual void reject();
};

#endif // PHIWIZARD_H

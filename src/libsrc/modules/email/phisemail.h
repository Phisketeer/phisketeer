#ifndef PHISEMAIL_H
#define PHISEMAIL_H
#include "phismodule.h"

class PHISEmail : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text )

public:
    PHISEmail();

public slots:
    inline QString text() { return QString( "testi" ); }
};

class PHISEmailModule : public PHISModule
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.phisketeer.phis.module.email" FILE "phisemail.json")

public:
    PHISEmailModule();
    virtual QObject* create( const QString &key, PHISInterface* ) const;
    virtual QStringList keys() const;
};


#endif // PHISEMAIL_H

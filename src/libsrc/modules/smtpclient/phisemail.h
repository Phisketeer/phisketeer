#ifndef PHISEMAIL_H
#define PHISEMAIL_H

#include "phismodule.h"

class PHISEmail : public PHISInterface, PHISModule
{
    Q_OBJECT
    Q_INTERFACES( PHISModule )

    Q_PROPERTY( int testi READ testi )

public:
    PHISEmail();
    inline virtual QString pluginName() const { return QString( "email" ); }

public slots:
    inline int testi() const { return 2; }
};

#endif // PHISEMAIL_H

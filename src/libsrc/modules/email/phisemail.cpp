#include "phisemail.h"

PHISEmail::PHISEmail()
{
    setObjectName( QStringLiteral( "email" ) );
}



PHISEmailModule::PHISEmailModule()
{
    qWarning( "PHISEmailModule::PHISEmailModule()" );
}

QObject* PHISEmailModule::create( const QString &key, PHISInterface* ) const
{
    if ( key==QStringLiteral( "email" ) ) return new PHISEmail();
    return 0;
}

QStringList PHISEmailModule::keys() const
{
    return QStringList() << QStringLiteral( "email" );
}

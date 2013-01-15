#include <QScriptEngine>
#include "phismodule.h"
#include "phisrequest.h"

PHISInterface::PHISInterface()
    : QObject( 0 )
{
    qWarning( "PHISInterface::PHISInterface()" );
}

PHISInterface::~PHISInterface()
{
    qWarning( "PHISInterface::~PHISInterface()" );
}

/*
PHISModule::PHISModule( PHISInterface* )
{
    qWarning( "PHISModule::PHISModule()" );
}

PHISModule::~PHISModule()
{
    qWarning( "PHISModule::~PHISModule()" );
}
*/

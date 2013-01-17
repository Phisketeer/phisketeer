#include <QScriptEngine>
#include "phismodule.h"
#include "phisrequest.h"

PHISInterface::PHISInterface( PHISRequest *req, QScriptEngine *engine )
    : _req( req ), _engine( engine )
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

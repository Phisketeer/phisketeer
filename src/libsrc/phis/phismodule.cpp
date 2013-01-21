#include <QScriptEngine>
#include "phismodule.h"
#include "phierror.h"

PHISInterface::PHISInterface( const PHISRequest *req, QScriptEngine *engine )
    : QObject( engine ), _req( req )
{
    qDebug( "PHISInterface::PHISInterface()" );
}

PHISInterface::~PHISInterface()
{
    qDebug( "PHISInterface::~PHISInterface()" );
}

void PHISInterface::log( LogType lt, const char *file, int line, const QDateTime &dt, const QString &m )
{
    switch( lt ) {
    case LTWarning: return _req->responseRec()->log( 0x04, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTCritical: return _req->responseRec()->log( 0x08, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTTrace: return _req->responseRec()->log( 0x01, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTDebug: return _req->responseRec()->log( 0x10, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTUser: return _req->responseRec()->log( 0x20, file, line, dt, PHIRC_MODULE_LOG, m );
    }
}

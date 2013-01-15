#ifndef PHISMODULE_H
#define PHISMODULE_H
#include <QtCore>
#include <QScriptEngine>

#define PHISMODULE Q_OBJECT; Q_INTERFACES( PHISModule )

class QSqlDatabase;
class PHISRequest;

#ifdef PHISLIB
#define PHISEXPORT Q_DECL_EXPORT
#else
#define PHISEXPORT Q_DECL_IMPORT
#endif

class PHISEXPORT PHISInterface : public QObject
{
    Q_OBJECT
    friend class PHIProcessor;

protected:
    explicit PHISInterface();
    virtual ~PHISInterface();

private:
    PHISRequest *_req;
    QSqlDatabase *_db;
    QScriptEngine *_engine;

    inline void setScriptEngine( QScriptEngine *engine ) { _engine=engine; }
    inline void setRequest( PHISRequest *req ) { _req=req; }

public:
    inline QScriptEngine* scriptEngine() const { return _engine; }
};

class PHISModule
{
public:
    //explicit PHISModule( PHISInterface* );
    //virtual ~PHISModule();
    virtual QString pluginName() const=0;
};

Q_DECLARE_INTERFACE( PHISModule, "org.phisketeer.phis.phismodule/1.0" )

#endif // PHISMODULE_H

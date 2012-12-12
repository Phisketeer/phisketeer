#include "phiservice.h"
#include <Cocoa/Cocoa.h>

int main( int argc, char **argv )
{
    PHIService service( argc, argv, "phis" );

    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    return service.exec();
}

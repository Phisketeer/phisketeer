/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phismodule.h"

void PHISInterface::log( LogType lt, const char *file, int line, const QDateTime &dt, const QString &m ) const
{
    switch( lt ) {
    case LTError: return _req->responseRec()->log( 0x02, file , line, dt, PHIRC_MODULE_LOG, m );
    case LTWarning: return _req->responseRec()->log( 0x04, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTCritical: return _req->responseRec()->log( 0x08, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTTrace: return _req->responseRec()->log( 0x01, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTDebug: return _req->responseRec()->log( 0x10, file, line, dt, PHIRC_MODULE_LOG, m );
    case LTUser: return _req->responseRec()->log( 0x20, file, line, dt, PHIRC_MODULE_LOG, m );
    }
}

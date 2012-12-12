/*
#    Copyright (C) 2008  Trolltech AS
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIMEMROTATE_H
#define PHIMEMROTATE_H
#include "qglobal.h"

QT_BEGIN_NAMESPACE

#define QT_ROTATION_CACHEDREAD 1
#define QT_ROTATION_CACHEDWRITE 2
#define QT_ROTATION_PACKING 3
#define QT_ROTATION_TILED 4

#ifndef QT_ROTATION_ALGORITHM
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
#define QT_ROTATION_ALGORITHM QT_ROTATION_TILED
#else
#define QT_ROTATION_ALGORITHM QT_ROTATION_CACHEDREAD
#endif
#endif

#define QT_DECL_MEMROTATE( srctype, desttype )                              \
    void phi_memrotate90( const srctype*, int, int, int, desttype*, int );  \
    void phi_memrotate180( const srctype*, int, int, int, desttype*, int ); \
    void phi_memrotate270( const srctype*, int, int, int, desttype*, int )

void phi_memrotate90( const quint32*, int, int, int, quint32*, int );

QT_DECL_MEMROTATE( quint32, quint32 );
QT_DECL_MEMROTATE( quint32, quint16 );
QT_DECL_MEMROTATE( quint16, quint32 );
QT_DECL_MEMROTATE( quint16, quint16 );
//QT_DECL_MEMROTATE( quint24, quint24 );
//QT_DECL_MEMROTATE( quint32, quint24 );
//QT_DECL_MEMROTATE( quint32, quint18 );
QT_DECL_MEMROTATE( quint32, quint8 );
QT_DECL_MEMROTATE( quint16, quint8 );
//QT_DECL_MEMROTATE( qrgb444, quint8 );
QT_DECL_MEMROTATE( quint8, quint8 );

#ifdef QT_QWS_ROTATE_BGR
QT_DECL_MEMROTATE( quint16, qbgr565 );
QT_DECL_MEMROTATE( quint32, qbgr565 );
QT_DECL_MEMROTATE( qrgb555, qbgr555 );
QT_DECL_MEMROTATE( quint32, qbgr555 );
#endif

#ifdef QT_QWS_DEPTH_GENERIC
QT_DECL_MEMROTATE( quint32, qrgb_generic16 );
QT_DECL_MEMROTATE( quint16, qrgb_generic16 );
#endif

#undef QT_DECL_MEMROTATE

QT_END_NAMESPACE

#endif // PHIMEMROTATE_H

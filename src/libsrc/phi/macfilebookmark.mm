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
#import <Foundation/Foundation.h>
#import <Foundation/NSAutoreleasePool.h>
#import <QFileInfo>
#import <QSettings>
#import <QStandardPaths>
#import "macfilebookmark.h"

QMutex PHISecDirPool::_mutex;

PHISecDirPool::PHISecDirPool()
{
    qDebug( "PHISecDirPool::PHISecDirPool()" );
}

PHISecDirPool::~PHISecDirPool()
{
    qDebug( "PHISecDirPool::~PHISecDirPool()" );
}

void PHISecDirPool::loadSecurityBookmarkPool()
{
    @autoreleasepool {
        _mutex.lock();
        QSettings s( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation )
            +QLatin1String( "/phis/secbookmarks" ), QSettings::IniFormat );
        const QStringList keys=s.allKeys();
        foreach ( QString key, keys ) {
            QByteArray arr=s.value( key ).toByteArray();
            qDebug( "Loading security bookmark %s (%d)", qPrintable( key ), arr.length() );
            NSData* bookmark=[NSData dataWithBytes:arr.constData() length:arr.length()];
            // NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
            BOOL stale;
            NSError* err=nil;
            // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
            NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
                options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
            if ( err!=nil ) {
                qWarning( "PHISecDirPool(): Cannot resolve security scoped bookmark: %s (%s)",
                          [[err localizedDescription] UTF8String], qPrintable( key ) );
                continue;
            }
            if ( [url respondsToSelector:@selector(startAccessingSecurityScopedResource)] ) {
                qDebug( "start accessing security scoped resource: %s (%d)", qPrintable( key ), arr.length() );
                // only supported by 10.7.3 or later
                [url startAccessingSecurityScopedResource]; // start using bookmarked resource
                // we keep it open until process termination...
            }
        }
        _mutex.unlock();
    }
}

bool PHISecDirPool::storeSecurityBookmarkForDir( const QString &dirname )
{
    const QFileInfo fi( dirname );
    const QString dn=fi.isDir() ? fi.filePath() : fi.path();
    @autoreleasepool {
        _mutex.lock();
        QSettings s( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation )
            +QLatin1String( "/phis/secbookmarks" ), QSettings::IniFormat );
        const QStringList keys=s.allKeys();
        QString key=dn;
        key.replace( QLatin1Char( '/' ), QString() );
        if ( keys.contains( key ) ) return true;

        NSString *ns=[NSString stringWithUTF8String:dn.toUtf8().constData()];
        NSURL *url=[NSURL fileURLWithPath:ns isDirectory:true];
        NSError* err=nil;
        // NSURLBookmarkCreationWithSecurityScope = (1UL << 11) // not available before 10.7.3
        NSData* bookmark=[url bookmarkDataWithOptions:(1UL << 11)
            includingResourceValuesForKeys:nil relativeToURL:nil error:&err];
        if ( err!=nil ) {
            qWarning( "PHISecDirPool::storeSecurityBookmarkForDir(): Can not create bookmark: %s (%s)",
                      [[err localizedDescription] UTF8String], qPrintable( dn ) );
            _mutex.unlock();
            return false;
        } else {
            QByteArray arr=QByteArray::fromRawData( (const char*)[bookmark bytes], [bookmark length] );
            s.setValue( key, arr );
            s.sync();
            qDebug( "stored key: %s (%d)", qPrintable( key ), arr.length() );
        }
        _mutex.unlock();
    }
    return true;
}

/*
PHISecFile::PHISecFile( const QString &filename )
    : QFile( filename )
{
    qDebug( "PHISecFile::PHISecFile()" );
    const QString cfp=QFileInfo( filename ).canonicalFilePath();
    @autoreleasepool {
        QString key=cfp;
        key.replace( QLatin1Char( '/' ), QString() );
        _mutex.lock();
        _mutex.unlock();

        // NSString *nscfp=[NSString stringWithUTF8String:cfp.toUtf8().constData()];
        // qDebug( "Resolve with secure file bookmark: %s %s", qPrintable( key ), [nscfp UTF8String] );
        // NSURL *cfpurl=[NSURL fileURLWithPath:nscfp isDirectory:false];

        NSData* bookmark=[NSData dataWithBytes:arr.constData() length:arr.length()];
        // NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
        BOOL stale;
        NSError* err=nil;
        // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
        NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
            options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
        if ( err!=nil ) {
            qWarning( "PHISecFile(): Cannot resolve security scoped bookmark: %s (%s)",
                      [[err localizedDescription] UTF8String], qPrintable( cfp ) );
            return;
        }
        if ( [url respondsToSelector:@selector(startAccessingSecurityScopedResource)] ) {
            qWarning( "PHISecFile(): start accessing security scoped resource: %s (%d)", qPrintable( cfp ), arr.length() );
            // only supported by 10.7.3 or later
            [url startAccessingSecurityScopedResource]; // start using bookmarked resource
        }
    }
}

PHISecFile::~PHISecFile()
{
    const QString cfp=QFileInfo( fileName() ).canonicalFilePath();
    @autoreleasepool {
        QString key=cfp;
        key.replace( QLatin1Char( '/' ), QString() );
        QSettings s( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation )
            +QLatin1String( "/phis/secbookmarks" ), QSettings::IniFormat );
        _mutex.lock();
        QByteArray arr=s.value( key ).toByteArray();
        _mutex.unlock();

        // NSString *fn=[NSString stringWithUTF8String:f.toUtf8().constData()];
        // NSString *key=[@"PHI" stringByAppendingString:fn];
        // qWarning( "Closing with secure file bookmark: %s", [fn UTF8String] );
        // NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
        // NSString *nscfp=[NSString stringWithUTF8String:cfp.toUtf8().constData()];
        // qDebug( "Resolve with secure file bookmark: %s %s", qPrintable( key ), [nscfp UTF8String] );
        // NSURL *cfpurl=[NSURL fileURLWithPath:nscfp isDirectory:false];

        NSData* bookmark=[NSData dataWithBytes:arr.constData() length:arr.length()];
        // NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
        BOOL stale;
        NSError* err=nil;
        // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) // not available before 10.7.3
        NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
            options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
        if ( err!=nil ) {
            qWarning( "~PHISecFile(): Cannot resolve security scoped bookmark: %s (%s)",
                [[err localizedDescription] UTF8String], qPrintable( cfp ) );
            return;
        }
        if ( [url respondsToSelector:@selector(stopAccessingSecurityScopedResource)] ) {
            // only supported by 10.7.3 or later
            // qWarning( "~PHISecFile(): stopp accessing security scoped resource: %s", qPrintable( cfp ) );
            // [url stopAccessingSecurityScopedResource]; // stop using bookmarked resource
        }
    }
    qDebug( "PHISecFile::~PHISecFile()" );
}

bool PHISecFile::storeSecurityBookmark( const QString &filename )
{
    const QFileInfo fi( filename );
    if ( !fi.isDir() ) return false;
    const QString cfp=fi.canonicalFilePath();
    @autoreleasepool {
        NSString *nscfp=[NSString stringWithUTF8String:cfp.toUtf8().constData()];
        qDebug( "Store with secure file bookmark: %s %s", qPrintable( cfp ), [nscfp UTF8String] );
        NSURL *cfpurl=[NSURL fileURLWithPath:nscfp isDirectory:fi.isDir()];
        NSError* err=nil;
        // NSURLBookmarkCreationWithSecurityScope = (1UL << 11) // not available before 10.7.3
        NSData* bookmark=[cfpurl bookmarkDataWithOptions:(1UL << 11)
            includingResourceValuesForKeys:nil relativeToURL:nil error:&err];
        if ( err!=nil ) {
            qWarning( "PHISecFile::storeSecurityBookmark(): Can not create bookmark: %s (%s)",
                      [[err localizedDescription] UTF8String], qPrintable( cfp ) );
            return false;
        } else {
            QString key=cfp;
            key.replace( QLatin1Char( '/' ), QString() );
            //NSString *key=[@"PHI" stringByAppendingString:fn];
            QByteArray arr=QByteArray::fromRawData( (const char*)[bookmark bytes], [bookmark length] );
            QSettings s( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation )
                +QLatin1String( "/phis/secbookmarks" ), QSettings::IniFormat );
            _mutex.lock();
            s.setValue( key, arr );
            s.sync();
            _mutex.unlock();
            qWarning( "PHISecFile::storeSecurityBookmark(): stored key: %s (%d)", qPrintable( key ), arr.length() );
            //[[NSUserDefaults standardUserDefaults] setObject:bookmark forKey:key];
            //[[NSUserDefaults standardUserDefaults] synchronize];
        }
    }
    return true;
}

bool PHISecFile::loadSecurityBookmarkForDir( const QString &filename )
{
    qDebug( "PHISecFile::loadSecurityBookmarkForDir()" );
    QFileInfo fi( filename );
    const QString cfp=fi.isDir() ? fi.canonicalFilePath() : fi.canonicalPath();
    @autoreleasepool {
        QString key=cfp;
        key.replace( QLatin1Char( '/' ), QString() );
        QSettings s( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation )
            +QLatin1String( "/phis/secbookmarks" ), QSettings::IniFormat );
        _mutex.lock();
        QByteArray arr=s.value( key ).toByteArray();
        _mutex.unlock();
        qWarning( "SEC %s %s (%s)", qPrintable( s.fileName() ), qPrintable( cfp ), arr.constData() );

        // NSString *nscfp=[NSString stringWithUTF8String:cfp.toUtf8().constData()];
        // qDebug( "Resolve with secure file bookmark: %s %s", qPrintable( key ), [nscfp UTF8String] );
        // NSURL *cfpurl=[NSURL fileURLWithPath:nscfp isDirectory:false];

        NSData* bookmark=[NSData dataWithBytes:arr.constData() length:arr.length()];
        // NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
        BOOL stale;
        NSError* err=nil;
        // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
        NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
            options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
        if ( err!=nil ) {
            qWarning( "PHISecFile(): Cannot resolve security scoped bookmark: %s (%s)",
                      [[err localizedDescription] UTF8String], qPrintable( cfp ) );
            return false;
        }
        if ( [url respondsToSelector:@selector(startAccessingSecurityScopedResource)] ) {
            qWarning( "PHISecFile(): start accessing security scoped resource: %s (%d)", qPrintable( cfp ), arr.length() );
            // only supported by 10.7.3 or later
            [url startAccessingSecurityScopedResource]; // start using bookmarked resource
            //[url stopAccessingSecurityScopedResource];
            // we keep it open until process termination: the phis service need access in a separate process
        }
    }
    return true;
}

bool PHISecFile::unloadSecurityBookmarkForDir( const QString &filename )
{
    NSString *fn=[NSString stringWithUTF8String:filename.toUtf8().constData()];

    NSString *key=[@"PHI" stringByAppendingString:fn];
    [[NSUserDefaults standardUserDefaults] synchronize];
    qWarning( "Resolve secure file bookmark for directory: %s", [fn UTF8String] );

    NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
    BOOL stale;
    NSError* err=nil;
    // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
    NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
        options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
    if ( err!=nil ) {
        qWarning( "unload(): Cannot resolve bookmark for dir: %s (%s)", qPrintable( filename ),
            [[err localizedDescription] UTF8String] );
        return false;
    }
    if ( [url respondsToSelector:@selector(stopAccessingSecurityScopedResource)] ) {
        // only supported by 10.7.3 or later
        qWarning( "unload(): stopping accessing security scoped resource: %s", qPrintable( filename ) );
        [url stopAccessingSecurityScopedResource]; // stop using bookmarked resource
    }
    qWarning( "Stopped accessing security scoped dir: %s", qPrintable( filename ) );
    return true;
}
*/

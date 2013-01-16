/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
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
#import <Foundation/Foundation.h>
#import <Foundation/NSAutoreleasePool.h>
#import <QFileInfo>
#import "macfilebookmark.h"

PHISecFile::PHISecFile( const QString &fn )
    : QFile( fn )
{
    qDebug( "PHISecFile::PHISecFile()" );
}

PHISecFile::~PHISecFile()
{
    qDebug( "PHISecFile::~PHISecFile()" );
}

bool PHISecFile::open( QIODevice::OpenMode flags )
{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
    QFileInfo f( this->fileName() );
    // is this the correct way to store 'filename' in a NSString?
    NSString *fn=[NSString stringWithUTF8String:f.canonicalFilePath().toUtf8()];

    NSString *key=[@"PHI" stringByAppendingString:fn];
    qDebug( "Open with secure file bookmark: %s", [fn UTF8String] );
    NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
    BOOL stale;
    NSError* err=nil;
    // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
    NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
        options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
    if ( err!=nil ) {
        qDebug( "Cannot resolve bookmark: %s", [[err localizedDescription] UTF8String] );
        [pool drain];
        return QFile::open( flags );
    }
    if ( [url respondsToSelector:@selector(startAccessingSecurityScopedResource)] ) {
        // only supported by 10.7.3 or later
        [url startAccessingSecurityScopedResource]; // start using bookmarked resource
    }
    [pool drain];
    return QFile::open( flags );
}

void PHISecFile::close()
{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
    QFileInfo f( this->fileName() );
    NSString *fn=[NSString stringWithUTF8String:f.canonicalFilePath().toUtf8()];
    NSString *key=[@"PHI" stringByAppendingString:fn];
    qDebug( "Closing with secure file bookmark: %s", [fn UTF8String] );
    NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
    BOOL stale;
    NSError* err=nil;
    // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) // not available before 10.7.3
    NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
        options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
    if ( err!=nil ) {
        qDebug( "Cannot resolve bookmark: %s", [[err localizedDescription] UTF8String] );
        [pool drain];
        return QFile::close();
    }
    if ( [url respondsToSelector:@selector(stopAccessingSecurityScopedResource)] ) {
        // only supported by 10.7.3 or later
        [url stopAccessingSecurityScopedResource]; // stop using bookmarked resource
    }
    [pool drain];
    QFile::close();
}

bool PHISecFile::storeSecurityBookmark( const QString &filename )
{
    NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
    QFileInfo f( filename );
    NSString *fn=[NSString stringWithUTF8String:f.canonicalFilePath().toUtf8()];
    qDebug( "Storing secure file bookmark: %s", [fn UTF8String] );
    NSURL *url=[NSURL fileURLWithPath:fn isDirectory:f.isDir()];
    NSError* err=nil;
    // NSURLBookmarkCreationWithSecurityScope = (1UL << 11) // not available before 10.7.3
    NSData* bookmark=[url bookmarkDataWithOptions:(1UL << 11)
        includingResourceValuesForKeys:nil relativeToURL:nil error:&err];
    if ( err!=nil ) {
        qDebug( "Can not create bookmark: %s", [[err localizedDescription] UTF8String] );
        [pool drain];
        return false;
    } else {
        NSString *key=[@"PHI" stringByAppendingString:fn];
        [[NSUserDefaults standardUserDefaults] setObject:bookmark forKey:key];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    [pool drain];
    return true;
}

bool PHISecFile::loadSecurityBookmarkForDir( const QString &filename )
{
    return false;
    /*
    QFileInfo f( filename );
    NSString *fn=[NSString stringWithUTF8String:f.canonicalFilePath().toUtf8()];

    NSString *key=[@"PHI" stringByAppendingString:fn];
    qDebug( "Load secure file bookmark for directory: %s", [fn UTF8String] );
    NSData* bookmark=[[NSUserDefaults standardUserDefaults] objectForKey:key];
    BOOL stale;
    NSError* err=nil;
    // NSURLBookmarkResolutionWithSecurityScope: (1UL << 10) - not available before 10.7.3
    NSURL* url=[NSURL URLByResolvingBookmarkData:bookmark
        options:(1UL << 10) relativeToURL:nil bookmarkDataIsStale:&stale error:&err];
    if ( err!=nil ) {
        qDebug( "Cannot resolve bookmark: %s", [[err localizedDescription] UTF8String] );
        return false;
    }
    if ( [url respondsToSelector:@selector(startAccessingSecurityScopedResource)] ) {
        // only supported by 10.7.3 or later
        [url startAccessingSecurityScopedResource]; // start using bookmarked resource
    }
    // The app has now access to the directory until process termination
    if ( [url respondsToSelector:@selector(stopAccessingSecurityScopedResource)] ) {
        // only supported by 10.7.3 or later
        [url stopAccessingSecurityScopedResource]; // stop using bookmarked resource
    }
    return true;
    */
}

/*
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
#ifndef PHIACONFIG_H
#define PHIACONFIG_H

#include <QObject>
#include <QDataStream>
#include <QDate>
#include <QFlags>
#include <QFile>
#include <QStringList>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QSslError>
#include <QTimer>
#include "phiadownloaddlg.h"
#include "phia.h"

class PHIAEXPORT PHIAAddress
{
public:
    enum Option { None=0x0, Private=0x1, Business=0x2 };
    Q_DECLARE_FLAGS( Options, Option ) //qint32 in DataStream
    QString title, firstname, lastname, organisation, unit, street, state, city, zip, country;
    Options options;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIAAddress::Options )
QDataStream& operator<<( QDataStream &out, const PHIAAddress& );
QDataStream& operator>>( QDataStream &in, PHIAAddress& );

class PHIAEXPORT PHIAContact
{
public:
    enum Option { None=0x0, Private=0x1, Business=0x2, HasUrl=0x4 };
    Q_DECLARE_FLAGS( Options, Option ) //qint32 in DataStream
    QString name, email, mobile, phone, fax, url;
    Options options;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIAContact::Options )
QDataStream& operator<<( QDataStream &out, const PHIAContact& );
QDataStream& operator>>( QDataStream &in, PHIAContact& );

class PHIACreditCard
{
public:
    enum Option { None=0x0, Private=0x1, Business=0x2, FullNumber=0x4 };
    Q_DECLARE_FLAGS( Options, Option ) //qint32 in DataStream
    QString name, number, type, civ;
    QDate valid;
    Options options;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIACreditCard::Options )
QDataStream& operator<<( QDataStream &out, const PHIACreditCard& );
QDataStream& operator>>( QDataStream &in, PHIACreditCard& );

class PHIADownloadEntry : public QObject
{
    Q_OBJECT

    friend QDataStream& operator<<( QDataStream &out, const PHIADownloadEntry* );
    friend QDataStream& operator>>( QDataStream &in, PHIADownloadEntry* );

public:
    PHIADownloadEntry( QObject *parent );
    virtual ~PHIADownloadEntry();
    enum Status { Init=0, Downloading=1, Error=2, Canceled=3, Completed=4 };
    void addDownload( QNetworkReply *reply, const QString &path, QWidget *parent );
    void abort();
    inline QDateTime started() const { return _dt; }
    inline Status status() const { return _status; }
    inline QUrl url() const { return _url; }
    inline QString file() const { return _location; }
    inline QNetworkReply* reply() const { return _reply; }
    inline qint64 totalBytes() const { return _totalBytes; }
    inline qint64 bytesDownloaded() const { return _bytesDownloaded; }
    inline void setTotalBytes( qint64 t ) { _totalBytes=t; }
    inline void setBytesDownloaded( qint64 b ) { _bytesDownloaded=b; }

private:
    Status _status;
    QDateTime _dt;
    QFile _file;
    QString _location;
    QUrl _url;
    QNetworkReply *_reply;
    qint64 _totalBytes, _bytesDownloaded;
    QWidget *_parent;

protected slots:
    void slotDownloadFinished();
    void slotReadyRead();

signals:
    void save();
    void aborted();
    void finished( const QString& filename );
};

QDataStream& operator<<( QDataStream&, const PHIADownloadEntry* );
QDataStream& operator>>( QDataStream&, PHIADownloadEntry* );

class PHIAEXPORT PHIAConfig : public QObject
{
    Q_OBJECT

    friend QDataStream& operator<<( QDataStream &out, const PHIAConfig* );
    friend QDataStream& operator>>( QDataStream &in, PHIAConfig* );

public:
    enum Option { ONone=0x0, OUsePasswd=0x1, OAskForApp=0x2, OAskForUnsecure=0x4, ONotifyUpdate=0x8,
        ODownloadInstall=0x10, OUseProxy=0x20 };
    enum UsedId { UNone=0x0, UAutoFillOut=0x1 }; // must be a hex value
    enum Interval { INever=0, IEveryDay=1, IEveryWeek=2, IEveryTwoWeeks=3, IEveryMonth=4 };
    Q_DECLARE_FLAGS( Options, Option ) //qint32 in DataStream
    Q_DECLARE_FLAGS( UsedIds, UsedId ); //qint32 in DataStream

    inline Options options() const { return _options; }
    inline QList <PHIAAddress> addresses() const { return _addressList; }
    inline void setAddresses( const QList <PHIAAddress> &add ) { _addressList=add; }
    inline QList <PHIAContact> contacts() const { return _contactList; }
    inline void setContacts( const QList <PHIAContact> &con ) { _contactList=con; }
    inline QList <PHIACreditCard> creditCards() const { return _creditCardList; }
    inline void setCreditCards( const QList <PHIACreditCard> &cards ) { _creditCardList=cards; }
    inline QDate endDate() const { return _endDate; }
    inline void setEndDate( const QDate &d ) { _endDate=d; }
    inline QDate startDate() const { return _startDate; }
    inline void setStartDate( const QDate &d ) { _startDate=d; }
    inline QStringList languages() const { return _languages; }
    inline void setLanguages( const QStringList &l ) { _languages=l; }
    inline bool usePasswd() const { return _options & OUsePasswd; }
    inline void setUsePasswd( bool b ) { b ? _options |= OUsePasswd : _options &= ~OUsePasswd; }
    inline QString fileName() const { return _fileName; }
    inline void setServer( const QString &s ) { _server=s; }
    inline QString server() const { return _server; }
    inline void setHome( const QString &h ) { _home=h; }
    inline QString home() const { return _home; }
    inline void setProxyUser( const QString &u ) { _proxyUser=u; }
    inline QString proxyUser() const { return _proxyUser; }
    inline void setProxyPasswd( const QString &p ) { _proxyPasswd=p; }
    inline QString proxyPasswd() const { return _proxyPasswd; }
    inline void setDownloadDir( const QString &d ) { _downloadDir=d; }
    inline QString downloadDir() const { return _downloadDir; }
    inline UsedIds usedIds() const { return _usedIds; }
    inline QDateTime lastUpdateCheck() const { return _lastUpdateCheck; }
    inline void setInterval( int i ) { _interval=static_cast<Interval>(i); }
    inline int interval() const { return static_cast<int>(_interval); }

    inline bool askForApp() const { return _options & OAskForApp; }
    inline void setAskForApp( bool b ) { b ? _options |= OAskForApp : _options &= ~OAskForApp; }
    inline bool askForUnsecure() const { return _options & OAskForUnsecure; }
    inline void setAskForUnsecure( bool b ) { b ? _options |= OAskForUnsecure : _options &= ~OAskForUnsecure; }
    //inline bool notifyUpdate() const { return _options & ONotifyUpdate; }
    //inline void setNotifyUpdate( bool b ) { b ? _options |= ONotifyUpdate : _options &= ~ONotifyUpdate; }
    inline bool downloadInstall() const { return _options & ODownloadInstall; }
    inline void setDownloadInstall( bool b ) { b ? _options |= ODownloadInstall : _options &= ~ODownloadInstall; }
    inline bool useProxy() const { return _options & OUseProxy; }
    inline void setUseProxy( bool b ) { b ? _options |= OUseProxy : _options &= ~OUseProxy; }

    inline bool passwdOpen() const { return _passwdOpen; }

    void setPasswd( const QString &pwd );
    void showUsedDialog( QWidget *parent, UsedId id, const QString &title, const QString &text );
    bool showPasswdDialog( QWidget *parent );
    bool showProxyDialog( QWidget *parent, const QNetworkProxy &proxy, QAuthenticator *auth );
    void showDownloadDialog( QWidget *parent );
    void load( const QString &file );
    void save( const QString &file, QWidget *parent=0 );
    PHIADownloadEntry* addDownload( QNetworkReply *reply, QWidget *parent, bool ask=true );

    static PHIAConfig* instance( QObject *parent=0 );
    static bool showAuthDialog( QWidget *parent, QNetworkReply *reply, QAuthenticator *auth );
    static bool showSslErrorDialog( QWidget *parent, QNetworkReply *reply, const QList <QSslError>& errors );
    static inline QStringList intervalNames() {
        return QStringList() << tr( "Never" ) << tr( "Every day" ) << tr( "Every week" )
        << tr( "Every two weeks" ) << tr( "Every month" ); }

public slots:
    void slotCheckForUpdate( QWidget *parent=0 );

protected:
    bool checkPasswd( const QString &pwd );
    void clear();
    bool loadDownloadEntries();

protected slots:
    void slotPasswordTimeout();
    void slotDownloadsClearClicked();
    bool slotSaveDownloadEntries();
    void slotGetUpdateInfoFinished();
    void slotUpdateAborted();
    void slotUpdateDownloadFinished( const QString &filename );
    void slotUpdateNewBytesAvailable();
    void slotUpdateReplyFinished();

protected:
    PHIAConfig( QObject *parent=0 );
    virtual ~PHIAConfig();

private:
    static PHIAConfig *_instance;
    quint8 _fileVersion;
    qint32 _pwTimeout;
    QList <PHIAAddress> _addressList;
    QList <PHIAContact> _contactList;
    QList <PHIACreditCard> _creditCardList;
    QList <PHIADownloadEntry*> _downloads;
    QDate _startDate, _endDate;
    QStringList _languages;
    QString _fileName, _home, _proxyUser, _proxyPasswd, _server, _downloadDir;
    QByteArray _passwd;
    bool _passwdOpen, _validProxy, _initDownloadDialog, _updateInProgress;
    UsedIds _usedIds; // qint32 in datastream
    Options _options;
    QTimer _timer;
    Interval _interval; // quint8 in datastream
    QDateTime _lastUpdateCheck;
    QNetworkReply *_updateReply;
    QWidget *_updateWidgetParent;
    QFile *_updateFile;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHIAConfig::Options )
Q_DECLARE_OPERATORS_FOR_FLAGS( PHIAConfig::UsedIds )

QDataStream& operator<<( QDataStream &out, const PHIAConfig* );
QDataStream& operator>>( QDataStream &in, PHIAConfig* );

#endif // PHIACONFIG_H

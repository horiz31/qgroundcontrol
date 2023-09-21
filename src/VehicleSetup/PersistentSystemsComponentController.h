/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/



/// @file
///     @brief Radio Config Qml Controller
///     @author Brad Stinson <bstisnon@echomav.com>

#include <QTimer>

#include "FactPanelController.h"
#include "UASInterface.h"
#include "QGCLoggingCategory.h"
#include "AutoPilotPlugin.h"

Q_DECLARE_LOGGING_CATEGORY(PersistentSystemsComponentControllerLog)

class PersistentSystemsConfigest;

namespace Ui {
    class PersistentSystemsComponentController;
}


class PersistentSystemsComponentController : public FactPanelController
{
    Q_OBJECT

public:
    PersistentSystemsComponentController(void);
    ~PersistentSystemsComponentController();


    Q_PROPERTY(QQuickItem* statusText              MEMBER _statusText                NOTIFY statusTextChanged)
    Q_PROPERTY(QQuickItem* clientListText          MEMBER _clientListText            NOTIFY clientListTextChanged)
    Q_PROPERTY(int channel                         READ channel                      NOTIFY channelChanged)
    Q_PROPERTY(bool isConnected                    READ isConnected                  NOTIFY isConnectedChanged)
    Q_PROPERTY(bool isChannelChangeOK              READ isChannelChangeOK            NOTIFY isChannelChangeOKChanged)
    Q_PROPERTY(bool isConnectionError              READ isConnectionError            NOTIFY isConnectionErrorChanged)
    Q_PROPERTY(bool isChannelLoadComplete          READ isChannelLoadComplete        NOTIFY isChannelLoadCompleteChanged)
    Q_PROPERTY(bool isIPEditable                   READ isIPEditable                 NOTIFY isIPEditableChanged)


    Q_INVOKABLE void start(void);
    Q_INVOKABLE void refresh(void);
    Q_INVOKABLE void setChannel(int channel);
    Q_INVOKABLE void updateChannelClicked();
    Q_INVOKABLE void clickIP(void);


    bool            isConnected             () { return _isPersistentSystemsConnected; }
    bool            isChannelChangeOK       () { return _isChannelChangeOK;}
    bool            isConnectionError       () { return _isConnectionError;}
    bool            isChannelLoadComplete   () { return _isChannelLoadComplete;}
    bool            isIPEditable            () { return _isIPEditable;}
    int channel () { return _channel; }

signals:
     void statusTextChanged(void);
     void clientListTextChanged(void);
     void isConnectedChanged(bool connected);
     void isChannelChangeOKChanged(bool changeOK);
     void isConnectionErrorChanged(bool errorChange);
     void isChannelLoadCompleteChanged(bool loadChanged);
     void isIPEditableChanged(bool loadChanged);
     void channelChanged(int channel);
     void PersistentSystemsRSSIChanged              ();
     void cancelDownload();

private slots:


private:
    void _loadSettings(void);
    void _storeSettings(void);
    void _getPersistentSystemsStatus(void);

    void _getPersistentSystemsChannel(QString, QUrl);
    void _getPersistentSystemsToken(QString, QString, QUrl);
    void _setPersistentSystemsChannel(QString, QUrl, qint16);
    void _getPersistentSystemsRSSIList(QString, QUrl);
    int PersistentSystemsRSSIMin() const;

    QList<PersistentSystemsRSSIEntry_t>        _PersistentSystemsRSSIList;

    static const char* _settingsGroup;
    QQuickItem* _statusText         = nullptr;
    QQuickItem* _clientListText     = nullptr;

    bool _isPersistentSystemsConnected = false;
    bool _isChannelChangeOK = false;
    bool _isConnectionError = false;
    bool _isChannelLoadComplete = false;
    bool _isIPEditable = false;
    int _channel;
    QString _token;
    SettingsManager* _settingsManager;
    int _setChannel = 1;
    int _membersInMesh = 0;
    int _ipClickCounter = 0;


};



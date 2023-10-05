/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ATAKMarkerManager.h"
#include "QGCLoggingCategory.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
//#include "ATAKMarkerManagerSettings.h"

#include <QDebug>

ATAKMarkerManager::ATAKMarkerManager(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
{
}

void ATAKMarkerManager::setToolbox(QGCToolbox* toolbox)
{
    QGCTool::setToolbox(toolbox);

    connect(&_ATAKMarkerCleanupTimer, &QTimer::timeout, this, &ATAKMarkerManager::_cleanupStaleMarkers);
    _ATAKMarkerCleanupTimer.setSingleShot(false);
    _ATAKMarkerCleanupTimer.start(1000);

    ATAKVehicleManagerSettings* settings = qgcApp()->toolbox()->settingsManager()->atakVehicleManagerSettings();
    QHostAddress _atakMcastAddress = QHostAddress(settings->atakServerHostAddress()->rawValue().toString());
    int _atakMcastPort = settings->atakServerPort()->rawValue().toInt();


    _udpLink = new ATAKUDPLink(_atakMcastAddress.toString(), _atakMcastPort, this);
    connect(_udpLink, &ATAKUDPLink::atakMarkerUpdate,  this, &ATAKMarkerManager::atakMarkerUpdate,   Qt::QueuedConnection);
    connect(_udpLink, &ATAKUDPLink::error,             this, &ATAKMarkerManager::_udpError,         Qt::QueuedConnection);

}

void ATAKMarkerManager::_cleanupStaleMarkers()
{
    // Remove all expired ATAK Markers
    for (int i=_ATAKMarkers.count()-1; i>=0; i--) {
        ATAKMarker* atakMarker = _ATAKMarkers.value<ATAKMarker*>(i);
        if (atakMarker->expired()) {
            qCDebug(ATAKMarkerManagerLog) << "Expired" << QStringLiteral("%1").arg(atakMarker->uid(), 0, 16);
            _ATAKMarkers.removeAt(i);
            _atakUidMap.remove(atakMarker->uid());
            atakMarker->deleteLater();
        }
    }
}

void ATAKMarkerManager::atakMarkerUpdate(const ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo)
{
    QString uid = atakMarkerInfo.uid;

    qDebug() << "Updating ATAK Marker with uid" << atakMarkerInfo.uid;

    if (_atakUidMap.contains(uid)) {
        _atakUidMap[uid]->update(atakMarkerInfo);
    } else {
            ATAKMarker* atakMarker = new ATAKMarker(atakMarkerInfo, this);
            _atakUidMap[uid] = atakMarker;
            _ATAKMarkers.append(atakMarker);

    }
}

void ATAKMarkerManager::_udpError(const QString errorMsg)
{
    qgcApp()->showAppMessage(tr("ADSB Server Error: %1").arg(errorMsg));
}


ATAKUDPLink::ATAKUDPLink(const QString& hostAddress, int port, QObject* parent)
    : QThread       (nullptr)  //parent
    , _hostAddress  (hostAddress)
    , _port         (port)
{
    moveToThread(this);
    start();
}

ATAKUDPLink::~ATAKUDPLink(void)
{
    if (_socket) {
        QObject::disconnect(_socket, &QUdpSocket::readyRead, this, &ATAKUDPLink::_readBytes);
        _socket->disconnectFromHost();
        _socket->deleteLater();
        _socket = nullptr;
    }
    quit();
    wait();
}

void ATAKUDPLink::run(void)
{
    _hardwareConnect();
    exec();
}

void ATAKUDPLink::_hardwareConnect()
{
    qDebug() << "Opening connection to receive ATAK data";

    _socket = new QUdpSocket();

    // Bind the socket to a specific port (you can choose any available port)


    if (!_socket->bind(QHostAddress::AnyIPv4, 6969, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qDebug() << "Failed to bind UDP socket to port 6969";
        return;
    }

    // Join a multicast group
    QHostAddress multicastGroup("239.2.3.1"); // Replace with your multicast group address
    if (!_socket->joinMulticastGroup(multicastGroup)) {
        qDebug() << "Failed to join multicast group" << multicastGroup.toString();
        return;
    }

    qDebug() << "Listening for multicast data on" << multicastGroup.toString() << "port 6969";


    // Connect the socket's readyRead() signal to a slot to handle incoming data
    connect(_socket, &QUdpSocket::readyRead, this, &ATAKUDPLink::_readBytes);


    qCDebug(ATAKMarkerManagerLog) << "ATAK UDP Socket connected";

}

void ATAKUDPLink::_readBytes(void)
{
    qDebug() << "got readBytes";
    if (_socket) {

        while (_socket->hasPendingDatagrams()) {
            qDebug() << _socket->pendingDatagramSize() << "number of byte to read";
            QNetworkDatagram dgram = _socket->receiveDatagram();
            qDebug() << "Received datagram from" << QString(dgram.data()) << dgram.senderAddress().toString() << dgram.senderPort();
        }
/*
        while (_socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(_socket->pendingDatagramSize());

            // Read the incoming datagram into the QByteArray
            _socket->readDatagram(datagram.data(), datagram.size());

            // Convert the QByteArray to a QString
            QString receivedData = QString::fromUtf8(datagram);

            qDebug() << "got the following on port 6969" << receivedData;

            _parseData(datagram);
        }
*/
        //QByteArray bytes = _socket->readAll();
        //_parseData(QString::fromLocal8Bit(bytes));
    }
}

void ATAKUDPLink::_parseData(const QString& data)
{
    //TBD
    //parse data and do a marker update
    /*
            double altitude = modeCAltitude / 3.048;
            QGeoCoordinate location(lat, lon);

            ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo;
            atakMarkerInfo.uid = uid;
            atakMarkerInfo.callsign = callsign;
            atakMarkerInfo.location = location;
            atakMarkerInfo.altitude = altitude;
            emit atakMarkerUpdate(atakMarkerInfo);

*/

}

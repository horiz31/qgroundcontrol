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
#include "cotevent.pb.h"
#include "contact.pb.h"
#include "detail.pb.h"
#include "group.pb.h"
#include "precisionlocation.pb.h"
#include "status.pb.h"
#include "takcontrol.pb.h"
#include "takmessage.pb.h"
#include "takv.pb.h"
#include "track.pb.h"
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

    connect(&_ATAKMarkerSendoutTimer, &QTimer::timeout, this, &ATAKMarkerManager::_sendOutLocalMarkers);
    _ATAKMarkerSendoutTimer.setSingleShot(false);
    _ATAKMarkerSendoutTimer.start(30000);

    ATAKVehicleManagerSettings* settings = qgcApp()->toolbox()->settingsManager()->atakVehicleManagerSettings();
    QString _atakMcastAddress = settings->atakServerHostAddress()->rawValue().toString();
    int _atakMcastPort = settings->atakServerPort()->rawValue().toInt();

    _udpLink = new ATAKUDPLink(_atakMcastAddress, _atakMcastPort, this);
    connect(_udpLink, &ATAKUDPLink::atakMarkerUpdate,  this, &ATAKMarkerManager::atakMarkerUpdate,   Qt::QueuedConnection);
    connect(_udpLink, &ATAKUDPLink::error,             this, &ATAKMarkerManager::_udpError,         Qt::QueuedConnection);

}

void ATAKMarkerManager::deleteMarker(QString uid)
{
    for (int i=_ATAKMarkers.count()-1; i>=0; i--) {
        ATAKMarker* atakMarker = _ATAKMarkers.value<ATAKMarker*>(i);
        if (atakMarker->uid() == uid) {  //&& atakMarker->isLocal(
            _ATAKMarkers.removeAt(i);
            _atakUidMap.remove(atakMarker->uid());
            atakMarker->deleteLater();
        }
    }
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

void ATAKMarkerManager::_sendOutLocalMarkers()
{
    // Send out data periodically for markers created locally
    for (int i=_ATAKMarkers.count()-1; i>=0; i--) {
        ATAKMarker* atakMarker = _ATAKMarkers.value<ATAKMarker*>(i);
        if (atakMarker->isLocal()) {
            _send(atakMarker);
        }
    }
}

void ATAKMarkerManager::_send(ATAKMarker* atakMarker)
{

    //to get ISO 8601 timetstamps
    QString currentTimeStamp = QDateTime::currentDateTime().toTimeSpec(Qt::UTC).toString(Qt::DateFormat::ISODate);
    QString startTimeStamp = QDateTime::fromMSecsSinceEpoch(atakMarker->startTime()).toTimeSpec(Qt::UTC).toString(Qt::DateFormat::ISODate);
    QString staleTimeStamp = QDateTime::fromMSecsSinceEpoch(atakMarker->staleTime()).toTimeSpec(Qt::UTC).toString(Qt::DateFormat::ISODate);

    //create the XML
    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(false);
    writer.writeStartDocument("1.0", true);

    writer.writeStartElement("event");
    writer.writeAttribute("version", "2.0");
    writer.writeAttribute("uid", atakMarker->uid());
    writer.writeAttribute("type", atakMarker->type());  //see mil std 2525, rover test type "a-f-G-E-V-C-A");
    writer.writeAttribute("how", "m-g"); //position derived from machine - gps
    writer.writeAttribute("time", currentTimeStamp);
    writer.writeAttribute("start", startTimeStamp);
    writer.writeAttribute("stale", staleTimeStamp);

    writer.writeStartElement("point");
    writer.writeAttribute("lat",  QString::number(atakMarker->coordinate().latitude()));
    writer.writeAttribute("lon", QString::number(atakMarker->coordinate().longitude()));
    writer.writeAttribute("ce", "10");  //Circular Error around point defined by lat and lon fields in meters.
    writer.writeAttribute("hae", "0");  //height above ellipsoid/point (meters, WGS84)
    writer.writeAttribute("le", "0"); //Linear Error in meters associated with the HAE field
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();

    //qDebug() << "XML OUT (ATAK Marker Manager) is" << output;

    if (_udpLink)
        _udpLink->sendBytes(output.toUtf8());

}

void ATAKMarkerManager::atakMarkerUpdate(const ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo)
{
    QString uid = atakMarkerInfo.uid;  

    if (_atakUidMap.contains(uid)) {
        if (!_atakUidMap[uid]->isLocal())  //if marker already exists and it's local, this is likely a network reflection, don't update uncessarily
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

void ATAKUDPLink::sendBytes(QByteArray bytes)
{

    if (_udpSendSocket)
    {
        //qDebug() << "sending CoT UDP data via ATAKMarkerManager";
        _udpSendSocket->writeDatagram(bytes, QHostAddress(_hostAddress), _port);
        _udpSendSocket->writeDatagram(bytes, QHostAddress(QHostAddress::LocalHost), 4242);  //also send to 4242 on localhost in case someone is running TAK on the same machine
    }


}

void ATAKUDPLink::_hardwareConnect()
{
    qDebug() << "Opening connection to receive ATAK data";

    _udpSendSocket = new QUdpSocket();  //create a socket to use for sending
    //_udpSendSocket->bind(QHostAddress(QHostAddress::AnyIPv4), 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    _udpSendSocket->bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    _socket = new QUdpSocket();  //receive socket

    // Bind the receive socket to a specific port, enabling port sharing to allow tak to be running on the same machine
    if (!_socket->bind(QHostAddress::AnyIPv4, _port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
    //if (!_socket->bind(QHostAddress::AnyIPv4, _port)) {
        qDebug() << "Failed to bind UDP socket to port 6969";
        return;
    }

    // Join the multicast group
    QHostAddress multicastGroup(_hostAddress);
    if (!_socket->joinMulticastGroup(multicastGroup)) {
        qDebug() << "Failed to join multicast group" << multicastGroup.toString();
        return;
    }

    //qDebug() << "Listening for multicast data on" << multicastGroup.toString() << "port 6969";

    // Connect the socket's readyRead() signal to a slot to handle incoming data
    connect(_socket, &QUdpSocket::readyRead, this, &ATAKUDPLink::_readBytes);

    //qCDebug(ATAKMarkerManagerLog) << "ATAK UDP Socket connected";

}

void ATAKUDPLink::_readBytes(void)
{
    //qDebug() << "got readBytes";
    if (_socket) {

        while (_socket->hasPendingDatagrams()) {
            //qDebug() << _socket->pendingDatagramSize() << "number of byte to read";
            QNetworkDatagram dgram = _socket->receiveDatagram();
            //qDebug() << "Received datagram from" << QString(dgram.data()) << dgram.senderAddress().toString() << dgram.senderPort();

            if (dgram.data().at(0) == (char)0xbf && dgram.data().at(2) == (char)0xbf)
            {
                //qDebug() << "TAK Packet detected!";
                QByteArray headerRemoved = dgram.data();
                headerRemoved.remove(0, 3);
                if (dgram.data().at(1) == (char)0x00)
                {
                    //use XML Parser
                    _parseDataXml(headerRemoved);
                }
                else if (dgram.data().at(1) == (char)0x01)
                {
                    //use Protobuf
                    //qDebug() << "Received protocol buf datagram:" << dgram.data().toHex() << "From:" << dgram.senderAddress().toString() << dgram.senderPort();
                    _parseDataProtoBuf(headerRemoved);
                }

            }
            else
            {
                //qDebug() << "Received datagram without header from" << QString(dgram.data()) << dgram.senderAddress().toString() << dgram.senderPort();
                //try to parse xml
                //_parseDataXml(dgram.data());
            }
        }
    }
}
void ATAKUDPLink::_parseDataXml(const QByteArray& data)
{
    //XML (Version 0) CoT Data
    bool callsignFound = false;
    QXmlStreamReader xml(data);
    ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo;
    if (xml.hasError())
        return;
    try {
        while (!xml.atEnd()) {
            xml.readNext();

            if (xml.isStartElement()) {
                if (xml.name() == "event") {
                    atakMarkerInfo.uid = xml.attributes().value("uid").toString();
                    atakMarkerInfo.type = xml.attributes().value("type").toString();
                    if (atakMarkerInfo.type=="a-f-A-M-F-Q")
                    {
                        //the issue here is that the echopilot puts out CoT data, which we show on top of the GCS's already present icon for the vehicle
                        //woudl be nice in the future to come up with a way to show other UAS data but not our own
                        return;
                    }
                    atakMarkerInfo.startTime = xml.attributes().value("start").toInt();
                    if (atakMarkerInfo.startTime == 0)
                        atakMarkerInfo.startTime = QDateTime::fromString(xml.attributes().value("start").toString(), Qt::ISODate).toMSecsSinceEpoch();
                    atakMarkerInfo.staleTime = xml.attributes().value("stale").toInt();
                    if (atakMarkerInfo.staleTime == 0)
                        atakMarkerInfo.staleTime = QDateTime::fromString(xml.attributes().value("stale").toString(), Qt::ISODate).toMSecsSinceEpoch();

                } else if (xml.name() == "point") {
                    QGeoCoordinate location = QGeoCoordinate(xml.attributes().value("lat").toDouble(), xml.attributes().value("lon").toDouble());
                    atakMarkerInfo.location = location;
                    atakMarkerInfo.altitude = xml.attributes().value("hae").toDouble();
                }
                else if (xml.name() == "contact") {
                    atakMarkerInfo.callsign = xml.attributes().value("callsign").toString();
                    if (atakMarkerInfo.callsign != "")
                        callsignFound = true;
                }
                if (!callsignFound)
                    atakMarkerInfo.callsign = atakMarkerInfo.uid;
            }
        }

        if (xml.hasError()) {
            qDebug() << "XML Error 1 in ATAKMarkerManager:" << xml.errorString();
        }
        else
        {            
            atakMarkerInfo.isLocal = false;
            atakMarkerInfo.heading = qQNaN();
            emit atakMarkerUpdate(atakMarkerInfo);
        }
    }
    catch (...)
    {
        qDebug() << "XML Error 2 in ATAKMarkerManager:" << xml.errorString();
    }
}

void ATAKUDPLink::_parseDataProtoBuf(const QByteArray& data)
{
    atakmap::commoncommo::protobuf::v1::TakMessage takMessage;

    // Parse the CoT message
    if (takMessage.ParseFromArray(data, data.size())) {

        ATAKMarker::ATAKMarkerInfo_t atakMarkerInfo;
        // Access the parsed data
        if (takMessage.cotevent().detail().has_contact())
        {
            qDebug() << "Callsign (contact):" << takMessage.cotevent().detail().contact().callsign().c_str();
            atakMarkerInfo.callsign = takMessage.cotevent().detail().contact().callsign().c_str();
        }
        else
        {
            QXmlStreamReader reader(takMessage.cotevent().detail().xmldetail().c_str());
            while(!reader.atEnd() && !reader.hasError()) {
                if(reader.readNext() == QXmlStreamReader::StartElement && reader.name() == "contact") {
                    qDebug() << "Callsign (xml):" << reader.attributes().value("callsign");
                    atakMarkerInfo.callsign = reader.attributes().value("callsign").toString();
                }
            }
         }

        qDebug() << "UID:" << takMessage.cotevent().uid().c_str();
        atakMarkerInfo.uid = takMessage.cotevent().uid().c_str();
        QGeoCoordinate location(takMessage.cotevent().lat(), takMessage.cotevent().lon());
        atakMarkerInfo.location = location;
        atakMarkerInfo.altitude = takMessage.cotevent().hae();
        atakMarkerInfo.staleTime = takMessage.cotevent().staletime();
        atakMarkerInfo.startTime = takMessage.cotevent().starttime();
        atakMarkerInfo.type = takMessage.cotevent().type().c_str();
        atakMarkerInfo.isLocal = false;
        emit atakMarkerUpdate(atakMarkerInfo);

    } else {
        qDebug() << "Failed to parse CoT message";
    }
}

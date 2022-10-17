/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ATAKController.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include <QDebug>

//the atak mcast address and port should come from settings
ATAKController::ATAKController(void)
{


    //qgcApp()->toolbox()->settingsManager()->appSettings()->->enableTelemetry()->rawValue().toBool()) {
        //-- TODO: This will s
    //hostile targets
    _cotMap.insert(QStringLiteral("SAM"), "a-h-A-W-M-S-A");
    _cotMap.insert(QStringLiteral("Radar"), "a-h-G-E-S-R");
    _cotMap.insert(QStringLiteral("Tank"), "a-h-G-E-V-A-T");
    _cotMap.insert(QStringLiteral("Howitzer"), "a-h-G-E-W-H");
    _cotMap.insert(QStringLiteral("Howitzer Self Pro"), "a-h-G-E-W-H-H-S");
    _cotMap.insert(QStringLiteral("Mortar"), "a-h-G-E-W-O");
    _cotMap.insert(QStringLiteral("Heavy Machine Gun"), "a-h-G-E-W-R-H");
    _cotMap.insert(QStringLiteral("Anti Tank Rocket"), "a-h-G-E-W-T");
    _cotMap.insert(QStringLiteral("Land Mines"), "a-h-G-E-X-M");
    _cotMap.insert(QStringLiteral("Hostile Airbase"), "a-h-G-I-B-A");
    _cotMap.insert(QStringLiteral("Hostile Base"), "a-h-G-I-B");
    _cotMap.insert(QStringLiteral("Armor, Wheeled Med"), "a-h-G-U-C-A-W-M");
    _cotMap.insert(QStringLiteral("Infantry Light"), "a-h-G-U-C-I-L");
    _cotMap.insert(QStringLiteral("Infantry Motorized"), "a-h-G-U-C-I-M");
    //neutral civ targets
    _cotMap.insert(QStringLiteral("Civ. Vehicle"), "a-n-G-E-V-C");
    _cotMap.insert(QStringLiteral("Bridge"), "a-n-G-I-c-b");
    _cotMap.insert(QStringLiteral("Residence"), "a-n-G-I-c-rah");
    _cotMap.insert(QStringLiteral("Electric Power Facility"), "a-n-G-I-U-E");

    //now for each key in map, appen to list
    QMap<QString, QString>::const_iterator i = _cotMap.constBegin();
    while (i != _cotMap.constEnd()) {
        _cotTypes.append(i.key());
        ++i;
    }

    //stale times, save in seconds
    _minuteMap.insert(QStringLiteral("10 minutes"), 10 * 60);
    _minuteMap.insert(QStringLiteral("30 minutes"), 30 * 60);
    _minuteMap.insert(QStringLiteral("1 hour"), 60 * 60);
    _minuteMap.insert(QStringLiteral("2 hours"), 120 * 60);
    _minuteMap.insert(QStringLiteral("4 hours"), 240 * 60);
    _minuteMap.insert(QStringLiteral("8 hours"), 480 * 60);
    _minuteMap.insert(QStringLiteral("24 hours"), 24 * 60 * 60);
    _minuteMap.insert(QStringLiteral("1 week"), 24 * 60 * 60 * 7);
    _minuteMap.insert(QStringLiteral("1 month"), 24 * 60 * 60 * 30); //30 days

    //now for each key in map, appen to list
    QMap<QString, int>::const_iterator j = _minuteMap.constBegin();
    while (j != _minuteMap.constEnd()) {
        _staleMinuteList.append(j.key());
        ++j;
    }

    //bind the socket
    udpSocket4.bind(QHostAddress(QHostAddress::AnyIPv4), 0);


}

void ATAKController::send(QGeoCoordinate coordinate, QString uid)
{

    ATAKVehicleManagerSettings* settings = qgcApp()->toolbox()->settingsManager()->atakVehicleManagerSettings();

    _atakMcastAddress = QHostAddress(settings->atakServerHostAddress()->rawValue().toString());
    _atakMcastPort = settings->atakServerPort()->rawValue().toInt();

    if (uid.isEmpty())
        _uid = QString("EchoMav.") + GetRandomString();
    else
        _uid = QString("EchoMav.") + uid;

    //qDebug() << "Sending ATAK mcast message";
    //qDebug() << "The specified type is" << _cotTypes[_cotType] << "and the CoT code is"<< _cotMap.value(_cotTypes[_cotType]);

    //to get ISO 8601 timetstamps
    QString currentTimeStamp = QDateTime::currentDateTime().toTimeSpec(Qt::UTC).toString(Qt::DateFormat::ISODate);
    QString startTimeStamp = currentTimeStamp;

    //create the XML
    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(false);
    writer.writeStartDocument("1.0", true);

    writer.writeStartElement("event");
        writer.writeAttribute("version", "2.0");
        writer.writeAttribute("uid", _uid);
        writer.writeAttribute("type", _cotMap.value(_cotTypes[_cotType]));  //see mil std 2525, rover test type "a-f-G-E-V-C-A");
        writer.writeAttribute("how", "m-g"); //position derived from machine - gps
        writer.writeAttribute("time", currentTimeStamp);
        writer.writeAttribute("start", startTimeStamp);
        writer.writeAttribute("stale", QDateTime::currentDateTime().toTimeSpec(Qt::UTC).addSecs(_minuteMap.value(_staleMinuteList[_staleMinutes])).toString(Qt::DateFormat::ISODate));

        writer.writeStartElement("point");
            writer.writeAttribute("lat", QString::number(coordinate.latitude()));
            writer.writeAttribute("lon", QString::number(coordinate.longitude()));
            writer.writeAttribute("ce", "10");  //Circular Error around point defined by lat and lon fields in meters.
            writer.writeAttribute("hae", "0");  //height above ellipsoid/point (meters, WGS84)
            writer.writeAttribute("le", "0"); //Linear Error in meters associated with the HAE field
        writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();

    //qDebug() << "XML OUT is" << output;

    //now send out udp mcast to atak mcast address and to localhost:4242
    udpSocket4.writeDatagram(output.toUtf8(), QHostAddress(QHostAddress::LocalHost), 4242);
    udpSocket4.writeDatagram(output.toUtf8(), _atakMcastAddress, _atakMcastPort);
}

void
ATAKController::setCotType(int idx)
{
    _cotType = idx;
}

void
ATAKController::setStaleMinutes(int idx)
{
    _staleMinutes = idx;
}

void
ATAKController::setUid(QString uid)
{
    _uid = uid;
}

int
ATAKController::cotType()
{
    return _cotType;
}

int
ATAKController::staleMinutes()
{
    return _staleMinutes;
}

QString
ATAKController::uid()
{
    return _uid;
}

QString
ATAKController::GetRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
   const int randomStringLength = 5; // assuming you want random strings of 12 characters

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = QRandomGenerator::global()->generate() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

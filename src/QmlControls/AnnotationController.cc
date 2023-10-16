/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AnnotationController.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include <QDebug>
#include "AnnotationManager.h"

//the atak mcast address and port should come from settings
AnnotationController::AnnotationController(void)
{
    _toolbox = qgcApp()->toolbox();

    _colorMap.append(QPair(QStringLiteral("Black"), QColor("black")));
    _colorMap.append(QPair(QStringLiteral("Red"), QColor("red")));
    _colorMap.append(QPair(QStringLiteral("Orange"), QColor("orange")));
    _colorMap.append(QPair(QStringLiteral("Yellow"), QColor("yellow")));
    _colorMap.append(QPair(QStringLiteral("Green"), QColor("green")));
    _colorMap.append(QPair(QStringLiteral("Blue"), QColor("blue")));
    _colorMap.append(QPair(QStringLiteral("Purple"), QColor("purple")));
    _colorMap.append(QPair(QStringLiteral("White"), QColor("white")));

    //now for each key in list, append to a QStringList I can bind to the model
    for( int j=0; j<_colorMap.count(); ++j )
    {
        _colorList.append(_colorMap[j].first);
    }
}

void AnnotationController::create(QGeoCoordinate coordinate, QString displayName, QString altitude)
{
    if (displayName.length() == 0)
        return;

    //uid should be unique, create a 10 digit char string at random
    Annotation::AnnotationInfo_t annotationInfo;

    bool validate;
    double converted = altitude.toDouble(&validate);
    if (validate)
        annotationInfo.altitude = converted;
    else
        annotationInfo.altitude = qQNaN();

    annotationInfo.uid = GetRandomString();
    annotationInfo.displayName = displayName;
    annotationInfo.color = _colorMap[_colorIndex].second;
    annotationInfo.location = coordinate;

    _toolbox->annotationManager()->annotationUpdate(annotationInfo);

}

void
AnnotationController::setColor(int idx)
{
    _colorIndex = idx;
}

void
AnnotationController::setUid(QString uid)
{
    _uid = uid;
}

void
AnnotationController::setAltitude(double altitude)
{
    _altitude = altitude;
}

QString
AnnotationController::uid()
{
    return _uid;
}

int
AnnotationController::color()
{
    return _colorIndex;
}

QString
AnnotationController::GetRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
   const int randomStringLength = 10; // set to how long I want the string to be

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = QRandomGenerator::global()->generate() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

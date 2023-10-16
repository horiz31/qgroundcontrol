/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "Annotation.h"
#include "QGCLoggingCategory.h"
#include "QGC.h"
#include <QDebug>
#include <QtMath>

Annotation::Annotation(const AnnotationInfo_t& AnnotationInfo, QObject* parent)
    : QObject       (parent)
    , _uid  (AnnotationInfo.uid)
    , _displayName     (AnnotationInfo.displayName)
    , _altitude     (qQNaN())
    , _color       (QColor("Black"))
{
    update(AnnotationInfo);
}

void Annotation::update(const AnnotationInfo_t& AnnotationInfo)
{
    if (_uid != AnnotationInfo.uid) {
        qDebug() << "uid mismatch expected:actual" << _uid << AnnotationInfo.uid;
        return;
    }
    if (AnnotationInfo.displayName != _displayName) {
        _displayName = AnnotationInfo.displayName;
        emit displayNameChanged();
    }
    if (AnnotationInfo.type != _type) {
        _type = AnnotationInfo.type;
        emit typeChanged();
    }

    if (_coordinate != AnnotationInfo.location) {
        _coordinate = AnnotationInfo.location;
        emit coordinateChanged();
    }
    if (!QGC::fuzzyCompare(AnnotationInfo.altitude, _altitude)) {
        _altitude = AnnotationInfo.altitude;
        emit altitudeChanged();
    }
    if (_color != AnnotationInfo.color) {
        _color = AnnotationInfo.color;
        emit colorChanged();
    }

}

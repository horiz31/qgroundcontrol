/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AnnotationManager.h"
#include "QGCApplication.h"
#include <QDebug>

namespace
{
QString _getAnnotationFilePath()
{
    //Qt documentation tells us that this should never be empty
    return  QDir{QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).at(0)}.absoluteFilePath("annotations.dat");
}
}


QDataStream &operator<<(QDataStream &out, const Annotation::AnnotationInfo_t& annotationData)
{
    out << annotationData.uid;
    out << annotationData.displayName;
    out << annotationData.location;
    out << annotationData.altitude;
    out << annotationData.radius;
    out << annotationData.color;
    out << annotationData.type;

    return out;
}

QDataStream &operator>>(QDataStream &in, Annotation::AnnotationInfo_t& annotationData)
{
    QString displayName;
    QString uid;
    QGeoCoordinate location;
    double altitude;
    double radius;
    QColor color;
    QString type;
    in >> uid;
    in >> displayName;
    in >> location;
    in >> altitude;
    in >> radius;
    in >> color;
    in >> type;

    annotationData.uid = uid;
    annotationData.displayName = displayName;
    annotationData.location = location;
    annotationData.altitude = altitude;
    annotationData.radius = radius;
    annotationData.color = color;
    annotationData.type = type;

    return in;
}


AnnotationManager::AnnotationManager(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , _Annotations()
    , _annotationUidMap()
    , _measureDistanceStartUid()
{
}

void AnnotationManager::setToolbox(QGCToolbox* toolbox)
{
    QGCTool::setToolbox(toolbox);
    _load();

}

void AnnotationManager::deleteMarker(QString uid)
{
    for (int i=_Annotations.count()-1; i>=0; i--) {
        Annotation* annotation = _Annotations.value<Annotation*>(i);
        if (annotation->uid() == uid) {
            _Annotations.removeAt(i);
            _annotationUidMap.remove(annotation->uid());
            annotation->deleteLater();
            _save();
        }
    }
}

void AnnotationManager::setMeasureDistanceAnchor(QString const& uid)
{
    _measureDistanceStartUid = uid;
    emit hideMeasuredDistance();
    emit showMeasureDistanceMessage();
}

void AnnotationManager::calculateDistance(bool const isLMB, QGeoCoordinate const& endPoint)
{
    emit hideMeasureDistanceMessage();
    qreal distance = -1;
    QColor color{};
    QGeoCoordinate startPoint{};
    if( isLMB && endPoint.isValid() )
    {
        if( auto const findItr = _annotationUidMap.find(_measureDistanceStartUid); findItr != _annotationUidMap.end() )
        {
            if( auto const p_startPointAnnotation = findItr.value(); p_startPointAnnotation )
            {
                startPoint = p_startPointAnnotation->coordinate();
                if(startPoint.isValid() )
                {
                    distance = startPoint.distanceTo(endPoint);
                    color = p_startPointAnnotation->color();
                }
            }
        }
    }
    if( distance >= 0 )
    {
        emit showMeasuredDistance(startPoint, endPoint, color, distance);
    }
    else
    {
        emit hideMeasuredDistance();
    }
    _measureDistanceStartUid = "";
}

void AnnotationManager::annotationUpdate(const Annotation::AnnotationInfo_t annotation)
{
    QString uid = annotation.uid;

    if (_annotationUidMap.contains(uid)) {
         _annotationUidMap[uid]->update(annotation);
    } else {
            Annotation* myAnnotation = new Annotation(annotation, this);
            _annotationUidMap[uid] = myAnnotation;
            _Annotations.append(myAnnotation);
    }
    _save();
}

//save the annotations to a file
void AnnotationManager::_save()
{

    QString filePath = ::_getAnnotationFilePath();
    qDebug() << "saving " << _Annotations.count() << " annotations to file '"<<filePath<<"'";
    QFile file = filePath;
    file.setPermissions(QFileDevice::WriteUser|QFileDevice::ReadUser|QFileDevice::WriteOwner|QFileDevice::ReadOwner);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);   // we will serialize the data into the file
        //iterate through the annotations and write each as a line
        //first write the number of annoations
        qDebug() << "writing count of" << _Annotations.count();
        out << _Annotations.count();
        for( int i = 0; i < _Annotations.count(); i++ ) {
            Annotation* myAnnotation = (Annotation*)_Annotations.get(i);
            Annotation::AnnotationInfo_t temp;
            temp.uid = myAnnotation->uid();
            temp.displayName = myAnnotation->displayName();
            temp.location = myAnnotation->coordinate();
            temp.altitude = myAnnotation->altitude();
            temp.radius = myAnnotation->radius();
            temp.color = myAnnotation->color();
            temp.type = myAnnotation->type();
            qDebug() << "writing annotation"<<temp.displayName;
            out << temp;
        }

        file.close();
    }
    else
    {
        qDebug() << "file failed to open for writing" << file.errorString();
    }

}

void AnnotationManager::_load()
{
    QString filePath = ::_getAnnotationFilePath();
    qDebug() << "loading annotations from file '"<<filePath<<"'";
    QFile file = filePath;
    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);    // read the data serialized from the file

        int annotationCount;
        in >> annotationCount;
        qDebug() << "Got" << annotationCount << "annotations to read in";
        for( int i = 0; i < annotationCount; i++ ) {
            Annotation::AnnotationInfo_t annotation;
            in >> annotation;
            qDebug() << "Read in annoation" << annotation.displayName;
            Annotation* myAnnotation = new Annotation(annotation, this);
            _annotationUidMap[annotation.uid] = myAnnotation;
            _Annotations.append(myAnnotation);
        }
        file.close();
    }
 }

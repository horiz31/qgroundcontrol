/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCToolbox.h"
#include "QmlObjectListModel.h"
#include "Annotation.h"

#include <QGeoCoordinate>

class AnnotationManagerSettings;

class AnnotationManager : public QGCTool {
    Q_OBJECT

    friend QDataStream & operator<< (QDataStream& stream, const Annotation::AnnotationInfo_t& object);
    friend QDataStream & operator>> (QDataStream& stream, Annotation::AnnotationInfo_t& object);

public:
    AnnotationManager(QGCApplication* app, QGCToolbox* toolbox);

    Q_PROPERTY(QmlObjectListModel* annotations READ annotations CONSTANT)

    QmlObjectListModel* annotations(void) { return &_Annotations; }

    // QGCTool overrides
    void setToolbox(QGCToolbox* toolbox) final;
    Q_INVOKABLE void deleteMarker       (const QString uid);

public slots:
    void annotationUpdate  (const Annotation::AnnotationInfo_t annotationInfo);

private slots:
    void _save(void);
    void _load(void);


private:
    QmlObjectListModel              _Annotations;
    QMap<QString, Annotation*>      _annotationUidMap;
};


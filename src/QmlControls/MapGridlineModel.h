#ifndef MAPGRIDLINEMODEL_H
#define MAPGRIDLINEMODEL_H

#include <QAbstractListModel>
#include <QGeoCoordinate>
class QGCApplication;

struct MapGridline
{
    QList<QGeoCoordinate> path;
};

class MapGridlineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        PathRole = Qt::UserRole + 1
    };

    MapGridlineModel(QGCApplication* p_app);

    int rowCount(QModelIndex const& parent = QModelIndex()) const override;
    QVariant data(QModelIndex const& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateGridlines(QGeoCoordinate const& topLeft,
                                     QGeoCoordinate const& bottomRight,
                                     int zoomLevel);

private:
    QList<MapGridline> m_gridLines;
};

#endif // MAPGRIDLINEMODEL_H

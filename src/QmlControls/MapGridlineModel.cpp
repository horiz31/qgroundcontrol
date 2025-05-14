#include "MapGridlineModel.h"
#include "QGCApplication.h"

MapGridlineModel::MapGridlineModel(QGCApplication* p_app)
    : QAbstractListModel(p_app)
{
}

int MapGridlineModel::rowCount(QModelIndex const&) const
{
    return m_gridLines.size();
}

QVariant MapGridlineModel::data(QModelIndex const& index, int role) const
{
    QVariant returnVal{};
    if (role == PathRole && index.isValid() && index.row() < m_gridLines.size())
    {
        returnVal = QVariant::fromValue(m_gridLines[index.row()].path);
    }
    return returnVal;
}

QHash<int, QByteArray> MapGridlineModel::roleNames() const
{
    return {{PathRole, "path"}};
}

void MapGridlineModel::updateGridlines(QGeoCoordinate const& topLeft,
                                       QGeoCoordinate const& bottomRight,
                                       int zoomLevel)
{
    beginResetModel();
    m_gridLines.clear();

    double latStep = (zoomLevel > 10) ? 0.01 : 0.1;
    double lonStep = (zoomLevel > 10) ? 0.01 : 0.1;

    for (double lat = topLeft.latitude(); lat > bottomRight.latitude(); lat -= latStep)
    {
        MapGridline line;
        line.path.append(QGeoCoordinate(lat, topLeft.longitude()));
        line.path.append(QGeoCoordinate(lat, bottomRight.longitude()));
        m_gridLines.append(line);
    }
    for (double lon = topLeft.longitude(); lon < bottomRight.longitude(); lon += lonStep)
    {
        MapGridline line;
        line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
        line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
        m_gridLines.append(line);
    }
    //qCritical() << "updateGridlines(" << topLeft << ", " << bottomRight << ", " << zoomLevel
    //            << "), size=" << m_gridLines.size();

    endResetModel();
}

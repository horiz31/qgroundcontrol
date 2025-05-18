#ifndef MAPGRIDLINEMODEL_H
#define MAPGRIDLINEMODEL_H

#include <QAbstractListModel>
#include <QGeoCoordinate>
class QGCApplication;

enum GridlinePrecisionLevel
{
    LatLong = -1,
    GZD = 0,
    Km100,
    km10,
    km1,
    m100,
    m10,
    m1
};

struct MapGridline
{
    QList<QGeoCoordinate> path;
    int precision;
};

class MapGridlineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        PathRole = Qt::UserRole + 1,
        PrecisionRole = Qt::UserRole + 2,
    };

    MapGridlineModel(QGCApplication* p_app);

    int rowCount(QModelIndex const& parent = QModelIndex()) const override;
    QVariant data(QModelIndex const& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateGridlines(QGeoCoordinate topLeft,
                                     QGeoCoordinate bottomRight,
                                     double zoomLevel);
    Q_INVOKABLE void clearGridlines();

private:
    bool _isMGRS() const;

    void _drawUTMPGrid(QGeoCoordinate const& topLeft,
                       QGeoCoordinate const& bottomRight,
                       double zoomLevel);

    void _drawUTMP100km(double bLat,
                        double lLon,
                        QGeoCoordinate const& topLeft,
                        QGeoCoordinate const& bottomRight,
                        double zoomLevel,
                        int lonZone,
                        int latBand);

    void _drawLatLines(QGeoCoordinate const& topLeft,
                       QGeoCoordinate const& bottomRight,
                       double latLonStep);

    void _drawLonLines(QGeoCoordinate const& topLeft,
                       QGeoCoordinate const& bottomRight,
                       double latLonStep);

    void _NorwayException(double lat,
                          double lon,
                          int lonZone,
                          bool overrun,
                          QGeoCoordinate const& topLeft,
                          QGeoCoordinate const& bottomRight,
                          double zoomLevel);

    void _SvalbardException(double lat,
                            double lon,
                            int lonZone,
                            bool overrun,
                            QGeoCoordinate const& topLeft,
                            QGeoCoordinate const& bottomRight,
                            double zoomLevel);

    void _MainGZDRoutine(double lat,
                         double lon,
                         int latBand,
                         int lonZone,
                         bool overrun,
                         QGeoCoordinate const& topLeft,
                         QGeoCoordinate const& bottomRight,
                         double zoomLevel);

    QList<MapGridline> m_gridLines;
    mutable std::recursive_mutex m_mut;
};

#endif // MAPGRIDLINEMODEL_H

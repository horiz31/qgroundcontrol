#include "MapGridlineModel.h"
#include "GPSUnitsController.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include "UnitsSettings.h"
#include <limits>

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

void MapGridlineModel::clearGridlines()
{
    beginResetModel();
    m_gridLines.clear();
    endResetModel();
}

void MapGridlineModel::updateGridlines(QGeoCoordinate const& topLeft,
                                       QGeoCoordinate const& bottomRight,
                                       double zoomLevel)
{
    beginResetModel();
    m_gridLines.clear();
    if (zoomLevel > 4)
    {
        double const verticalDist = topLeft.latitude() - bottomRight.latitude();

        bool overrun = false;
        double horizontalDist = bottomRight.longitude() - topLeft.longitude();
        if (bottomRight.longitude() < topLeft.longitude())
        {
            overrun = true;
            horizontalDist += 360.0;
        }

#if 0
        if (qgcApp()
                ->toolbox()
                ->settingsManager()
                ->unitsSettings()
                ->geoCoordinateSystem()
                ->rawValue()
                .toUInt()
            == (uint32_t) UnitsSettings::MGRS)
        {

//MGRS not yet supported
            double topLat = std::min<double>(84,topLeft.latitude());
            double botLat = std::max<double,bottomRight.latitude();
            double leftLong = topLeft.longitude();
            double rightLong = bottomRight.longitude();
            
            GPSUnitsController coordinateConverter{}
            auto const topLeftMGRS =  coordinateConverter.convertToMGRS(topLeft.latitude(), topLeft.longitude(), 5);
            auto const bottomRightMGRS =  coordinateConverter.convertToMGRS(bottomRight.latitude(), bottomRight.longitude(), 5);
            
            {
                //draw the grid zone designators
                double latStep = 8;
                double lonStep = 6;
                double latStart = std::floor(topLeft.latitude() / latStep) * latStep;
                double lonStart = std::ceil(topLeft.longitude() / lonStep) * lonStep;
                for (double lat = latStart; lat > bottomRight.latitude(); lat -= latStep)
                {
                    MapGridline line;
                    if (overrun)
                    {
                        line.path.append(QGeoCoordinate(lat, bottomRight.longitude()));
                        line.path.append(QGeoCoordinate(lat, topLeft.longitude()));
                    }
                    else
                    {
                        line.path.append(QGeoCoordinate(lat, topLeft.longitude()));
                        line.path.append(QGeoCoordinate(lat, bottomRight.longitude()));
                    }

                    m_gridLines.append(line);
                }
                if (overrun)
                {
                    double lon = lonStart;
                    for (; lon < 180; lon += lonStep)
                    {
                        MapGridline line;
                        line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                        line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                        m_gridLines.append(line);
                    }
                    for (lon -= 360; lon < bottomRight.longitude(); lon += lonStep)
                    {
                        MapGridline line;
                        line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                        line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                        m_gridLines.append(line);
                    }
                }
                else
                {
                    for (double lon = lonStart; lon < bottomRight.longitude(); lon += lonStep)
                    {
                        MapGridline line;
                        line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                        line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                        m_gridLines.append(line);
                    }
                }
            }
            if (zoomLevel <= 6)
            {
                //print the 100km squares
                
            }
            else if (zoomLevel <= 7)
            {
                //print the 10km squares
            }
            else if (zoomLevel <= 8)
            {
                //print the 1km squares
            }
            else if (zoomLevel <= 9)
            {
                //print the 100m squares
            }
            else if (zoomLevel <= 10)
            {
                //print the 10m squares
            }
            else
            {
                //print the 1m squares
            }

        }
        else
#endif
        {
            double latLonStep = -1;
            if (zoomLevel <= 4.7)
            {
                latLonStep = 10;
            }
            else if (zoomLevel <= 5.9)
            {
                latLonStep = 5;
            }
            else if (zoomLevel <= 7.1)
            {
                latLonStep = 2;
            }
            else if (zoomLevel <= 8.0)
            {
                latLonStep = 1;
            }
            else if (zoomLevel <= 9.1)
            {
                latLonStep = 0.5;
            }
            else if (zoomLevel <= 10.5)
            {
                latLonStep = 0.2;
            }
            else if (zoomLevel <= 11.5)
            {
                latLonStep = 0.1;
            }
            else if (zoomLevel <= 12.3)
            {
                latLonStep = 0.05;
            }
            else if (zoomLevel <= 13.7)
            {
                latLonStep = 0.02;
            }
            else if (zoomLevel <= 14.7)
            {
                latLonStep = 0.01;
            }
            else if (zoomLevel <= 15.7)
            {
                latLonStep = 0.005;
            }
            else if (zoomLevel <= 17)
            {
                latLonStep = 0.002;
            }
            else if (zoomLevel <= 18)
            {
                latLonStep = 0.001;
            }
            else if (zoomLevel <= 19.1)
            {
                latLonStep = 0.0005;
            }
            else if (zoomLevel <= 20.4)
            {
                latLonStep = 0.0002;
            }
            else if (zoomLevel <= 21.3)
            {
                latLonStep = 0.0001;
            }
            else if (zoomLevel <= 22.3)
            {
                latLonStep = 0.00005;
            }
            else
            {
                latLonStep = 0.00002;
            }
            double latStart = std::floor(topLeft.latitude() / latLonStep) * latLonStep;
            double lonStart = std::ceil(topLeft.longitude() / latLonStep) * latLonStep;
            for (double lat = latStart; lat > bottomRight.latitude(); lat -= latLonStep)
            {
                MapGridline line;
                if (overrun)
                {
                    line.path.append(QGeoCoordinate(lat, bottomRight.longitude()));
                    line.path.append(QGeoCoordinate(lat, topLeft.longitude()));
                }
                else
                {
                    line.path.append(QGeoCoordinate(lat, topLeft.longitude()));
                    line.path.append(QGeoCoordinate(lat, bottomRight.longitude()));
                }

                m_gridLines.append(line);
            }
            if (overrun)
            {
                double lon = lonStart;
                for (; lon < 180; lon += latLonStep)
                {
                    MapGridline line;
                    line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                    line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                    m_gridLines.append(line);
                }
                for (lon -= 360; lon < bottomRight.longitude(); lon += latLonStep)
                {
                    MapGridline line;
                    line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                    line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                    m_gridLines.append(line);
                }
            }
            else
            {
                for (double lon = lonStart; lon < bottomRight.longitude(); lon += latLonStep)
                {
                    MapGridline line;
                    line.path.append(QGeoCoordinate(topLeft.latitude(), lon));
                    line.path.append(QGeoCoordinate(bottomRight.latitude(), lon));
                    m_gridLines.append(line);
                }
            }
            qCritical() << "updateGridlines(" << topLeft << ", " << bottomRight << ", " << zoomLevel
                        << "), size=" << m_gridLines.size() << ", latLonStep=" << latLonStep
                        << ", latStart=" << latStart << ", lonStart=" << lonStart
                        << ", horizontalDist=" << horizontalDist
                        << ", verticalDist=" << verticalDist << ", overrun=" << overrun;
        }
    }
    endResetModel();
}

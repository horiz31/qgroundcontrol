#include "MapGridlineModel.h"
#include "MGRS.hpp"
#include "PolarStereographic.hpp"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include "TransverseMercator.hpp"
#include "UTMUPS.hpp"
#include "UnitsSettings.h"
#include "Utility.hpp"
#include <complex>
#include <limits>
#include <cfloat>

namespace
{
using namespace GeographicLib;
static const char* const upsband_ = "ABYZ";
static const char* const latband_ = "CDEFGHJKLMNPQRSTUVWX";
//static const char* const hemispheres_ = "SN";

enum
{
    base_ = 10,
    // Top-level tiles are 10^5 m = 100 km on a side
    tilelevel_ = 5,
    // Period of UTM row letters
    utmrowperiod_ = 20,
    // Row letters are shifted by 5 for even zones
    utmevenrowshift_ = 5,
    // Maximum precision is um
    maxprec_ = 5 + 6,
    // For generating digits at maxprec
    mult_ = 1000000,
};

enum
{
    tile_ = 100000, // Size MGRS blocks
    minutmcol_ = 1,
    maxutmcol_ = 9,
    minutmSrow_ = 10,
    maxutmSrow_ = 100, // Also used for UTM S false northing
    minutmNrow_ = 0,   // Also used for UTM N false northing
    maxutmNrow_ = 95,
    minupsSind_ = 8, // These 4 ind's apply to easting and northing
    maxupsSind_ = 32,
    minupsNind_ = 13,
    maxupsNind_ = 27,
    upseasting_ = 20, // Also used for UPS false northing
    utmeasting_ = 5,  // UTM false easting
    // Difference between S hemisphere northing and N hemisphere northing
    utmNshift_ = (maxutmSrow_ - minutmNrow_) * tile_
};

static const int mineasting_[] = {minupsSind_, minupsNind_, minutmcol_, minutmcol_};
static const int maxeasting_[] = {maxupsSind_, maxupsNind_, maxutmcol_, maxutmcol_};
static const int minnorthing_[] = {minupsSind_,
                                   minupsNind_,
                                   minutmSrow_,
                                   minutmSrow_ - (maxutmSrow_ - minutmNrow_)};
static const int maxnorthing_[] = {maxupsSind_,
                                   maxupsNind_,
                                   maxutmNrow_ + (maxutmSrow_ - minutmNrow_),
                                   maxutmNrow_};

static const int falseeasting_[] = {upseasting_ * tile_,
                                    upseasting_* tile_,
                                    utmeasting_* tile_,
                                    utmeasting_* tile_};
static const int falsenorthing_[] = {upseasting_ * tile_,
                                     upseasting_* tile_,
                                     maxutmSrow_* tile_,
                                     minutmNrow_* tile_};

static const char* const utmcols_[] = {"ABCDEFGH", "JKLMNPQR", "STUVWXYZ"};
static const char* const utmrow_ = "ABCDEFGHJKLMNPQRSTUV";
static const char* const upscols_[] = {"JKLPQRSTUXYZ", "ABCFGHJKLPQR", "RSTUXYZ", "ABCFGHJ"};
static const char* const upsrows_[] = {"ABCDEFGHJKLMNPQRSTUVWXYZ", "ABCDEFGHJKLMNP"};

enum zonespec
{
    /**
       * The smallest pseudo-zone number.
       **********************************************************************/
    MINPSEUDOZONE = -4,
    /**
       * A marker for an undefined or invalid zone.  Equivalent to NaN.
       **********************************************************************/
    INVALID = -4,
    /**
       * If a coordinate already include zone information (e.g., it is an MGRS
       * coordinate), use that, otherwise apply the UTMUPS::STANDARD rules.
       **********************************************************************/
    MATCH = -3,
    /**
       * Apply the standard rules for UTM zone assigment extending the UTM zone
       * to each pole to give a zone number in [1, 60].  For example, use UTM
       * zone 38 for longitude in [42&deg;, 48&deg;).  The rules include the
       * Norway and Svalbard exceptions.
       **********************************************************************/
    UTM = -2,
    /**
       * Apply the standard rules for zone assignment to give a zone number in
       * [0, 60].  If the latitude is not in [&minus;80&deg;, 84&deg;), then
       * use UTMUPS::UPS = 0, otherwise apply the rules for UTMUPS::UTM.  The
       * tests on latitudes and longitudes are all closed on the lower end open
       * on the upper.  Thus for UTM zone 38, latitude is in [&minus;80&deg;,
       * 84&deg;) and longitude is in [42&deg;, 48&deg;).
       **********************************************************************/
    STANDARD = -1,
    /**
       * The largest pseudo-zone number.
       **********************************************************************/
    MAXPSEUDOZONE = -1,
    /**
       * The smallest physical zone number.
       **********************************************************************/
    MINZONE = 0,
    /**
       * The zone number used for UPS
       **********************************************************************/
    UPS = 0,
    /**
       * The smallest UTM zone number.
       **********************************************************************/
    MINUTMZONE = 1,
    /**
       * The largest UTM zone number.
       **********************************************************************/
    MAXUTMZONE = 60,
    /**
       * The largest physical zone number.
       **********************************************************************/
    MAXZONE = 60,
};

static int _LatitudeBand(double lat)
{
    int ilat = int(std::floor(lat));
    return (std::max)(-10, (std::min)(9, ((ilat + 80) / 8) - 10));
}

struct _MGRSData
{
    int zone = INVALID;
    int gridDesignatorX = -1;

    char gridDesignatorY = 0;

    char km100X = 0;
    char km100Y = 0;
    int km10x = 0;
    int km10y = 0;
    int km1x = 0;
    int km1y = 0;
    int m100x = 0;
    int m100y = 0;
    int m10x = 0;
    int m10y = 0;
    int m1x = 0;
    int m1y = 0;
    //double lat=0;
    //double lon=0;
    bool northern = false;
    //double x=0;
    //double y=0;

    QString toString() const
    {
        std::stringstream ss;
        if (isUTMP())
        {
            ss << gridDesignatorX;
        }
        ss << gridDesignatorY;
        ss << km100X;
        ss << km100Y;
        ss << " ";

        ss << km10x;
        ss << km1x;
        ss << m100x;
        ss << m10x;
        ss << m1x;
        ss << " ";

        ss << km10y;
        ss << km1y;
        ss << m100y;
        ss << m10y;
        ss << m1y;
        return QString::fromStdString(ss.str());
    }

    QGeoCoordinate getGeo(GridlinePrecisionLevel precision) const
    {
        QGeoCoordinate coord;
        if (isValid())
        {
            if (precision == GridlinePrecisionLevel::LatLong)
            {
                precision = GridlinePrecisionLevel::GZD;
            }

            try
            {
                double lat = 0;
                double lon = 0;
                assert(zone >= MINZONE && zone <= MAXZONE);
                //if (!(zone >= MINZONE && zone <= MAXZONE))
                //{
                //    throw GeographicErr("Zone " + Utility::str(zone) + " not in range [0, 60]");
                //}
                double x;
                double y;

                //GeographicLib::MGRS::Reverse(mgrs.simplified().replace(" ", "").toStdString(), zone, northp, x, y, prec);
                {
                    bool utmp = zone != UTMUPS::UPS;
                    int zonem1 = zone - 1;
                    const char* band = utmp ? latband_ : upsband_;
                    int iband = Utility::lookup(band, this->gridDesignatorY);
                    assert(iband >= 0);
                    //if (iband < 0)
                    //{
                    //    throw GeographicErr("Band letter " + Utility::str(this->gridDesignatorY)
                    //                        + " not in " + (utmp ? "UTM" : "UPS") + " set " + band);
                    //}
                    if (precision == GridlinePrecisionLevel::GZD)
                    {
                        // Approx length of a degree of meridian arc in units of tile.
                        double deg = double(utmNshift_) / (90 * tile_);
                        if (utmp)
                        {
                            // Pick central meridian except for 31V
                            x = ((zone == 31 && iband == 17) ? 4 : 5) * tile_;
                            // Pick center of 8deg latitude bands
                            y = floor(8 * (iband - double(9.5)) * deg + double(0.5)) * tile_
                                + (northern ? 0 : utmNshift_);
                        }
                        else
                        {
                            // Pick point at lat 86N or 86S
                            x = ((iband & 1 ? 1 : -1) * floor(4 * deg + double(0.5)) + upseasting_)
                                * tile_;
                            // Pick point at lon 90E or 90W.
                            y = upseasting_ * tile_;
                        }
                    }
                    else
                    {
                        const char* col = utmp ? utmcols_[zonem1 % 3] : upscols_[iband];
                        const char* row = utmp ? utmrow_ : upsrows_[northern];
                        int icol = Utility::lookup(col, km100X);
                        assert(icol >= 0);
                        //if (icol < 0)
                        //{
                        //    throw GeographicErr("Column letter " + Utility::str(km100X) + " not in "
                        //                        + (utmp ? "zone " : "UPS band ") + " set " + col);
                        //}
                        int irow = Utility::lookup(row, km100Y);
                        assert(irow >= 0);
                        //if (irow < 0)
                        //{
                        //    throw GeographicErr(
                        //        "Row letter " + Utility::str(km100Y) + " not in "
                        //        + (utmp ? "UTM" : "UPS " + Utility::str(hemispheres_[northern]))
                        //        + " set " + row);
                        //}
                        if (utmp)
                        {
                            if (zonem1 & 1)
                            {
                                irow = (irow + utmrowperiod_ - utmevenrowshift_) % utmrowperiod_;
                            }
                            iband -= 10;
                            irow = _UTMRow(iband, icol, irow);
                            assert(irow != maxutmSrow_);
                            //if (irow == maxutmSrow_)
                            //{
                            //    throw GeographicErr("Block not in zone/band ");
                            //}
                            irow = northern ? irow : irow + 100;
                            icol = icol + minutmcol_;
                        }
                        else
                        {
                            bool eastp = iband & 1;
                            icol += eastp ? upseasting_ : (northern ? minupsNind_ : minupsSind_);
                            irow += northern ? minupsNind_ : minupsSind_;
                        }
                        double unit = 1;
                        double x1 = icol;
                        double y1 = irow;
                        if (precision >= GridlinePrecisionLevel::km10)
                        {
                            unit *= base_;
                            x1 = base_ * x1 + this->km10x;
                            y1 = base_ * y1 + this->km10y;
                            if (precision >= GridlinePrecisionLevel::km1)
                            {
                                unit *= base_;
                                x1 = base_ * x1 + this->km1x;
                                y1 = base_ * y1 + this->km1y;
                                if (precision >= GridlinePrecisionLevel::m100)
                                {
                                    unit *= base_;
                                    x1 = base_ * x1 + this->m100x;
                                    y1 = base_ * y1 + this->m100y;
                                    if (precision >= GridlinePrecisionLevel::m10)
                                    {
                                        unit *= base_;
                                        x1 = base_ * x1 + this->m10x;
                                        y1 = base_ * y1 + this->m10y;
                                        if (precision >= GridlinePrecisionLevel::m1)
                                        {
                                            unit *= base_;
                                            x1 = base_ * x1 + this->m1x;
                                            y1 = base_ * y1 + this->m1y;
                                        }
                                    }
                                }
                            }
                        }
                        unit *= 2;
                        x = (tile_ * (2 * x1 + 1)) / unit;
                        y = (tile_ * (2 * y1 + 1)) / unit;
                    }
                }
                //GeographicLib::UTMUPS::Reverse(zone, northern, x, y, lat, lon);
                {
                    int ind = (zone == UPS ? 0 : 2) + (northern ? 1 : 0);
                    x -= falseeasting_[ind];
                    y -= falsenorthing_[ind];
                    double gamma;
                    double k;
                    if (zone == UPS)
                    {
                        PolarStereographic::UPS().Reverse(northern, x, y, lat, lon, gamma, k);
                    }
                    else
                    {
                        TransverseMercator::UTM()
                            .Reverse(_CentralMeridian(), x, y, lat, lon, gamma, k);
                    }
                }
                coord.setLatitude(lat);
                coord.setLongitude(lon);
            }
            catch (...)
            {
            }
        }
        return coord;
    }

    _MGRSData(QGeoCoordinate const& coord)
        : _MGRSData{coord.latitude(), coord.longitude()}
    {
    }

    _MGRSData(double lat, double lon)
    {
        double x;
        double y;
        //GeographicLib::UTMUPS::Forward(lat, lon, zone, northern, x, y);
        {
            assert(abs(lat) <= 90);
            //if (abs(lat) > 90)
            //    throw GeographicErr("Latitude " + Utility::str(lat)
            //                        + "d not in [-90d, 90d]");
            northern = lat >= 0;
            zone = _getZone(lat, lon);
            if (zone == UPS)
            {
                assert(abs(lat) >= 70);
                //if (abs(lat) < 70)
                //    // Check isn't really necessary ... (see above).
                //    throw GeographicErr("Latitude " + Utility::str(lat) + "d more than 20d from "
                //                        + (northp1 ? "N" : "S") + " pole");
                double gamma;
                double k;
                PolarStereographic::UPS().Forward(northern, lat, lon, x, y, gamma, k);
            }
            else
            {
                double lon0 = _CentralMeridian();
                double dlon = lon - lon0;
                dlon = abs(dlon - 360 * floor((dlon + 180) / 360));
                assert(dlon <= 60);
                //if (!(dlon <= 60))
                //{
                //    // Check isn't really necessary because CheckCoords catches this case.
                //    // But this allows a more meaningful error message to be given.
                //    throw GeographicErr("Longitude " + Utility::str(lon)
                //                        + "d more than 60d from center of UTM zone "
                //                        + Utility::str(zone1));
                //}
                double gamma;
                double k;
                TransverseMercator::UTM().Forward(lon0, lat, lon, x, y, gamma, k);
            }
            int ind = (zone == UPS ? 0 : 2) + (northern ? 1 : 0);
            x += falseeasting_[ind];
            y += falsenorthing_[ind];
            //if (!CheckCoords(zone1 != UPS, northp1, x1, y1, false, false))
            //{
            //    throw GeographicErr("Latitude " + Utility::str(lat) + ", longitude "
            //                        + Utility::str(lon) + " out of legal range for "
            //                        + (utmp ? "UTM zone " + Utility::str(zone1) : "UPS"));
            //}
        }
        //GeographicLib::MGRS::Forward(zone, northern, x, y, lat, 5, mgrs);
        {
            //_CheckCoords(x, y);
            {
                // Limits are all multiples of 100km and are all closed on the lower end
                // and open on the upper end -- and this is reflected in the error
                // messages.  However if a coordinate lies on the excluded upper end (e.g.,
                // after rounding), it is shifted down by eps.  This also folds UTM
                // northings to the correct N/S hemisphere.

                // The smallest length s.t., 1.0e7 - eps() < 1.0e7 (approx 1.9 nm)
                // 25 = ceil(log_2(2e7)) -- use half circumference here because
                // northing 195e5 is a legal in the "southern" hemisphere.
                static const double eps = ldexp(double(1), -(Math::digits() - 25));
                int ix = int(floor(x / tile_));
                int iy = int(floor(y / tile_));
                int ind = (zone == UPS ? 0 : 2) + (northern ? 1 : 0);
                if (!(ix >= mineasting_[ind] && ix < maxeasting_[ind]))
                {
                    assert(ix == maxeasting_[ind] && x == maxeasting_[ind] * tile_);
                    //if (ix == maxeasting_[ind] && x == maxeasting_[ind] * tile_)
                    {
                        x -= eps;
                    }
                    //else
                    //{
                    //    throw GeographicErr(
                    //        "Easting " + Utility::str(int(floor(x / 1000))) + "km not in MGRS/"
                    //        + (utmp ? "UTM" : "UPS") + " range for " + (northern ? "N" : "S")
                    //        + " hemisphere [" + Utility::str(mineasting_[ind] * tile_ / 1000)
                    //        + "km, " + Utility::str(maxeasting_[ind] * tile_ / 1000) + "km)");
                    //}
                }
                if (!(iy >= minnorthing_[ind] && iy < maxnorthing_[ind]))
                {
                    assert(iy == maxnorthing_[ind] && y == maxnorthing_[ind] * tile_);
                    //if (iy == maxnorthing_[ind] && y == maxnorthing_[ind] * tile_)
                    {
                        y -= eps;
                    }
                    //else
                    //{
                    //    throw GeographicErr(
                    //        "Northing " + Utility::str(int(floor(y / 1000))) + "km not in MGRS/"
                    //        + (utmp ? "UTM" : "UPS") + " range for " + (northern ? "N" : "S")
                    //        + " hemisphere [" + Utility::str(minnorthing_[ind] * tile_ / 1000)
                    //        + "km, " + Utility::str(maxnorthing_[ind] * tile_ / 1000) + "km)");
                    //}
                }

                // Correct the UTM northing and hemisphere if necessary
                if (zone != UPS)
                {
                    if (northern && iy < minutmNrow_)
                    {
                        northern = false;
                        y += utmNshift_;
                    }
                    else if (!northern && iy >= maxutmSrow_)
                    {
                        if (y == maxutmSrow_ * tile_)
                        {
                            // If on equator retain S hemisphere
                            y -= eps;
                        }
                        else
                        {
                            northern = true;
                            y -= utmNshift_;
                        }
                    }
                }
            }
            assert(zone >= UTMUPS::MINZONE && zone <= UTMUPS::MAXZONE);
            //if (!(zone >= UTMUPS::MINZONE && zone <= UTMUPS::MAXZONE))
            //{
            //    throw GeographicErr("Zone " + Utility::str(zone) + " not in [0,60]");
            //}
            if (zone == UPS)
            {
                gridDesignatorX = -1;
            }
            else
            {
                gridDesignatorX = (int(zone / base_) * base_) + int(zone % base_);
            }
            // The C++ standard mandates 64 bits for long long.  But
            // check, to make sure.
            GEOGRAPHICLIB_STATIC_ASSERT(std::numeric_limits<long long>::digits >= 44,
                                        "long long not wide enough to store 10e12");
            long long ix = (long long) (floor(x * mult_));
            long long iy = (long long) (floor(y * mult_));
            long long m = (long long) (mult_) * (long long) (tile_);
            int xh = int(ix / m);
            int yh = int(iy / m);
            if (zone == UPS)
            {
                bool eastp = xh >= upseasting_;
                int iband = (northern ? 2 : 0) + (eastp ? 1 : 0);
                gridDesignatorY = upsband_[iband];
                km100X
                    = upscols_[iband]
                              [xh - (eastp ? upseasting_ : (northern ? minupsNind_ : minupsSind_))];
                km100Y = upsrows_[northern][yh - (northern ? minupsNind_ : minupsSind_)];
            }
            else
            {
                // The smallest angle s.t., 90 - angeps() < 90 (approx 50e-12 arcsec)
                // 7 = ceil(log_2(90))
                static const double angeps = ldexp(double(1), -(Math::digits() - 7));
                // Correct fuzziness in latitude near equator
                int iband = abs(lat) > angeps ? _LatitudeBand(lat) : (northern ? 0 : -1);
                int icol = xh - minutmcol_;
                //int irow = _UTMRow(iband, icol, yh % utmrowperiod_);
                //assert(irow == yh - (northern ? minutmNrow_ : maxutmSrow_));
                //if (irow != yh - (northern ? minutmNrow_ : maxutmSrow_))
                //{
                //    throw GeographicErr("Latitude " + Utility::str(lat)
                //                        + " is inconsistent with UTM coordinates");
                //}
                int zone1 = zone - 1;
                gridDesignatorY = latband_[10 + iband];
                km100X = utmcols_[zone1 % 3][icol];
                km100Y = utmrow_[(yh + (zone1 & 1 ? utmevenrowshift_ : 0)) % utmrowperiod_];
            }
            long long d = (long long) (pow(double(base_), maxprec_ - 5));
            km10x = (ix = (ix - (m * xh)) / d) % base_;
            km10x = (iy = (iy - (m * yh)) / d) % base_;
            km1x = (ix /= base_) % base_;
            km1y = (iy /= base_) % base_;
            m100x = (ix /= base_) % base_;
            m100y = (iy /= base_) % base_;
            m10x = (ix /= base_) % base_;
            m10y = (iy /= base_) % base_;
            m1x = (ix / base_) % base_;
            m1y = (iy / base_) % base_;
        }
    }

    bool isValid() const { return zone != INVALID; }
    bool isUTMP() const { return isValid() && zone != UPS; }

private:
    static int _UTMRow(int iband, int icol, int irow)
    {
        // Input is iband = band index in [-10, 10) (as returned by LatitudeBand),
        // icol = column index in [0,8) with origin of easting = 100km, and irow =
        // periodic row index in [0,20) with origin = equator.  Output is true row
        // index in [-90, 95).  Returns maxutmSrow_ = 100, if irow and iband are
        // incompatible.

        // Estimate center row number for latitude band
        // 90 deg = 100 tiles; 1 band = 8 deg = 100*8/90 tiles
        double c = 100 * (8 * iband + 4) / double(90);
        bool northp = iband >= 0;
        // These are safe bounds on the rows
        //  iband minrow maxrow
        //   -10    -90    -81
        //    -9    -80    -72
        //    -8    -71    -63
        //    -7    -63    -54
        //    -6    -54    -45
        //    -5    -45    -36
        //    -4    -36    -27
        //    -3    -27    -18
        //    -2    -18     -9
        //    -1     -9     -1
        //     0      0      8
        //     1      8     17
        //     2     17     26
        //     3     26     35
        //     4     35     44
        //     5     44     53
        //     6     53     62
        //     7     62     70
        //     8     71     79
        //     9     80     94
        int minrow = iband > -10 ? int(floor(c - double(4.3) - double(0.1) * northp)) : -90;
        int maxrow = iband < 9 ? int(floor(c + double(4.4) - double(0.1) * northp)) : 94;
        int baserow = (minrow + maxrow) / 2 - utmrowperiod_ / 2;
        // Offset irow by the multiple of utmrowperiod_ which brings it as close as
        // possible to the center of the latitude band, (minrow + maxrow) / 2.
        // (Add maxutmSrow_ = 5 * utmrowperiod_ to ensure operand is positive.)
        irow = (irow - baserow + maxutmSrow_) % utmrowperiod_ + baserow;
        if (!(irow >= minrow && irow <= maxrow))
        {
            // Outside the safe bounds, so need to check...
            // Northing = 71e5 and 80e5 intersect band boundaries
            //   y = 71e5 in scol = 2 (x = [3e5,4e5] and x = [6e5,7e5])
            //   y = 80e5 in scol = 1 (x = [2e5,3e5] and x = [7e5,8e5])
            // This holds for all the ellipsoids given in NGA.SIG.0012_2.0.0_UTMUPS.
            // The following deals with these special cases.
            // Fold [-10,-1] -> [9,0]
            int sband = iband >= 0 ? iband : -iband - 1;
            // Fold [-90,-1] -> [89,0]
            int srow = irow >= 0 ? irow : -irow - 1;
            // Fold [4,7] -> [3,0]
            int scol = icol < 4 ? icol : -icol + 7;
            // For example, the safe rows for band 8 are 71 - 79.  However row 70 is
            // allowed if scol = [2,3] and row 80 is allowed if scol = [0,1].
            if (!((srow == 70 && sband == 8 && scol >= 2) || (srow == 71 && sband == 7 && scol <= 2)
                  || (srow == 79 && sband == 9 && scol >= 1)
                  || (srow == 80 && sband == 8 && scol <= 1)))
            {
                irow = maxutmSrow_;
            }
        }
        return irow;
    }

    static double _getLatitudeBandFloor(double lat)
    {
        int band = _LatitudeBand(lat);

        return double(((band + 10) * 8) - 80);
    }

    static int _getZone(double lat, double lon)
    {
        if ((lat >= -80 && lat < 84))
        {
            int ilon = int(floor(Math::AngNormalize(lon)));
            if (ilon == 180)
            {
                ilon = -180; // ilon now in [-180,180)
            }
            int zone = (ilon + 186) / 6;
            int band = _LatitudeBand(lat);
            if (band == 7 && zone == 31 && ilon >= 3)
            { // The Norway exception
                zone = 32;
            }
            else if (band == 9 && ilon >= 0 && ilon < 42)
            { // The Svalbard exception
                zone = 2 * ((ilon + 183) / 12) + 1;
            }
            return zone;
        }
        else
        {
            return UPS;
        }
    }

    double _CentralMeridian() const { return double(6 * zone - 183); }
};

static double _getLatLonStep(double zoomLevel)
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
    return latLonStep;
}
static MapGridline _getLatLine(double lat, QGeoCoordinate const& tl, QGeoCoordinate const& br)
{
    MapGridline line;
    line.path.append(QGeoCoordinate(lat, tl.longitude()));
    line.path.append(QGeoCoordinate(lat, br.longitude()));
    return line;
}

static MapGridline _getLonLine(double topLat, double botLat, double lon)
{
    MapGridline line;
    line.path.append(QGeoCoordinate(botLat, lon));
    line.path.append(QGeoCoordinate(topLat, lon));
    return line;
}
} // namespace

MapGridlineModel::MapGridlineModel(QGCApplication* p_app)
    : QAbstractListModel(p_app)
{
}

int MapGridlineModel::rowCount(QModelIndex const&) const
{
    //std::lock_guard<std::recursive_mutex> lock(m_mut);
    return m_gridLines.size();
}

QVariant MapGridlineModel::data(QModelIndex const& index, int role) const
{
    //std::lock_guard<std::recursive_mutex> lock(m_mut);
    QVariant returnVal{};
    if (index.isValid() && index.row() < m_gridLines.size())
    {
        if (role == PathRole)
        {
            returnVal = QVariant::fromValue(m_gridLines[index.row()].path);
        }
        else
        {
            returnVal = QVariant::fromValue(m_gridLines[index.row()].precision);
        }
    }
    return returnVal;
}

QHash<int, QByteArray> MapGridlineModel::roleNames() const
{
    return {{PathRole, "path"}, {PrecisionRole, "precision"}};
}

void MapGridlineModel::clearGridlines()
{
    //std::lock_guard<std::recursive_mutex> lock(m_mut);
    beginResetModel();
    m_gridLines.clear();
    endResetModel();
}

bool MapGridlineModel::_isMGRS() const
{
    return qgcApp()
               ->toolbox()
               ->settingsManager()
               ->unitsSettings()
               ->geoCoordinateSystem()
               ->rawValue()
               .toUInt()
           == (uint32_t) UnitsSettings::MGRS;
}

void MapGridlineModel::_drawLatLines(QGeoCoordinate const& topLeft,
                                     QGeoCoordinate const& bottomRight,
                                     double latLonStep)
{
    for (auto lat = std::floor(topLeft.latitude() / latLonStep) * latLonStep;
         lat > bottomRight.latitude();
         lat -= latLonStep)
    {
        auto line = _getLatLine(lat, topLeft, bottomRight);
        line.precision = GridlinePrecisionLevel::LatLong;
        m_gridLines.append(line);
    }
}

void MapGridlineModel::_drawLonLines(QGeoCoordinate const& topLeft,
                                     QGeoCoordinate const& bottomRight,
                                     double latLonStep)
{
    //draw the longitude lines from left to right
    auto lon = std::ceil(topLeft.longitude() / latLonStep) * latLonStep;
    if (bottomRight.longitude() < topLeft.longitude())
    {
        for (; lon < 180; lon += latLonStep)
        {
            auto line = _getLonLine(topLeft.latitude(), bottomRight.latitude(), lon);
            line.precision = GridlinePrecisionLevel::LatLong;
            m_gridLines.append(line);
        }
        lon -= 360;
    }
    for (; lon < bottomRight.longitude(); lon += latLonStep)
    {
        auto line = _getLonLine(topLeft.latitude(), bottomRight.latitude(), lon);
        line.precision = GridlinePrecisionLevel::LatLong;
        m_gridLines.append(line);
    }
}

void MapGridlineModel::_drawUTMP100km(double bLat,
                                      double lLon,
                                      QGeoCoordinate const& topLeft,
                                      QGeoCoordinate const& bottomRight,
                                      double zoomLevel,
                                      int lonZone,
                                      int latBand)
{
    if (zoomLevel >= 8)
    {
        //the latBand and the lonZone will always meet at the lower left corner of the GZD

        //TODO
    }
}

void MapGridlineModel::_NorwayException(double lat,
                                        double lon,
                                        int lonZone,
                                        bool overrun,
                                        QGeoCoordinate const& topLeft,
                                        QGeoCoordinate const& bottomRight,
                                        double zoomLevel)
{
    if (lonZone == 31 && lon >= 3)
    {
        lonZone = 32;
        lon = 3;
    }
    if (overrun)
    {
        for (;;)
        {
            if (lonZone > 60)
            {
                lon = -180;
                lonZone = 1;
                break;
            }
            _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, 7);
            auto line = _getLonLine(lat + 8, lat, lon);
            line.precision = GridlinePrecisionLevel::GZD;
            m_gridLines.append(line);
            switch (lonZone)
            {
            case 31:
                lon = 3;
                break;
            case 32:
                lon = 12;
                break;
            default:
                lon += 6;
            }
            ++lonZone;
        }
    }
    for (;;)
    {
        _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, 7);
        if (lon > bottomRight.longitude())
        {
            break;
        }
        auto line = _getLonLine(lat + 8, lat, lon);
        line.precision = GridlinePrecisionLevel::GZD;
        m_gridLines.append(line);
        switch (lonZone)
        {
        case 31:
            lon = 3;
            break;
        case 32:
            lon = 12;
            break;
        default:
            lon += 6;
        }
        ++lonZone;
    }
}

void MapGridlineModel::_SvalbardException(double lat,
                                          double lon,
                                          int lonZone,
                                          bool overrun,
                                          QGeoCoordinate const& topLeft,
                                          QGeoCoordinate const& bottomRight,
                                          double zoomLevel)
{
    if (lon >= 0 && lon < 42)
    {
        lonZone = 2 * ((int(lon) + 183) / 12) + 1;
        switch (lonZone)
        {
        case 33:
            lon = 9;
            break;
        case 35:
            lon = 21;
            break;
        case 37:
            lon = 33;
            break;
        default:
            break;
        }
    }
    if (overrun)
    {
        for (;;)
        {
            if (lonZone > 60)
            {
                lon = -180;
                lonZone = 1;
                break;
            }
            _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, 9);
            auto line = _getLonLine(84, lat, lon);
            line.precision = GridlinePrecisionLevel::GZD;
            m_gridLines.append(line);
            switch (lonZone)
            {
            case 31:
                lon = 9;
                lonZone = 33;
                break;
            case 33:
                lon = 21;
                lonZone = 35;
                break;
            case 35:
                lon = 33;
                lonZone = 37;
                break;
            case 37:
                lon = 42;
                lonZone = 38;
                break;
            default:
                ++lonZone;
                lon += 6;
            }
        }
    }
    for (;;)
    {
        _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, 9);
        if (lon > bottomRight.longitude())
        {
            break;
        }
        auto line = _getLonLine(84, lat, lon);
        line.precision = GridlinePrecisionLevel::GZD;
        m_gridLines.append(line);
        switch (lonZone)
        {
        case 31:
            lon = 9;
            lonZone = 33;
            break;
        case 33:
            lon = 21;
            lonZone = 35;
            break;
        case 35:
            lon = 33;
            lonZone = 37;
            break;
        case 37:
            lon = 42;
            lonZone = 38;
            break;
        default:
            ++lonZone;
            lon += 6;
        }
    }
}

void MapGridlineModel::_MainGZDRoutine(double lat,
                                       double lon,
                                       int latBand,
                                       int lonZone,
                                       bool overrun,
                                       QGeoCoordinate const& topLeft,
                                       QGeoCoordinate const& bottomRight,
                                       double zoomLevel)
{
    if (overrun)
    {
        for (;;)
        {
            if (lonZone > 60)
            {
                lon = -180;
                lonZone = 1;
                break;
            }
            _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, latBand);
            auto line = _getLonLine(lat + 8, lat, lon);
            line.precision = GridlinePrecisionLevel::GZD;
            m_gridLines.append(line);
            lon += 6;
            ++lonZone;
        }
    }
    for (;;)
    {
        _drawUTMP100km(lat, lon, topLeft, bottomRight, zoomLevel, lonZone, latBand);
        if (lon > bottomRight.longitude())
        {
            break;
        }
        auto line = _getLonLine(lat + 8, lat, lon);
        line.precision = GridlinePrecisionLevel::GZD;
        m_gridLines.append(line);
        lon += 6;
        ++lonZone;
    }
}

void MapGridlineModel::_drawUTMPGrid(QGeoCoordinate const& topLeft,
                                     QGeoCoordinate const& bottomRight,
                                     double zoomLevel)
{
    if (bottomRight.latitude() < 84 && topLeft.latitude() >= -80)
    {
        auto const overrun = bottomRight.longitude() < topLeft.longitude();
        if (topLeft.latitude() >= 84)
        {
            //top the top gridline (special case)
            auto line = _getLatLine(84, topLeft, bottomRight);
            line.precision = GridlinePrecisionLevel::GZD;
            m_gridLines.append(line);
        }
        auto latBand = _LatitudeBand(topLeft.latitude());
        auto ilonStart = int(floor(Math::AngNormalize(topLeft.longitude())));
        if (ilonStart == 180)
        {
            ilonStart = -180; // ilon now in [-180,180)
        }
        auto const zoneStart = (ilonStart + 186) / 6;
        auto const lonStart = double(((zoneStart) * 6) - 186);
        for (;;)
        {
            assert(latBand >= -10);
            assert(latBand <= 9);
            auto const lat = double(((latBand + 10) * 8) - 80);
            switch (latBand)
            {
            case 7:
                _NorwayException(lat, lonStart, zoneStart, overrun, topLeft, bottomRight, zoomLevel);
                break;
            case 9:
                _SvalbardException(lat,
                                   lonStart,
                                   zoneStart,
                                   overrun,
                                   topLeft,
                                   bottomRight,
                                   zoomLevel);
                break;
            default:
                _MainGZDRoutine(lat,
                                lonStart,
                                latBand,
                                zoneStart,
                                overrun,
                                topLeft,
                                bottomRight,
                                zoomLevel);
                break;
            }
            if (latBand <= -10 || lat <= bottomRight.latitude())
            {
                break;
            }
            auto line = _getLatLine(lat, topLeft, bottomRight);
            line.precision = GridlinePrecisionLevel::GZD;
            m_gridLines.append(line);
            --latBand;
        }
    }
}

void MapGridlineModel::updateGridlines(QGeoCoordinate topLeft,
                                       QGeoCoordinate bottomRight,
                                       double zoomLevel)
{
    //std::lock_guard<std::recursive_mutex> lock(m_mut);
    beginResetModel();
    m_gridLines.clear();
    if (zoomLevel <= 4)
    {
        topLeft.setLongitude(-180.0);
        bottomRight.setLongitude(std::nextafter(180, -DBL_MAX));
    }
    //TODO investigate why somtimes only some of the lines show up
    //seems to happen more on lower zoom levels
    //if (zoomLevel > 4.3)
    {
        if (_isMGRS())
        {
            _drawUTMPGrid(topLeft, bottomRight, zoomLevel);
            //TODO handle polar regions? Right now they are just blank
        }
        else
        {
            auto const latLonStep = _getLatLonStep(zoomLevel);
            _drawLatLines(topLeft, bottomRight, latLonStep);
            _drawLonLines(topLeft, bottomRight, latLonStep);
        }
    }
    endResetModel();
}

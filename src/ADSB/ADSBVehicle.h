/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include <QGeoCoordinate>
#include <QElapsedTimer>

#include "QGCMAVLink.h"

class ADSBVehicle : public QObject
{
    Q_OBJECT

public:
    enum class Emitter : uint8_t {
        NO_INFO = 0,
        LIGHT = 1,
        SMALL = 2,
        LARGE = 3,
        HIGH_VORTEX_LARGE = 4,
        HEAVY = 5,
        HIGHLY_MANUV = 6,
        ROTOCRAFT = 7,
        UNASSIGNED = 8,
        GLIDER = 9,
        LIGHTER_AIR = 10	,
        PARACHUTE = 11,
        ULTRA_LIGHT = 12,
        UNASSIGNED2	= 13,
        UAV	 = 14,
        SPACE = 15,
        UNASSGINED3 = 16,
        EMERGENCY_SURFACE = 17,
        SERVICE_SURFACE = 18,
        POINT_OBSTACLE = 19
    };

    Q_ENUM(Emitter);

    enum {
        CallsignAvailable =     1 << 1,
        LocationAvailable =     1 << 2,
        AltitudeAvailable =     1 << 3,
        HeadingAvailable =      1 << 4,
        AlertAvailable =        1 << 5,
    };

    struct VehicleInfo_t {
        uint32_t        icaoAddress;    // Required
        QString         callsign;
        QGeoCoordinate  location;
        double          altitude;
        double          heading;
        bool            alert;
        uint32_t        availableFlags;
        Emitter         emitter;
    };

    ADSBVehicle(const VehicleInfo_t& vehicleInfo, QObject* parent);

    Q_PROPERTY(int              icaoAddress READ icaoAddress    CONSTANT)
    Q_PROPERTY(QString          callsign    READ callsign       NOTIFY callsignChanged)
    Q_PROPERTY(QGeoCoordinate   coordinate  READ coordinate     NOTIFY coordinateChanged)
    Q_PROPERTY(double           altitude    READ altitude       NOTIFY altitudeChanged)     // NaN for not available
    Q_PROPERTY(double           heading     READ heading        NOTIFY headingChanged)      // NaN for not available
    Q_PROPERTY(bool             alert       READ alert          NOTIFY alertChanged)        // Collision path
    Q_PROPERTY(Emitter          emitter     READ emitter        NOTIFY emitterChanged)
    int             icaoAddress (void) const { return static_cast<int>(_icaoAddress); }
    QString         callsign    (void) const { return _callsign; }
    QGeoCoordinate  coordinate  (void) const { return _coordinate; }
    double          altitude    (void) const { return _altitude; }
    double          heading     (void) const { return _heading; }
    bool            alert       (void) const { return _alert; }
    Emitter         emitter     () const { return _emitter; }

    void update(const VehicleInfo_t& vehicleInfo);

    Q_INVOKABLE QString emitterString(Emitter emitter) const;

    /// check if the vehicle is expired and should be removed
    bool expired();

signals:
    void coordinateChanged  ();
    void callsignChanged    ();
    void altitudeChanged    ();
    void headingChanged     ();
    void alertChanged       ();
    void emitterChanged();

private:
    uint32_t        _icaoAddress;
    QString         _callsign;
    QGeoCoordinate  _coordinate;
    double          _altitude;
    double          _heading;
    bool            _alert;
    Emitter         _emitter;

    QElapsedTimer   _lastUpdateTimer;

    static constexpr qint64 expirationTimeoutMs = 120000;   ///< timeout with no update in ms after which the vehicle is removed.
                                                            ///< AirMap sends updates for each vehicle every second.
};

Q_DECLARE_METATYPE(ADSBVehicle::VehicleInfo_t)


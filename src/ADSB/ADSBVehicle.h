/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QLoggingCategory>
#include <QtCore/QtNumeric>
#include <QtCore/QObject>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

#include "ADSB.h"

Q_DECLARE_LOGGING_CATEGORY(ADSBVehicleLog)

class ADSBVehicle : public QObject
{
    Q_OBJECT
    // QML_ELEMENT

public:
    /// QML-visible mirror of the MAVLink ADSB_EMITTER_TYPE enum, so the map can pick an icon per vehicle type.
    enum class EmitterType : uint8_t {
        EMITTER_TYPE_NO_INFO =         ADSB_EMITTER_TYPE_NO_INFO,
        EMITTER_TYPE_LIGHT =           ADSB_EMITTER_TYPE_LIGHT,
        EMITTER_TYPE_SMALL =           ADSB_EMITTER_TYPE_SMALL,
        EMITTER_TYPE_LARGE =           ADSB_EMITTER_TYPE_LARGE,
        EMITTER_TYPE_HIGH_VORTEX_LARGE = ADSB_EMITTER_TYPE_HIGH_VORTEX_LARGE,
        EMITTER_TYPE_HEAVY =           ADSB_EMITTER_TYPE_HEAVY,
        EMITTER_TYPE_HIGHLY_MANUV =    ADSB_EMITTER_TYPE_HIGHLY_MANUV,
        EMITTER_TYPE_ROTOCRAFT =       ADSB_EMITTER_TYPE_ROTOCRAFT,
        EMITTER_TYPE_UNASSIGNED =      ADSB_EMITTER_TYPE_UNASSIGNED,
        EMITTER_TYPE_GLIDER =          ADSB_EMITTER_TYPE_GLIDER,
        EMITTER_TYPE_LIGHTER_AIR =     ADSB_EMITTER_TYPE_LIGHTER_AIR,
        EMITTER_TYPE_PARACHUTE =       ADSB_EMITTER_TYPE_PARACHUTE,
        EMITTER_TYPE_ULTRA_LIGHT =     ADSB_EMITTER_TYPE_ULTRA_LIGHT,
        EMITTER_TYPE_UNASSIGNED2 =     ADSB_EMITTER_TYPE_UNASSIGNED2,
        EMITTER_TYPE_UAV =             ADSB_EMITTER_TYPE_UAV,
        EMITTER_TYPE_SPACE =           ADSB_EMITTER_TYPE_SPACE,
        EMITTER_TYPE_UNASSGINED3 =     ADSB_EMITTER_TYPE_UNASSGINED3,
        EMITTER_TYPE_EMERGENCY_SURFACE = ADSB_EMITTER_TYPE_EMERGENCY_SURFACE,
        EMITTER_TYPE_SERVICE_SURFACE = ADSB_EMITTER_TYPE_SERVICE_SURFACE,
        EMITTER_TYPE_POINT_OBSTACLE =  ADSB_EMITTER_TYPE_POINT_OBSTACLE
    };
    Q_ENUM(EmitterType)

    Q_PROPERTY(uint             icaoAddress READ    icaoAddress CONSTANT)
    Q_PROPERTY(QString          callsign    READ    callsign    NOTIFY callsignChanged)
    Q_PROPERTY(QGeoCoordinate   coordinate  READ    coordinate  NOTIFY coordinateChanged)
    Q_PROPERTY(double           altitude    READ    altitude    NOTIFY altitudeChanged)
    Q_PROPERTY(double           heading     READ    heading     NOTIFY headingChanged)
    Q_PROPERTY(double           velocity    READ    velocity    NOTIFY velocityChanged)
    Q_PROPERTY(double           verticalVel READ    verticalVel NOTIFY verticalVelChanged)
    Q_PROPERTY(uint16_t         squawk      READ    squawk      NOTIFY squawkChanged)
    Q_PROPERTY(bool             alert       READ    alert       NOTIFY alertChanged)
    Q_PROPERTY(EmitterType      emitterType READ    emitterType NOTIFY emitterTypeChanged)  // Vehicle type (MAVLink ADSB_EMITTER_TYPE)
    Q_PROPERTY(bool             hidden      READ    hidden      NOTIFY hiddenChanged)       // Hidden from fly view map
    Q_PROPERTY(bool             oldSignal   READ    oldSignal   NOTIFY oldSignalChanged)    // No update for a while

    explicit ADSBVehicle(const ADSB::VehicleInfo_t &vehicleInfo, QObject *parent = nullptr);
    ~ADSBVehicle();

    uint32_t icaoAddress() const { return _info.icaoAddress; }
    QString callsign() const { return _info.callsign; }
    QGeoCoordinate coordinate() const { return _info.location; }
    double altitude() const { return _info.location.altitude(); }
    double heading() const { return _info.heading; }
    double velocity() const { return _info.velocity; }
    double verticalVel() const { return _info.verticalVel; }
    uint16_t squawk() const { return _info.squawk; }
    bool alert() const { return _info.alert; }
    EmitterType emitterType() const { return static_cast<EmitterType>(_info.emitterType); }
    bool hidden() const { return _hidden; }
    bool oldSignal() const { return _oldSignal; }
    bool expired() const { return _lastUpdateTimer.hasExpired(_expirationTimeoutMs); }
    bool isOldSignal() const { return _lastUpdateTimer.hasExpired(_oldSignalTimeoutMs); }
    void update(const ADSB::VehicleInfo_t &vehicleInfo);
    void setHidden(bool hidden);
    void setIsOldSignal(bool isOldSignal);

signals:
    void coordinateChanged();
    void callsignChanged();
    void altitudeChanged();
    void headingChanged();
    void velocityChanged();
    void verticalVelChanged();
    void squawkChanged();
    void alertChanged();
    void emitterTypeChanged();
    void hiddenChanged();
    void oldSignalChanged();

private:
    ADSB::VehicleInfo_t _info{};
    bool _hidden = false;
    bool _oldSignal = false;
    QElapsedTimer _lastUpdateTimer;

    static constexpr qint64 _oldSignalTimeoutMs = 60000;    ///< timeout with no update in ms after which the vehicle is marked as old.
    static constexpr qint64 _expirationTimeoutMs = 240000;  ///< timeout with no update in ms after which the vehicle is removed.
};

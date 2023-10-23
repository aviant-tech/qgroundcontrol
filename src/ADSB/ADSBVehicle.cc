/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ADSBVehicle.h"
#include "QGCLoggingCategory.h"
#include "QGC.h"

#include <QDebug>
#include <QtMath>

ADSBVehicle::ADSBVehicle(const VehicleInfo_t& vehicleInfo, QObject* parent)
    : QObject       (parent)
    , _icaoAddress  (vehicleInfo.icaoAddress)
    , _altitude     (qQNaN())
    , _heading      (qQNaN())
    , _alert        (false)
{
    update(vehicleInfo);
}

void ADSBVehicle::update(const VehicleInfo_t& vehicleInfo)
{
    if (_icaoAddress != vehicleInfo.icaoAddress) {
        qCWarning(ADSBVehicleManagerLog) << "ICAO address mismatch expected:actual" << _icaoAddress << vehicleInfo.icaoAddress;
        return;
    }

    if(vehicleInfo.emitter != _emitter) {
        _emitter = vehicleInfo.emitter;
        emit emitterChanged();
    }

    if (vehicleInfo.availableFlags & CallsignAvailable) {
        if (vehicleInfo.callsign != _callsign) {
            _callsign = vehicleInfo.callsign;
            emit callsignChanged();
        }
    }
    if (vehicleInfo.availableFlags & LocationAvailable) {
        if (_coordinate != vehicleInfo.location) {
            _coordinate = vehicleInfo.location;
            emit coordinateChanged();
        }
    }
    if (vehicleInfo.availableFlags & AltitudeAvailable) {
        if (!QGC::fuzzyCompare(vehicleInfo.altitude, _altitude)) {
            _altitude = vehicleInfo.altitude;
            emit altitudeChanged();
        }
    }
    if (vehicleInfo.availableFlags & HeadingAvailable) {
        if (!QGC::fuzzyCompare(vehicleInfo.heading, _heading)) {
            _heading = vehicleInfo.heading;
            emit headingChanged();
        }
    }
    if (vehicleInfo.availableFlags & AlertAvailable) {
        if (vehicleInfo.alert != _alert) {
            _alert = vehicleInfo.alert;
            emit alertChanged();
        }
    }
    _lastUpdateTimer.restart();
}

bool ADSBVehicle::expired()
{
    return _lastUpdateTimer.hasExpired(expirationTimeoutMs);
}

QString ADSBVehicle::emitterString(Emitter emitter) const {
    switch(emitter) {
    case Emitter::NO_INFO: return tr("No info");
    case Emitter::LIGHT: return tr("Light");
    case Emitter::SMALL: return tr("Small");
    case Emitter::LARGE : return tr("Large");
    case Emitter::HIGH_VORTEX_LARGE: return tr("High vortex large");
    case Emitter::HEAVY: return tr("Heavy");
    case Emitter::HIGHLY_MANUV: return tr("Higly manuvourable");
    case Emitter::ROTOCRAFT: return tr("Rotocraft");
    case Emitter::UNASSIGNED: return tr("N/A");
    case Emitter::GLIDER : return tr("Glider");
    case Emitter::LIGHTER_AIR: return tr("Lighter aircraft");
    case Emitter::PARACHUTE : return tr("Parachute");
    case Emitter::ULTRA_LIGHT : return tr("Ultralight");
    case Emitter::UNASSIGNED2: return tr("N/A");
    case Emitter::UAV: return tr("UAV");
    case Emitter::SPACE: return tr("Space");
    case Emitter::UNASSGINED3 : return tr("N/A");
    case Emitter::EMERGENCY_SURFACE: return tr("Emergency surface");
    case Emitter::SERVICE_SURFACE: return tr("Service surface");
    case Emitter::POINT_OBSTACLE : return tr("Obstacle");
    }
    return QString{};
}

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

    setIsOldSignal(false);

	if (vehicleInfo.emitterType != _emitterType) {
            _emitterType = vehicleInfo.emitterType;
            emit emitterTypeChanged();
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

bool ADSBVehicle::isOldSignal()
{
    return _lastUpdateTimer.hasExpired(oldSignalMs);
}

void ADSBVehicle::setIsOldSignal(bool isOldSignal)
{
    if (_oldSignal != isOldSignal) {
        if (!isOldSignal && _oldSignal) {
            qCDebug(ADSBVehicleManagerLog) << "Signal restored" << QStringLiteral("%1").arg(_icaoAddress, 0, 16);
        } else {
            qCDebug(ADSBVehicleManagerLog) << "Old signal" << QStringLiteral("%1").arg(_icaoAddress, 0, 16);
        }
        _oldSignal = isOldSignal;
        emit oldSignalChanged();
    }
}

bool ADSBVehicle::expired()
{
    return _lastUpdateTimer.hasExpired(expirationTimeoutMs);
}

void ADSBVehicle::setHidden(bool hidden)
{
    if (hidden != _hidden) {
        _hidden = hidden;
        emit hiddenChanged();
    }
}

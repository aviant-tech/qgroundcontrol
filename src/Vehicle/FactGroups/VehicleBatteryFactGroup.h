/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QString>

class Vehicle;

class VehicleBatteryFactGroup : public FactGroup
{
    Q_OBJECT
    Q_PROPERTY(Fact *id                     READ id                     CONSTANT)
    Q_PROPERTY(Fact *function               READ function               CONSTANT)
    Q_PROPERTY(Fact *type                   READ type                   CONSTANT)
    Q_PROPERTY(Fact *temperature            READ temperature            CONSTANT)
    Q_PROPERTY(Fact *voltage                READ voltage                CONSTANT)
    Q_PROPERTY(Fact *current                READ current                CONSTANT)
    Q_PROPERTY(Fact *mahConsumed            READ mahConsumed            CONSTANT)
    Q_PROPERTY(Fact *rawMahConsumed         READ rawMahConsumed         CONSTANT)
    Q_PROPERTY(Fact *consumedBasedRemaining READ consumedBasedRemaining CONSTANT)
    Q_PROPERTY(Fact *percentRemaining       READ percentRemaining       CONSTANT)
    Q_PROPERTY(Fact *timeRemaining          READ timeRemaining          CONSTANT)
    Q_PROPERTY(Fact *timeRemainingStr       READ timeRemainingStr       CONSTANT)
    Q_PROPERTY(Fact *chargeState            READ chargeState            CONSTANT)
    Q_PROPERTY(Fact *instantPower           READ instantPower           CONSTANT)
    Q_PROPERTY(Fact *timeUntilNextThreshold READ timeUntilNextThreshold CONSTANT)
    Q_PROPERTY(Fact *mahUntilNextThreshold  READ mahUntilNextThreshold  CONSTANT)
    Q_PROPERTY(Fact *nextThresholdName      READ nextThresholdName      CONSTANT)
    Q_PROPERTY(Fact *maxCapacity            READ maxCapacity            CONSTANT)

    Q_PROPERTY(bool   hasPersistedConsumed  READ hasPersistedConsumed   NOTIFY hasPersistedConsumedChanged)
    Q_PROPERTY(double consumedOffset        READ consumedOffset         NOTIFY hasPersistedConsumedChanged)

public:
    explicit VehicleBatteryFactGroup(uint8_t batteryId, QObject *parent = nullptr, Vehicle *vehicle = nullptr);

    Fact *id() { return &_batteryIdFact; }
    Fact *function() { return &_batteryFunctionFact; }
    Fact *type() { return &_batteryTypeFact; }
    Fact *voltage() { return &_voltageFact; }
    Fact *percentRemaining() { return &_percentRemainingFact; }
    Fact *consumedBasedRemaining() { return &_consumedBasedRemainingFact; }
    Fact *mahConsumed() { return &_mahConsumedFact; }
    Fact *rawMahConsumed() { return &_rawMahConsumedFact; }
    Fact *current() { return &_currentFact; }
    Fact *temperature() { return &_temperatureFact; }
    Fact *instantPower() { return &_instantPowerFact; }
    Fact *timeRemaining() { return &_timeRemainingFact; }
    Fact *timeRemainingStr() { return &_timeRemainingStrFact; }
    Fact *chargeState() { return &_chargeStateFact; }
    Fact *timeUntilNextThreshold() { return &_timeUntilNextThresholdFact; }
    Fact *mahUntilNextThreshold() { return &_mahUntilNextThresholdFact; }
    Fact *nextThresholdName() { return &_nextThresholdNameFact; }
    Fact *maxCapacity() { return &_maxCapacityFact; }

    bool   hasPersistedConsumed();
    double consumedOffset();

    /// Creates a new fact group for the battery id as needed and updates the Vehicle with it
    static void handleMessageForFactGroupCreation(Vehicle *vehicle, const mavlink_message_t &message);

    // Persistent-consumption (bingo) accounting. When link is lost or the vehicle reboots the
    // currently observed consumed value is latched as an offset so a subsequent flight on the same
    // battery accumulates from the previous value rather than resetting to zero.
    static void   persistConsumedForVehicle(Vehicle *vehicle);
    static void   resetPersistedConsumedForVehicle(Vehicle *vehicle);
    static void   setCurrentConsumed(Vehicle *vehicle, uint8_t batteryId, double consumed);
    static bool   hasPersistedConsumedForVehicle(Vehicle *vehicle);
    static double getPersistedConsumed(Vehicle *vehicle, uint8_t batteryId);

    // Overrides from FactGroup
    void handleMessage(Vehicle *vehicle, const mavlink_message_t &message) final;

signals:
    void hasPersistedConsumedChanged();

private slots:
    void _timeRemainingChanged(const QVariant &value);
    void _parametersReady(bool parametersReady);

private:
    static void _handleHighLatency(Vehicle *vehicle, const mavlink_message_t &message);
    static void _handleHighLatency2(Vehicle *vehicle, const mavlink_message_t &message);
    static void _handleBatteryStatus(Vehicle *vehicle, const mavlink_message_t &message);
    static VehicleBatteryFactGroup *_findOrAddBatteryGroupById(Vehicle *vehicle, uint8_t batteryId);

    void updateTimeUntilNextThreshold();
    void _loadBatteryParameters();

    static constexpr const char *_batteryFactGroupNamePrefix = "battery";

    Fact _batteryIdFact = Fact(0, QStringLiteral("id"), FactMetaData::valueTypeUint8);
    Fact _batteryFunctionFact = Fact(0, QStringLiteral("batteryFunction"), FactMetaData::valueTypeUint8);
    Fact _batteryTypeFact = Fact(0, QStringLiteral("batteryType"), FactMetaData::valueTypeUint8);
    Fact _voltageFact = Fact(0, QStringLiteral("voltage"), FactMetaData::valueTypeDouble);
    Fact _currentFact = Fact(0, QStringLiteral("current"), FactMetaData::valueTypeDouble);
    Fact _mahConsumedFact = Fact(0, QStringLiteral("mahConsumed"), FactMetaData::valueTypeDouble);
    Fact _rawMahConsumedFact = Fact(0, QStringLiteral("rawMahConsumed"), FactMetaData::valueTypeDouble);
    Fact _temperatureFact = Fact(0, QStringLiteral("temperature"), FactMetaData::valueTypeDouble);
    Fact _percentRemainingFact = Fact(0, QStringLiteral("percentRemaining"), FactMetaData::valueTypeDouble);
    Fact _consumedBasedRemainingFact = Fact(0, QStringLiteral("consumedBasedRemaining"), FactMetaData::valueTypeDouble);
    Fact _timeRemainingFact = Fact(0, QStringLiteral("timeRemaining"), FactMetaData::valueTypeDouble);
    Fact _timeRemainingStrFact = Fact(0, QStringLiteral("timeRemainingStr"), FactMetaData::valueTypeString);
    Fact _chargeStateFact = Fact(0, QStringLiteral("chargeState"), FactMetaData::valueTypeUint8);
    Fact _instantPowerFact = Fact(0, QStringLiteral("instantPower"), FactMetaData::valueTypeDouble);
    Fact _timeUntilNextThresholdFact = Fact(0, QStringLiteral("timeUntilNextThreshold"), FactMetaData::valueTypeDouble);
    Fact _mahUntilNextThresholdFact = Fact(0, QStringLiteral("mahUntilNextThreshold"), FactMetaData::valueTypeDouble);
    Fact _nextThresholdNameFact = Fact(0, QStringLiteral("nextThresholdName"), FactMetaData::valueTypeString);
    Fact _maxCapacityFact = Fact(0, QStringLiteral("maxCapacity"), FactMetaData::valueTypeDouble);

    struct ThresholdInfo {
        double  mahThreshold;
        QString name;
    };
    QList<ThresholdInfo> _thresholds;

    Vehicle *_vehicle = nullptr;

    // vehicleId -> (batteryId -> (persistedOffset, currentObserved))
    static QMap<int, QMap<uint8_t, QPair<double, double>>> _persistedConsumed;
};

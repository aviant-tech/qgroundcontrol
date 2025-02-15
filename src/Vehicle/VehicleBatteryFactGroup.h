/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"
#include "FactPanelController.h"

class Vehicle;

class VehicleBatteryFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleBatteryFactGroup(uint8_t batteryId, QObject* parent = nullptr, Vehicle* vehicle = nullptr);

    Q_PROPERTY(Fact* id                     READ id                     CONSTANT)
    Q_PROPERTY(Fact* function               READ function               CONSTANT)
    Q_PROPERTY(Fact* type                   READ type                   CONSTANT)
    Q_PROPERTY(Fact* temperature            READ temperature            CONSTANT)
    Q_PROPERTY(Fact* voltage                READ voltage                CONSTANT)
    Q_PROPERTY(Fact* current                READ current                CONSTANT)
    Q_PROPERTY(Fact* mahConsumed            READ mahConsumed            CONSTANT)
    Q_PROPERTY(Fact* percentRemaining       READ percentRemaining       CONSTANT)
    Q_PROPERTY(Fact* timeRemaining          READ timeRemaining          CONSTANT)
    Q_PROPERTY(Fact* timeRemainingStr       READ timeRemainingStr       CONSTANT)
    Q_PROPERTY(Fact* chargeState            READ chargeState            CONSTANT)
    Q_PROPERTY(Fact* instantPower           READ instantPower           CONSTANT)
    Q_PROPERTY(Fact* timeUntilNextThreshold READ timeUntilNextThreshold CONSTANT)
    Q_PROPERTY(Fact* nextThresholdName      READ nextThresholdName      CONSTANT)
    Q_PROPERTY(Fact* maxCapacity            READ maxCapacity            CONSTANT)

    Fact* id                        () { return &_batteryIdFact; }
    Fact* function                  () { return &_batteryFunctionFact; }
    Fact* type                      () { return &_batteryTypeFact; }
    Fact* voltage                   () { return &_voltageFact; }
    Fact* percentRemaining          () { return &_percentRemainingFact; }
    Fact* mahConsumed               () { return &_mahConsumedFact; }
    Fact* current                   () { return &_currentFact; }
    Fact* temperature               () { return &_temperatureFact; }
    Fact* instantPower              () { return &_instantPowerFact; }
    Fact* timeRemaining             () { return &_timeRemainingFact; }
    Fact* timeRemainingStr          () { return &_timeRemainingStrFact; }
    Fact* chargeState               () { return &_chargeStateFact; }
    Fact* timeUntilNextThreshold    () { return &_timeUntilNextThresholdFact; }
    Fact* nextThresholdName         () { return &_nextThresholdNameFact; }
    Fact* maxCapacity               () { return &_maxCapacityFact; }

    static const char* _batteryIdFactName;
    static const char* _batteryFunctionFactName;
    static const char* _batteryTypeFactName;
    static const char* _temperatureFactName;
    static const char* _voltageFactName;
    static const char* _currentFactName;
    static const char* _mahConsumedFactName;
    static const char* _percentRemainingFactName;
    static const char* _timeRemainingFactName;
    static const char* _timeRemainingStrFactName;
    static const char* _chargeStateFactName;
    static const char* _instantPowerFactName;
    static const char* _timeUntilNextThresholdFactName;
    static const char* _nextThresholdNameFactName;
    static const char* _maxCapacityFactName;


    void updateTimeUntilNextThreshold();
    
    static const char* _settingsGroup;

    /// Creates a new fact group for the battery id as needed and updates the Vehicle with it
    static void handleMessageForFactGroupCreation(Vehicle* vehicle, mavlink_message_t& message);

    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

    static void persistConsumedForVehicle(int vehicleId);
    static void resetPersistedConsumedForVehicle(int vehicleId);
    static void setCurrentConsumed(int vehicleId, uint8_t batteryId, double consumed);
    static bool hasPersistedConsumedForVehicle(int vehicleId);
    static double getPersistedConsumed(int vehicleId, uint8_t batteryId);
    

private slots:
    void _timeRemainingChanged(QVariant value);
    void _parametersReady(bool parametersReady);


private:
    static void                     _handleHighLatency          (Vehicle* vehicle, mavlink_message_t& message);
    static void                     _handleHighLatency2         (Vehicle* vehicle, mavlink_message_t& message);
    static void                     _handleBatteryStatus        (Vehicle* vehicle, mavlink_message_t& message);
    static VehicleBatteryFactGroup* _findOrAddBatteryGroupById  (Vehicle* vehicle, uint8_t batteryId);

    Fact            _batteryIdFact;
    Fact            _batteryFunctionFact;
    Fact            _batteryTypeFact;
    Fact            _voltageFact;
    Fact            _currentFact;
    Fact            _mahConsumedFact;
    Fact            _temperatureFact;
    Fact            _percentRemainingFact;
    Fact            _timeRemainingFact;
    Fact            _timeRemainingStrFact;
    Fact            _chargeStateFact;
    Fact            _instantPowerFact;
    Fact            _timeUntilNextThresholdFact;
    Fact            _nextThresholdNameFact;
    Fact            _maxCapacityFact;
    struct ThresholdInfo {
        double mahThreshold;
        QString name;
    };

    QList<ThresholdInfo> _thresholds;

    static const char* _batteryFactGroupNamePrefix;

    
    Vehicle* _vehicle;

    void _loadBatteryParameters();

    static QMap<int, QMap<uint8_t, QPair<double, double>>> _persistedConsumed;
};

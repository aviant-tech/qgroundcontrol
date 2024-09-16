/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleBatteryFactGroup.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include "ParameterManager.h"

const char* VehicleBatteryFactGroup::_batteryFactGroupNamePrefix    = "battery";

const char* VehicleBatteryFactGroup::_batteryIdFactName              = "id";
const char* VehicleBatteryFactGroup::_batteryFunctionFactName        = "batteryFunction";
const char* VehicleBatteryFactGroup::_batteryTypeFactName            = "batteryType";
const char* VehicleBatteryFactGroup::_voltageFactName                = "voltage";
const char* VehicleBatteryFactGroup::_percentRemainingFactName       = "percentRemaining";
const char* VehicleBatteryFactGroup::_mahConsumedFactName            = "mahConsumed";
const char* VehicleBatteryFactGroup::_currentFactName                = "current";
const char* VehicleBatteryFactGroup::_temperatureFactName            = "temperature";
const char* VehicleBatteryFactGroup::_instantPowerFactName           = "instantPower";
const char* VehicleBatteryFactGroup::_timeRemainingFactName          = "timeRemaining";
const char* VehicleBatteryFactGroup::_timeRemainingStrFactName       = "timeRemainingStr";
const char* VehicleBatteryFactGroup::_chargeStateFactName            = "chargeState";
const char* VehicleBatteryFactGroup::_timeUntilNextThresholdFactName = "timeUntilNextThreshold";
const char* VehicleBatteryFactGroup::_nextThresholdNameFactName      = "nextThresholdName";
const char* VehicleBatteryFactGroup::_maxCapacityFactName            = "maxCapacity";


const char* VehicleBatteryFactGroup::_settingsGroup =                       "Vehicle.battery";

QMap<int, QMap<uint8_t, double>> VehicleBatteryFactGroup::_persistedConsumed;

VehicleBatteryFactGroup::VehicleBatteryFactGroup(uint8_t batteryId, QObject* parent, Vehicle* vehicle)
    : FactGroup                  (1000, ":/json/Vehicle/BatteryFact.json", parent)
    , _batteryIdFact             (0, _batteryIdFactName,                 FactMetaData::valueTypeUint8)
    , _batteryFunctionFact       (0, _batteryFunctionFactName,           FactMetaData::valueTypeUint8)
    , _batteryTypeFact           (0, _batteryTypeFactName,               FactMetaData::valueTypeUint8)
    , _voltageFact               (0, _voltageFactName,                   FactMetaData::valueTypeDouble)
    , _currentFact               (0, _currentFactName,                   FactMetaData::valueTypeDouble)
    , _mahConsumedFact           (0, _mahConsumedFactName,               FactMetaData::valueTypeDouble)
    , _temperatureFact           (0, _temperatureFactName,               FactMetaData::valueTypeDouble)
    , _percentRemainingFact      (0, _percentRemainingFactName,          FactMetaData::valueTypeDouble)
    , _timeRemainingFact         (0, _timeRemainingFactName,             FactMetaData::valueTypeDouble)
    , _timeRemainingStrFact      (0, _timeRemainingStrFactName,          FactMetaData::valueTypeString)
    , _chargeStateFact           (0, _chargeStateFactName,               FactMetaData::valueTypeUint8)
    , _instantPowerFact          (0, _instantPowerFactName,              FactMetaData::valueTypeDouble)
    , _timeUntilNextThresholdFact(0, _timeUntilNextThresholdFactName,    FactMetaData::valueTypeDouble)
    , _nextThresholdNameFact     (0, _nextThresholdNameFactName,         FactMetaData::valueTypeString)
    , _maxCapacityFact           (0, _maxCapacityFactName,               FactMetaData::valueTypeDouble)
    , _vehicle                   (vehicle)
{
    _addFact(&_batteryIdFact,               _batteryIdFactName);
    _addFact(&_batteryFunctionFact,         _batteryFunctionFactName);
    _addFact(&_batteryTypeFact,             _batteryTypeFactName);
    _addFact(&_voltageFact,                 _voltageFactName);
    _addFact(&_currentFact,                 _currentFactName);
    _addFact(&_mahConsumedFact,             _mahConsumedFactName);
    _addFact(&_temperatureFact,             _temperatureFactName);
    _addFact(&_percentRemainingFact,        _percentRemainingFactName);
    _addFact(&_timeRemainingFact,           _timeRemainingFactName);
    _addFact(&_timeRemainingStrFact,        _timeRemainingStrFactName);
    _addFact(&_chargeStateFact,             _chargeStateFactName);
    _addFact(&_instantPowerFact,            _instantPowerFactName);
    _addFact(&_timeUntilNextThresholdFact,  _timeUntilNextThresholdFactName);
    _addFact(&_nextThresholdNameFact,       _nextThresholdNameFactName);
    _addFact(&_maxCapacityFact,             _maxCapacityFactName);

    _batteryIdFact.setRawValue             (batteryId);
    _batteryFunctionFact.setRawValue       (MAV_BATTERY_FUNCTION_UNKNOWN);
    _batteryTypeFact.setRawValue           (MAV_BATTERY_TYPE_UNKNOWN);
    _voltageFact.setRawValue               (qQNaN());
    _currentFact.setRawValue               (qQNaN());
    _mahConsumedFact.setRawValue           (qQNaN());
    _temperatureFact.setRawValue           (qQNaN());
    _percentRemainingFact.setRawValue      (qQNaN());
    _timeRemainingFact.setRawValue         (qQNaN());
    _chargeStateFact.setRawValue           (MAV_BATTERY_CHARGE_STATE_UNDEFINED);
    _instantPowerFact.setRawValue          (qQNaN());
    _timeUntilNextThresholdFact.setRawValue(qQNaN());
    _nextThresholdNameFact.setRawValue     ("");
    _maxCapacityFact.setRawValue           (qQNaN());

    connect(&_timeRemainingFact, &Fact::rawValueChanged, this, &VehicleBatteryFactGroup::_timeRemainingChanged);
    if (_vehicle) {
        connect(_vehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &VehicleBatteryFactGroup::_parametersReady);
    }
}


void VehicleBatteryFactGroup::updateTimeUntilNextThreshold()
{
    double currentMahConsumed = mahConsumed()->rawValue().toDouble();
    double maxCapacity = _maxCapacityFact.rawValue().toDouble();
    double mahRemaining = maxCapacity - currentMahConsumed;
    double timeRemainingInSeconds = _timeRemainingFact.rawValue().toDouble();
    double timeRemainingInHours = timeRemainingInSeconds / 3600;

    if (qIsNaN(currentMahConsumed) || qIsNaN(maxCapacity) || qIsNaN(timeRemainingInHours) || timeRemainingInHours <= 0) {
        _timeUntilNextThresholdFact.setRawValue(qQNaN());
        _nextThresholdNameFact.setRawValue("N/A");
        return;
    }
    if (_thresholds.size() == 0) {
        return;
    }

    for (const auto& threshold : _thresholds) {
        if (currentMahConsumed < threshold.mahThreshold) {
            double estimatedAverageCurrentInMa = mahRemaining / timeRemainingInHours;
            double timeUntilThresholdInHours = (threshold.mahThreshold - currentMahConsumed) / estimatedAverageCurrentInMa;
            double timeUntilThresholdInSeconds = timeUntilThresholdInHours * 3600;

            _timeUntilNextThresholdFact.setRawValue(timeUntilThresholdInSeconds);
            _nextThresholdNameFact.setRawValue(threshold.name);
            return;
        }
    }
    // If we're past all thresholds
    _timeUntilNextThresholdFact.setRawValue(0);
    _nextThresholdNameFact.setRawValue("All thresholds passed");
}

void VehicleBatteryFactGroup::handleMessageForFactGroupCreation(Vehicle* vehicle, mavlink_message_t& message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        _findOrAddBatteryGroupById(vehicle, 0);
        break;
    case MAVLINK_MSG_ID_BATTERY_STATUS:
    {
        mavlink_battery_status_t batteryStatus;
        mavlink_msg_battery_status_decode(&message, &batteryStatus);
        _findOrAddBatteryGroupById(vehicle, batteryStatus.id);
    }
        break;
    }
}

void VehicleBatteryFactGroup::handleMessage(Vehicle* vehicle, mavlink_message_t& message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
        _handleHighLatency(vehicle, message);
        break;
    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        _handleHighLatency2(vehicle, message);
        break;
    case MAVLINK_MSG_ID_BATTERY_STATUS:
        _handleBatteryStatus(vehicle, message);
        break;
    }
}

void VehicleBatteryFactGroup::_parametersReady(bool parametersReady)
{
    if (parametersReady) {
        _loadBatteryParameters();
        disconnect(_vehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &VehicleBatteryFactGroup::_parametersReady);
    }
}

void VehicleBatteryFactGroup::_loadBatteryParameters()
{
    auto aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();

    if (_vehicle &&  _vehicle->parameterManager()->parametersReady()) {
        Fact* batCapacityFact = _vehicle->parameterManager()->getParameter(-1, "BAT1_CAPACITY");
        Fact* batCritFact = _vehicle->parameterManager()->getParameter(-1, "BAT_CRIT_THR");
        Fact* batEmergenFact = _vehicle->parameterManager()->getParameter(-1, "BAT_EMERGEN_THR");

        int batCapacity = batCapacityFact ? batCapacityFact->rawValue().toInt() : 0;
        double critThreshold = batCritFact ? batCritFact->rawValue().toDouble() : 0.0;
        double emergenThreshold = batEmergenFact ? batEmergenFact->rawValue().toDouble() : 0.0;

        _thresholds = {
            {aviantSettings->preDeliveryBingoLimit()->rawValue().toDouble(), aviantSettings->preDeliveryBingoLimit()->shortDescription()},
            {aviantSettings->postDeliveryBingoLimit()->rawValue().toDouble(), aviantSettings->postDeliveryBingoLimit()->shortDescription()},
            {batCapacity * (1 - critThreshold), batCritFact->shortDescription()},
            {batCapacity * (1 - emergenThreshold), batEmergenFact->shortDescription()}
        };

        // Sort thresholds in descending order
        std::sort(_thresholds.begin(), _thresholds.end(), [](const ThresholdInfo& a, const ThresholdInfo& b) {
            return a.mahThreshold < b.mahThreshold;
        });

        if (batCapacityFact) {
            _maxCapacityFact.setRawValue(batCapacityFact->rawValue());
        }
        
        disconnect(_vehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &VehicleBatteryFactGroup::_loadBatteryParameters);
    }
}

void VehicleBatteryFactGroup::_handleHighLatency(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_high_latency_t highLatency;
    mavlink_msg_high_latency_decode(&message, &highLatency);

    VehicleBatteryFactGroup* group = _findOrAddBatteryGroupById(vehicle, 0);
    group->percentRemaining()->setRawValue(highLatency.battery_remaining == UINT8_MAX ? qQNaN() : highLatency.battery_remaining);
    group->_setTelemetryAvailable(true);
}

void VehicleBatteryFactGroup::_handleHighLatency2(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_high_latency2_t highLatency2;
    mavlink_msg_high_latency2_decode(&message, &highLatency2);

    VehicleBatteryFactGroup* group = _findOrAddBatteryGroupById(vehicle, 0);
    group->percentRemaining()->setRawValue(highLatency2.battery == -1 ? qQNaN() : highLatency2.battery);
    group->_setTelemetryAvailable(true);
}

void VehicleBatteryFactGroup::persistConsumedForBatteries(int vehicleId, const QMap<uint8_t, double>& batteryConsumed)
{
    for (auto it = batteryConsumed.constBegin(); it != batteryConsumed.constEnd(); ++it) {
        _persistedConsumed[vehicleId][it.key()] = it.value();
    }
}

void VehicleBatteryFactGroup::resetPersistedConsumedForVehicle(int vehicleId)
{
    _persistedConsumed.remove(vehicleId);
}

double VehicleBatteryFactGroup::getPersistedConsumed(int vehicleId, uint8_t batteryId)
{
    return _persistedConsumed.value(vehicleId).value(batteryId, 0.0);
}

void VehicleBatteryFactGroup::_handleBatteryStatus(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_battery_status_t batteryStatus;
    mavlink_msg_battery_status_decode(&message, &batteryStatus);

    VehicleBatteryFactGroup* group = _findOrAddBatteryGroupById(vehicle, batteryStatus.id);

    double totalVoltage = qQNaN();
    for (int i=0; i<10; i++) {
        double cellVoltage = batteryStatus.voltages[i] == UINT16_MAX ? qQNaN() : static_cast<double>(batteryStatus.voltages[i]) / 1000.0;
        if (qIsNaN(cellVoltage)) {
            break;
        }
        if (i == 0) {
            totalVoltage = cellVoltage;
        } else {
            totalVoltage += cellVoltage;
        }
    }
    for (int i=0; i<4; i++) {
        double cellVoltage = batteryStatus.voltages_ext[i] == UINT16_MAX ? qQNaN() : static_cast<double>(batteryStatus.voltages_ext[i]) / 1000.0;
        if (qIsNaN(cellVoltage)) {
            break;
        }
        totalVoltage += cellVoltage;
    }

    double current = batteryStatus.current_battery == -1 ? qQNaN() : static_cast<double>(batteryStatus.current_battery) / 100.0;
    

    double consumed;

    if (batteryStatus.current_consumed < 0) {
        consumed = qQNaN();
    } else {
        consumed = batteryStatus.current_consumed;
        consumed += getPersistedConsumed(vehicle->id(), batteryStatus.id);
    }

    group->function()->setRawValue          (batteryStatus.battery_function);
    group->type()->setRawValue              (batteryStatus.type);
    group->temperature()->setRawValue       (batteryStatus.temperature == INT16_MAX ?   qQNaN() : static_cast<double>(batteryStatus.temperature) / 100.0);
    group->voltage()->setRawValue           (totalVoltage);
    group->current()->setRawValue           (current);
    group->mahConsumed()->setRawValue       (consumed);
    group->percentRemaining()->setRawValue  (batteryStatus.battery_remaining == -1 ?    qQNaN() : batteryStatus.battery_remaining);
    group->timeRemaining()->setRawValue     (batteryStatus.time_remaining == 0 ?        qQNaN() : batteryStatus.time_remaining);
    group->chargeState()->setRawValue       (batteryStatus.charge_state);
    group->instantPower()->setRawValue      (totalVoltage * group->current()->rawValue().toDouble());
    group->_setTelemetryAvailable(true);
    group->updateTimeUntilNextThreshold();
}

VehicleBatteryFactGroup* VehicleBatteryFactGroup::_findOrAddBatteryGroupById(Vehicle* vehicle, uint8_t batteryId)
{
    QmlObjectListModel* batteries = vehicle->batteries();

    // We maintain the list in order sorted by battery id so the ui shows them sorted.
    for (int i=0; i<batteries->count(); i++) {
        VehicleBatteryFactGroup* group = batteries->value<VehicleBatteryFactGroup*>(i);
        int listBatteryId = group->id()->rawValue().toInt();
        if (listBatteryId >  batteryId) {
            VehicleBatteryFactGroup* newBatteryGroup = new VehicleBatteryFactGroup(batteryId, batteries, vehicle);
            batteries->insert(i, newBatteryGroup);
            vehicle->_addFactGroup(newBatteryGroup, QStringLiteral("%1%2").arg(_batteryFactGroupNamePrefix).arg(batteryId));
            return newBatteryGroup;
        } else if (listBatteryId == batteryId) {
            return group;
        }
    }

    VehicleBatteryFactGroup* newBatteryGroup = new VehicleBatteryFactGroup(batteryId, batteries, vehicle);
    batteries->append(newBatteryGroup);
    vehicle->_addFactGroup(newBatteryGroup, QStringLiteral("%1%2").arg(_batteryFactGroupNamePrefix).arg(batteryId));

    return newBatteryGroup;
}

void VehicleBatteryFactGroup::_timeRemainingChanged(QVariant value)
{
    if (qIsNaN(value.toDouble())) {
        _timeRemainingStrFact.setRawValue("--:--:--");
    } else {
        int totalSeconds    = value.toInt();
        int hours           = totalSeconds / 3600;
        int minutes         = (totalSeconds % 3600) / 60;
        int seconds         = totalSeconds % 60;

        _timeRemainingStrFact.setRawValue(QString::asprintf("%02dH:%02dM:%02dS", hours, minutes, seconds));
    }
}

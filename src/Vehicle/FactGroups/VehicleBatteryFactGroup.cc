/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleBatteryFactGroup.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"
#include "ParameterManager.h"
#include "SettingsManager.h"
#include "AviantSettings.h"

#include <algorithm>

QMap<int, QMap<uint8_t, QPair<double, double>>> VehicleBatteryFactGroup::_persistedConsumed;

VehicleBatteryFactGroup::VehicleBatteryFactGroup(uint8_t batteryId, QObject *parent, Vehicle *vehicle)
    : FactGroup(1000, QStringLiteral(":/json/Vehicle/BatteryFact.json"), parent)
    , _vehicle(vehicle)
{
    _addFact(&_batteryIdFact);
    _addFact(&_batteryFunctionFact);
    _addFact(&_batteryTypeFact);
    _addFact(&_voltageFact);
    _addFact(&_currentFact);
    _addFact(&_mahConsumedFact);
    _addFact(&_rawMahConsumedFact);
    _addFact(&_temperatureFact);
    _addFact(&_percentRemainingFact);
    _addFact(&_consumedBasedRemainingFact);
    _addFact(&_timeRemainingFact);
    _addFact(&_timeRemainingStrFact);
    _addFact(&_chargeStateFact);
    _addFact(&_instantPowerFact);
    _addFact(&_timeUntilNextThresholdFact);
    _addFact(&_mahUntilNextThresholdFact);
    _addFact(&_nextThresholdNameFact);
    _addFact(&_maxCapacityFact);

    _batteryIdFact.setRawValue(batteryId);
    _batteryFunctionFact.setRawValue(MAV_BATTERY_FUNCTION_UNKNOWN);
    _batteryTypeFact.setRawValue(MAV_BATTERY_TYPE_UNKNOWN);
    _voltageFact.setRawValue(qQNaN());
    _currentFact.setRawValue(qQNaN());
    _mahConsumedFact.setRawValue(qQNaN());
    _rawMahConsumedFact.setRawValue(qQNaN());
    _temperatureFact.setRawValue(qQNaN());
    _percentRemainingFact.setRawValue(qQNaN());
    _consumedBasedRemainingFact.setRawValue(qQNaN());
    _timeRemainingFact.setRawValue(qQNaN());
    _chargeStateFact.setRawValue(MAV_BATTERY_CHARGE_STATE_UNDEFINED);
    _instantPowerFact.setRawValue(qQNaN());
    _timeUntilNextThresholdFact.setRawValue(qQNaN());
    _mahUntilNextThresholdFact.setRawValue(qQNaN());
    _nextThresholdNameFact.setRawValue(QString());
    _maxCapacityFact.setRawValue(qQNaN());

    (void) connect(&_timeRemainingFact, &Fact::rawValueChanged, this, &VehicleBatteryFactGroup::_timeRemainingChanged);

    if (_vehicle) {
        (void) connect(_vehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &VehicleBatteryFactGroup::_parametersReady);
        _loadBatteryParameters(); // Parameters will sometimes load before we can connect the signal, so call this explicitly as well
    }
}

void VehicleBatteryFactGroup::_parametersReady(bool parametersReady)
{
    if (parametersReady) {
        _loadBatteryParameters();
    }
}

void VehicleBatteryFactGroup::_loadBatteryParameters()
{
    if (!_vehicle || !_vehicle->parameterManager()->parametersReady()) {
        return;
    }

    AviantSettings *aviantSettings = SettingsManager::instance()->aviantSettings();

    ParameterManager *pm = _vehicle->parameterManager();
    Fact *batCapacityFact = pm->parameterExists(-1, QStringLiteral("BAT1_CAPACITY")) ? pm->getParameter(-1, QStringLiteral("BAT1_CAPACITY")) : nullptr;
    Fact *batCritFact = pm->parameterExists(-1, QStringLiteral("BAT_CRIT_THR")) ? pm->getParameter(-1, QStringLiteral("BAT_CRIT_THR")) : nullptr;
    Fact *batEmergenFact = pm->parameterExists(-1, QStringLiteral("BAT_EMERGEN_THR")) ? pm->getParameter(-1, QStringLiteral("BAT_EMERGEN_THR")) : nullptr;

    const double batCapacity = batCapacityFact ? batCapacityFact->rawValue().toDouble() : 0.0;
    const double critThreshold = batCritFact ? batCritFact->rawValue().toDouble() : 0.0;
    const double emergenThreshold = batEmergenFact ? batEmergenFact->rawValue().toDouble() : 0.0;

    _thresholds = {
        {aviantSettings->preDeliveryBingoLimit()->rawValue().toDouble(),  aviantSettings->preDeliveryBingoLimit()->shortDescription()},
        {aviantSettings->postDeliveryBingoLimit()->rawValue().toDouble(), aviantSettings->postDeliveryBingoLimit()->shortDescription()},
        {batCapacity * (1 - critThreshold),    batCritFact ? batCritFact->shortDescription() : QStringLiteral("Critical")},
        {batCapacity * (1 - emergenThreshold), batEmergenFact ? batEmergenFact->shortDescription() : QStringLiteral("Emergency")}
    };

    // Sort thresholds in ascending order of consumed mAh
    std::sort(_thresholds.begin(), _thresholds.end(), [](const ThresholdInfo &a, const ThresholdInfo &b) {
        return a.mahThreshold < b.mahThreshold;
    });

    if (batCapacityFact) {
        _maxCapacityFact.setRawValue(batCapacityFact->rawValue());
    }
}

void VehicleBatteryFactGroup::updateTimeUntilNextThreshold()
{
    const double currentMahConsumed = _mahConsumedFact.rawValue().toDouble();
    const double maxCapacity = _maxCapacityFact.rawValue().toDouble();
    const double mahRemaining = maxCapacity - currentMahConsumed;
    const double timeRemainingInSeconds = _timeRemainingFact.rawValue().toDouble();
    const double timeRemainingInHours = timeRemainingInSeconds / 3600.0;

    if (qIsNaN(currentMahConsumed) || qIsNaN(maxCapacity) || qIsNaN(timeRemainingInHours) || timeRemainingInHours <= 0) {
        _timeUntilNextThresholdFact.setRawValue(qQNaN());
        _mahUntilNextThresholdFact.setRawValue(qQNaN());
        _nextThresholdNameFact.setRawValue(QStringLiteral("N/A"));
        return;
    }
    if (_thresholds.isEmpty()) {
        return;
    }

    for (const ThresholdInfo &threshold : _thresholds) {
        if (currentMahConsumed < threshold.mahThreshold) {
            const double estimatedAverageCurrentInMa = mahRemaining / timeRemainingInHours;
            const double timeUntilThresholdInHours = (threshold.mahThreshold - currentMahConsumed) / estimatedAverageCurrentInMa;
            const double timeUntilThresholdInSeconds = timeUntilThresholdInHours * 3600.0;
            const double mahUntilThreshold = threshold.mahThreshold - currentMahConsumed;

            _timeUntilNextThresholdFact.setRawValue(timeUntilThresholdInSeconds);
            _mahUntilNextThresholdFact.setRawValue(mahUntilThreshold);
            _nextThresholdNameFact.setRawValue(threshold.name);
            return;
        }
    }

    // If we're past all thresholds
    _timeUntilNextThresholdFact.setRawValue(0);
    _mahUntilNextThresholdFact.setRawValue(0);
    _nextThresholdNameFact.setRawValue(QStringLiteral("All thresholds passed"));
}

bool VehicleBatteryFactGroup::hasPersistedConsumed()
{
    return hasPersistedConsumedForVehicle(_vehicle);
}

double VehicleBatteryFactGroup::consumedOffset()
{
    if (!_vehicle) {
        return 0.0;
    }
    return getPersistedConsumed(_vehicle, static_cast<uint8_t>(_batteryIdFact.rawValue().toUInt()));
}

void VehicleBatteryFactGroup::persistConsumedForVehicle(Vehicle *vehicle)
{
    if (vehicle && _persistedConsumed.contains(vehicle->id())) {
        QMap<uint8_t, QPair<double, double>> &batteriesMap = _persistedConsumed[vehicle->id()];

        for (auto it = batteriesMap.begin(); it != batteriesMap.end(); ++it) {
            it.value().first = it.value().second;

            VehicleBatteryFactGroup *group = _findOrAddBatteryGroupById(vehicle, it.key());
            if (group) {
                emit group->hasPersistedConsumedChanged();
            }
        }
    }
}

void VehicleBatteryFactGroup::resetPersistedConsumedForVehicle(Vehicle *vehicle)
{
    if (vehicle && _persistedConsumed.contains(vehicle->id())) {
        QMap<uint8_t, QPair<double, double>> &batteriesMap = _persistedConsumed[vehicle->id()];

        for (auto it = batteriesMap.begin(); it != batteriesMap.end(); ++it) {
            it.value().first = 0.0; // Reset before sending signal, then delete all

            VehicleBatteryFactGroup *group = _findOrAddBatteryGroupById(vehicle, it.key());
            if (group) {
                emit group->hasPersistedConsumedChanged();
            }
        }
        _persistedConsumed.remove(vehicle->id());
    }
}

bool VehicleBatteryFactGroup::hasPersistedConsumedForVehicle(Vehicle *vehicle)
{
    if (vehicle && _persistedConsumed.contains(vehicle->id())) {
        const QMap<uint8_t, QPair<double, double>> &batteriesMap = _persistedConsumed[vehicle->id()];
        for (const auto &batteryPair : batteriesMap) {
            if (batteryPair.first > 0) {
                return true;
            }
        }
    }
    return false;
}

double VehicleBatteryFactGroup::getPersistedConsumed(Vehicle *vehicle, uint8_t batteryId)
{
    if (vehicle && _persistedConsumed.contains(vehicle->id())) {
        const auto &batteriesMap = _persistedConsumed[vehicle->id()];
        if (batteriesMap.contains(batteryId)) {
            return batteriesMap[batteryId].first;
        }
    }
    return 0.0;
}

void VehicleBatteryFactGroup::setCurrentConsumed(Vehicle *vehicle, uint8_t batteryId, double consumed)
{
    if (!vehicle) {
        return;
    }
    if (!_persistedConsumed.contains(vehicle->id())) {
        _persistedConsumed[vehicle->id()] = QMap<uint8_t, QPair<double, double>>();
    }
    _persistedConsumed[vehicle->id()][batteryId].second = consumed;
}

void VehicleBatteryFactGroup::handleMessage(Vehicle *vehicle, const mavlink_message_t &message)
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
    default:
        break;
    }
}

void VehicleBatteryFactGroup::_handleHighLatency(Vehicle *vehicle, const mavlink_message_t &message)
{
    mavlink_high_latency_t highLatency{};
    mavlink_msg_high_latency_decode(&message, &highLatency);

    VehicleBatteryFactGroup *const group = _findOrAddBatteryGroupById(vehicle, 0);
    group->percentRemaining()->setRawValue((highLatency.battery_remaining == UINT8_MAX) ? qQNaN() : highLatency.battery_remaining);

    group->_setTelemetryAvailable(true);
}

void VehicleBatteryFactGroup::_handleHighLatency2(Vehicle *vehicle, const mavlink_message_t &message)
{
    mavlink_high_latency2_t highLatency2{};
    mavlink_msg_high_latency2_decode(&message, &highLatency2);

    VehicleBatteryFactGroup *const group = _findOrAddBatteryGroupById(vehicle, 0);
    group->percentRemaining()->setRawValue((highLatency2.battery == -1) ? qQNaN() : highLatency2.battery);

    group->_setTelemetryAvailable(true);
}

void VehicleBatteryFactGroup::_handleBatteryStatus(Vehicle *vehicle, const mavlink_message_t &message)
{
    mavlink_battery_status_t batteryStatus{};
    mavlink_msg_battery_status_decode(&message, &batteryStatus);

    VehicleBatteryFactGroup *const group = _findOrAddBatteryGroupById(vehicle, batteryStatus.id);

    double totalVoltage = qQNaN();
    for (int i = 0; i < 10; i++) {
        const double cellVoltage = ((batteryStatus.voltages[i] == UINT16_MAX)) ? qQNaN() : (static_cast<double>(batteryStatus.voltages[i]) / 1000.0);
        if (qIsNaN(cellVoltage)) {
            break;
        }
        if (i == 0) {
            totalVoltage = cellVoltage;
        } else {
            totalVoltage += cellVoltage;
        }
    }

    for (int i = 0; i < 4; i++) {
        const double cellVoltage = ((batteryStatus.voltages_ext[i] == 0)) ? qQNaN() : (static_cast<double>(batteryStatus.voltages_ext[i]) / 1000.0);
        if (qIsNaN(cellVoltage)) {
            break;
        }
        totalVoltage += cellVoltage;
    }

    const double current = (batteryStatus.current_battery == -1) ? qQNaN() : (static_cast<double>(batteryStatus.current_battery) / 100.0);

    double consumed;
    double consumedRaw;
    double consumedBasedRemaining;

    if (batteryStatus.current_consumed < 0) {
        consumed = qQNaN();
        consumedRaw = qQNaN();
        consumedBasedRemaining = qQNaN();
    } else {
        consumedRaw = batteryStatus.current_consumed;
        consumed = consumedRaw + getPersistedConsumed(vehicle, batteryStatus.id);
        setCurrentConsumed(vehicle, batteryStatus.id, consumed);

        const double maxCapacity = group->maxCapacity()->rawValue().toDouble();
        if (!qIsNaN(maxCapacity) && (maxCapacity != 0.0)) {
            consumedBasedRemaining = (maxCapacity - consumed) / maxCapacity * 100.0;
        } else {
            consumedBasedRemaining = qQNaN();
        }
    }

    group->function()->setRawValue(batteryStatus.battery_function);
    group->type()->setRawValue(batteryStatus.type);
    group->temperature()->setRawValue((batteryStatus.temperature == INT16_MAX) ? qQNaN() : (static_cast<double>(batteryStatus.temperature) / 100.0));
    group->voltage()->setRawValue(totalVoltage);
    group->current()->setRawValue(current);
    group->mahConsumed()->setRawValue(consumed);
    group->rawMahConsumed()->setRawValue(consumedRaw);
    group->consumedBasedRemaining()->setRawValue(consumedBasedRemaining);
    group->percentRemaining()->setRawValue((batteryStatus.battery_remaining == -1) ? qQNaN() : batteryStatus.battery_remaining);
    group->timeRemaining()->setRawValue((batteryStatus.time_remaining == 0) ? qQNaN() : batteryStatus.time_remaining);
    group->chargeState()->setRawValue(batteryStatus.charge_state);
    group->instantPower()->setRawValue(totalVoltage * group->current()->rawValue().toDouble());
    group->updateTimeUntilNextThreshold();

    group->_setTelemetryAvailable(true);
}

void VehicleBatteryFactGroup::handleMessageForFactGroupCreation(Vehicle *vehicle, const mavlink_message_t &message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        _findOrAddBatteryGroupById(vehicle, 0);
        break;
    case MAVLINK_MSG_ID_BATTERY_STATUS:
    {
        mavlink_battery_status_t batteryStatus{};
        mavlink_msg_battery_status_decode(&message, &batteryStatus);
        _findOrAddBatteryGroupById(vehicle, batteryStatus.id);
    }
    default:
        break;
    }
}

VehicleBatteryFactGroup *VehicleBatteryFactGroup::_findOrAddBatteryGroupById(Vehicle *vehicle, uint8_t batteryId)
{
    QmlObjectListModel *const batteries = vehicle->batteries();

    // We maintain the list in order sorted by battery id so the ui shows them sorted.
    for (int i = 0; i < batteries->count(); i++) {
        VehicleBatteryFactGroup *const group = batteries->value<VehicleBatteryFactGroup*>(i);
        const int listBatteryId = group->id()->rawValue().toInt();
        if (listBatteryId > batteryId) {
            VehicleBatteryFactGroup *const newBatteryGroup = new VehicleBatteryFactGroup(batteryId, batteries, vehicle);
            batteries->insert(i, newBatteryGroup);
            vehicle->_addFactGroup(newBatteryGroup, QStringLiteral("%1%2").arg(_batteryFactGroupNamePrefix).arg(batteryId));
            return newBatteryGroup;
        } else if (listBatteryId == batteryId) {
            return group;
        }
    }

    VehicleBatteryFactGroup *const newBatteryGroup = new VehicleBatteryFactGroup(batteryId, batteries, vehicle);
    batteries->append(newBatteryGroup);
    vehicle->_addFactGroup(newBatteryGroup, QStringLiteral("%1%2").arg(_batteryFactGroupNamePrefix).arg(batteryId));

    return newBatteryGroup;
}

void VehicleBatteryFactGroup::_timeRemainingChanged(const QVariant &value)
{
    if (qIsNaN(value.toDouble())) {
        _timeRemainingStrFact.setRawValue("--:--:--");
    } else {
        const int totalSeconds = value.toInt();
        const int hours = totalSeconds / 3600;
        const int minutes = (totalSeconds % 3600) / 60;
        const int seconds = totalSeconds % 60;

        _timeRemainingStrFact.setRawValue(QString::asprintf("%02dH:%02dM:%02dS", hours, minutes, seconds));
    }
}

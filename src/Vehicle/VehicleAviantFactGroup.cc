#include "VehicleAviantFactGroup.h"
#include "Vehicle.h"
#include "ParameterManager.h"
#include "mavlink.h"

#include <QtMath>
#include <algorithm>

const QColor VehicleAviantFactGroup::COLOR_UNKNOWN { QColor::fromRgb(128, 128, 128) }; // Gray
const QColor VehicleAviantFactGroup::COLOR_NOMINAL { QColor::fromRgb(0, 128, 0) }; // Green
const QColor VehicleAviantFactGroup::COLOR_WARNING { QColor::fromRgb(255, 170, 0) }; // Orange
const QColor VehicleAviantFactGroup::COLOR_CRITICAL { QColor::fromRgb(255, 0, 0) }; // Red

VehicleAviantFactGroup::VehicleAviantFactGroup(QObject* parent)
    : FactGroup     (100, ":/json/Vehicle/AviantFactGroup.json", parent),
    _navigationAccuracyFact   (0, navigationAccuracyFactName,   FactMetaData::valueTypeUint8),
    _navigationRedundancyFact (0, navigationRedundancyFactName, FactMetaData::valueTypeUint8),
    _navigationStatusFact     (0, navigationStatusFactName,     FactMetaData::valueTypeString),
    _atsStatusFact            (0, atsStatusFactName,            FactMetaData::valueTypeUint32),
    _fwIcingFact              (0, fwIcingFactName,              FactMetaData::valueTypeUint8),
    _tempPiInternalFact       (0, tempPiInternalFactName,       FactMetaData::valueTypeInt8),
    _tempAvionicsAmbientFact  (0, tempAvionicsAmbientFactName,  FactMetaData::valueTypeInt8),
    _tempFuselageAmbientFact  (0, tempFuselageAmbientFactName,  FactMetaData::valueTypeInt8),
    _tempOutsideAmbientFact   (0, tempOutsideAmbientFactName,   FactMetaData::valueTypeInt8),
    _tempModemInternalFact    (0, tempModemInternalFactName,     FactMetaData::valueTypeInt8),
    _tempBatteryInternalFact  (0, tempBatteryInternalFactName,   FactMetaData::valueTypeInt8),
    _tempAirspeedInternalFact (0, tempAirspeedInternalFactName,  FactMetaData::valueTypeInt8),
    _tempImuInternalFact      (0, tempImuInternalFactName,       FactMetaData::valueTypeInt8),
    _tempBaroInternalFact     (0, tempBaroInternalFactName,      FactMetaData::valueTypeInt8),
    _anomalousCurrentFact     (0, anomalousCurrentFactName,      FactMetaData::valueTypeDouble),
    _topMotorLoadFact         (0, topMotorLoadFactName,          FactMetaData::valueTypeDouble),
    _motVoltage0Fact          (0, motVoltage0FactName,           FactMetaData::valueTypeDouble),
    _motVoltage1Fact          (0, motVoltage1FactName,           FactMetaData::valueTypeDouble),
    _motVoltage2Fact          (0, motVoltage2FactName,           FactMetaData::valueTypeDouble),
    _motVoltage3Fact          (0, motVoltage3FactName,           FactMetaData::valueTypeDouble),
    _motVoltage4Fact          (0, motVoltage4FactName,           FactMetaData::valueTypeDouble),
    _motVoltage5Fact          (0, motVoltage5FactName,           FactMetaData::valueTypeDouble),
    _motVoltage6Fact          (0, motVoltage6FactName,           FactMetaData::valueTypeDouble),
    _motVoltage7Fact          (0, motVoltage7FactName,           FactMetaData::valueTypeDouble),
    _motVoltage8Fact          (0, motVoltage8FactName,           FactMetaData::valueTypeDouble),
    _motVoltage9Fact          (0, motVoltage9FactName,           FactMetaData::valueTypeDouble),
    _motVoltage10Fact         (0, motVoltage10FactName,          FactMetaData::valueTypeDouble),
    _motVoltage11Fact         (0, motVoltage11FactName,          FactMetaData::valueTypeDouble)
{
    _addFact(&_navigationAccuracyFact, navigationAccuracyFactName);
    _addFact(&_navigationRedundancyFact, navigationRedundancyFactName);
    _addFact(&_navigationStatusFact, navigationStatusFactName);
    _addFact(&_atsStatusFact, atsStatusFactName);
    _addFact(&_fwIcingFact, fwIcingFactName);
    _addFact(&_tempPiInternalFact, tempPiInternalFactName);
    _addFact(&_tempAvionicsAmbientFact, tempAvionicsAmbientFactName);
    _addFact(&_tempFuselageAmbientFact, tempFuselageAmbientFactName);
    _addFact(&_tempOutsideAmbientFact, tempOutsideAmbientFactName);
    _addFact(&_tempModemInternalFact, tempModemInternalFactName);
    _addFact(&_tempBatteryInternalFact, tempBatteryInternalFactName);
    _addFact(&_tempAirspeedInternalFact, tempAirspeedInternalFactName);
    _addFact(&_tempImuInternalFact, tempImuInternalFactName);
    _addFact(&_tempBaroInternalFact, tempBaroInternalFactName);
    _addFact(&_anomalousCurrentFact, anomalousCurrentFactName);
    _addFact(&_topMotorLoadFact, topMotorLoadFactName);
    _addFact(&_motVoltage0Fact, motVoltage0FactName);
    _addFact(&_motVoltage1Fact, motVoltage1FactName);
    _addFact(&_motVoltage2Fact, motVoltage2FactName);
    _addFact(&_motVoltage3Fact, motVoltage3FactName);
    _addFact(&_motVoltage4Fact, motVoltage4FactName);
    _addFact(&_motVoltage5Fact, motVoltage5FactName);
    _addFact(&_motVoltage6Fact, motVoltage6FactName);
    _addFact(&_motVoltage7Fact, motVoltage7FactName);
    _addFact(&_motVoltage8Fact, motVoltage8FactName);
    _addFact(&_motVoltage9Fact, motVoltage9FactName);
    _addFact(&_motVoltage10Fact, motVoltage10FactName);
    _addFact(&_motVoltage11Fact, motVoltage11FactName);
}

void VehicleAviantFactGroup::handleMessage(Vehicle* vehicle, mavlink_message_t& message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_AVIANT_NAV:
            return handleNavMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_ATS_STATUS:
            return handleAtsStatusMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_INDICATOR_MOTORS:
            return handleMotorsMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_INDICATOR_FW_ICING:
            return handleFwIcingMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_INDICATOR_TEMP_LOGGER:
            return handleTempLoggerMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_INDICATOR_TEMP_FC:
            return handleTempFcMsg(vehicle, message);
        default:
            return;
    }
}

void VehicleAviantFactGroup::setIndicatorColorOverride(Fact& fact, int state)
{
    switch (state) {
        case AVIANT_INDICATOR_STATE_INACTIVE:
            fact.setOverrideColor(COLOR_UNKNOWN);
            break;
        case AVIANT_INDICATOR_STATE_NOMINAL:
            fact.setOverrideColor(COLOR_NOMINAL);
            break;
        case AVIANT_INDICATOR_STATE_WARNING:
            fact.setOverrideColor(COLOR_WARNING);
            break;
        case AVIANT_INDICATOR_STATE_CRITICAL:
            fact.setOverrideColor(COLOR_CRITICAL);
            break;
        default:
            fact.unsetOverrideColor();
            break;
    }
}

void VehicleAviantFactGroup::handleNavMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_nav_t nav;
    mavlink_msg_aviant_nav_decode(&message, &nav);

    switch (nav.accuracy) {
        case AVIANT_NAV_ACCURACY_UNKNOWN:
            _navigationAccuracyFact.setOverrideColor(COLOR_UNKNOWN);
            break;
        case AVIANT_NAV_ACCURACY_ACCURATE:
            _navigationAccuracyFact.setOverrideColor(COLOR_NOMINAL);
            break;
        case AVIANT_NAV_ACCURACY_APPROXIMATE:
            _navigationAccuracyFact.setOverrideColor(COLOR_WARNING);
            break;
        case AVIANT_NAV_ACCURACY_FAILED:
            _navigationAccuracyFact.setOverrideColor(COLOR_CRITICAL);
            break;
        default:
            _navigationAccuracyFact.unsetOverrideColor();
            break;
    }
    _navigationAccuracyFact.setRawValue(nav.accuracy);

    switch (nav.redundancy){
        case AVIANT_NAV_REDUNDANCY_UNKNOWN:
            _navigationRedundancyFact.setOverrideColor(COLOR_UNKNOWN);
            break;
        case AVIANT_NAV_REDUNDANCY_REDUNDANT:
            _navigationRedundancyFact.setOverrideColor(COLOR_NOMINAL);
            break;
        case AVIANT_NAV_REDUNDANCY_SINGLE:
            _navigationRedundancyFact.setOverrideColor(COLOR_WARNING);
            break;
        case AVIANT_NAV_REDUNDANCY_FAILED:
            _navigationRedundancyFact.setOverrideColor(COLOR_CRITICAL);
            break;
        default:
            _navigationRedundancyFact.unsetOverrideColor();
            break;
    }
    _navigationRedundancyFact.setRawValue(nav.redundancy);

    // Combined navigationStatus: text from non-nominal sub-indicators, color from worst
    // Nav enums: UNKNOWN=0, nominal=1, degraded=2, FAILED=3 — maps directly to indicator state severity
    QString accText;
    if (nav.accuracy == AVIANT_NAV_ACCURACY_APPROXIMATE) accText = QStringLiteral("APX");
    else if (nav.accuracy == AVIANT_NAV_ACCURACY_FAILED) accText = QStringLiteral("FAIL");

    QString redText;
    if (nav.redundancy == AVIANT_NAV_REDUNDANCY_SINGLE) redText = QStringLiteral("SING");
    else if (nav.redundancy == AVIANT_NAV_REDUNDANCY_FAILED) redText = QStringLiteral("FAIL");

    QString statusText;
    if (!accText.isEmpty() && !redText.isEmpty()) {
        statusText = accText + QStringLiteral(" ") + redText;
    } else if (!accText.isEmpty()) {
        statusText = accText;
    } else if (!redText.isEmpty()) {
        statusText = redText;
    } else {
        statusText = QStringLiteral("--");
    }

    setIndicatorColorOverride(_navigationStatusFact, std::max((int)nav.accuracy, (int)nav.redundancy));
    _navigationStatusFact.setRawValue(statusText);
}

void VehicleAviantFactGroup::handleAtsStatusMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_ats_status_t atsStatus;
    mavlink_msg_aviant_ats_status_decode(&message, &atsStatus);

    // These should match atsStatus in AviantFactGroup.json
    constexpr uint32_t VALUE_DEACT = 0;
    constexpr uint32_t VALUE_OK = 1;
    constexpr uint32_t VALUE_WARN = 2;
    constexpr uint32_t VALUE_FAIL = 3;

    if (!atsStatus.ats_enabled) {
        _atsStatusFact.setOverrideColor(COLOR_UNKNOWN);
        _atsStatusFact.setRawValue(VALUE_DEACT);
        return;
    }

    // If the ATS is enabled, a timeout is critical
    _atsStatusFact.setTimeoutIndicatorColor(COLOR_CRITICAL);

    uint32_t ats_status_flags = atsStatus.ats_status_flags;
    if (!atsStatus.power_loss_trigger_enabled) {
        ats_status_flags &= ~AVIANT_ATS_STATUS_FLAG_POWER_LOSS;
        ats_status_flags &= ~AVIANT_ATS_STATUS_FLAG_UPS_UNHEALTHY;
    }
    const bool ats_status_ok = ats_status_flags == 0;

    const time_t now = time(nullptr);

    if (ats_status_ok) {
        _time_last_ats_status_ok = now;
    }

    if (ats_status_ok) {
        _atsStatusFact.setOverrideColor(COLOR_NOMINAL);
        _atsStatusFact.setRawValue(VALUE_OK);
    } else if (now - _time_last_ats_status_ok > ATS_STATUS_HYSTERESIS_S) {
        _atsStatusFact.setOverrideColor(COLOR_CRITICAL);
        _atsStatusFact.setRawValue(VALUE_FAIL);
    } else {
        _atsStatusFact.setOverrideColor(COLOR_WARNING);
        _atsStatusFact.setRawValue(VALUE_WARN);
    }
}

void VehicleAviantFactGroup::resolveRotorClassification(Vehicle* vehicle)
{
    if (_rotorClassificationResolved || !vehicle) {
        return;
    }

    ParameterManager* pm = vehicle->parameterManager();
    if (!pm || !pm->parametersReady() || !pm->parameterExists(-1, QStringLiteral("CA_ROTOR_COUNT"))) {
        return;
    }

    int rotorCount = pm->getParameter(-1, QStringLiteral("CA_ROTOR_COUNT"))->rawValue().toInt();
    _upwardsMotorsMask = 0;

    for (int i = 0; i < std::min(rotorCount, NUM_ROTORS_MAX); i++) {
        QString axParam = QStringLiteral("CA_ROTOR%1_AX").arg(i);
        QString ayParam = QStringLiteral("CA_ROTOR%1_AY").arg(i);
        QString azParam = QStringLiteral("CA_ROTOR%1_AZ").arg(i);

        if (!pm->parameterExists(-1, axParam)) {
            continue;
        }

        float ax = pm->getParameter(-1, axParam)->rawValue().toFloat();
        float ay = pm->getParameter(-1, ayParam)->rawValue().toFloat();
        float az = pm->getParameter(-1, azParam)->rawValue().toFloat();

        // Same logic as PX4 getUpwardsMotors(): vertical thrust = axis ~(0, 0, -1)
        if (fabsf(ax) < 0.1f && fabsf(ay) < 0.1f && az < -0.5f) {
            _upwardsMotorsMask |= (1u << i);
        }
    }

    _rotorClassificationResolved = true;
}

void VehicleAviantFactGroup::handleMotorsMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_indicator_motors_t motors;
    mavlink_msg_aviant_indicator_motors_decode(&message, &motors);

    if (!_rotorClassificationResolved) {
        resolveRotorClassification(vehicle);
    }

    for (int i = 0; i < 12; i++) {
        setIndicatorColorOverride(*_motVoltageFacts[i], motors.mot_load_status[i]);
        _motVoltageFacts[i]->setRawValue(motors.mot_voltage_cv[i] / 100.0);
    }

    // topMotorLoad: max voltage across upwards (top) motors, colored by worst state
    if (_upwardsMotorsMask != 0) {
        int worstState = AVIANT_INDICATOR_STATE_INACTIVE;
        int16_t maxVoltage = INT16_MIN;

        for (int i = 0; i < NUM_ROTORS_MAX; i++) {
            if (!(_upwardsMotorsMask & (1u << i))) {
                continue;
            }
            worstState = std::max(worstState, (int)motors.mot_load_status[i]);
            maxVoltage = std::max(maxVoltage, motors.mot_voltage_cv[i]);
        }

        setIndicatorColorOverride(_topMotorLoadFact, worstState);
        _topMotorLoadFact.setRawValue(maxVoltage / 100.0);
    }

    setIndicatorColorOverride(_anomalousCurrentFact, motors.anomalous_current_status);
    _anomalousCurrentFact.setRawValue(motors.anomalous_current_ca / 100.0);
}

void VehicleAviantFactGroup::handleFwIcingMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_indicator_fw_icing_t fwIcing;
    mavlink_msg_aviant_indicator_fw_icing_decode(&message, &fwIcing);

    setIndicatorColorOverride(_fwIcingFact, fwIcing.state);
    _fwIcingFact.setRawValue(fwIcing.state);
}

void VehicleAviantFactGroup::handleTempLoggerMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_indicator_temp_logger_t temp;
    mavlink_msg_aviant_indicator_temp_logger_decode(&message, &temp);

    setIndicatorColorOverride(_tempPiInternalFact, temp.pi_internal_state);
    _tempPiInternalFact.setRawValue(temp.pi_internal);

    setIndicatorColorOverride(_tempAvionicsAmbientFact, temp.avionics_ambient_state);
    _tempAvionicsAmbientFact.setRawValue(temp.avionics_ambient);

    setIndicatorColorOverride(_tempFuselageAmbientFact, temp.fuselage_ambient_state);
    _tempFuselageAmbientFact.setRawValue(temp.fuselage_ambient);

    setIndicatorColorOverride(_tempOutsideAmbientFact, temp.outside_ambient_state);
    _tempOutsideAmbientFact.setRawValue(temp.outside_ambient);

    setIndicatorColorOverride(_tempModemInternalFact, temp.modem_internal_state);
    _tempModemInternalFact.setRawValue(temp.modem_internal);
}

void VehicleAviantFactGroup::handleTempFcMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_indicator_temp_fc_t temp;
    mavlink_msg_aviant_indicator_temp_fc_decode(&message, &temp);

    setIndicatorColorOverride(_tempBatteryInternalFact, temp.battery_internal_state);
    _tempBatteryInternalFact.setRawValue(temp.battery_internal);

    setIndicatorColorOverride(_tempAirspeedInternalFact, temp.airspeed_internal_state);
    _tempAirspeedInternalFact.setRawValue(temp.airspeed_internal);

    setIndicatorColorOverride(_tempImuInternalFact, temp.imu_internal_state);
    _tempImuInternalFact.setRawValue(temp.imu_internal);

    setIndicatorColorOverride(_tempBaroInternalFact, temp.baro_internal_state);
    _tempBaroInternalFact.setRawValue(temp.baro_internal);
}

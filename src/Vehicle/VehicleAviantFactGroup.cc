#include "VehicleAviantFactGroup.h"
#include "Vehicle.h"
#include "mavlink.h"

#include <QtMath>

const QColor VehicleAviantFactGroup::COLOR_UNKNOWN { QColor::fromRgb(128, 128, 128) }; // Gray
const QColor VehicleAviantFactGroup::COLOR_NOMINAL { QColor::fromRgb(0, 128, 0) }; // Green
const QColor VehicleAviantFactGroup::COLOR_WARNING { QColor::fromRgb(255, 170, 0) }; // Orange
const QColor VehicleAviantFactGroup::COLOR_CRITICAL { QColor::fromRgb(255, 0, 0) }; // Red

VehicleAviantFactGroup::VehicleAviantFactGroup(QObject* parent)
    : FactGroup     (100, ":/json/Vehicle/AviantFactGroup.json", parent),
    _navigationAccuracyFact(0, navigationAccuracyFactName, FactMetaData::valueTypeUint8),
    _navigationRedundancyFact(0, navigationRedundancyFactName, FactMetaData::valueTypeUint8),
    _atsStatusFact(0, atsStatusFactName, FactMetaData::valueTypeUint32)
{

    _addFact(&_navigationAccuracyFact, navigationAccuracyFactName);
    _addFact(&_navigationRedundancyFact, navigationRedundancyFactName);
    _addFact(&_atsStatusFact, atsStatusFactName);
    _navigationAccuracyFact.setRawValue(qQNaN());
    _navigationRedundancyFact.setRawValue(qQNaN());
    _atsStatusFact.setRawValue(0);
}

void VehicleAviantFactGroup::handleMessage(Vehicle* vehicle, mavlink_message_t& message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_AVIANT_NAV:
            return handleNavMsg(vehicle, message);
        case MAVLINK_MSG_ID_AVIANT_ATS_STATUS:
            return handleAtsStatusMsg(vehicle, message);
        default:
            return;
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
}

void VehicleAviantFactGroup::handleAtsStatusMsg(Vehicle* vehicle, mavlink_message_t& message)
{
    mavlink_aviant_ats_status_t atsStatus;
    mavlink_msg_aviant_ats_status_decode(&message, &atsStatus);

    // These should match atsStatus in AviantFactGroup.json
    constexpr uint32_t VALUE_BLANK = 0;
    constexpr uint32_t VALUE_OK = 1;
    constexpr uint32_t VALUE_WARN = 2;
    constexpr uint32_t VALUE_FAIL = 3;

    if (!atsStatus.ats_enabled) {
        _atsStatusFact.setOverrideColor(COLOR_UNKNOWN);
        _atsStatusFact.setRawValue(VALUE_BLANK);
        return;
    }
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

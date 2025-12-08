#include "VehicleAviantFactGroup.h"
#include "Vehicle.h"

#include <QtMath>

const QColor VehicleAviantFactGroup::COLOR_UNKNOWN { Qt::gray };
const QColor VehicleAviantFactGroup::COLOR_NOMINAL  { Qt::green };
const QColor VehicleAviantFactGroup::COLOR_WARNING  { Qt::yellow };
const QColor VehicleAviantFactGroup::COLOR_CRITICAL { Qt::red };

VehicleAviantFactGroup::VehicleAviantFactGroup(QObject* parent)
    : FactGroup     (100, ":/json/Vehicle/AviantFactGroup.json", parent),
    _navigationAccuracyFact(0, navigationAccuracyFactName, FactMetaData::valueTypeUint8),
    _navigationRedundancyFact(0, navigationRedundancyFactName, FactMetaData::valueTypeUint8)
{

    _addFact(&_navigationAccuracyFact, navigationAccuracyFactName);
    _addFact(&_navigationRedundancyFact, navigationRedundancyFactName);
    _navigationAccuracyFact.setRawValue(qQNaN());
    _navigationRedundancyFact.setRawValue(qQNaN());
}

void VehicleAviantFactGroup::handleMessage(Vehicle* vehicle, mavlink_message_t& message)
{
    switch (message.msgid) {
        case MAVLINK_MSG_ID_AVIANT_NAV:
            return handleNavMsg(vehicle, message);
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

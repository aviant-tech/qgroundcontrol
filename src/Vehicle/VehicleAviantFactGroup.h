#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"

#include <QColor>
#include <ctime>

class VehicleAviantFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleAviantFactGroup(QObject* parent = nullptr);
    Q_PROPERTY(Fact* navigationAccuracy    READ navigationAccuracy    CONSTANT)
    Q_PROPERTY(Fact* navigationRedundancy  READ navigationRedundancy  CONSTANT)
    Q_PROPERTY(Fact* atsStatus             READ atsStatus             CONSTANT)

    Fact* navigationAccuracy() { return &_navigationAccuracyFact; }
    Fact* navigationRedundancy() { return &_navigationRedundancyFact; }
    Fact* atsStatus() { return &_atsStatusFact; }
    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

private:
    void handleNavMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleAtsStatusMsg(Vehicle* vehicle, mavlink_message_t& message);

    static void setDefaultColorOverride(Fact& fact, int state);

    Fact _navigationAccuracyFact;
    Fact _navigationRedundancyFact;
    Fact _atsStatusFact;

    static constexpr const char* navigationAccuracyFactName = "navigationAccuracy";
    static constexpr const char* navigationRedundancyFactName = "navigationRedundancy";
    static constexpr const char* atsStatusFactName = "atsStatus";

    static const QColor COLOR_UNKNOWN;
    static const QColor COLOR_NOMINAL;
    static const QColor COLOR_WARNING;
    static const QColor COLOR_CRITICAL;

    static constexpr uint32_t ATS_STATUS_HYSTERESIS_S = 1;
    time_t _time_last_ats_status_ok = 0;
};

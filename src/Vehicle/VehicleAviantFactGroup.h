#pragma once

#include "FactGroup.h"
#include "QGCMAVLink.h"

#include <QColor>

class VehicleAviantFactGroup : public FactGroup
{
    Q_OBJECT

public:
    VehicleAviantFactGroup(QObject* parent = nullptr);
    Q_PROPERTY(Fact* navigationAccuracy    READ navigationAccuracy    CONSTANT)
    Q_PROPERTY(Fact* navigationRedundancy  READ navigationRedundancy  CONSTANT)

    Fact* navigationAccuracy() { return &_navigationAccuracyFact; }
    Fact* navigationRedundancy() { return &_navigationRedundancyFact; }
    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

private:
    void handleNavMsg(Vehicle* vehicle, mavlink_message_t& message);

    static void setDefaultColorOverride(Fact& fact, int state);

    Fact _navigationAccuracyFact;
    Fact _navigationRedundancyFact;

    static constexpr const char* navigationAccuracyFactName = "navigationAccuracy";
    static constexpr const char* navigationRedundancyFactName = "navigationRedundancy";

    static const QColor COLOR_UNKNOWN;
    static const QColor COLOR_NOMINAL;
    static const QColor COLOR_WARNING;
    static const QColor COLOR_CRITICAL;
};

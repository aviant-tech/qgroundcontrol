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
    Q_PROPERTY(Fact* navigationStatus      READ navigationStatus      CONSTANT)
    Q_PROPERTY(Fact* atsStatus             READ atsStatus             CONSTANT)
    Q_PROPERTY(Fact* fwIcing               READ fwIcing               CONSTANT)
    Q_PROPERTY(Fact* tempPiInternal        READ tempPiInternal        CONSTANT)
    Q_PROPERTY(Fact* tempAvionicsAmbient   READ tempAvionicsAmbient   CONSTANT)
    Q_PROPERTY(Fact* tempFuselageAmbient   READ tempFuselageAmbient   CONSTANT)
    Q_PROPERTY(Fact* tempOutsideAmbient    READ tempOutsideAmbient    CONSTANT)
    Q_PROPERTY(Fact* tempModemInternal     READ tempModemInternal     CONSTANT)
    Q_PROPERTY(Fact* tempBatteryInternal   READ tempBatteryInternal   CONSTANT)
    Q_PROPERTY(Fact* tempAirspeedInternal  READ tempAirspeedInternal  CONSTANT)
    Q_PROPERTY(Fact* tempImuInternal       READ tempImuInternal       CONSTANT)
    Q_PROPERTY(Fact* tempBaroInternal      READ tempBaroInternal      CONSTANT)
    Q_PROPERTY(Fact* anomalousCurrent      READ anomalousCurrent      CONSTANT)
    Q_PROPERTY(Fact* topMotorLoad          READ topMotorLoad          CONSTANT)
    Q_PROPERTY(Fact* motVoltage0           READ motVoltage0           CONSTANT)
    Q_PROPERTY(Fact* motVoltage1           READ motVoltage1           CONSTANT)
    Q_PROPERTY(Fact* motVoltage2           READ motVoltage2           CONSTANT)
    Q_PROPERTY(Fact* motVoltage3           READ motVoltage3           CONSTANT)
    Q_PROPERTY(Fact* motVoltage4           READ motVoltage4           CONSTANT)
    Q_PROPERTY(Fact* motVoltage5           READ motVoltage5           CONSTANT)
    Q_PROPERTY(Fact* motVoltage6           READ motVoltage6           CONSTANT)
    Q_PROPERTY(Fact* motVoltage7           READ motVoltage7           CONSTANT)
    Q_PROPERTY(Fact* motVoltage8           READ motVoltage8           CONSTANT)
    Q_PROPERTY(Fact* motVoltage9           READ motVoltage9           CONSTANT)
    Q_PROPERTY(Fact* motVoltage10          READ motVoltage10          CONSTANT)
    Q_PROPERTY(Fact* motVoltage11          READ motVoltage11          CONSTANT)

    Fact* navigationAccuracy() { return &_navigationAccuracyFact; }
    Fact* navigationRedundancy() { return &_navigationRedundancyFact; }
    Fact* navigationStatus() { return &_navigationStatusFact; }
    Fact* atsStatus() { return &_atsStatusFact; }
    Fact* fwIcing() { return &_fwIcingFact; }
    Fact* tempPiInternal() { return &_tempPiInternalFact; }
    Fact* tempAvionicsAmbient() { return &_tempAvionicsAmbientFact; }
    Fact* tempFuselageAmbient() { return &_tempFuselageAmbientFact; }
    Fact* tempOutsideAmbient() { return &_tempOutsideAmbientFact; }
    Fact* tempModemInternal() { return &_tempModemInternalFact; }
    Fact* tempBatteryInternal() { return &_tempBatteryInternalFact; }
    Fact* tempAirspeedInternal() { return &_tempAirspeedInternalFact; }
    Fact* tempImuInternal() { return &_tempImuInternalFact; }
    Fact* tempBaroInternal() { return &_tempBaroInternalFact; }
    Fact* anomalousCurrent() { return &_anomalousCurrentFact; }
    Fact* topMotorLoad() { return &_topMotorLoadFact; }
    Fact* motVoltage0() { return &_motVoltage0Fact; }
    Fact* motVoltage1() { return &_motVoltage1Fact; }
    Fact* motVoltage2() { return &_motVoltage2Fact; }
    Fact* motVoltage3() { return &_motVoltage3Fact; }
    Fact* motVoltage4() { return &_motVoltage4Fact; }
    Fact* motVoltage5() { return &_motVoltage5Fact; }
    Fact* motVoltage6() { return &_motVoltage6Fact; }
    Fact* motVoltage7() { return &_motVoltage7Fact; }
    Fact* motVoltage8() { return &_motVoltage8Fact; }
    Fact* motVoltage9() { return &_motVoltage9Fact; }
    Fact* motVoltage10() { return &_motVoltage10Fact; }
    Fact* motVoltage11() { return &_motVoltage11Fact; }

    // Overrides from FactGroup
    void handleMessage(Vehicle* vehicle, mavlink_message_t& message) override;

private:
    void handleNavMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleAtsStatusMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleMotorsMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleFwIcingMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleTempLoggerMsg(Vehicle* vehicle, mavlink_message_t& message);
    void handleTempFcMsg(Vehicle* vehicle, mavlink_message_t& message);

    static void setIndicatorColorOverride(Fact& fact, int state);
    void resolveRotorClassification(Vehicle* vehicle);

    Fact _navigationAccuracyFact;
    Fact _navigationRedundancyFact;
    Fact _navigationStatusFact;
    Fact _atsStatusFact;
    Fact _fwIcingFact;
    Fact _tempPiInternalFact;
    Fact _tempAvionicsAmbientFact;
    Fact _tempFuselageAmbientFact;
    Fact _tempOutsideAmbientFact;
    Fact _tempModemInternalFact;
    Fact _tempBatteryInternalFact;
    Fact _tempAirspeedInternalFact;
    Fact _tempImuInternalFact;
    Fact _tempBaroInternalFact;
    Fact _anomalousCurrentFact;
    Fact _topMotorLoadFact;
    Fact _motVoltage0Fact;
    Fact _motVoltage1Fact;
    Fact _motVoltage2Fact;
    Fact _motVoltage3Fact;
    Fact _motVoltage4Fact;
    Fact _motVoltage5Fact;
    Fact _motVoltage6Fact;
    Fact _motVoltage7Fact;
    Fact _motVoltage8Fact;
    Fact _motVoltage9Fact;
    Fact _motVoltage10Fact;
    Fact _motVoltage11Fact;

    Fact* _motVoltageFacts[12] = {
        &_motVoltage0Fact, &_motVoltage1Fact, &_motVoltage2Fact, &_motVoltage3Fact,
        &_motVoltage4Fact, &_motVoltage5Fact, &_motVoltage6Fact, &_motVoltage7Fact,
        &_motVoltage8Fact, &_motVoltage9Fact, &_motVoltage10Fact, &_motVoltage11Fact
    };

    static constexpr const char* navigationAccuracyFactName = "navigationAccuracy";
    static constexpr const char* navigationRedundancyFactName = "navigationRedundancy";
    static constexpr const char* navigationStatusFactName = "navigationStatus";
    static constexpr const char* atsStatusFactName = "atsStatus";
    static constexpr const char* fwIcingFactName = "fwIcing";
    static constexpr const char* tempPiInternalFactName = "tempPiInternal";
    static constexpr const char* tempAvionicsAmbientFactName = "tempAvionicsAmbient";
    static constexpr const char* tempFuselageAmbientFactName = "tempFuselageAmbient";
    static constexpr const char* tempOutsideAmbientFactName = "tempOutsideAmbient";
    static constexpr const char* tempModemInternalFactName = "tempModemInternal";
    static constexpr const char* tempBatteryInternalFactName = "tempBatteryInternal";
    static constexpr const char* tempAirspeedInternalFactName = "tempAirspeedInternal";
    static constexpr const char* tempImuInternalFactName = "tempImuInternal";
    static constexpr const char* tempBaroInternalFactName = "tempBaroInternal";
    static constexpr const char* anomalousCurrentFactName = "anomalousCurrent";
    static constexpr const char* topMotorLoadFactName = "topMotorLoad";
    static constexpr const char* motVoltage0FactName = "motVoltage0";
    static constexpr const char* motVoltage1FactName = "motVoltage1";
    static constexpr const char* motVoltage2FactName = "motVoltage2";
    static constexpr const char* motVoltage3FactName = "motVoltage3";
    static constexpr const char* motVoltage4FactName = "motVoltage4";
    static constexpr const char* motVoltage5FactName = "motVoltage5";
    static constexpr const char* motVoltage6FactName = "motVoltage6";
    static constexpr const char* motVoltage7FactName = "motVoltage7";
    static constexpr const char* motVoltage8FactName = "motVoltage8";
    static constexpr const char* motVoltage9FactName = "motVoltage9";
    static constexpr const char* motVoltage10FactName = "motVoltage10";
    static constexpr const char* motVoltage11FactName = "motVoltage11";

    static const QColor COLOR_UNKNOWN;
    static const QColor COLOR_NOMINAL;
    static const QColor COLOR_WARNING;
    static const QColor COLOR_CRITICAL;

    static constexpr uint32_t ATS_STATUS_HYSTERESIS_S = 1;
    time_t _time_last_ats_status_ok = 0;

    static constexpr int NUM_ROTORS_MAX = 12;
    uint32_t _upwardsMotorsMask = 0;
    bool     _rotorClassificationResolved = false;
};

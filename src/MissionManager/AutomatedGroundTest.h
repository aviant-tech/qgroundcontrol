/****************************************************************************
 *
 * (c) 2023 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>
#include <QThread>

#include "Vehicle.h"

class PlanMasterController;

class AutoGroundTestWorker : public QObject
{
    Q_OBJECT

public slots:
    void doWork() {
        emit runLoop();
    }

signals:
    void runLoop();
};

class AutomatedGroundTest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(State currentState READ currentState NOTIFY currentStateChanged)

public:

    enum State {
        Init = 0,
        FileLoaded,
        Running,
        Stop
    };

    Q_ENUM(State)

    AutomatedGroundTest(PlanMasterController* masterController, QObject* parent = nullptr);
    ~AutomatedGroundTest();

    QString statusText() { return _statusText; }
    State currentState() { return _currentState; }

    bool load (const QJsonObject& json, QString& errorString);

signals:
    void statusTextChanged();
    void currentStateChanged();
    void runLoop();

public slots:
    Q_INVOKABLE void buttonClicked();
    void setActiveVehicle  (Vehicle* vehicle);

private:

    void _run();
    void _startOffboardMode();
    void _loopHandler();

    PlanMasterController*   _masterController;
    MultiVehicleManager *_manager;
    Vehicle* _vehicle;

    const QString kCmdArm = "arm";
    const QString kCmdDisarm = "disarm";
    const QString kCmdActuatorControl = "actuatorControl";

    const QString kMethoPromptly = "promptly";
    const QString kMethodRamp = "ramp";
    const QString kMethodFlat = "flat";

    const uint32_t kSampleTime = 100; //ms

    const float kVehicleModeFlagCustomModeEnabled = 1; // In PX4 VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED
    const float kPx4CustomMainModeOffboard = 6; // In PX4 PX4_CUSTOM_MAIN_MODE_OFFBOARD

    State _currentState = Init;
    QString _statusText = "Load test plan";
    QJsonObject _jsonObject;
    int32_t _item_nuber = 0;
    int32_t _last_item_nuber = 0;
    QElapsedTimer _timer;
    QThread _workerThread;
};

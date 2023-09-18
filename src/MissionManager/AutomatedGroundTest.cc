/****************************************************************************
 *
 * (c) 2023 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AutomatedGroundTest.h"
#include "PlanMasterController.h"

#include "JsonHelper.h"
#include "QJsonArray"

AutomatedGroundTest::AutomatedGroundTest(PlanMasterController* masterController, QObject* parent)
    : QObject (parent)
    , _masterController     (masterController)
{
    AutoGroundTestWorker *worker = new AutoGroundTestWorker;
    worker->moveToThread(&_workerThread);
    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &AutomatedGroundTest::runLoop, worker, &AutoGroundTestWorker::doWork);
    connect(worker, &AutoGroundTestWorker::runLoop, this, &AutomatedGroundTest::_loopHandler);
    _workerThread.start();
    _workerThread.setPriority(QThread::HighPriority);

    _manager = qgcApp()->toolbox()->multiVehicleManager();
    connect(_manager, &MultiVehicleManager::activeVehicleChanged, this, &AutomatedGroundTest::setActiveVehicle);
    setActiveVehicle(_manager->activeVehicle());
}

AutomatedGroundTest::~AutomatedGroundTest()
{
    _workerThread.quit();
    _workerThread.wait();
}

void AutomatedGroundTest::setActiveVehicle(Vehicle* vehicle)
{
    _vehicle = vehicle;
}

void AutomatedGroundTest::_loopHandler()
{
    _run();
}

void AutomatedGroundTest::buttonClicked()
{
    switch (_currentState) {

    case FileLoaded:
        _currentState = Running;
        break;

    case Stop:
        _currentState = Running;
        emit runLoop();
        break;

    case Running:
        _vehicle->emergencyStop();
        _currentState = Stop;
        _statusText = "Emergency Stop";
        emit statusTextChanged();
        break;

    default:
        break;
    }
    emit currentStateChanged();
}

bool AutomatedGroundTest::load(const QJsonObject& json, QString& errorString)
{
    errorString.clear();

    if (json[JsonHelper::jsonVersionKey].toInt() != 1) {
        errorString = tr("The plan file is not compatible with this version of Automated ground test.");
        return false;
    }

    _jsonObject = json;
    _statusText = "Test plan successfully loaded";
    _currentState = FileLoaded;
    emit currentStateChanged();
    emit statusTextChanged();
    emit runLoop();

    return true;
}

void AutomatedGroundTest::_startOffboardMode()
{
    _vehicle->sendMavCommandWithHandler(
        nullptr,                            // Ack callback
        this,                               // Ack callback data
        MAV_COMP_ID_AUTOPILOT1,             // the ID of the autopilot
        MAV_CMD_DO_SET_MODE,                // the mavlink command
        kVehicleModeFlagCustomModeEnabled,  // param1
        kPx4CustomMainModeOffboard,         // param2
        0,                                  // unused param3
        0,                                  // unused param4
        0,                                  // unused param5
        0,                                  // unused param6
        0);                                 // unused param7
}

void AutomatedGroundTest::_run()
{

    bool keepRunning = true;

    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float throttle = 0.0f;

    switch (_currentState) {

    case Running: {

        bool nextItem = false;
        QJsonArray jsonItems = _jsonObject["items"].toArray();

        QString command = jsonItems[_item_nuber].toObject()["command"].toString();

        _statusText = command;
        emit statusTextChanged();

        if (_last_item_nuber != _item_nuber) {
            _timer.restart();
        }

        _last_item_nuber = _item_nuber;

        if (kCmdArm == command) {

            _startOffboardMode();
            _vehicle->setArmedShowError(true);
            nextItem = true;

        } else if (kCmdDisarm == command) {

            //_vehicle->setArmed(false, true); //TODO: graceful or emergency end?
            _vehicle->emergencyStop();
            nextItem = true;

        } else if (kCmdActuatorControl == command) {

            float coefficient = 0.0f;

            QString method = jsonItems[_item_nuber].toObject()["method"].toString();

            if (kMethoPromptly == method) {
                coefficient = 1.0f;
                nextItem = true;
            } else {

                qint64 timeElapsed = _timer.elapsed();
                qint64 executionTime = jsonItems[_item_nuber].toObject()["executionTime"].toInt() * 1000;
                if (timeElapsed > executionTime) {
                    nextItem = true;
                }

                if (kMethodRamp == method) {

                    coefficient = 1.0f - (float(executionTime - timeElapsed) / float(executionTime));

                    if (coefficient > 1.0f) {
                        coefficient = 1.0f;
                    }

                } else if (kMethodFlat == method) {
                    coefficient = 1.0f;
                }
            }

            roll = coefficient * float(jsonItems[_item_nuber].toObject()["actuatorControl"].toObject()["roll"].toInt()) / 100.0f;
            pitch = coefficient * float(jsonItems[_item_nuber].toObject()["actuatorControl"].toObject()["pitch"].toInt()) / 100.0f;
            yaw = coefficient * float(jsonItems[_item_nuber].toObject()["actuatorControl"].toObject()["yaw"].toInt()) / 100.0f;
            throttle = coefficient * float(jsonItems[_item_nuber].toObject()["actuatorControl"].toObject()["throttle"].toInt()) / 100.0f;
        }

        if (nextItem && (_item_nuber < jsonItems.size())) {
            ++_item_nuber;
        } else if (_item_nuber >= jsonItems.size()) {
            _item_nuber = 0;
            _currentState = Stop;
            emit currentStateChanged();
        }

        break;
    }

    case Stop:
        keepRunning = false;
        break;

    case FileLoaded:
    case Init:
    default:
        break;

    }

    //MAVLink send MAVLINK_MSG_ID_SET_ACTUATOR_CONTROL_TARGET
    WeakLinkInterfacePtr weakLink = _vehicle->vehicleLinkManager()->primaryLink();
    if (!weakLink.expired()) {
        SharedLinkInterfacePtr sharedLink = weakLink.lock();

        float controls[8] = {0};
        controls[0] = roll;
        controls[1] = pitch;
        controls[2] = yaw;
        controls[3] = throttle;

        MAVLinkProtocol* mavlink = qgcApp()->toolbox()->mavlinkProtocol();
        mavlink_message_t msg;
        mavlink_msg_set_actuator_control_target_pack_chan(
            static_cast<uint8_t>(mavlink->getSystemId()),
            static_cast<uint8_t>(mavlink->getComponentId()),
            sharedLink->mavlinkChannel(),
            &msg,
            0, // time_usec
            0, // group_mlx
            0, // target_system
            MAV_COMP_ID_AUTOPILOT1,
            controls);

        _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), msg);
    }

    if (keepRunning) {
        _workerThread.msleep(kSampleTime);
        emit runLoop();
    }
}

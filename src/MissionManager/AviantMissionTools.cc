/****************************************************************************
 *
 * (c) 2023 Aviant As
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "AviantMissionTools.h"
#include "PlanMasterController.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include "AppSettings.h"
#include "JsonHelper.h"

AviantMissionTools::AviantMissionTools(QObject* parent)
    : QObject           (parent)
    , _masterController (nullptr)
{
    _networkAccessManager = new QNetworkAccessManager();
    connect(_networkAccessManager, &QNetworkAccessManager::finished, this, &AviantMissionTools::_requestComplete);
}

AviantMissionTools::AviantMissionTools(PlanMasterController* masterController, QObject* parent)
    : QObject           (parent)
    , _masterController (masterController)
{
    _networkAccessManager = new QNetworkAccessManager();
    connect(_networkAccessManager, &QNetworkAccessManager::finished, this, &AviantMissionTools::_requestComplete);
}

AviantMissionTools::~AviantMissionTools()
{
    if (_networkAccessManager) delete _networkAccessManager;
    _networkAccessManager = nullptr;
}
    
void AviantMissionTools::setTakeoffType(TakeoffType takeoffType)
{
    if (takeoffType == _takeoffType) return;

    // Invalidate prior mission validation when type changes:
    _lastValidatedJson = QJsonDocument();
    _validationResult = "Not validated";
    _takeoffType = takeoffType;
    emit stateChanged();
}

void AviantMissionTools::setWinchType(WinchType winchType)
{
    if (winchType == _winchType) return;

    // Invalidate prior mission validation when type changes:
    _lastValidatedJson = QJsonDocument();
    _validationResult = "Not validated";
    _winchType = winchType;
    emit stateChanged();
}

QUrl AviantMissionTools::_getMmsUrl(Operation operation, QString base)
{
    switch (operation) {
        case MissionValidation:
            return QUrl(base + "/validate_mission");
        case RallyPointHeight:
            return QUrl(base + "/set_rally_points_height");
            break;
        case NoOperation:
        default:
            return QUrl();
    }
}

QUrl AviantMissionTools::_getKyteBackendUrl(Operation operation, QString base)
{
    switch (operation) {
        case FetchKyteOrders:
            return QUrl(base + "/orders/api/v2/orders/active/"); 
        case NoOperation:
        default:
            return QUrl();
    }
}

QUrl AviantMissionTools::_getKyteBackendUrl(Operation operation, QString base, int orderId) {
    if (operation == FetchKyteOrderMissionFile) {
        return QUrl(base + "/orders/api/v2/orders/" + QString::number(orderId) + "/mission-file/");
    } else {
        return _getKyteBackendUrl(operation, base);
    }
}

QString AviantMissionTools::_getOperationName(Operation operation)
{
    switch (operation) {
        case MissionValidation:
            return QString("MissionValidation");
        case RallyPointHeight:
            return QString("RallyPointHeight");
        case NoOperation:
            return QString("NoOperation");
        case FetchKyteOrderMissionFile:
            return QString("FetchKyteOrderMissionFile");
        default:
            return QString("Unknown");
    }
}

// Returns the name that is used in request parameter "takeoff"
QString AviantMissionTools::_getTakeoffTypeName(TakeoffType takeoffType)
{
    switch (takeoffType) {
        case TakeoffTypeNotSet:
            return QString("NOT_SET");
        case Headless:
            return QString("HEADLESS");
        case VTOL:
            return QString("VTOL");
        default:
            return QString("UNKNOWN");
    }
}
// Returns the name that is used in request parameter "winch"
QString AviantMissionTools::_getWinchTypeName(WinchType winchType)
{
    switch (winchType) {
        case WinchTypeNotSet:
            return QString("NOT_SET");
        case NoWinch:
            return QString("NONE");
        case V1:
            return QString("V1");
        case V2:
            return QString("V2");
        case V3:
            return QString("V3");
        case Custom:
            return QString("CUSTOM");
        default:
            return QString("UNKNOWN");
    }
}

QStringList AviantMissionTools::takeoffTypeList(void) const
{
    return {"---", "VTOL", "Headless"};
}

QStringList AviantMissionTools::winchTypeList(void) const
{
    return {"---", "None", "V1", "V2", "V3", "Custom"};
}

bool AviantMissionTools::_takeoffTypeRequired(Operation operation)
{
    switch (operation) {
        case MissionValidation:
            return true;
        case RallyPointHeight:
        case NoOperation:
        default:
            return false;
    }
}

bool AviantMissionTools::_winchTypeRequired(Operation operation)
{
    switch (operation) {
        case MissionValidation:
            return true;
        case RallyPointHeight:
        case NoOperation:
        default:
            return false;
    }
}

void AviantMissionTools::requestOperation(Operation operation)
{
    if (!_masterController) return;

    // Check if a request is currently outstanding
    if (_currentOperation != NoOperation) return;

    // Tell user to set TakeoffType/WinchType if required and not set
    if ((_winchTypeRequired(operation) && _winchType == WinchTypeNotSet) ||
            (_takeoffTypeRequired(operation) && _takeoffType == TakeoffTypeNotSet)) {
        qgcApp()->showAppMessage(tr("Takeoff/winch type must be set in order to use this operation."), _getOperationName(operation));
        return;
    }
    
    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();
    QUrl url = _getMmsUrl(operation, aviantSettings->missionToolsUrl()->rawValue().toString());
    if (url.isEmpty()) {
        // The button is not active in UI if not the URL setting is set, so this should not happen
        // we will catch it, in case some unforeseen empty value parses to an empty URL,
        // but we will not notify the user (may change in the future, based on user testing).
        return;
    }
    _networkRequest.setUrl(url);

    QHttpMultiPart *missionPayload = nullptr;

    // Do pre-validation, and generate mission payload if applicable 
    switch (operation) {
        case MissionValidation:
        case RallyPointHeight:
            {
                QJsonDocument missionJsonDoc = _masterController->saveToJson();
                if (operation == MissionValidation) {
                    if (_validationConcluded && missionJsonDoc == _lastValidatedJson) {
                        // If we have validated the identical mission before, skip
                        return;
                    }
                    _lastValidatedJson = missionJsonDoc;
                    _validationResult = tr("Validation requested, waiting for result...");
                    _validationConcluded = false;
                }

                missionPayload = new QHttpMultiPart(QHttpMultiPart::FormDataType);
                QHttpPart planFilePart;
                planFilePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
                planFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"mission\";filename=\"noname.plan\""));
                planFilePart.setBody(missionJsonDoc.toJson());
                missionPayload->append(planFilePart);
            }
            break;
        case NoOperation:
        default:
            return;
    }

    if (missionPayload && _takeoffTypeRequired(operation)) {
        QHttpPart typePart;
        typePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"takeoff\""));
        typePart.setBody(_getTakeoffTypeName(_takeoffType).toUtf8());
        missionPayload->append(typePart);
    }

    if (missionPayload && _winchTypeRequired(operation)) {
        QHttpPart typePart;
        typePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"winch\""));
        typePart.setBody(_getWinchTypeName(_winchType).toUtf8());
        missionPayload->append(typePart);
    }

    // Set API token
    _networkRequest.setRawHeader(QByteArray("X-API-KEY"), aviantSettings->missionToolsToken()->rawValue().toString().toUtf8());
        
    // Due to problems with qt 5.15 and Ubuntu 22.04 with ssl libraries (openssl 3 not supported by qt 5.15.2)
    // we need to disable certificate checking in order to get this to work.
    // Make it optionally so it can be enabled if needed.
    // Note: This can also be used to test against an "unsafe" (e.g. local) server.
    QSslConfiguration sslConf = _networkRequest.sslConfiguration();
    sslConf.setPeerVerifyMode(aviantSettings->missionToolsInsecureHttps()->rawValue().toBool() ? QSslSocket::VerifyNone : QSslSocket::AutoVerifyPeer);
    _networkRequest.setSslConfiguration(sslConf);

    QNetworkReply *reply = nullptr;
    if (missionPayload) {
        // All currently supported operation used GET.
        // The QT framework does not support the payload we need to include by
        // using the standard ->get() call, so we need to use ->sendCustomRequest()
        reply = _networkAccessManager->sendCustomRequest(_networkRequest, "GET", missionPayload);
        missionPayload->setParent(reply);  // missionPayload is deleted when reply is deleted
    } else {
        // Currently not used, included for future operation that do not
        // include mission file as payload.
        // May need to add other types than "GET"
        reply = _networkAccessManager->get(_networkRequest);
    }

    if (reply) {
        connect(this, &AviantMissionTools::cancelPendingRequest, reply, &QNetworkReply::abort);
        _currentOperation = operation;
        emit stateChanged();
    } else {
        // Unsure if this ever happen, most errors should be handled by _requestComplete
        // with error set in the QNetworkReply object.
        // Give a error message, otherwise ignore this.
        if (operation == MissionValidation) {
            // Special case for validation
            // Set the validation result text instead of signaling error in a pop-up
            _validationResult = tr("Unknown error validating mission");
        } else {
            qgcApp()->showAppMessage(tr("Unknown error with mission tool request"), tr("Mission Tools"));
        }
    }
}

void AviantMissionTools::cancelOperation(Operation operation)
{
    if (_currentOperation == NoOperation) return;  // Nothing to cancel
    if (operation == _currentOperation) {
        // Cancel if operation match
        emit cancelPendingRequest();
    }
}

void AviantMissionTools::_requestComplete(QNetworkReply *reply)
{
    reply->deleteLater();

    if (_currentOperation == NoOperation) {
        // Simple sanity check
        // We will only have one outstanding request at a time,
        // and no response should be received when we do not expect it.
        // Print an error message and drop the rest
        qgcApp()->showAppMessage(tr("Consistency error with mission tool request"), tr("Mission Tools"));
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        if (_currentOperation == MissionValidation) {
            // Special case for validation
            // Set the validation result text instead of signaling error in a pop-up
            _validationResult = tr("Error validating mission:") + "\n" + reply->errorString();
        } else {
            qgcApp()->showAppMessage(tr("Request failed:\n") + reply->errorString(), tr("Mission Tools"));
        }
        _currentOperation = NoOperation;
        emit stateChanged();
        return;
    }

    QByteArray bytes = reply->readAll();
    switch (_currentOperation) {
        case MissionValidation:
            _parseValidationResponse(bytes);
            break;
        case RallyPointHeight:
            _parseAndLoadMissionResponse(bytes);
            break;
        case FetchKyteOrders:
            _parseKyteOrdersResponse(bytes);
            break;
        case FetchKyteOrderMissionFile:
            _expectedHash = reply->rawHeader("X-File-Hash");
            if (_expectedHash.isEmpty()) {
                qgcApp()->showAppMessage(tr("No hash received with mission file"), tr("Mission Tools"));
            }
            else if (_validateFileHash(bytes, _expectedHash)) {
                _parseAndLoadMissionResponse(bytes);
            } else {
                qgcApp()->showAppMessage(tr("Mission file integrity check failed"), tr("Mission Tools"));
            }
            break;
        case NoOperation:
        default:
            // Ignore any error that leads to this (should not happen)
        break;
    }
    _currentOperation = NoOperation;
    emit stateChanged();
}

void AviantMissionTools::_parseValidationResponse(const QByteArray &bytes)
{
    QJsonDocument jsonDoc;
    QString errorString;
    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorString)) {
        // Unable to convert to JSON, present the response as an error,
        // but do not cache it, as we do not know what kind of error it is
        // (it could be OK to just try again)
        _validationResult = tr("Error validating mission:") + "\n" + errorString + "\n" + bytes;
        return;
    }
    
    QJsonObject json = jsonDoc.object();

    QList<JsonHelper::KeyValidateInfo> rgKeyInfo = {
        { "failed", QJsonValue::Object, true },
        { "passed", QJsonValue::Object, true },
    };
    if (!JsonHelper::validateKeys(json, rgKeyInfo, errorString)) {
        // If we do not get failed/passed objects, consider the entire response as the faults to show
        _validationResult = tr("Error validating mission:") + "\n" + jsonDoc.toJson(QJsonDocument::Indented);
        _validationConcluded = true;  // Cache the result
        return;
    }

    QJsonObject failed = json["failed"].toObject();

    // Remove empty arrays from failed
    auto it = failed.begin();
    while (it != failed.end()) {
        if (it.value().toArray().count() == 0) {
            it = failed.erase(it);
        } else {
            ++it;
        }
    }

    if (!failed.empty()) {
        QJsonDocument failedDoc(failed);
        _validationResult = tr("Error validating mission:") + "\n" + failedDoc.toJson(QJsonDocument::Indented);
        _validationConcluded = true;  // Cache the result
        return;
    }
        
    _validationResult = tr("Plan validated without errors");
    _validationConcluded = true;  // Cache the result
}

void AviantMissionTools::_parseAndLoadMissionResponse(const QByteArray &bytes)
{
    if (!_masterController) return;

    QJsonDocument jsonDoc;
    QString errorString;
    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorString)) {
        qgcApp()->showAppMessage(tr("Received response is not JSON\n") + errorString + "\n" + QString(bytes),
                tr("Mission Tools - ") + _getOperationName(_currentOperation));
        return;
    }

    if (!_masterController->loadFromJson(jsonDoc, errorString)) {
        qgcApp()->showAppMessage(tr("Could not load received response as mission\n") + errorString + "\n" + QString(bytes),
                tr("Mission Tools - ") + _getOperationName(_currentOperation));
        return;
    }

    if (_currentOperation == FetchKyteOrderMissionFile) {
        _masterController->clearCurrentPlanFile();
    }

    qgcApp()->showAppMessage(tr("Operation successful"), tr("Mission Tools - ") + _getOperationName(_currentOperation));
}

void AviantMissionTools::_initiateNetworkRequest(Operation operationType, const QUrl& url)
{
    if (_currentOperation != NoOperation) {
        qgcApp()->showAppMessage(tr("Another operation is in progress"), tr("Error"));
        return;
    }

    if (url.isEmpty()) {
        qgcApp()->showAppMessage(tr("Invalid URL for operation."), tr("Error"));
        return;
    }

    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();

    _networkRequest.setUrl(url);
    _networkRequest.setRawHeader(QByteArray("Authorization"), QByteArray("Token ") + aviantSettings->kyteBackendToken()->rawValue().toString().toUtf8());

    QSslConfiguration sslConf = _networkRequest.sslConfiguration();
    sslConf.setPeerVerifyMode(aviantSettings->missionToolsInsecureHttps()->rawValue().toBool() ? QSslSocket::VerifyNone : QSslSocket::AutoVerifyPeer);
    _networkRequest.setSslConfiguration(sslConf);

    QNetworkReply *reply = _networkAccessManager->get(_networkRequest);

    if (reply) {
        connect(this, &AviantMissionTools::cancelPendingRequest, reply, &QNetworkReply::abort);
        _currentOperation = operationType;
        emit stateChanged();
    } else {
        qgcApp()->showAppMessage(tr("Failed to create network request."), tr("Error"));
    }
}

void AviantMissionTools::fetchKyteOrderMissions()
{
    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();
    QUrl url = _getKyteBackendUrl(FetchKyteOrders, aviantSettings->kyteBackendUrl()->rawValue().toString());
    _initiateNetworkRequest(FetchKyteOrders, url);
}

void AviantMissionTools::downloadMissionFileFromOrder(int orderId)
{
    AviantSettings* aviantSettings = qgcApp()->toolbox()->settingsManager()->aviantSettings();
    QUrl url = _getKyteBackendUrl(FetchKyteOrderMissionFile, aviantSettings->kyteBackendUrl()->rawValue().toString(), orderId);
    _initiateNetworkRequest(FetchKyteOrderMissionFile, url);
}

void AviantMissionTools::_parseKyteOrdersResponse(const QByteArray &bytes)
{
    QJsonDocument jsonDoc;
    QString errorString;
    if (!JsonHelper::isJsonFile(bytes, jsonDoc, errorString)) {
        qgcApp()->showAppMessage(tr("Error parsing Kyte orders response: ") + errorString, tr("Error"));
        return;
    }

    QJsonArray ordersArray = jsonDoc.array();
    _kyteOrders.clear();
    for (const QJsonValue &value : ordersArray) {
        if (value.isObject()) {
            _kyteOrders.append(value.toObject());
        }
    }
    emit kyteOrdersChanged(_kyteOrders);
}

bool AviantMissionTools::_validateFileHash(const QByteArray &fileData, const QByteArray &expectedHash)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(fileData);
    QByteArray calculatedHash = hash.result().toHex();
    
    return (calculatedHash == expectedHash);
}

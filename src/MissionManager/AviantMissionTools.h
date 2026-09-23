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
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>

class PlanMasterController;

class AviantMissionTools : public QObject
{
    Q_OBJECT
    
public:
    AviantMissionTools(QObject* parent = nullptr);
    AviantMissionTools(PlanMasterController* masterController, QObject* parent = nullptr);
    ~AviantMissionTools();

    enum Operation {
        NoOperation,
        MissionValidation,
        RallyPointHeight,
        FetchScheduledFlights,
        FetchLandingPointAdjustedMission
    };

    enum TakeoffType {
        TakeoffTypeNotSet,
        VTOL,
        Headless
    };
    
    enum WinchType {
        WinchTypeNotSet,
        NoWinch,
        V1,
        V2,
        V3,
        Custom
    };

    Q_ENUM(Operation)
    Q_ENUM(TakeoffType)
    Q_ENUM(WinchType)
    
    Q_PROPERTY(PlanMasterController* masterController  READ masterController  CONSTANT)
    Q_PROPERTY(bool                  requestInProgress READ requestInProgress                      NOTIFY   stateChanged)
    Q_PROPERTY(Operation             currentOperation  READ currentOperation                       NOTIFY   stateChanged)
    Q_PROPERTY(TakeoffType           takeoffType       READ takeoffType       WRITE setTakeoffType NOTIFY   stateChanged)
    Q_PROPERTY(WinchType             winchType         READ winchType         WRITE setWinchType   NOTIFY   stateChanged)
    Q_PROPERTY(QStringList           takeoffTypeList   READ takeoffTypeList   CONSTANT)
    Q_PROPERTY(QStringList           winchTypeList     READ winchTypeList     CONSTANT)
    Q_PROPERTY(QString               validationResult  READ validationResult                       NOTIFY   stateChanged)
    
    Q_INVOKABLE void requestOperation(Operation operation);
    Q_INVOKABLE void cancelOperation(Operation operation);
    Q_INVOKABLE void fetchScheduledFlights();
    Q_INVOKABLE void downloadMissionFileFromScheduledFlight(int missionPlanId, const QString& aircraftName);

    PlanMasterController* masterController       (void) const { return _masterController; }
    bool                  requestInProgress      (void) const { return _currentOperation != NoOperation; }
    Operation             currentOperation       (void) const { return _currentOperation; }
    TakeoffType           takeoffType            (void) const { return _takeoffType; }
    WinchType             winchType              (void) const { return _winchType; }
    QStringList           takeoffTypeList        (void) const;
    QStringList           winchTypeList          (void) const;
    void                  setTakeoffType         (TakeoffType takeoffType);
    void                  setWinchType           (WinchType winchType);
    QString               validationResult       (void) const { return _validationResult; }

signals:
    void stateChanged          (void);
    void cancelPendingRequest  (void);
    void scheduledFlightsChanged (QList<QJsonObject> scheduledFlights);

private slots:
    void _requestComplete (QNetworkReply *reply);

private:
    QUrl           _getMmsUrl                   (Operation operation, QString base);
    QUrl           _getMmsUrl                   (Operation operation, QString base, int missionPlanId, QString aircraftName);
    void           _parseValidationResponse     (const QByteArray &bytes);
    void           _parseAndLoadMissionResponse (const QByteArray &bytes);
    static QString _getOperationName            (Operation operation);
    static QString _getTakeoffTypeName          (TakeoffType takeoffType);
    static QString _getWinchTypeName            (WinchType winchType);
    static bool    _takeoffTypeRequired(Operation operation);
    static bool    _winchTypeRequired(Operation operation);
    void           _parseScheduledFlightsResponse(const QByteArray &bytes);
    bool           _validateFileHash(const QByteArray &fileData, const QByteArray &expectedHash);
    void           _initiateNetworkRequest(Operation operationType, const QUrl& url);

    PlanMasterController*   _masterController;
    Operation               _currentOperation =     NoOperation;
    TakeoffType             _takeoffType =          TakeoffTypeNotSet;
    WinchType               _winchType =            WinchTypeNotSet;
    QNetworkAccessManager*  _networkAccessManager = nullptr;
    QString                 _validationResult =     "Not validated";
    bool                    _validationConcluded =  false;
    QJsonDocument           _lastValidatedJson;
    QList<QJsonObject>      _scheduledFlights;
    QByteArray              _expectedHash;
    static qint64           _requestIdCounter; // Static counter for Request-Id
};

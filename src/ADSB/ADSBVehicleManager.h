/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCToolbox.h"
#include "QmlObjectListModel.h"
#include "ADSBVehicle.h"

#include <QThread>
#include <QTcpSocket>
#include <QTimer>
#include <QGeoCoordinate>

class ADSBVehicleManagerSettings;

class ADSBTCPLink : public QThread
{
    Q_OBJECT

public:
    ADSBTCPLink(const QString& hostAddress, int port, QObject* parent);
    ~ADSBTCPLink();

signals:
    void adsbVehicleUpdate(const ADSBVehicle::VehicleInfo_t vehicleInfo);
    void error(const QString errorMsg);

protected:
    void run(void) final;

private slots:
    void _readBytes(void);

private:
    void _hardwareConnect(void);
    void _parseLine(const QString& line);

    QString         _hostAddress;
    int             _port;
    QTcpSocket*     _socket =   nullptr;
};

class ADSBVehicleManager : public QGCTool {
    Q_OBJECT
    
public:
    ADSBVehicleManager(QGCApplication* app, QGCToolbox* toolbox);

    Q_PROPERTY(QmlObjectListModel* adsbVehicles READ adsbVehicles CONSTANT)
    Q_PROPERTY(QmlObjectListModel* hiddenADSBVehicles  READ hiddenADSBVehicles NOTIFY hasHiddenADSBVehicleChanged)
    Q_PROPERTY(QmlObjectListModel* visibleADSBVehicles READ visibleADSBVehicles NOTIFY hasVisibleADSBVehicleChanged)
    Q_PROPERTY(bool hasHiddenADSBVehicle               READ hasHiddenADSBVehicle NOTIFY hasHiddenADSBVehicleChanged)

    QmlObjectListModel* adsbVehicles(void) { return &_adsbVehicles; }
    QmlObjectListModel* hiddenADSBVehicles(void);
    QmlObjectListModel* visibleADSBVehicles(void);

    bool hasHiddenADSBVehicle(void);

    // QGCTool overrides
    void setToolbox(QGCToolbox* toolbox) final;

    Q_INVOKABLE void unhideAllVehicles(void);
    Q_INVOKABLE void setHiddenForADSBVehicle(quint32 icaoAddress, bool hidden);

public slots:
    void adsbVehicleUpdate  (const ADSBVehicle::VehicleInfo_t vehicleInfo);
    void _tcpError          (const QString errorMsg);

signals:
    void hasHiddenADSBVehicleChanged();
    void hasVisibleADSBVehicleChanged();

private slots:
    void _cleanupStaleVehicles(void);
    void _adsbVehicleHiddenChanged(void);

private:
    QmlObjectListModel              _adsbVehicles;
    QMap<uint32_t, ADSBVehicle*>    _adsbICAOMap;
    QTimer                          _adsbVehicleCleanupTimer;
    ADSBTCPLink*                    _tcpLink = nullptr;
};

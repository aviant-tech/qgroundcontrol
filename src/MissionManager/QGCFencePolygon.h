/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "QGCMapPolygon.h"

/// The QGCFencePolygon class provides a polygon used by GeoFence support.
class QGCFencePolygon : public QGCMapPolygon
{
    Q_OBJECT

public:
    QGCFencePolygon(bool inclusion, QObject* parent = nullptr);
    QGCFencePolygon(const QGCFencePolygon& other, QObject* parent = nullptr);

    const QGCFencePolygon& operator=(const QGCFencePolygon& other);

    Q_PROPERTY(bool inclusion READ inclusion WRITE setInclusion NOTIFY inclusionChanged)
    Q_PROPERTY(int fenceAction READ fenceAction WRITE setFenceAction NOTIFY fenceActionChanged)
    Q_PROPERTY(int maxAltitude READ maxAltitude WRITE setMaxAltitude NOTIFY maxAltitudeChanged)

    /// Saves the QGCFencePolygon to the json object.
    ///     @param json Json object to save to
    void saveToJson(QJsonObject& json);

    /// Load a QGCFencePolygon from json
    ///     @param json Json object to load from
    ///     @param required true: no polygon in object will generate error
    ///     @param errorString Error string if return is false
    /// @return true: success, false: failure (errorString set)
    bool loadFromJson(const QJsonObject& json, bool required, QString& errorString);

    // Property methods

    bool inclusion      (void) const { return _inclusion; }
    int  fenceAction    (void) const { return _fenceAction; }
    int  maxAltitude    (void) const { return _maxAltitude; }
    void setInclusion   (bool inclusion);
    void setFenceAction (int fenceAction);
    void setMaxAltitude(int maxAltitude);

signals:
    void inclusionChanged   (bool inclusion);
    void fenceActionChanged(int fenceAction);
    void maxAltitudeChanged(int maxAltitude);

private slots:
    void _setDirty(void);

private:

    constexpr static int DEFAULT_FENCE_ACTION = 0; // 0 - Use the value from the parameter
    constexpr static int DEFAULT_MAX_ALTITUDE = 0; // 0 - Disabled

    void _init(void);

    bool _inclusion;
    int _fenceAction{DEFAULT_FENCE_ACTION};
    int _maxAltitude{DEFAULT_MAX_ALTITUDE};

    static const int _jsonCurrentVersion = 1;

    static const char* _jsonInclusionKey;
    static const char* _jsonFenceActionKey;
    static const char* _jsonMaxAltitudeKey;
};

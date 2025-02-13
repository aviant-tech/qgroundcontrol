/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "QGCFenceCircle.h"
#include "JsonHelper.h"

const char* QGCFenceCircle::_jsonInclusionKey = "inclusion";
const char* QGCFenceCircle::_jsonFenceActionKey = "fenceAction";
const char* QGCFenceCircle::_jsonMaxAltitudeKey = "maxAltitude";

QGCFenceCircle::QGCFenceCircle(QObject* parent)
    : QGCMapCircle  (parent)
    , _inclusion    (true)
{
    _init();
}

QGCFenceCircle::QGCFenceCircle(const QGeoCoordinate& center, double radius, bool inclusion, QObject* parent)
    : QGCMapCircle  (center, radius, false /* showRotation */, true /* clockwiseRotation */, parent)
    , _inclusion    (inclusion)
{
    _init();
}

QGCFenceCircle::QGCFenceCircle(const QGCFenceCircle& other, QObject* parent)
    : QGCMapCircle  (other, parent)
    , _inclusion    (other._inclusion)
    , _fenceAction  (other._fenceAction)
    , _maxAltitude  (other._maxAltitude)
{
    _init();
}

void QGCFenceCircle::_init(void)
{
    connect(this, &QGCFenceCircle::inclusionChanged, this, &QGCFenceCircle::_setDirty);
    connect(this, &QGCFenceCircle::fenceActionChanged, this, &QGCFenceCircle::_setDirty);
    connect(this, &QGCFenceCircle::maxAltitudeChanged, this, &QGCFenceCircle::_setDirty);
}

const QGCFenceCircle& QGCFenceCircle::operator=(const QGCFenceCircle& other)
{
    QGCMapCircle::operator=(other);

    setInclusion(other._inclusion);
    setFenceAction(other._fenceAction);
    setMaxAltitude(other._maxAltitude);

    return *this;
}

void QGCFenceCircle::_setDirty(void)
{
    setDirty(true);
}

void QGCFenceCircle::saveToJson(QJsonObject& json)
{
    json[JsonHelper::jsonVersionKey] = _jsonCurrentVersion;
    json[_jsonInclusionKey] = _inclusion;
    json[_jsonFenceActionKey] = _fenceAction;
    json[_jsonMaxAltitudeKey] = _inclusion ? _maxAltitude : 0;
    QGCMapCircle::saveToJson(json);
}

bool QGCFenceCircle::loadFromJson(const QJsonObject& json, QString& errorString)
{
    errorString.clear();

    QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { JsonHelper::jsonVersionKey,   QJsonValue::Double, true },
        { _jsonInclusionKey,            QJsonValue::Bool,   true },
        { _jsonFenceActionKey,          QJsonValue::Double, false },
        { _jsonMaxAltitudeKey,          QJsonValue::Double, false },
    };
    if (!JsonHelper::validateKeys(json, keyInfoList, errorString)) {
        return false;
    }

    if (json[JsonHelper::jsonVersionKey].toInt() != _jsonCurrentVersion) {
        errorString = tr("GeoFence Circle only supports version %1").arg(_jsonCurrentVersion);
        return false;
    }

    if (!QGCMapCircle::loadFromJson(json, errorString)) {
        return false;
    }

    setMaxAltitude(json[_jsonMaxAltitudeKey].toInt());
    setFenceAction(json[_jsonFenceActionKey].toInt());
    setInclusion(json[_jsonInclusionKey].toBool());

    return true;
}

void QGCFenceCircle::setInclusion(bool inclusion)
{
    if (inclusion == _inclusion) return;
    _inclusion = inclusion;
    emit inclusionChanged(inclusion);
}

void QGCFenceCircle::setFenceAction (int fenceAction)
{
    if (fenceAction == _fenceAction) return;
    _fenceAction = fenceAction;
    emit fenceActionChanged(fenceAction);
}

void QGCFenceCircle::setMaxAltitude (int maxAltitude)
{
    if (maxAltitude == _maxAltitude) return;
    _maxAltitude = maxAltitude;
    emit maxAltitudeChanged(maxAltitude);
}

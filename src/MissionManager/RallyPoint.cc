/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include <QStringList>
#include <QDebug>

#include "RallyPoint.h"

const char* RallyPoint::_longitudeFactName =    "Longitude";
const char* RallyPoint::_latitudeFactName =     "Latitude";
const char* RallyPoint::_altitudeFactName =     "ABSOLUTE ALTITUDE";
const char* RallyPoint::_typeFactName =         "Active";

QMap<QString, FactMetaData*> RallyPoint::_metaDataMap;

RallyPoint::RallyPoint(const QGeoCoordinate& coordinate, int type, QObject* parent)
    : QObject(parent)
    , _dirty(false)
    , _longitudeFact(0, _longitudeFactName, FactMetaData::valueTypeDouble)
    , _latitudeFact(0, _latitudeFactName, FactMetaData::valueTypeDouble)
    , _altitudeFact(0, _altitudeFactName, FactMetaData::valueTypeDouble)
    , _typeFact(0, _typeFactName, FactMetaData::valueTypeUint32)
{
    setCoordinate(coordinate);
    setType(type);

    _factSetup();
}

RallyPoint::RallyPoint(const RallyPoint& other, QObject* parent)
    : QObject(parent)
    , _dirty(false)
    , _longitudeFact(0, _longitudeFactName, FactMetaData::valueTypeDouble)
    , _latitudeFact(0, _latitudeFactName, FactMetaData::valueTypeDouble)
    , _altitudeFact(0, _altitudeFactName, FactMetaData::valueTypeDouble)
    , _typeFact(0, _typeFactName, FactMetaData::valueTypeUint32)
{
    _longitudeFact.setRawValue(other._longitudeFact.rawValue());
    _latitudeFact.setRawValue(other._latitudeFact.rawValue());
    _altitudeFact.setRawValue(other._altitudeFact.rawValue());
    _typeFact.setRawValue(other._typeFact.rawValue());

    _factSetup();
}

const RallyPoint& RallyPoint::operator=(const RallyPoint& other)
{
    _longitudeFact.setRawValue(other._longitudeFact.rawValue());
    _latitudeFact.setRawValue(other._latitudeFact.rawValue());
    _altitudeFact.setRawValue(other._altitudeFact.rawValue());
    _typeFact.setRawValue(other._typeFact.rawValue());

    emit coordinateChanged(coordinate());

    return *this;
}

RallyPoint::~RallyPoint()
{    

}

void RallyPoint::_factSetup(void)
{
    _cacheFactMetadata();

    _longitudeFact.setMetaData(_metaDataMap[_longitudeFactName]);
    _latitudeFact.setMetaData(_metaDataMap[_latitudeFactName]);
    _altitudeFact.setMetaData(_metaDataMap[_altitudeFactName]);
    _typeFact.setMetaData(_metaDataMap[_typeFactName]);

    _textFieldFacts.append(QVariant::fromValue(&_longitudeFact));
    _textFieldFacts.append(QVariant::fromValue(&_latitudeFact));
    _textFieldFacts.append(QVariant::fromValue(&_altitudeFact));
    _textFieldFacts.append(QVariant::fromValue(&_typeFact));

    connect(&_longitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
    connect(&_latitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
    connect(&_altitudeFact, &Fact::valueChanged, this, &RallyPoint::_sendCoordinateChanged);
    connect(&_typeFact, &Fact::valueChanged, this, &RallyPoint::_sendTypeChanged);
}

void RallyPoint::_cacheFactMetadata() {
    if (_metaDataMap.isEmpty()) {
        _metaDataMap = FactMetaData::createMapFromJsonFile(QStringLiteral(":/json/RallyPoint.FactMetaData.json"), nullptr /* metaDataParent */);
    }
}

void RallyPoint::setCoordinate(const QGeoCoordinate& coordinate)
{
    if (coordinate != this->coordinate()) {
        _longitudeFact.setRawValue(coordinate.longitude());
        _latitudeFact.setRawValue(coordinate.latitude());
        _altitudeFact.setRawValue(coordinate.altitude());
        emit coordinateChanged(coordinate);
        setDirty(true);
    }
}

void RallyPoint::setDirty(bool dirty)
{
    if (dirty != _dirty) {
        _dirty = dirty;
        emit dirtyChanged(dirty);
    }
}

double RallyPoint::getDefaultFactAltitude() {
    _cacheFactMetadata();
    auto it = _metaDataMap.find(QString(_altitudeFactName));
    if(it != _metaDataMap.end() && (*it)->defaultValueAvailable()) {
        return (*it)->rawDefaultValue().toDouble();
    }
    return 0.0;
}

QGeoCoordinate RallyPoint::coordinate(void) const
{
    return QGeoCoordinate(_latitudeFact.rawValue().toDouble(), _longitudeFact.rawValue().toDouble(), _altitudeFact.rawValue().toDouble());
}

void RallyPoint::_sendCoordinateChanged(void)
{
    emit coordinateChanged(coordinate());
}

int RallyPoint::type(void) const
{
    return _typeFact.rawValue().toInt();
}

void RallyPoint::setType(int type)
{
    if (type != this->type()) {
        _typeFact.setRawValue(type);
        emit typeChanged(type);
    }
}

void RallyPoint::_sendTypeChanged(void)
{
    emit typeChanged(type());
}

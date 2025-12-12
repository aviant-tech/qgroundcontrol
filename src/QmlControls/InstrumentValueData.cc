/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "InstrumentValueData.h"
#include "FactValueGrid.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"

#include <QSettings>

const char*  InstrumentValueData::vehicleFactGroupName =   "Vehicle";

// Important: The indices of these strings must match the InstrumentValueData::RangeType enum
const QStringList InstrumentValueData::_rangeTypeNames = {
    QT_TRANSLATE_NOOP("InstrumentValue", "None"),
    QT_TRANSLATE_NOOP("InstrumentValue", "Color"),
    QT_TRANSLATE_NOOP("InstrumentValue", "Opacity"),
    QT_TRANSLATE_NOOP("InstrumentValue", "Icon"),
};

InstrumentValueData::InstrumentValueData(FactValueGrid* factValueGrid, QObject* parent)
    : QObject       (parent)
    , _factValueGrid(factValueGrid)
{
    MultiVehicleManager* multiVehicleManager = qgcApp()->toolbox()->multiVehicleManager();
    connect(multiVehicleManager, &MultiVehicleManager::activeVehicleChanged, this, &InstrumentValueData::_activeVehicleChanged);
    _activeVehicleChanged(multiVehicleManager->activeVehicle());

    connect(this, &InstrumentValueData::rangeTypeChanged,       this, &InstrumentValueData::_resetRangeInfo);
    connect(this, &InstrumentValueData::rangeTypeChanged,       this, &InstrumentValueData::_updateRanges);
    connect(this, &InstrumentValueData::rangeValuesChanged,     this, &InstrumentValueData::_updateRanges);
    connect(this, &InstrumentValueData::rangeColorsChanged,     this, &InstrumentValueData::_updateRanges);
    connect(this, &InstrumentValueData::rangeOpacitiesChanged,  this, &InstrumentValueData::_updateRanges);
    connect(this, &InstrumentValueData::rangeIconsChanged,      this, &InstrumentValueData::_updateRanges);
}

void InstrumentValueData::_activeVehicleChanged(Vehicle* activeVehicle)
{
    if (_activeVehicle) {
        disconnect(_activeVehicle, &Vehicle::factGroupNamesChanged, this, &InstrumentValueData::_lookForMissingFact);
    }

    if (!activeVehicle) {
        activeVehicle = qgcApp()->toolbox()->multiVehicleManager()->offlineEditingVehicle();
    }

    _activeVehicle = activeVehicle;
    connect(_activeVehicle, &Vehicle::factGroupNamesChanged, this, &InstrumentValueData::_lookForMissingFact);

    emit factGroupNamesChanged();

    if (!_factGroupName.isEmpty() && !_factName.isEmpty()) {
        _setFactWorker();
    }
}

void InstrumentValueData::_lookForMissingFact(void)
{
    // This is called when new fact groups show up on the vehicle. We need to see if we can fill in any
    // facts which may have been missing up to now.
    if (!_fact) {
        _setFactWorker();
    }
}

void InstrumentValueData::clearFact(void)
{
    _fact = nullptr;
    _factName.clear();
    _text.clear();
    _icon.clear();
    _showUnits = true;

    emit factValueNamesChanged  ();
    emit factChanged            (_fact);
    emit factNameChanged        (_factName);
    emit factGroupNameChanged   (_factGroupName);
    emit textChanged            (_text);
    emit iconChanged            (_icon);
    emit showUnitsChanged       (_showUnits);
}

void InstrumentValueData::_setFactWorker(void)
{
    if (_fact) {
        disconnect(_fact, &Fact::rawValueChanged, this, &InstrumentValueData::_updateRanges);
        _fact = nullptr;
    }

    FactGroup* factGroup = nullptr;
    if (_factGroupName == vehicleFactGroupName) {
        factGroup = _activeVehicle;
    } else {
        factGroup = _activeVehicle->getFactGroup(_factGroupName);
    }

    QString nonEmptyFactName;
    if (factGroup) {
        if (_factName.isEmpty()) {
            nonEmptyFactName = factValueNames()[0];
        } else {
            nonEmptyFactName = _factName;
        }
        _fact = factGroup->getFact(nonEmptyFactName);
    }

    if (_fact) {
        _factName = nonEmptyFactName;
        connect(_fact, &Fact::rawValueChanged, this, &InstrumentValueData::_updateRanges);
    }

    emit factValueNamesChanged  ();
    emit factChanged            (_fact);
    emit factNameChanged        (_factName);
    emit factGroupNameChanged   (_factGroupName);

    _updateRanges();
}
void InstrumentValueData::setFact(const QString& factGroupName, const QString& factName)
{
    _factGroupName  = factGroupName;
    _factName       = factName;

    _setFactWorker();
}

void InstrumentValueData::setText(const QString& text)
{
    if (text != _text) {
        _text = text;
        emit textChanged(text);
    }
}

void InstrumentValueData::setShowUnits(bool showUnits)
{
    if (showUnits != _showUnits) {
        _showUnits = showUnits;
        emit showUnitsChanged(showUnits);
    }
}

void InstrumentValueData::setIcon(const QString& icon)
{
    if (icon != _icon) {
        _icon = icon;
        emit iconChanged(_icon);
    }
}

void InstrumentValueData::setRangeType(RangeType rangeType)
{
    if (rangeType != _rangeType) {
        _rangeType = rangeType;
        emit rangeTypeChanged(rangeType);
    }
}

void InstrumentValueData::setRangeValues(const QVariantList& rangeValues)
{
    _range.values = rangeValues;
    emit rangeValuesChanged(rangeValues);
}

void InstrumentValueData::setRangeColors (const QVariantList& rangeColors)
{
    _range.colors = rangeColors;
    emit rangeColorsChanged(rangeColors);
}

void InstrumentValueData::setRangeIcons(const QVariantList& rangeIcons)
{
    _range.icons = rangeIcons;
    emit rangeIconsChanged(rangeIcons);
}

void InstrumentValueData::setRangeOpacities(const QVariantList& rangeOpacities)
{
    _range.opacities = rangeOpacities;
    emit rangeOpacitiesChanged(rangeOpacities);
}

void InstrumentValueData::setIndividualFwMrRanges(bool individualFwMrRanges) {
    if (_individualFwMrRanges != individualFwMrRanges) {
        _individualFwMrRanges = individualFwMrRanges;
        emit individualFwMrRangesChanged(individualFwMrRanges);
    }
}

void InstrumentValueData::_resetRangeInfoCommon(RangeSet& rangeSet) {
    rangeSet.values.clear();
    rangeSet.colors.clear();
    rangeSet.opacities.clear();
    rangeSet.icons.clear();

    if (_rangeType != NoRangeInfo) {
        rangeSet.values = { 0.0, 100.0 };
    }

    for (int i = 0; i < rangeSet.values.count() + 1; i++) {
        switch (_rangeType) {
        case NoRangeInfo:
            break;
        case ColorRange:
            rangeSet.colors.append(QColor("green"));
            break;
        case OpacityRange:
            rangeSet.opacities.append(1.0);
            break;
        case IconSelectRange:
            rangeSet.icons.append(_factValueGrid->iconNames()[0]);
            break;
        }
    }
}

void InstrumentValueData::_resetRangeInfo(void)
{
    _resetRangeInfoCommon(_range);
    _emitRangeSignals(_range, NormalRange);

    if (_individualFwMrRanges) {
        _resetRangeInfoCommon(_fwRange);
        _emitRangeSignals(_fwRange, FwRange);

        _resetRangeInfoCommon(_mrRange);
        _emitRangeSignals(_mrRange, MrRange);
    }
}

void InstrumentValueData::_addRangeValueCommon(RangeSet& rangeSet) {
    if (rangeSet.values.isEmpty()) { 
        rangeSet.values.append(0.0); 
    } else {
        rangeSet.values.append(rangeSet.values.last().toDouble() + 1); 
    }

    switch (_rangeType) {
    case NoRangeInfo:
        break;
    case ColorRange:
        rangeSet.colors.append(QColor("green"));
        break;
    case OpacityRange:
        rangeSet.opacities.append(1.0);
        break;
    case IconSelectRange:
        if (_factValueGrid && !_factValueGrid->iconNames().isEmpty()) {
            rangeSet.icons.append(_factValueGrid->iconNames()[0]);
        } else {
            rangeSet.icons.append(QString());
        }
        break;
    }
}

void InstrumentValueData::addRangeValue(void)
{
    _addRangeValueCommon(_range);
    _emitRangeSignals(_range, NormalRange);
}

void InstrumentValueData::_removeRangeValueCommon(int index, RangeSet& rangeSet) {
    if (rangeSet.values.count() < 2 || index < 0 || index >= rangeSet.values.count()) {
        qDebug() << "_removeRangeValueCommon: Invalid index" << index << "for values count" << rangeSet.values.count();
        return;
    }

    rangeSet.values.removeAt(index);

    switch (_rangeType) {
    case NoRangeInfo:
        break;
    case ColorRange:
        rangeSet.colors.removeAt(index + 1);
        break;
    case OpacityRange:
        rangeSet.opacities.removeAt(index + 1);
        break;
    case IconSelectRange:
        rangeSet.icons.removeAt(index + 1);
        break;
    }
}

void InstrumentValueData::removeRangeValue(int index)
{
    if (_range.values.count() < 2 || index < 0 || index >= _range.values.count()) { 
        qDebug() << "removeRangeValue: Invalid index or not enough values.";
        return;
    }
    _removeRangeValueCommon(index, _range);
    _emitRangeSignals(_range, NormalRange);
}

void InstrumentValueData::_updateRanges(void)
{
    _updateColor();
    _updateIcon();
    _updateOpacity();
}

void InstrumentValueData::_updateColor(void)
{
    QColor newColor;
    
    if (_fact->overrideColorEnabled()) {
        newColor = _fact->overrideColor();

    } else{
        const double factValue = _fact ? _fact->rawValue().toDouble() : qQNaN();
        const RangeSet* activeRangeSetToUse = &_range;

        if (_rangeType == ColorRange) {
            if (_individualFwMrRanges && _activeVehicle) {
                const bool isVtol = _activeVehicle->vtol();
                const bool isVtolInFwdFlight = isVtol && _activeVehicle->vtolInFwdFlight();
                const bool isConsideredFixedWing = _activeVehicle->fixedWing() || (isVtol && isVtolInFwdFlight);
                const bool isConsideredMultiRotor = _activeVehicle->multiRotor() || (isVtol && !isVtolInFwdFlight);

                if (isConsideredFixedWing) {
                    activeRangeSetToUse = &_fwRange;
                } else if (isConsideredMultiRotor) {
                    activeRangeSetToUse = &_mrRange;
                }
            }

            if (_fact && !qIsNaN(factValue)) {
                const int rangeIndex = _currentRangeIndex(factValue, activeRangeSetToUse->values);
                if (rangeIndex != -1 && rangeIndex < activeRangeSetToUse->colors.count()) {
                    newColor = activeRangeSetToUse->colors[rangeIndex].value<QColor>();
                }
            }
        }
    }

    if (newColor != _currentColor) {
        _currentColor = newColor;
        emit currentColorChanged(_currentColor);
    }
}

void InstrumentValueData::_updateOpacity(void)
{
    double newOpacity = 1.0;
    double factValue = _fact ? _fact->rawValue().toDouble() : qQNaN();
    const RangeSet* activeRangeSetToUse = &_range;

    if (_rangeType == OpacityRange) {
        if (_individualFwMrRanges && _activeVehicle) {
            const bool isVtol = _activeVehicle->vtol();
            const bool isVtolInFwdFlight = isVtol && _activeVehicle->vtolInFwdFlight();
            const bool isConsideredFixedWing = _activeVehicle->fixedWing() || (isVtol && isVtolInFwdFlight);
            const bool isConsideredMultiRotor = _activeVehicle->multiRotor() || (isVtol && !isVtolInFwdFlight);

            if (isConsideredFixedWing) {
                activeRangeSetToUse = &_fwRange;
            } else if (isConsideredMultiRotor) {
                activeRangeSetToUse = &_mrRange;
            }
        }

        if (_fact && !qIsNaN(factValue)) {
            const int rangeIndex = _currentRangeIndex(factValue, activeRangeSetToUse->values);
            if (rangeIndex != -1 && rangeIndex < activeRangeSetToUse->opacities.count()) {
                newOpacity = activeRangeSetToUse->opacities[rangeIndex].toDouble();
            }
        }
    }

    if (!QGC::fuzzyCompare(newOpacity, _currentOpacity)) {
        _currentOpacity = newOpacity;
        emit currentOpacityChanged(newOpacity);
    }
}

void InstrumentValueData::_updateIcon(void)
{
    QString newIcon;
    double factValue = _fact ? _fact->rawValue().toDouble() : qQNaN();
    const RangeSet* activeRangeSetToUse = &_range;

    if (_rangeType == IconSelectRange) {
        if (_individualFwMrRanges && _activeVehicle) {
            const bool isVtol = _activeVehicle->vtol();
            const bool isVtolInFwdFlight = isVtol && _activeVehicle->vtolInFwdFlight();
            const bool isConsideredFixedWing = _activeVehicle->fixedWing() || (isVtol && isVtolInFwdFlight);
            const bool isConsideredMultiRotor = _activeVehicle->multiRotor() || (isVtol && !isVtolInFwdFlight);

            if (isConsideredFixedWing) {
                activeRangeSetToUse = &_fwRange;
            } else if (isConsideredMultiRotor) {
                activeRangeSetToUse = &_mrRange;
            }
        }

        if (_fact && !qIsNaN(factValue)) {
            const int rangeIndex = _currentRangeIndex(factValue, activeRangeSetToUse->values);
            if (rangeIndex != -1 && rangeIndex < activeRangeSetToUse->icons.count()) {
                newIcon = activeRangeSetToUse->icons[rangeIndex].toString();
            }
        }
    }

    if (newIcon != _currentIcon) {
        _currentIcon = newIcon;
        emit currentIconChanged(newIcon);
    }
}

int InstrumentValueData::_currentRangeIndex(const QVariant& value, const QVariantList& rangeValuesToUse)
{
    if (qIsNaN(value.toDouble()) || rangeValuesToUse.isEmpty()) {
        return 0;
    }
    for (int i = 0; i < rangeValuesToUse.count(); i++) {
        if (value.toDouble() <= rangeValuesToUse[i].toDouble()) {
            return i;
        }
    }
    return rangeValuesToUse.count();
}

QStringList InstrumentValueData::factGroupNames(void) const
{
    QStringList groupNames = _activeVehicle->factGroupNames();

    for (QString& name: groupNames) {
        name[0] = name[0].toUpper();
    }
    groupNames.prepend(vehicleFactGroupName);

    return groupNames;
}

QStringList InstrumentValueData::factValueNames(void) const
{
    QStringList valueNames;

    FactGroup* factGroup = nullptr;
    if (_factGroupName == vehicleFactGroupName) {
        factGroup = _activeVehicle;
    } else {
        factGroup = _activeVehicle->getFactGroup(_factGroupName);
    }

    if (factGroup) {
        valueNames = factGroup->factNames();
        for (QString& name: valueNames) {
            name[0] = name[0].toUpper();
        }
    }

    return valueNames;
}

void InstrumentValueData::setFwRangeValues(const QVariantList& fwRangeValues)
{
    if (_fwRange.values != fwRangeValues) {
        _fwRange.values = fwRangeValues;
        emit fwRangeValuesChanged(_fwRange.values);
    }
}

void InstrumentValueData::setFwRangeColors(const QVariantList& fwRangeColors)
{
    if (_fwRange.colors != fwRangeColors) {
        _fwRange.colors = fwRangeColors;
        emit fwRangeColorsChanged(_fwRange.colors);
    }
}

void InstrumentValueData::setFwRangeIcons(const QVariantList& fwRangeIcons)
{
    if (_fwRange.icons != fwRangeIcons) {
        _fwRange.icons = fwRangeIcons;
        emit fwRangeIconsChanged(_fwRange.icons);
    }
}

void InstrumentValueData::setFwRangeOpacities(const QVariantList& fwRangeOpacities)
{
    if (_fwRange.opacities != fwRangeOpacities) {
        _fwRange.opacities = fwRangeOpacities;
        emit fwRangeOpacitiesChanged(_fwRange.opacities);
    }
}

void InstrumentValueData::setMrRangeValues(const QVariantList& mrRangeValues)
{
    if (_mrRange.values != mrRangeValues) {
        _mrRange.values = mrRangeValues;
        emit mrRangeValuesChanged(_mrRange.values);
    }
}

void InstrumentValueData::setMrRangeColors(const QVariantList& mrRangeColors)
{
    if (_mrRange.colors != mrRangeColors) {
        _mrRange.colors = mrRangeColors;
        emit mrRangeColorsChanged(_mrRange.colors);
    }
}

void InstrumentValueData::setMrRangeIcons(const QVariantList& mrRangeIcons)
{
    if (_mrRange.icons != mrRangeIcons) {
        _mrRange.icons = mrRangeIcons;
        emit mrRangeIconsChanged(_mrRange.icons);
    }
}

void InstrumentValueData::setMrRangeOpacities(const QVariantList& mrRangeOpacities)
{
    if (_mrRange.opacities != mrRangeOpacities) {
        _mrRange.opacities = mrRangeOpacities;
        emit mrRangeOpacitiesChanged(_mrRange.opacities);
    }
}

void InstrumentValueData::_emitRangeSignals(const InstrumentValueData::RangeSet& rangeSet, InstrumentValueData::RangeSignalType signalType)
{
    switch (signalType) {
    case NormalRange:
        emit rangeValuesChanged(rangeSet.values);
        emit rangeColorsChanged(rangeSet.colors);
        emit rangeOpacitiesChanged(rangeSet.opacities);
        emit rangeIconsChanged(rangeSet.icons);
        break;
    case FwRange:
        emit fwRangeValuesChanged(rangeSet.values);
        emit fwRangeColorsChanged(rangeSet.colors);
        emit fwRangeOpacitiesChanged(rangeSet.opacities);
        emit fwRangeIconsChanged(rangeSet.icons);
        break;
    case MrRange:
        emit mrRangeValuesChanged(rangeSet.values);
        emit mrRangeColorsChanged(rangeSet.colors);
        emit mrRangeOpacitiesChanged(rangeSet.opacities);
        emit mrRangeIconsChanged(rangeSet.icons);
        break;
    }
}

/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactSystem
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.ScreenTools
import QGroundControl.MultiVehicleManager
import QGroundControl.Palette

// Aviant-specific application settings, split out from GeneralSettings into their
// own dedicated tab to keep the General page focused on stock QGC options.
SettingsPage {
    property var _settingsManager: QGroundControl.settingsManager
    property var _aviantSettings:  _settingsManager.aviantSettings

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Aviant Mission Tools")

        QGCLabel { text: _aviantSettings.missionToolsUrl.shortDescription }
        FactTextField {
            fact:               _aviantSettings.missionToolsUrl
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.missionToolsToken.shortDescription }
        FactTextField {
            fact:               _aviantSettings.missionToolsToken
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.kyteBackendUrl.shortDescription }
        FactTextField {
            fact:               _aviantSettings.kyteBackendUrl
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.kyteBackendToken.shortDescription }
        FactTextField {
            fact:               _aviantSettings.kyteBackendToken
            Layout.fillWidth:   true
        }

        FactCheckBox {
            text:               _aviantSettings.missionToolsInsecureHttps.shortDescription
            fact:               _aviantSettings.missionToolsInsecureHttps
        }

        FactCheckBox {
            text:               qsTr("Show winch control menu")
            fact:               _aviantSettings.showWinchControlMenu
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Aviant Traffic")

        FactCheckBox {
            id:                 showTrafficIndicatorsCheckBox
            text:               _aviantSettings.showTrafficIndicators.shortDescription
            fact:               _aviantSettings.showTrafficIndicators
        }

        QGCLabel { text: _aviantSettings.horizontalConflictDistance.shortDescription }
        FactTextField {
            fact:               _aviantSettings.horizontalConflictDistance
            enabled:            showTrafficIndicatorsCheckBox.checked
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.verticalConflictDistance.shortDescription }
        FactTextField {
            fact:               _aviantSettings.verticalConflictDistance
            enabled:            showTrafficIndicatorsCheckBox.checked
            Layout.fillWidth:   true
        }

        FactCheckBox {
            id:                 showMultidroneConflictCircleCheckBox
            text:               _aviantSettings.showMultidroneConflictCircle.shortDescription
            fact:               _aviantSettings.showMultidroneConflictCircle
        }

        QGCLabel { text: _aviantSettings.multidroneConflictDistance.shortDescription }
        FactTextField {
            fact:               _aviantSettings.multidroneConflictDistance
            enabled:            showMultidroneConflictCircleCheckBox.checked
            Layout.fillWidth:   true
        }
    }

    SettingsGroupLayout {
        Layout.fillWidth:   true
        heading:            qsTr("Aviant Battery")

        FactCheckBox {
            text:               _aviantSettings.showBatteryWidget.shortDescription
            fact:               _aviantSettings.showBatteryWidget
        }

        QGCLabel { text: _aviantSettings.preDeliveryBingoLimit.shortDescription }
        FactTextField {
            fact:               _aviantSettings.preDeliveryBingoLimit
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.postDeliveryBingoLimit.shortDescription }
        FactTextField {
            fact:               _aviantSettings.postDeliveryBingoLimit
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.persistentConsumeWarnLimit.shortDescription }
        FactTextField {
            fact:               _aviantSettings.persistentConsumeWarnLimit
            Layout.fillWidth:   true
        }

        QGCLabel { text: _aviantSettings.unitUntilThreshold.shortDescription }
        FactComboBox {
            fact:               _aviantSettings.unitUntilThreshold
            indexModel:         false
            Layout.fillWidth:   true
        }
    }
}

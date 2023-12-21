/****************************************************************************
 *
 * (c) 2009-2023 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QGroundControl.FlightDisplay 1.0

GuidedToolStripAction {
    text:       _guidedController.branchTitle
    iconSource: "/res/branch.svg"
    visible:    _guidedController.showBranch
    enabled:    _guidedController.showBranch
    actionID:   _guidedController.actionBranch
}

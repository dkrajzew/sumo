/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleTypeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// The Widget for edit Vehicle Type elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/dialogs/GNEVehicleTypeDialog.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/demandelements/GNEVehicle.h>
#include <netedit/demandelements/GNEVehicleType.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEVehicleTypeFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVehicleTypeFrame::VehicleTypeSelector) vehicleTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNEVehicleTypeFrame::VehicleTypeSelector::onCmdSelectItem)
};

FXDEFMAP(GNEVehicleTypeFrame::VehicleTypeEditor) vehicleTypeEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_CREATE,    GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCreateVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_DELETE,    GNEVehicleTypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_VEHICLETYPEFRAME_COPY,      GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCopyVehicleType)
};

// Object implementation
FXIMPLEMENT(GNEVehicleTypeFrame::VehicleTypeSelector,   FXGroupBox,     vehicleTypeSelectorMap,     ARRAYNUMBER(vehicleTypeSelectorMap))
FXIMPLEMENT(GNEVehicleTypeFrame::VehicleTypeEditor,     FXGroupBox,     vehicleTypeEditorMap,       ARRAYNUMBER(vehicleTypeEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame::VehicleTypeSelector - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::VehicleTypeSelector::VehicleTypeSelector(GNEVehicleTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Current Vehicle Type", GUIDesignGroupBoxFrame),
    myVehicleTypeFrameParent(vehicleTypeFrameParent) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // VehicleTypeSelector is always shown
    show();
}


GNEVehicleTypeFrame::VehicleTypeSelector::~VehicleTypeSelector() {}


GNEDemandElement*
GNEVehicleTypeFrame::VehicleTypeSelector::getCurrentVehicleType() const {
    // obtain current VType ID (To improve code legibly)
    std::string vTypeID = myTypeMatchBox->getItem(myTypeMatchBox->getCurrentItem()).text();
    // check if ID of myTypeMatchBox is a valid ID
    if (myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).count(vTypeID) == 1) {
        return myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE).at(vTypeID);
    } else {
        return nullptr;
    }
}


void
GNEVehicleTypeFrame::VehicleTypeSelector::setCurrentVehicleType(GNEDemandElement *vType) {
    bool valid = false;
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == vType->getID()) {
            myTypeMatchBox->setCurrentItem(i);
            valid = true;
        }
    }
    // Check that give vType type is valid
    if (valid) {
        // refresh vehicle type selector
        refreshVehicleTypeSelector();
        // show moduls if selected item is valid
        myVehicleTypeFrameParent->enableModuls(vType);
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleTypeFrameParent->disableModuls();
    }
}


void 
GNEVehicleTypeFrame::VehicleTypeSelector::refreshVehicleTypeSelector() {
    // first clear items
    myTypeMatchBox->clearItems();
    // fill myTypeMatchBox with list of VTypes IDs
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // refresh vehicle type editor
    myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
}


long
GNEVehicleTypeFrame::VehicleTypeSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myVehicleTypeFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE)) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // show moduls if selected item is valid
            myVehicleTypeFrameParent->enableModuls(i.second);
            // refresh vehicle type editor
            myVehicleTypeFrameParent->myVehicleTypeEditor->refreshVehicleTypeEditorModul();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in VehicleTypeSelector").text());
            return 1;
        }
    }
    // hide all moduls if selected item isn't valid
    myVehicleTypeFrameParent->disableModuls();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in VehicleTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame::VehicleTypeEditor - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::VehicleTypeEditor::VehicleTypeEditor(GNEVehicleTypeFrame* vehicleTypeFrameParent) :
    FXGroupBox(vehicleTypeFrameParent->myContentFrame, "Vehicle Type Editor", GUIDesignGroupBoxFrame),
    myVehicleTypeFrameParent(vehicleTypeFrameParent) {
    // Create new vehicle type
    myCreateVehicleTypeButton = new FXButton(this, "Create Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_CREATE, GUIDesignButton);
    // Create copy vehicle type
    myDeleteVehicleTypeButton = new FXButton(this, "Delete Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_DELETE, GUIDesignButton);
    // Create copy vehicle type
    myCopyVehicleTypeButton = new FXButton(this, "Copy Vehicle Type", nullptr, this, MID_GNE_VEHICLETYPEFRAME_COPY, GUIDesignButton);
}


GNEVehicleTypeFrame::VehicleTypeEditor::~VehicleTypeEditor() {}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::showVehicleTypeEditorModul() {
    show();
}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::hideVehicleTypeEditorModul() {
    hide();
}


void 
GNEVehicleTypeFrame::VehicleTypeEditor::refreshVehicleTypeEditorModul() {
    // disable delete button if DEFAULT_VTYPE_ID is selected
    if (myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType()->getID() == DEFAULT_VTYPE_ID) {
        myDeleteVehicleTypeButton->disable();
    } else {
        myDeleteVehicleTypeButton->enable();
    }
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCreateVehicleType(FXObject*, FXSelector, void*) {
    // create new vehicle type
    std::string vehicleTypeID = myVehicleTypeFrameParent->myViewNet->getNet()->generateDemandElementID(SUMO_TAG_VTYPE);
    GNEDemandElement *vehicleType = new GNEVehicleType(myVehicleTypeFrameParent->myViewNet, vehicleTypeID);
    myVehicleTypeFrameParent->myViewNet->getUndoList()->add(new GNEChange_DemandElement(vehicleType, true), true);
    // set created vehicle type in selector
    myVehicleTypeFrameParent->myVehicleTypeSelector->setCurrentVehicleType(vehicleType);
    return 1;
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdDeleteVehicleType(FXObject*, FXSelector, void*) {
    // delete current vehicle type (except DEFAULT_VTYPE)
    myVehicleTypeFrameParent->myViewNet->getUndoList()->add(
        new GNEChange_DemandElement(myVehicleTypeFrameParent->myVehicleTypeSelector->getCurrentVehicleType(), false), true);
    return 1;
}


long 
GNEVehicleTypeFrame::VehicleTypeEditor::onCmdCopyVehicleType(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEVehicleTypeFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleTypeFrame::GNEVehicleTypeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Vehicle Types") {

    // create modul for edit vehicle types (Create, copy, etc.)
    myVehicleTypeEditor = new VehicleTypeEditor(this);

    // create vehicle type selector
    myVehicleTypeSelector = new VehicleTypeSelector(this);

    // Create vehicle parameters
    myVehicleTypeAttributes = new ACAttributes(this);

    // create modul for open extended attributes dialog
    myACAttributesExtended = new ACAttributesExtended(this);

    // set "VTYPE_DEFAULT" as default vehicle Type
    myVehicleTypeSelector->setCurrentVehicleType(myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID));
}


GNEVehicleTypeFrame::~GNEVehicleTypeFrame() {}


void
GNEVehicleTypeFrame::show() {
    // refresh vehicle type selector
    myVehicleTypeSelector->refreshVehicleTypeSelector();
    // show frame
    GNEFrame::show();
}


GNEVehicleTypeFrame::VehicleTypeSelector* 
GNEVehicleTypeFrame::getVehicleTypeSelector() const {
    return myVehicleTypeSelector;
}


void
GNEVehicleTypeFrame::enableModuls(GNEDemandElement *vType) {
    // show vehicle type attributes moduls (Except the extended attributes)
    myVehicleTypeAttributes->showACAttributesModul(vType->getTagProperty(), false);
}


void
GNEVehicleTypeFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myVehicleTypeAttributes->hideACAttributesModul();
}


void 
GNEVehicleTypeFrame::openACAttributesExtendedDialog() {
    // open vehicle type dialog
    if (myVehicleTypeSelector->getCurrentVehicleType()) {
        GNEVehicleTypeDialog(myVehicleTypeSelector->getCurrentVehicleType(), true);
    }
}

/****************************************************************************/

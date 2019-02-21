/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/dialogs/GNEGenericParameterDialog.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>

#include "GNEFrame.h"
#include "GNEInspectorFrame.h"
#include "GNEDeleteFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrame::ItemSelector) ItemSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEFrame::ItemSelector::onCmdSelectItem),
};

FXDEFMAP(GNEFrame::ACAttributes) ACAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrame::ACAttributes::onCmdHelp),
};

FXDEFMAP(GNEFrame::ACAttributes::Row) RowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_TEXT,         GNEFrame::ACAttributes::Row::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,         GNEFrame::ACAttributes::Row::onCmdSetBooleanAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEFrame::ACAttributes::Row::onCmdSetColorAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_RADIOBUTTON,  GNEFrame::ACAttributes::Row::onCmdSelectRadioButton),
};

FXDEFMAP(GNEFrame::ACAttributesExtended) ACAttributesExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrame::ACAttributesExtended::onCmdOpenDialog),
};

FXDEFMAP(GNEFrame::ACHierarchy) ACHierarchyMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_CENTER,      GNEFrame::ACHierarchy::onCmdCenterItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_INSPECT,     GNEFrame::ACHierarchy::onCmdInspectItem),
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_DELETE,      GNEFrame::ACHierarchy::onCmdDeleteItem),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   MID_GNE_DELETEFRAME_CHILDS,         GNEFrame::ACHierarchy::onCmdShowChildMenu),
};

FXDEFMAP(GNEFrame::GenericParametersEditor) GenericParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrame::GenericParametersEditor::onCmdEditGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrame::GenericParametersEditor::onCmdSetGenericParameter),
};

FXDEFMAP(GNEFrame::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrame::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrame::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrame::DrawingShape::onCmdAbortDrawing),
};

FXDEFMAP(GNEFrame::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrame::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEFrame::NeteditAttributes::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEFrame::ItemSelector,             FXGroupBox,         ItemSelectorMap,            ARRAYNUMBER(ItemSelectorMap))
FXIMPLEMENT(GNEFrame::ACAttributes,             FXGroupBox,         ACAttributesMap,            ARRAYNUMBER(ACAttributesMap))
FXIMPLEMENT(GNEFrame::ACAttributes::Row,        FXHorizontalFrame,  RowMap,                     ARRAYNUMBER(RowMap))
FXIMPLEMENT(GNEFrame::ACAttributesExtended,     FXGroupBox,         ACAttributesExtendedMap,    ARRAYNUMBER(ACAttributesExtendedMap))
FXIMPLEMENT(GNEFrame::ACHierarchy,              FXGroupBox,         ACHierarchyMap,             ARRAYNUMBER(ACHierarchyMap))
FXIMPLEMENT(GNEFrame::GenericParametersEditor,  FXGroupBox,         GenericParametersEditorMap, ARRAYNUMBER(GenericParametersEditorMap))
FXIMPLEMENT(GNEFrame::DrawingShape,             FXGroupBox,         DrawingShapeMap,            ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrame::NeteditAttributes,        FXGroupBox,         NeteditAttributesMap,       ARRAYNUMBER(NeteditAttributesMap))

// ===========================================================================
// static members
// ===========================================================================

FXFont* GNEFrame::myFrameHeaderFont = nullptr;

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrame::ItemSelector - methods
// ---------------------------------------------------------------------------

GNEFrame::ItemSelector::ItemSelector(GNEFrame* frameParent, GNEAttributeCarrier::TagType type, bool onlyDrawables) :
    FXGroupBox(frameParent->myContentFrame, "Element", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // first check that property is valid
    switch (type)     {
        case GNEAttributeCarrier::TagType::TAGTYPE_NETELEMENT:
            setText("Net element");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_ADDITIONAL:
            setText("Additional element");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_SHAPE:
            setText("Shape element");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_TAZ:
            setText("TAZ element");
            break;
        case GNEAttributeCarrier::TagType::TAGTYPE_VEHICLE:
            setText("Vehicle");
            break;
        default:
            throw ProcessError("invalid tag property");
    }
    // fill myListOfTags
    myListOfTags = GNEAttributeCarrier::allowedTagsByCategory(type, onlyDrawables);
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with list of tags
    for (const auto& i : myListOfTags) {
        myTypeMatchBox->appendItem(toString(i).c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // ItemSelector is always shown
    show();
}


GNEFrame::ItemSelector::~ItemSelector() {}


const GNEAttributeCarrier::TagProperties&
GNEFrame::ItemSelector::getCurrentTagProperties() const {
    return myCurrentTagProperties;
}


void
GNEFrame::ItemSelector::setCurrentTypeTag(SumoXMLTag typeTag) {
    // make sure that tag is in myTypeMatchBox
    for (int i = 0; i < (int)myTypeMatchBox->getNumItems(); i++) {
        if (myTypeMatchBox->getItem(i).text() == toString(typeTag)) {
            myTypeMatchBox->setCurrentItem(i);
            // Set new current type
            myCurrentTagProperties = GNEAttributeCarrier::getTagProperties(typeTag);
        }
    }
    // Check that typeTag type is valid
    if (myCurrentTagProperties.getTag() != SUMO_TAG_NOTHING) {
        // show moduls if selected item is valid
        myFrameParent->enableModuls(myCurrentTagProperties);
    } else {
        // hide all moduls if selected item isn't valid
        myFrameParent->disableModuls();
    }
}


void
GNEFrame::ItemSelector::refreshTagProperties() {
    // simply call onCmdSelectItem (to avoid duplicated code)
    onCmdSelectItem(0, 0, 0);
}


long
GNEFrame::ItemSelector::onCmdSelectItem(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myListOfTags) {
        if (toString(i) == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentTagProperties = GNEAttributeCarrier::getTagProperties(i);
            // show moduls if selected item is valid
            myFrameParent->enableModuls(myCurrentTagProperties);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in ItemSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentTagProperties = myInvalidTagProperty;
    // hide all moduls if selected item isn't valid
    myFrameParent->disableModuls();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in ItemSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrame::ACAttributes::ACAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // Create single parameters
    for (int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myRows.push_back(new Row(this));
    }
    // Create help button
    new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrame::ACAttributes::~ACAttributes() {
}


void
GNEFrame::ACAttributes::showACAttributesModul(const GNEAttributeCarrier::TagProperties& tagProperties, bool includeExtendedAttributes) {
    // get current tag Properties
    myTagProperties = tagProperties;
    // Hide all fields
    for (int i = 0; i < (int)myRows.size(); i++) {
        myRows.at(i)->hideParameter();
    }
    // iterate over tag attributes and show it
    for (auto i : myTagProperties) {
        //  make sure that only non-unique attributes are shown (And depending of includeExtendedAttributes)
        if (!i.second.isUnique() && (includeExtendedAttributes || !i.second.isExtended())) {
            myRows.at(i.second.getPositionListed())->showParameter(i.second);
        }
    }
    // update disjoint attributes
    updateDisjointAttributes(nullptr);
    // recalc frame and show again
    recalc();
    show();
}


void
GNEFrame::ACAttributes::hideACAttributesModul() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEFrame::ACAttributes::getAttributesAndValues(bool includeAll) const {
    std::map<SumoXMLAttr, std::string> values;
    // get standard parameters
    for (int i = 0; i < (int)myRows.size(); i++) {
        if (myRows.at(i)->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            // ignore default values (except for disjont attributes, that has to be always writted)
            if (myRows.at(i)->isRowEnabled() &&
               (includeAll || myTagProperties.isDisjointAttributes(myRows.at(i)->getAttrProperties().getAttr()) || !myRows.at(i)->getAttrProperties().hasDefaultValue() || (myRows.at(i)->getAttrProperties().getDefaultValue() != myRows.at(i)->getValue()))) {
                values[myRows.at(i)->getAttrProperties().getAttr()] = myRows.at(i)->getValue();
            }
        }
    }
    return values;
}


void
GNEFrame::ACAttributes::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (auto i : myTagProperties) {
        if (errorMessage.empty()) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myRows.at(i.second.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + extra;
    }

    // set message in status bar
    myFrameParent->getViewNet()->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEFrame::ACAttributes::areValuesValid() const {
    // iterate over standar parameters
    for (auto i : myTagProperties) {
        // Return false if error message of attriuve isn't empty
        if (myRows.at(i.second.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


void 
GNEFrame::ACAttributes::updateDisjointAttributes(ACAttributes::Row *row) {
    // currently only Flows supports disjoint attributes
    if (myTagProperties.getTag() == SUMO_TAG_FLOW) {
        // obtain all rows (to improve code legibility)
        Row* endRow = myRows[myTagProperties.getAttributeProperties(SUMO_ATTR_END).getPositionListed()];
        Row* numberRow = myRows[myTagProperties.getAttributeProperties(SUMO_ATTR_NUMBER).getPositionListed()];
        Row* vehsperhourRow = myRows[myTagProperties.getAttributeProperties(SUMO_ATTR_VEHSPERHOUR).getPositionListed()];
        Row* periodRow = myRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PERIOD).getPositionListed()];
        Row* probabilityRow = myRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PROB).getPositionListed()];
        if(row == nullptr) {
            // by default flows uses end and number
            endRow->setRadioButtonCheck(true);
            numberRow->setRadioButtonCheck(true);
            vehsperhourRow->setRadioButtonCheck(false);
            periodRow->setRadioButtonCheck(false);
            probabilityRow->setRadioButtonCheck(false);
        } else {
            // check what row was clicked
            switch (row->getAttrProperties().getAttr()) {
                // end has more priority as number
                case SUMO_ATTR_END:
                    endRow->setRadioButtonCheck(true);
                    // disable other combinations
                    vehsperhourRow->setRadioButtonCheck(false);
                    periodRow->setRadioButtonCheck(false);
                    probabilityRow->setRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_NUMBER:
                    numberRow->setRadioButtonCheck(true);
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getRadioButtonCheck()) {
                        endRow->setRadioButtonCheck(false);
                    } else {
                        // disable other combinations
                        vehsperhourRow->setRadioButtonCheck(false);
                        periodRow->setRadioButtonCheck(false);
                        probabilityRow->setRadioButtonCheck(false);
                    }
                    break;
                case SUMO_ATTR_VEHSPERHOUR:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getRadioButtonCheck() && numberRow->getRadioButtonCheck()) {
                        numberRow->setRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setRadioButtonCheck(true);
                    periodRow->setRadioButtonCheck(false);
                    probabilityRow->setRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PERIOD:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getRadioButtonCheck() && numberRow->getRadioButtonCheck()) {
                        numberRow->setRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setRadioButtonCheck(false);
                    periodRow->setRadioButtonCheck(true);
                    probabilityRow->setRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PROB:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getRadioButtonCheck() && numberRow->getRadioButtonCheck()) {
                        numberRow->setRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setRadioButtonCheck(false);
                    periodRow->setRadioButtonCheck(false);
                    probabilityRow->setRadioButtonCheck(true);
                    break;
                default:
                    break;
            }
        }
    }
}


long
GNEFrame::ACAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTagProperties);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::ACAttributesExtended- methods
// ---------------------------------------------------------------------------

GNEFrame::ACAttributesExtended::ACAttributesExtended(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Extended attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // Create open dialog button
    new FXButton(this, "Open attributes editor", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrame::ACAttributesExtended::~ACAttributesExtended() {}


void 
GNEFrame::ACAttributesExtended::showACAttributesExtendedModul() {
    show();
}


void 
GNEFrame::ACAttributesExtended::hideACAttributesExtendedModul() {
    hide();
}


long 
GNEFrame::ACAttributesExtended::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // open ACAttributes extended dialog
    myFrameParent->openACAttributesExtendedDialog();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::ACAttributes::Row - methods
// ---------------------------------------------------------------------------

GNEFrame::ACAttributes::Row::Row(ACAttributes* ACAttributesParent) :
    FXHorizontalFrame(ACAttributesParent, GUIDesignAuxiliarHorizontalFrame),
    myACAttributesParent(ACAttributesParent) {
    // Create left visual elements
    myLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myColorEditor = new FXButton(this, "ColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myRadioButton = new FXRadioButton(this, "name", this, MID_GNE_SET_ATTRIBUTE_RADIOBUTTON, GUIDesignRadioButtonAttribute);
    // Create right visual elements
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldInt);
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextFieldReal);
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE_TEXT, GUIDesignTextField);
    myBoolCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    // Hide elements
    hideParameter();
}


GNEFrame::ACAttributes::Row::~Row() {}


void
GNEFrame::ACAttributes::Row::showParameter(const GNEAttributeCarrier::AttributeProperties& attrProperties) {
    myAttrProperties = attrProperties;
    myInvalidValue = "";
    // show label, button for edit colors or radio button
    if (myAttrProperties.isColor()) {
        myColorEditor->setTextColor(FXRGB(0, 0, 0));
        myColorEditor->setText(myAttrProperties.getAttrStr().c_str());
        myColorEditor->show();
    } else if (myACAttributesParent->myTagProperties.isDisjointAttributes(myAttrProperties.getAttr())) {
        myRadioButton->setText(myAttrProperties.getAttrStr().c_str());
        myRadioButton->show();
    } else {
        myLabel->setText(myAttrProperties.getAttrStr().c_str());
        myLabel->show();
    }
    if (myAttrProperties.isInt()) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->setText(attrProperties.getDefaultValue().c_str());
        myTextFieldInt->show();
        // if it's associated to a radio button and is disabled, then disabled myTextFieldInt
        if(myRadioButton->shown() && (myRadioButton->getCheck() == FALSE)) {
            myTextFieldInt->disable();
        }
    } else if (myAttrProperties.isFloat()) {
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->setText(attrProperties.getDefaultValue().c_str());
        myTextFieldReal->show();
        // if it's associated to a radio button and is disabled, then disable myTextFieldReal
        if(myRadioButton->shown() && (myRadioButton->getCheck() == FALSE)) {
            myTextFieldReal->disable();
        }
    } else if (myAttrProperties.isBool()) {
        if (GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue())) {
            myBoolCheckButton->setCheck(true);
            myBoolCheckButton->setText("true");
        } else {
            myBoolCheckButton->setCheck(false);
            myBoolCheckButton->setText("false");
        }
        myBoolCheckButton->show();
        // if it's associated to a radio button and is disabled, then disable myBoolCheckButton
        if(myRadioButton->shown() && (myRadioButton->getCheck() == FALSE)) {
            myBoolCheckButton->disable();
        }
    } else {
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->setText(attrProperties.getDefaultValue().c_str());
        myTextFieldStrings->show();
        // if it's associated to a radio button and is disabled, then disable myTextFieldStrings
        if(myRadioButton->shown() && (myRadioButton->getCheck() == FALSE)) {
            myTextFieldStrings->disable();
        }
    }
    show();
}


void
GNEFrame::ACAttributes::Row::hideParameter() {
    myAttrProperties = GNEAttributeCarrier::AttributeProperties();
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myBoolCheckButton->hide();
    myColorEditor->hide();
    myRadioButton->hide();
    hide();
}


const GNEAttributeCarrier::AttributeProperties &
GNEFrame::ACAttributes::Row::getAttrProperties() const {
    return myAttrProperties;
}


std::string
GNEFrame::ACAttributes::Row::getValue() const {
    if (myAttrProperties.isBool()) {
        return (myBoolCheckButton->getCheck() == 1) ? "1" : "0";
    } else if (myAttrProperties.isInt()) {
        return myTextFieldInt->getText().text();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isTime()) {
        return myTextFieldReal->getText().text();
    } else {
        return myTextFieldStrings->getText().text();
    }
}


bool
GNEFrame::ACAttributes::Row::getRadioButtonCheck() const {
    if (shown()) {
        return myRadioButton->getCheck();
    } else {
        return false;
    }
}


void
GNEFrame::ACAttributes::Row::setRadioButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myRadioButton->setCheck(value);
        // enable or disable input fields
        if (value) {
            if (myAttrProperties.isBool()) {
                myBoolCheckButton->enable();
            } else if (myAttrProperties.isInt()) {
                myTextFieldInt->enable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isTime()) {
                myTextFieldReal->enable();
            } else {
                myTextFieldStrings->enable();
            }
        } else {
            if (myAttrProperties.isBool()) {
                myBoolCheckButton->disable();
            } else if (myAttrProperties.isInt()) {
                myTextFieldInt->disable();
            } else if (myAttrProperties.isFloat() || myAttrProperties.isTime()) {
                myTextFieldReal->disable();
            } else {
                myTextFieldStrings->disable();
            }
        }
    }
}


bool 
GNEFrame::ACAttributes::Row::isRowEnabled() const {
    if (!shown()) {
        return false;
    } else if (myAttrProperties.isBool()) {
        return myBoolCheckButton->isEnabled();
    } else if (myAttrProperties.isInt()) {
        return myTextFieldInt->isEnabled();
    } else if (myAttrProperties.isFloat() || myAttrProperties.isTime()) {
        return myTextFieldReal->isEnabled();
    } else {
        return myTextFieldStrings->isEnabled();
    }
}


const std::string&
GNEFrame::ACAttributes::Row::isAttributeValid() const {
    return myInvalidValue;
}


GNEFrame::ACAttributes*
GNEFrame::ACAttributes::Row::getACAttributesParent() const {
    return myACAttributesParent;
}


long
GNEFrame::ACAttributes::Row::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // Check if format of current value of myTextField is correct
    if (myAttrProperties.isInt()) {
        if (GNEAttributeCarrier::canParse<int>(myTextFieldInt->getText().text())) {
            // convert string to int
            int intValue = GNEAttributeCarrier::parse<int>(myTextFieldInt->getText().text());
            // Check if int value must be positive
            if (myAttrProperties.isPositive() && (intValue < 0)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'int' format";
        }
    } else if (myAttrProperties.isTime()) {
        // time attributes work as positive doubles
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if parsed value is negative
            if (doubleValue < 0) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'time' format";
        }
    } else if (myAttrProperties.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myTextFieldReal->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myTextFieldReal->getText().text());
            // Check if double value must be positive
            if (myAttrProperties.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
                // check if double value is a probability
            } else if (myAttrProperties.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between 0 and 1";
            } else if (myAttrProperties.hasAttrRange() && ((doubleValue < myAttrProperties.getMinimumRange()) || doubleValue > myAttrProperties.getMaximumRange())) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between " + toString(myAttrProperties.getMinimumRange()) + " and " + toString(myAttrProperties.getMaximumRange());
            } else if ((myACAttributesParent->myTagProperties.getTag() == SUMO_TAG_E2DETECTOR) && (myAttrProperties.getAttr() == SUMO_ATTR_LENGTH) && (doubleValue == 0)) {
                myInvalidValue = "E2 length cannot be 0";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'float' format";
        }
    } else if (myAttrProperties.isColor()) {
        // check if filename format is valid
        if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text()) == false) {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'RBGColor' format";
        }
    } else if (myAttrProperties.isFilename()) {
        std::string file = myTextFieldStrings->getText().text();
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(file) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        } else if (myAttrProperties.getAttr() == SUMO_ATTR_IMGFILE) {
            if (!file.empty()) {
                // only load value if file exist and can be loaded
                if (GUITexturesHelper::getTextureID(file) == -1) {
                    myInvalidValue = "doesn't exist image '" + file + "'";
                }
            }
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_NAME) {
        std::string name = myTextFieldStrings->getText().text();
        // check if name format is valid
        if (SUMOXMLDefinitions::isValidAttribute(name) == false) {
            myInvalidValue = "input contains invalid characters";
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_VTYPES) {
        std::string name = myTextFieldStrings->getText().text();
        // if list of VTypes isn't empty, check that all characters are valid
        if (!name.empty() && !SUMOXMLDefinitions::isValidListOfTypeID(name)) {
            myInvalidValue = "list of IDs contains invalid characters";
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->killFocus();
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->killFocus();
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEFrame::ACAttributes::Row::onCmdSetBooleanAttribute(FXObject*, FXSelector, void*) {
    if (myBoolCheckButton->getCheck()) {
        myBoolCheckButton->setText("true");
    } else {
        myBoolCheckButton->setText("false");
    }
    return 0;
}


long 
GNEFrame::ACAttributes::Row::onCmdSetColorAttribute(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldStrings->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myAttrProperties.getDefaultValue())));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myTextFieldStrings->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(nullptr, 0, nullptr);
    }
    return 0;
}

long 
GNEFrame::ACAttributes::Row::onCmdSelectRadioButton(FXObject*, FXSelector, void*) {
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected radio button for attribute '" + myAttrProperties.getAttrStr() + "'");
    // update disjoint attributes in AC Attributes parent
    myACAttributesParent->updateDisjointAttributes(this);
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame::ACHierarchy - methods
// ---------------------------------------------------------------------------

GNEFrame::ACHierarchy::ACHierarchy(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Hierarchy", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myAC(nullptr) {
    // Create three list
    myTreelist = new FXTreeList(this, this, MID_GNE_DELETEFRAME_CHILDS, GUIDesignTreeListFrame);
    hide();
}


GNEFrame::ACHierarchy::~ACHierarchy() {}


void
GNEFrame::ACHierarchy::showACHierarchy(GNEAttributeCarrier* AC) {
    myAC = AC;
    // show ACHierarchy and refresh ACHierarchy
    if (myAC) {
        show();
        refreshACHierarchy();
    }
}


void
GNEFrame::ACHierarchy::hideACHierarchy() {
    myAC = nullptr;
    hide();
}


void
GNEFrame::ACHierarchy::refreshACHierarchy() {
    // clear items
    myTreelist->clearItems();
    myTreeItemToACMap.clear();
    myTreeItemsConnections.clear();
    // show ACChilds of myAC
    if (myAC) {
        showAttributeCarrierChilds(myAC, showAttributeCarrierParents());
    }
}


long
GNEFrame::ACHierarchy::onCmdShowChildMenu(FXObject*, FXSelector, void* eventData) {
    // Obtain event
    FXEvent* e = (FXEvent*)eventData;
    // obtain FXTreeItem in the given position
    FXTreeItem* item = myTreelist->getItemAt(e->win_x, e->win_y);
    // open Pop-up if FXTreeItem has a Attribute Carrier vinculated
    if (item && (myTreeItemsConnections.find(item) == myTreeItemsConnections.end())) {
        createPopUpMenu(e->root_x, e->root_y, myTreeItemToACMap[item]);
    }
    return 1;
}


long
GNEFrame::ACHierarchy::onCmdCenterItem(FXObject*, FXSelector, void*) {
    GUIGlObject* glObject = dynamic_cast<GUIGlObject*>(myRightClickedAC);
    if (glObject) {
        myFrameParent->getViewNet()->centerTo(glObject->getGlID(), false);
        myFrameParent->getViewNet()->update();
    }
    return 1;
}


long
GNEFrame::ACHierarchy::onCmdInspectItem(FXObject*, FXSelector, void*) {
    if ((myAC != nullptr) && (myRightClickedAC != nullptr)) {
        myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectChild(myRightClickedAC, myAC);
    }
    return 1;
}


long
GNEFrame::ACHierarchy::onCmdDeleteItem(FXObject*, FXSelector, void*) {
    // check if Inspector frame was opened before removing
    const std::vector<GNEAttributeCarrier*>& currentInspectedACs = myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getInspectedACs();
    // Remove Attribute Carrier
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->removeAttributeCarrier(myRightClickedAC);
    myFrameParent->getViewNet()->getViewParent()->getDeleteFrame()->hide();
    // check if inspector frame has to be shown again
    if (currentInspectedACs.size() == 1) {
        if (currentInspectedACs.front() != myRightClickedAC) {
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(currentInspectedACs.front());
        } else {
            // inspect a nullprt element to reset inspector frame
            myFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->inspectSingleElement(nullptr);
        }
    }
    return 1;
}


void
GNEFrame::ACHierarchy::createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac) {
    // create FXMenuPane
    FXMenuPane* pane = new FXMenuPane(myTreelist);
    // set current clicked AC
    myRightClickedAC = ac;
    // set name
    new MFXMenuHeader(pane, myFrameParent->getViewNet()->getViewParent()->getGUIMainWindow()->getBoldFont(), myRightClickedAC->getPopUpID().c_str(), myRightClickedAC->getIcon());
    new FXMenuSeparator(pane);
    // Fill FXMenuCommand
    new FXMenuCommand(pane, "Center", GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_GNE_INSPECTORFRAME_CENTER);
    new FXMenuCommand(pane, "Inspect", GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_INSPECTORFRAME_INSPECT);
    new FXMenuCommand(pane, "Delete", GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_INSPECTORFRAME_DELETE);
    // Center in the mouse position and create pane
    pane->setX(X);
    pane->setY(Y);
    pane->create();
    pane->show();
}


FXTreeItem*
GNEFrame::ACHierarchy::showAttributeCarrierParents() {
    // Switch gl type of ac
    switch (myAC->getTagProperty().getTag()) {
        case SUMO_TAG_EDGE: {
            // obtain Edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(myAC->getID(), false);
            if (edge) {
                // insert Junctions of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                // return junction destiny Item
                return junctionDestinyItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_LANE: {
            // obtain lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(myAC->getID(), false);
            if (lane) {
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                // return edge item
                return edgeItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_POILANE: {
            // Obtain POILane
            GNEPOI* POILane = myFrameParent->getViewNet()->getNet()->retrievePOI(myAC->getID(), false);
            if (POILane) {
                // obtain lane parent
                GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(POILane->getLane()->getID());
                // obtain edge parent
                GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                //inser Junctions of lane of edge in tree (Pararell because a edge has always two Junctions)
                FXTreeItem* junctionSourceItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " origin").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                FXTreeItem* junctionDestinyItem = myTreelist->insertItem(nullptr, nullptr, (edge->getGNEJunctionSource()->getHierarchyName() + " destiny").c_str(), edge->getGNEJunctionSource()->getIcon(), edge->getGNEJunctionSource()->getIcon());
                junctionDestinyItem->setExpanded(true);
                // Create edge item
                FXTreeItem* edgeItem = myTreelist->insertItem(nullptr, junctionDestinyItem, edge->getHierarchyName().c_str(), edge->getIcon(), edge->getIcon());
                edgeItem->setExpanded(true);
                // Create lane item
                FXTreeItem* laneItem = myTreelist->insertItem(nullptr, edgeItem, lane->getHierarchyName().c_str(), lane->getIcon(), lane->getIcon());
                laneItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionSourceItem] = edge->getGNEJunctionSource();
                myTreeItemToACMap[junctionDestinyItem] = edge->getGNEJunctionDestiny();
                myTreeItemToACMap[edgeItem] = edge;
                myTreeItemToACMap[laneItem] = lane;
                // return Lane item
                return laneItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_CROSSING: {
            // obtain Crossing
            GNECrossing* crossing = myFrameParent->getViewNet()->getNet()->retrieveCrossing(myAC->getID(), false);
            if (crossing) {
                // obtain junction
                GNEJunction* junction = crossing->getParentJunction();
                // create junction item
                FXTreeItem* junctionItem = myTreelist->insertItem(nullptr, nullptr, junction->getHierarchyName().c_str(), junction->getIcon(), junction->getIcon());
                junctionItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[junctionItem] = junction;
                // return junction Item
                return junctionItem;
            } else {
                return nullptr;
            }
        }
        case SUMO_TAG_CONNECTION: {
            // obtain Connection
            GNEConnection* connection = myFrameParent->getViewNet()->getNet()->retrieveConnection(myAC->getID(), false);
            if (connection) {
                // create edge from item
                FXTreeItem* edgeFromItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeFrom()->getHierarchyName().c_str(), connection->getEdgeFrom()->getIcon(), connection->getEdgeFrom()->getIcon());
                edgeFromItem->setExpanded(true);
                // create edge to item
                FXTreeItem* edgeToItem = myTreelist->insertItem(nullptr, nullptr, connection->getEdgeTo()->getHierarchyName().c_str(), connection->getEdgeTo()->getIcon(), connection->getEdgeTo()->getIcon());
                edgeToItem->setExpanded(true);
                // create connection item
                FXTreeItem* connectionItem = myTreelist->insertItem(nullptr, edgeToItem, connection->getHierarchyName().c_str(), connection->getIcon(), connection->getIcon());
                connectionItem->setExpanded(true);
                // Save items in myTreeItemToACMap
                myTreeItemToACMap[edgeFromItem] = connection->getEdgeFrom();
                myTreeItemToACMap[edgeToItem] = connection->getEdgeTo();
                myTreeItemToACMap[connectionItem] = connection;
                // return connection item
                return connectionItem;
            } else {
                return nullptr;
            }
        }
        default: {
            // obtain tag property (only for improve code legibility)
            const auto& tagProperty = myAC->getTagProperty();
            // check if is an additional or a TAZ, and in other case return nullptr
            if (tagProperty.isAdditional() || tagProperty.isTAZ()) {
                // Obtain Additional
                GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(myAC->getTagProperty().getTag(), myAC->getID(), false);
                if (additional) {
                    // first check if additional has another additional as parent (to add it into root)
                    if (tagProperty.hasParent() && tagProperty.getParentTag() != SUMO_TAG_LANE) {
                        GNEAdditional* additionalParent = myFrameParent->getViewNet()->getNet()->retrieveAdditional(tagProperty.getParentTag(), additional->getAttribute(GNE_ATTR_PARENT));
                        // return additional item
                        return addListItem(additionalParent);
                    }
                    if (tagProperty.hasAttribute(SUMO_ATTR_EDGE)) {
                        // obtain edge parent
                        GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(additional->getAttribute(SUMO_ATTR_EDGE));
                        // insert junctions parents in tree (Pararell because a edge has always two Junctions)
                        addListItem(edge, nullptr, " origin");
                        FXTreeItem* junctionDestinyItem = addListItem(edge, nullptr, "", " destiny");
                        // return edge item
                        return addListItem(edge, junctionDestinyItem);
                    } 
                    if (tagProperty.hasAttribute(SUMO_ATTR_LANE)) {
                        // obtain lane parent
                        GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(additional->getAttribute(SUMO_ATTR_LANE));
                        // obtain edge parent
                        GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lane->getParentEdge().getID());
                        // insert junctions parents in tree (Pararell because a edge has always two Junctions)
                        addListItem(edge, nullptr, "", " origin");
                        FXTreeItem* junctionDestinyItem = addListItem(edge, nullptr, "", " destiny");
                        // Create edge item
                        FXTreeItem* edgeItem = addListItem(edge, junctionDestinyItem);
                        // return lane item
                        return addListItem(lane, edgeItem);
                    }
                    if (tagProperty.hasAttribute(SUMO_ATTR_LANES)) {
                        // obtain lane parent
                        std::vector<GNELane*> lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(myFrameParent->getViewNet()->getNet(), additional->getAttribute(SUMO_ATTR_LANES));
                        // check that there is at least one lane
                        if(lanes.size() == 0) {
                            // nothing to insert
                            return nullptr;
                        } else {
                            // obtain edge parent
                            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(lanes.front()->getParentEdge().getID());
                            // insert junctions parents in tree (Pararell because a edge has always two Junctions)
                            addListItem(edge, nullptr, " origin");
                            FXTreeItem* junctionDestinyItem = addListItem(edge, nullptr, "", " destiny");
                            // Create edge item
                            FXTreeItem* edgeItem = addListItem(edge, junctionDestinyItem);
                            // return lane item
                            return addListItem(lanes.front(), edgeItem);
                        }
                    }
                }
            }
            // check if is an demandElement or a TAZ, and in other case return nullptr
            if (tagProperty.isDemandElement()) {
                // Obtain DemandElement
                GNEDemandElement* demandElement = myFrameParent->getViewNet()->getNet()->retrieveDemandElement(myAC->getTagProperty().getTag(), myAC->getID(), false);
                if (demandElement) {
                    // declare auxiliar FXTreeItem, due a demand element can have multiple "roots" 
                    FXTreeItem* root = nullptr;
                    // check if demand element has as attribute "routes"
                    if (tagProperty.hasAttribute(SUMO_ATTR_ROUTE)) {
                        // retrieve route
                        GNEDemandElement* demandElementParent = myFrameParent->getViewNet()->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, demandElement->getAttribute(SUMO_ATTR_ROUTE));
                        // return route item
                        root = addListItem(demandElementParent);
                    }
                    // check if demand element has attribute "VTypes"
                    if (tagProperty.hasAttribute(SUMO_ATTR_TYPE)) {
                        // retrieve route
                        GNEDemandElement* demandElementParent = myFrameParent->getViewNet()->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, demandElement->getAttribute(SUMO_ATTR_TYPE));
                        // return route item
                        root = addListItem(demandElementParent);
                    }
                    // check if demand element has attribute "Edges" (note: Only first and last edge will be shown
                    if (tagProperty.hasAttribute(SUMO_ATTR_EDGES)) {
                        // obtain edge parent
                        std::vector<GNEEdge*> edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myFrameParent->getViewNet()->getNet(), demandElement->getAttribute(SUMO_ATTR_EDGES));
                        // insert junctions parents in tree (Pararell because a edge has always two Junctions)
                        if(edges.size() > 0) {
                            // check if we have more than one edge
                            if (edges.size() > 1) {
                                // insert first item
                                addListItem(edges.front());
                                // insert "spacer"
                                if (edges.size() > 2) {
                                    addListItem(nullptr, ("..." + toString((int)edges.size() - 2) + " edges...").c_str(), 0, false);
                                }
                            }
                            // return last edge item
                            root = addListItem(edges.back());
                        }
                    }
                    // return last inserted list item
                    return root;
                }
            }
        }
    }
    // there isn't parents
    return nullptr;
}


void
GNEFrame::ACHierarchy::showAttributeCarrierChilds(GNEAttributeCarrier* AC, FXTreeItem* itemParent) {
    // Switch gl type of ac
    switch (AC->getTagProperty().getTag()) {
        case SUMO_TAG_JUNCTION: {
            // retrieve junction
            GNEJunction* junction = myFrameParent->getViewNet()->getNet()->retrieveJunction(AC->getID(), false);
            if (junction) {
                // insert junction item
                FXTreeItem* junctionItem = addListItem(AC, itemParent);
                // insert edges
                for (auto i : junction->getGNEEdges()) {
                    showAttributeCarrierChilds(i, junctionItem);
                }
                // insert crossings
                for (auto i : junction->getGNECrossings()) {
                    showAttributeCarrierChilds(i, junctionItem);
                }
            }
            break;
        }
        case SUMO_TAG_EDGE: {
            // retrieve edge
            GNEEdge* edge = myFrameParent->getViewNet()->getNet()->retrieveEdge(AC->getID(), false);
            if (edge) {
                // insert edge item
                FXTreeItem* edgeItem = addListItem(AC, itemParent);
                // insert lanes
                for (int i = 0; i < (int)edge->getLanes().size(); i++) {
                    showAttributeCarrierChilds(edge->getLanes().at(i), edgeItem);
                }
                // insert additionals of edge
                for (auto i : edge->getAdditionalChilds()) {
                    showAttributeCarrierChilds(i, edgeItem);
                }
                // insert demand elements of edge
                for (auto i : edge->getDemandElementChilds()) {
                    showAttributeCarrierChilds(i, edgeItem);
                }
            }
            break;
        }
        case SUMO_TAG_LANE: {
            // retrieve lane
            GNELane* lane = myFrameParent->getViewNet()->getNet()->retrieveLane(AC->getID(), false);
            if (lane) {
                // insert lane item
                FXTreeItem* laneItem = addListItem(AC, itemParent);
                // insert additionals of lanes
                for (auto i : lane->getAdditionalChilds()) {
                    showAttributeCarrierChilds(i, laneItem);
                }
                // insert demand elements of lane
                for (auto i : lane->getDemandElementChilds()) {
                    showAttributeCarrierChilds(i, laneItem);
                }
                // insert incoming connections of lanes (by default isn't expanded)
                if (lane->getGNEIncomingConnections().size() > 0) {
                    std::vector<GNEConnection*> incomingLaneConnections = lane->getGNEIncomingConnections();
                    // insert intermediate list item
                    FXTreeItem* incomingConnections = addListItem(laneItem, "Incomings", incomingLaneConnections.front()->getIcon(), false);
                    // insert incoming connections
                    for (auto i : incomingLaneConnections) {
                        showAttributeCarrierChilds(i, incomingConnections);
                    }
                }
                // insert outcoming connections of lanes (by default isn't expanded)
                if (lane->getGNEOutcomingConnections().size() > 0) {
                    std::vector<GNEConnection*> outcomingLaneConnections = lane->getGNEOutcomingConnections();
                    // insert intermediate list item
                    FXTreeItem* outgoingConnections = addListItem(laneItem, "Outgoing", outcomingLaneConnections.front()->getIcon(), false);
                    // insert outcoming connections
                    for (auto i : outcomingLaneConnections) {
                        showAttributeCarrierChilds(i, outgoingConnections);
                    }
                }
            }
            break;
        }
        case SUMO_TAG_POI:
        case SUMO_TAG_POLY:
        case SUMO_TAG_CROSSING:
        case SUMO_TAG_CONNECTION: {
            // insert connection item
            addListItem(AC, itemParent);
            break;
        }
        default: {
            // check if is an additional or TAZ
            if (AC->getTagProperty().isAdditional() || AC->getTagProperty().isTAZ()) {
                // retrieve additional
                GNEAdditional* additional = myFrameParent->getViewNet()->getNet()->retrieveAdditional(AC->getTagProperty().getTag(), AC->getID(), false);
                if (additional) {
                    // insert additional item
                    FXTreeItem* additionalItem = addListItem(AC, itemParent);
                    // insert additionals childs
                    for (auto i : additional->getAdditionalChilds()) {
                        showAttributeCarrierChilds(i, additionalItem);
                    }
                }
            } else if (AC->getTagProperty().isDemandElement()) {
                // retrieve demandElement
                GNEDemandElement* demandElement = myFrameParent->getViewNet()->getNet()->retrieveDemandElement(AC->getTagProperty().getTag(), AC->getID(), false);
                if (demandElement) {
                    // insert demandElement item
                    FXTreeItem* demandElementItem = addListItem(AC, itemParent);
                    // insert demandElements childs
                    for (auto i : demandElement->getDemandElementChilds()) {
                        showAttributeCarrierChilds(i, demandElementItem);
                    }
                }
            }
            break;
        }
    }
}


FXTreeItem*
GNEFrame::ACHierarchy::addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent, std::string prefix, std::string sufix) {
    // insert item in Tree list
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, (prefix + AC->getHierarchyName() + sufix).c_str(), AC->getIcon(), AC->getIcon());
    // insert item in map
    myTreeItemToACMap[item] = AC;
    // by default item is expanded
    item->setExpanded(true);
    return item;
}


FXTreeItem* 
GNEFrame::ACHierarchy::addListItem(FXTreeItem* itemParent, const std::string &text, FXIcon* icon, bool expanded) {
    // insert item in Tree list
    FXTreeItem* item = myTreelist->insertItem(nullptr, itemParent, text.c_str(), icon, icon);
    // set exapnded
    item->setExpanded(expanded);
    return item;
}

// ---------------------------------------------------------------------------
// GNEFrame::GenericParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrame::GenericParametersEditor::GenericParametersEditor(GNEFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Generic parameters", GUIDesignGroupBoxFrame),
    myFrameParent(inspectorFrameParent),
    myAC(nullptr),
    myGenericParameters(nullptr) {
    // create empty vector with generic parameters
    myGenericParameters = new std::vector<std::pair<std::string, std::string> >;
    // create textfield and buttons
    myTextFieldGenericParameter = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEditGenericParameterButton = new FXButton(this, "Edit generic parameter", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrame::GenericParametersEditor::~GenericParametersEditor() {
    delete myGenericParameters;
}


void
GNEFrame::GenericParametersEditor::showGenericParametersEditor(GNEAttributeCarrier* AC) {
    if (AC != nullptr) {
        myAC = AC;
        myACs.clear();
        // obtain a copy of generic parameters of AC
        if (myAC) {
            *myGenericParameters = myAC->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrame::GenericParametersEditor::showGenericParametersEditor(std::vector<GNEAttributeCarrier*> ACs) {
    if (ACs.size() > 0) {
        myAC = nullptr;
        myACs = ACs;
        // check if generic parameters are different
        bool differentsGenericParameters = false;
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);
        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                differentsGenericParameters = true;
            }
        }
        // set generic Parameters editor
        if (differentsGenericParameters) {
            myGenericParameters->clear();
        } else {
            *myGenericParameters = myACs.front()->getGenericParameters();
        }
        // refresh GenericParametersEditor
        refreshGenericParametersEditor();
        // show groupbox
        show();
    }
}


void
GNEFrame::GenericParametersEditor::hideGenericParametersEditor() {
    myAC = nullptr;
    // hide groupbox
    hide();
}


void
GNEFrame::GenericParametersEditor::refreshGenericParametersEditor() {
    // update text field depending of AC
    if (myAC) {
        myTextFieldGenericParameter->setText(getGenericParametersStr().c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldGenericParameter if we're in demand mode and inspected AC isn't a demand element
        if ((myFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myAC->getTagProperty().isDemandElement()) {
            myTextFieldGenericParameter->disable();
            myEditGenericParameterButton->disable();
        } else {
            myTextFieldGenericParameter->enable();
            myEditGenericParameterButton->enable();
        }
    } else if (myACs.size() > 0) {
        // check if generic parameters of all inspected ACs are different
        std::string genericParameter = myACs.front()->getAttribute(GNE_ATTR_GENERIC);

        for (auto i : myACs) {
            if (genericParameter != i->getAttribute(GNE_ATTR_GENERIC)) {
                genericParameter = "different generic attributes";
            }
        }
        myTextFieldGenericParameter->setText(genericParameter.c_str());
        myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldGenericParameter if we're in demand mode and inspected AC isn't a demand element
        if ((myFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACs.front()->getTagProperty().isDemandElement()) {
            myTextFieldGenericParameter->disable();
            myEditGenericParameterButton->disable();
        } else {
            myTextFieldGenericParameter->enable();
            myEditGenericParameterButton->enable();
        }
    }
}


std::string
GNEFrame::GenericParametersEditor::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        result += i->first + "=" + i->second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


long
GNEFrame::GenericParametersEditor::onCmdEditGenericParameter(FXObject*, FXSelector, void*) {
    // edit generic parameters using dialog
    if (GNEGenericParameterDialog(myFrameParent->getViewNet(), myGenericParameters).execute()) {
        // set values edited in Parameter dialog in Edited AC
        if (myAC) {
            myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
        } else if (myACs.size() > 0) {
            myFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple generic attributes");
            for (auto i : myACs) {
                i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
            }
            myFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // Refresh parameter editor
        refreshGenericParametersEditor();
    }
    return 1;
}


long
GNEFrame::GenericParametersEditor::onCmdSetGenericParameter(FXObject*, FXSelector, void*) {
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer st(myTextFieldGenericParameter->getText().text(), "|", true);
    while (st.hasNext()) {
        parsedValues.push_back(st.next());
    }
    // first check if parsed generic parameters are valid
    for (auto i : parsedValues) {
        if (!GNEAttributeCarrier::isGenericParametersValid(i)) {
            WRITE_WARNING("Invalid format of Generic Parameter (" + i + ")");
            myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
            return 1;
        }
    }
    // now check if there is duplicated parameters
    std::sort(parsedValues.begin(), parsedValues.end());
    for (auto i = parsedValues.begin(); i != parsedValues.end(); i++) {
        if (((i + 1) != parsedValues.end())) {
            std::vector<std::string> firstKey, secondKey;
            StringTokenizer stKey1(*i, "=", true);
            StringTokenizer stKey2(*(i + 1), "=", true);
            //parse both keys
            while (stKey1.hasNext()) {
                firstKey.push_back(stKey1.next());
            }
            while (stKey2.hasNext()) {
                secondKey.push_back(stKey2.next());
            }
            // compare both keys and stop if are equal
            if ((firstKey.size() != 2) || (secondKey.size() != 2) || (firstKey.front() == secondKey.front())) {
                WRITE_WARNING("Generic Parameters wit the same key aren't allowed (" + (*i) + "," + * (i + 1) + ")");
                myTextFieldGenericParameter->setTextColor(FXRGB(255, 0, 0));
                return 1;
            }
        }
    }
    // parsed generic parameters ok, then set text field black and continue
    myTextFieldGenericParameter->setTextColor(FXRGB(0, 0, 0));
    myTextFieldGenericParameter->killFocus();
    // clear current existent generic parameters and set parsed generic parameters
    myGenericParameters->clear();
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            myGenericParameters->push_back(std::make_pair(parsedParameters.front(), parsedParameters.back()));
        }
    }
    // if we're editing generic attributes of an AttributeCarrier, set it
    if (myAC) {
        myAC->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
    } else if (myACs.size() > 0) {
        myFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple generic attributes");
        for (auto i : myACs) {
            i->setAttribute(GNE_ATTR_GENERIC, getGenericParametersStr(), myFrameParent->getViewNet()->getUndoList());
        }
        myFrameParent->getViewNet()->getUndoList()->p_end();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrame::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrame::DrawingShape::DrawingShape(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);

    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws shape boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates shape.\n"
            << "- 'Shift + Click'\n"
            << "  removes last created point.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed shape.";
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrame::DrawingShape::~DrawingShape() {}


void GNEFrame::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrame::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrame::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrame::DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->buildShape()) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->getViewNet()->update();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if shape cannot be created
        abortDrawing();
    }
}


void
GNEFrame::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->getViewNet()->update();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrame::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrame::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrame::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrame::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrame::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrame::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrame::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrame::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrame::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrame::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentLengthValid(true),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    FXHorizontalFrame* lengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLengthLabel = new FXLabel(lengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(lengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block movement label and checkBox (By default disabled)
    FXHorizontalFrame* blockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockMovementLabel = new FXLabel(blockMovement, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(blockMovement, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    FXHorizontalFrame* blockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBlockShapeLabel = new FXLabel(blockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(blockShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    // Create Frame for block close polygon and checkBox (By default disabled)
    FXHorizontalFrame* closePolygonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myClosePolygonLabel = new FXLabel(closePolygonFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(closePolygonFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBlockShapeCheckButton->setCheck(false);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEFrame::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrame::NeteditAttributes::showNeteditAttributesModul(const GNEAttributeCarrier::TagProperties& tagProperty) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if lenght text field has to be showed
    if (tagProperty.canMaskStartEndPos()) {
        myLengthLabel->show();
        myLengthTextField->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthLabel->hide();
        myLengthTextField->hide();
        myReferencePointMatchBox->hide();
    }
    // check if block movement check button has to be show
    if (tagProperty.canBlockMovement()) {
        myBlockMovementLabel->show();
        myBlockMovementCheckButton->show();
        showFrame = true;
    } else {
        myBlockMovementLabel->hide();
        myBlockMovementCheckButton->hide();
    }
    // check if block shape check button has to be show
    if (tagProperty.canBlockShape()) {
        myBlockShapeLabel->show();
        myBlockShapeCheckButton->show();
        showFrame = true;
    } else {
        myBlockShapeLabel->hide();
        myBlockShapeCheckButton->hide();
    }
    // check if close shape check button has to be show
    if (tagProperty.canCloseShape()) {
        myClosePolygonLabel->show();
        myCloseShapeCheckButton->show();
        showFrame = true;
    } else {
        myClosePolygonLabel->hide();
        myCloseShapeCheckButton->hide();
    }
    // if at least one element is show, show modul
    if (showFrame) {
        show();
    } else {
        hide();
    }
}


void
GNEFrame::NeteditAttributes::hideNeteditAttributesModul() {
    hide();
}


bool
GNEFrame::NeteditAttributes::getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane) const {
    // check if we need to obtain a start and end position over an edge
    if (myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getShape().nearest_offset_to_point2D(myFrameParent->myViewNet->snapToActiveGrid(myFrameParent->myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myActualAdditionalReferencePoint == GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                std::string errorMessage = "Current selected reference point isn't valid";
                myFrameParent->myViewNet->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain lenght
                double lenght = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                valuesMap[SUMO_ATTR_STARTPOS] = toString(setStartPosition(mousePositionOverLane, lenght));
                valuesMap[SUMO_ATTR_ENDPOS] = toString(setEndPosition(mousePositionOverLane, lenght));
            }
        } else {
            return false;
        }
    }
    // Save block value if element can be blocked
    if (myBlockMovementCheckButton->shown()) {
        if (myBlockMovementCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "0";
        }
    }
    // Save block shape value if shape's element can be blocked
    if (myBlockShapeCheckButton->shown()) {
        if (myBlockShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "0";
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        if (myCloseShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "0";
        }
    }
    // all ok, then return true to continue creating element
    return true;
}


long
GNEFrame::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBlockMovementCheckButton) {
        if (myBlockMovementCheckButton->getCheck()) {
            myBlockMovementCheckButton->setText("true");
        } else {
            myBlockMovementCheckButton->setText("false");
        }
    } else if (obj == myBlockShapeCheckButton) {
        if (myBlockShapeCheckButton->getCheck()) {
            myBlockShapeCheckButton->setText("true");
        } else {
            myBlockShapeCheckButton->setText("false");
        }
    } else if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText("true");
        } else {
            myCloseShapeCheckButton->setText("false");
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
                GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
            myCurrentLengthValid = true;
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
            myCurrentLengthValid = false;
        }
        // Update aditional frame
        update();
    } else if (obj == myReferencePointMatchBox) {
        // Cast actual reference point type
        if (myReferencePointMatchBox->getText() == "reference left") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference right") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference center") {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_INVALID;
            myLengthTextField->disable();
        }
    }

    return 1;
}


long
GNEFrame::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- Referece point: Mark the initial position of the additional element.\n"
            << "  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "  - Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "  - Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "  - Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes help dialog");
    return 1;
    /**********
    help from PolygonFrame
            << "- Block movement: If enabled, the created polygon element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Block shape: If enabled, the shape of created polygon element will be blocked. i.e. their geometry points\n"
            << "  cannot be edited be moved with the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Close shape: If enabled, the created polygon element will be closed. i.e. the last created geometry point\n"
            << "  will be connected with the first geometry point automatically. This option can be modified inspecting element.";

    ****************/
}


double
GNEFrame::NeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEFrame::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

// ---------------------------------------------------------------------------
// GNEFrame - methods
// ---------------------------------------------------------------------------

GNEFrame::GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel) :
    FXVerticalFrame(horizontalFrameParent, GUIDesignAuxiliarFrame),
    myViewNet(viewNet),
    myEdgeCandidateColor(RGBColor(0, 64, 0, 255)),
    myEdgeCandidateSelectedColor(RGBColor::GREEN) {

    // fill myPredefinedTagsMML (to avoid repeating this fill during every element creation)
    int i = 0;
    while (SUMOXMLDefinitions::attrs[i].key != SUMO_ATTR_NOTHING) {
        myPredefinedTagsMML[SUMOXMLDefinitions::attrs[i].key] = toString(SUMOXMLDefinitions::attrs[i].str);
        myPredefinedTagsMML[SUMOXMLDefinitions::attrs[i].key] = SUMOXMLDefinitions::attrs[i].str;
        i++;
    }

    // Create font only one time
    if(myFrameHeaderFont == nullptr) {
        myFrameHeaderFont = new FXFont(getApp(), "Arial", 14, FXFont::Bold);
    }

    // Create frame for header
    myHeaderFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create frame for left elements of header (By default unused)
    myHeaderLeftFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderLeftFrame->hide();

    // Create titel frame
    myFrameHeaderLabel = new FXLabel(myHeaderFrame, frameLabel.c_str(), nullptr, GUIDesignLabelFrameInformation);

    // Create frame for right elements of header (By default unused)
    myHeaderRightFrame = new FXHorizontalFrame(myHeaderFrame, GUIDesignAuxiliarHorizontalFrame);
    myHeaderRightFrame->hide();

    // Add separator
    new FXHorizontalSeparator(this, GUIDesignHorizontalSeparator);

    // Create frame for contents
    myScrollWindowsContents = new FXScrollWindow(this, GUIDesignContentsScrollWindow);

    // Create frame for contents
    myContentFrame = new FXVerticalFrame(myScrollWindowsContents, GUIDesignContentsFrame);

    // Set font of header
    myFrameHeaderLabel->setFont(myFrameHeaderFont);

    // Hide Frame
    FXVerticalFrame::hide();
}


GNEFrame::~GNEFrame() {
    // delete frame header only one time
    if(myFrameHeaderFont) {
        delete myFrameHeaderFont;
        myFrameHeaderFont = nullptr;
    }
}


void
GNEFrame::focusUpperElement() {
    myFrameHeaderLabel->setFocus();
}


void
GNEFrame::show() {
    // show scroll window
    FXVerticalFrame::show();
    // Show and update Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEFrame::hide() {
    // hide scroll window
    FXVerticalFrame::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEFrame::setFrameWidth(int newWidth) {
    setWidth(newWidth);
    myScrollWindowsContents->setWidth(newWidth);
}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


FXLabel*
GNEFrame::getFrameHeaderLabel() const {
    return myFrameHeaderLabel;
}


FXFont*
GNEFrame::getFrameHeaderFont() const {
    return myFrameHeaderFont;
}


bool
GNEFrame::buildShape() {
    // this function has to be reimplemente in all child frames that needs to draw a polygon (for example, GNEFrame or GNETAZFrame)
    return false;
}


void
GNEFrame::enableModuls(const GNEAttributeCarrier::TagProperties&) {
    // this function has to be reimplemente in all child frames that uses a ItemSelector modul
}


void
GNEFrame::disableModuls() {
    // this function has to be reimplemente in all child frames that uses a ItemSelector modul
}


void
GNEFrame::openACAttributesExtendedDialog()  {
    // this function has to be reimplemente in all child frames that uses a ACAttributes editor with extended attributes
}


void
GNEFrame::openHelpAttributesDialog(const GNEAttributeCarrier::TagProperties& tagProperties) const {
    FXDialogBox* attributesHelpDialog = new FXDialogBox(myScrollWindowsContents, ("Parameters of " + tagProperties.getTagStr()).c_str(), GUIDesignDialogBoxResizable, 0, 0, 0, 0, 10, 10, 10, 38, 4, 4);
    // Create FXTable
    FXTable* myTable = new FXTable(attributesHelpDialog, attributesHelpDialog, MID_TABLE, GUIDesignTableNotEditable);
    attributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEINSPECT));
    int sizeColumnDescription = 0;
    int sizeColumnDefinitions = 0;
    myTable->setVisibleRows((FXint)(tagProperties.getNumberOfAttributes()));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(tagProperties.getNumberOfAttributes()), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Attribute");
    myTable->setColumnText(1, "Description");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    // Iterate over vector of additional parameters
    int itemIndex = 0;
    for (auto i : tagProperties) {
        // Set attribute
        FXTableItem* attribute = new FXTableItem(toString(i.first).c_str());
        attribute->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 0, attribute);
        // Set description of element
        FXTableItem* type = new FXTableItem("");
        type->setText(i.second.getDescription().c_str());
        sizeColumnDescription = MAX2(sizeColumnDescription, (int)i.second.getDescription().size());
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(itemIndex, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(i.second.getDefinition().c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(itemIndex, 2, definition);
        sizeColumnDefinitions = MAX2(sizeColumnDefinitions, (int)i.second.getDefinition().size());
        itemIndex++;
    }
    // set header
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, sizeColumnDescription * 7);
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, sizeColumnDefinitions * 6);
    // Create horizontal separator
    new FXHorizontalSeparator(attributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(attributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), attributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening HelpAttributes dialog for tag '" + tagProperties.getTagStr() + "' showing " + toString(tagProperties.getNumberOfAttributes()) + " attributes");
    // create Dialog
    attributesHelpDialog->create();
    // show in the given position
    attributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(attributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing HelpAttributes dialog for tag '" + tagProperties.getTagStr() + "'");
}


const RGBColor&
GNEFrame::getEdgeCandidateColor() const {
    return myEdgeCandidateColor;
}


const RGBColor&
GNEFrame::getEdgeCandidateSelectedColor() const {
    return myEdgeCandidateSelectedColor;
}


const std::map<int, std::string> &
GNEFrame::getPredefinedTagsMML() const {
    return myPredefinedTagsMML;
}

/****************************************************************************/

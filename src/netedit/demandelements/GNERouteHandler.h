/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/
#ifndef GNERouteHandler_h
#define GNERouteHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/vehicle/SUMORouteHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNETAZ;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNERouteHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNERouteHandler : public SUMORouteHandler {
public:
    /// @brief Constructor
    GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements = true);

    /// @brief Destructor
    ~GNERouteHandler();
    
    /// @brief build vehicle
    static void buildVehicle(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* vehicleParameters);

    /// @brief build flow
    static void buildFlow(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* flowParameters);

    /// @brief build trip
    static void buildTrip(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter* tripParameters, GNEEdge* from, GNEEdge* to);

protected:
    /// @brief opens a type distribution for reading
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs);

    /// @brief closes (ends) the building of a distribution
    void closeVehicleTypeDistribution();

    /// @brief opens a route for reading
    void openRoute(const SUMOSAXAttributes& attrs);

    /**closes (ends) the building of a route.
     * Afterwards no edges may be added to it;
     * this method may throw exceptions when
     * a) the route is empty or
     * b) another route with the same id already exists 
     */
    void closeRoute(const bool mayBeDisconnected = false);

    /// @brief opens a route distribution for reading
    void openRouteDistribution(const SUMOSAXAttributes& attrs);

    /// @brief closes (ends) the building of a distribution
    void closeRouteDistribution();

    /// @brief Ends the processing of a vehicle
    void closeVehicle();

    /// @brief Ends the processing of a vehicle Type
    void closeVType();

    /// @brief Ends the processing of a person
    void closePerson();

    /// @brief Ends the processing of a container
    void closeContainer();

    /// @brief Ends the processing of a flow
    void closeFlow();

    /// @brief Processing of a stop
    void addStop(const SUMOSAXAttributes& attrs);

    /// @brief add a routing request for a walking or intermodal person
    void addPersonTrip(const SUMOSAXAttributes& attrs);

    /// @brief add a fully specified walk
    void addWalk(const SUMOSAXAttributes& attrs);

private:
    /// @brief flag used for parsing route attribute
    bool myAbort;

    /// @brief string for saving parsed Route ID
    std::string myRouteID;

    /// @brief string for saving parsed edges
    std::string myEdgeIDs;

    /// @brief string for saving parsed route colors
    RGBColor myRouteColor;

    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;
};


#endif

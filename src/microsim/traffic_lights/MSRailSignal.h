/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRailSignal.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Jan 2015
/// @version $Id$
///
// A rail signal logic
/****************************************************************************/
#ifndef MSRailSignal_h
#define MSRailSignal_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

//#include <utility>
//#include <vector>
//#include <bitset>
//#include <map>
//#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
//#include "MSSimpleTrafficLightLogic.h"
#include "MSPhaseDefinition.h"
#include "MSTLLogicControl.h"
//#include <microsim/output/MSInductLoop.h>



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRailSignal
 * @brief A signal for rails
 */
class MSRailSignal : public MSTrafficLightLogic {
public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] programID This tls' sub-id (program id)
     * @param[in] parameters This tls' parameters
     */
    MSRailSignal(MSTLLogicControl& tlcontrol,
                 const std::string& id, const std::string& programID,
                 const std::map<std::string, std::string>& parameters);


    /** @brief Initialises the rail signal with information about adjacent rail signals
     * @param[in] nb The detector builder
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder& nb);


    /// @brief Destructor
    ~MSRailSignal();

    /** @brief Adds a link on building
     * @param[in] link The controlled link
     * @param[in] lane The lane this link starts at
     * @param[in] pos The link's index (signal group) within this program
     */
    void addLink(MSLink* link, MSLane* lane, int pos);

    /// @name Handling of controlled links
    /// @{

    /** @brief Applies information about controlled links and lanes from the given logic
     * @param[in] logic The logic to use the information about controlled links/lanes from
     * @see MSTrafficLightLogic::adaptLinkInformationFrom
     */
    void adaptLinkInformationFrom(const MSTrafficLightLogic& logic);
    /// @}


    /// @name Switching and setting current rows
    /// @{


    /** @brief returns the state of the signal that actually required
     *
     * Returns the state of the rail signal that is actually required based on the
     *  occupation of the adjoining blocks.
     *
     * @return The state actually required for this signal.
     */
    bool conflictLaneOccupied(int index) const;
    bool hasLinkConflict(int index) const;

    /// @brief updates the current phase of the signal
    void updateCurrentPhase();

    /** @brief Switches to the next phase
    * @return The time of the next switch (always the next step)
    * @see MSTrafficLightLogic::trySwitch
    */
    SUMOTime trySwitch();

    /// @}


    /// @name Static Information Retrieval
    /// @{

    /** @brief Returns the number of phases
     * @return The number of this tls program's phases (always zero)
     * @see MSTrafficLightLogic::getPhaseNumber
     */
    int getPhaseNumber() const;


    /** @brief Returns the phases of this tls program
     * @return The phases of this tls program
     * @see MSTrafficLightLogic::getPhases
     */
    const Phases& getPhases() const;

    /** @brief Returns the definition of the phase from the given position within the plan
    *
    * Returns the current phase as there does not exist a plan of the phases.
    *
    * @param[in] givenstep The index of the phase within the plan
    * @return The definition of the phase at the given position
    * @see MSTrafficLightLogic::getPhase
    */
    const MSPhaseDefinition& getPhase(int givenstep) const;
    /// @}


    /// @name Dynamic Information Retrieval
    /// @{

    /** @brief Returns the current index within the program
     * @return The index of the current phase within the tls (here, always zero will be returned)
     * @see MSTrafficLightLogic::getCurrentPhaseIndex
     */
    int getCurrentPhaseIndex() const;


    /** @brief Returns the definition of the current phase
    * @return The current phase
    */
    const MSPhaseDefinition& getCurrentPhaseDef() const;
    /// @}


    /// @name Conversion between time and phase
    /// @{

    /** @brief Returns the index of the logic at the given simulation step
     * @return The (estimated) index of the tls at the given simulation time step (here, always zero will be returned)
     * @see MSTrafficLightLogic::getPhaseIndexAtTime
     */
    SUMOTime getPhaseIndexAtTime(SUMOTime simStep) const;


    /** @brief Returns the position (start of a phase during a cycle) from of a given step
     * @param[in] index The index of the phase to return the begin of
     * @return The begin time of the phase (here, always zero will be returned)
     * @see MSTrafficLightLogic::getOffsetFromIndex
     */
    SUMOTime getOffsetFromIndex(int index) const;


    /** @brief Returns the step (the phasenumber) of a given position of the cycle
    * @param[in] offset The offset (time) for which the according phase shall be returned
    * @return The according phase (here, always zero will be returned)
    * @see MSTrafficLightLogic::getIndexFromOffset
    */
    int getIndexFromOffset(SUMOTime offset) const;
    /// @}


    /// @name Changing phases and phase durations
    /// @{

    /** @brief Changes the current phase and her duration
     * @param[in] tlcontrol The responsible traffic lights control
     * @param[in] simStep The current simulation step
     * @param[in] step Index of the phase to use
     * @param[in] stepDuration The left duration of the phase
     * @see MSTrafficLightLogic::changeStepAndDuration
     */
    void changeStepAndDuration(MSTLLogicControl& tlcontrol, SUMOTime simStep, int step, SUMOTime stepDuration) {
        UNUSED_PARAMETER(tlcontrol);
        UNUSED_PARAMETER(simStep);
        UNUSED_PARAMETER(step);
        UNUSED_PARAMETER(stepDuration);
    }
    /// @}

protected:

    /*  The conflict lanes for each link index
     *  If any of them is occupied the signal must be red */
    std::vector<std::vector<MSLane*> > myConflictLanes;

    /* The conflict links for each link index
     * Conflict resolution must be performed if vehicles are approaching the
     * current link and any of the conflict links */
    std::vector<std::vector<MSLink*> > myConflictLinks;

    /* Conflict lanes for each conflictLink of the current link
     * linkIndex -> conflictLinkIndex -> lanes
     * They must only be checked if the approaching vehicle's route enters the
     * bidirectional block beyond that conflichLink */
    std::vector<std::vector<std::vector<MSLane*> > > myRouteConflictLanes;

    /* Conflict links for each conflictLink of the current link
     * linkIndex -> conflictLinkIndex -> links
     * They must only be checked if the approaching vehicle's route enters the
     * bidirectional block beyond that conflictLink, then they are treated like
     * regular conflictLinks */
    std::vector<std::vector<std::vector<MSLink*> > > myRouteConflictLinks;

    /// Storage for rerouting times to avoid superfluous calls
    mutable std::vector<std::pair<SUMOVehicle*, SUMOTime> > myLastRerouteAttempt;

    typedef std::set<MSLane*, ComparatorNumericalIdLess> LaneSet;

    /// @brief collect conflict lanes in step 1
    void collectForwardBlock(MSLane* toLane, double length, std::vector<MSLane*>& forwardBlock, LaneSet& visited);

    /// @brief collect bidirectional conflict lanes in step 2
    void collectBidiBlock(MSLane* toLane, double length, bool foundSwitch, std::vector<MSLane*>& bidiBlock, LaneSet& visited);

    /// @brief collect additional conflict lanes and conflict links in step 3
    void collectConflictLinks(MSLane* toLane, double length, 
            std::vector<MSLane*>& backwardBlock,
            std::vector<MSLink*>& conflictLinks,
            LaneSet& visited);

    /// @brief whether there are other controlled links from the same incoming lane
    static bool hasAlternativeTrack(MSLink* link);

    /// @brief whether there is an alternative track between the end of the forwardBlock and cLink
    static bool hasAlternativeTrackBetween(const std::vector<MSLane*>& forwardBlock, MSLink* cLink);

    /// @brief return logicID_linkIndex
    static std::string getTLLinkID(MSLink* link);

protected:

    /** @brief The list of phases this logic uses
    *
    *   This vector is always empty and only constructed because the superclass MSTrafficLightLogic requires it.
    */
    Phases myPhases;

    /// @brief The current phase
    MSPhaseDefinition myCurrentPhase;

    static int myNumWarnings;

};


#endif

/****************************************************************************/


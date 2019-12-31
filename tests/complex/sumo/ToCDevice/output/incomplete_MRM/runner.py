#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Leonhard Luecken
# @date    2009-03-26

from __future__ import absolute_import
from __future__ import print_function

from runnerlib import get_options, printToCParams, requestToC  # sys.path modification is done there
import traci
from sumolib import checkBinary

ToC_vehicle = "ToC_veh"
timeTillMRM = 21


def run():
    """execute the TraCI control loop"""
    step = 0
    while step < 1000:
        traci.simulationStep()
        if step == 200:
            requestToC(ToC_vehicle, timeTillMRM)
            t = traci.simulation.getTime()
            print("Requested ToC of veh0 at t=%s (until t=%s)" % (t, t + timeTillMRM))
        printToCParams(ToC_vehicle, True)
        step += 1


# this is the main entry point of this script
if __name__ == "__main__":
    options = get_options()

    # this script has been called from the command line. It will start sumo as a
    # server, then connect and run
    if options.nogui:
        sumoBinary = checkBinary('sumo')
    else:
        sumoBinary = checkBinary('sumo-gui')

    # this is the normal way of using traci. sumo is started as a
    # subprocess and then the python script connects and runs
    traci.start([sumoBinary, "-n", "input_net.net.xml", "-r", "input_routes.rou.xml", "-a",
                 "input_additional.add.xml", "--fcd-output", "fcd.xml",
                 "--no-step-log", "true",
                 "--default.speeddev", "0"])

    # Wait until the vehicle enters
    while ToC_vehicle not in traci.vehicle.getIDList():
        traci.simulationStep()

    printToCParams(ToC_vehicle)

    run()

    traci.close()

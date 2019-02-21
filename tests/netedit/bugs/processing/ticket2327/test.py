#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# rebuild network
netedit.rebuildNetwork()

# go to select mode
netedit.selectMode()

# select node 1
netedit.leftClick(referencePosition, 50, 50)

# select node 2
netedit.leftClick(referencePosition, 160, 50)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 3
netedit.leftClick(referencePosition, 265, 50)

# select node 4
netedit.leftClick(referencePosition, 380, 50)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 5
netedit.leftClick(referencePosition, 55, 178)

# select node 6
netedit.leftClick(referencePosition, 150, 178)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# select node 8
netedit.leftClick(referencePosition, 340, 170)

# select node 9
netedit.leftClick(referencePosition, 450, 170)

# select node 10
netedit.leftClick(referencePosition, 550, 170)

# inspect node 11
netedit.leftClick(referencePosition, 340, 275)

# inspect node 12
netedit.leftClick(referencePosition, 450, 275)

# inspect node 13
netedit.leftClick(referencePosition, 550, 275)

# inspect node 14
netedit.leftClick(referencePosition, 340, 390)

# inspect node 15
netedit.leftClick(referencePosition, 450, 390)

# inspect node 16
netedit.leftClick(referencePosition, 550, 390)

# join selected junctions
netedit.joinSelectedJunctions()

# rebuild network
netedit.rebuildNetwork()

# Undo joining
netedit.undo(referencePosition, 4)

# rebuild network
netedit.rebuildNetwork()

# redo joining
netedit.redo(referencePosition, 4)

# rebuild network
netedit.rebuildNetwork()

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)

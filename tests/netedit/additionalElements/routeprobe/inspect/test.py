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
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot, ['--gui-testing-debug-gl'])

# go to additional mode
netedit.additionalMode()

# select routeProbe
netedit.changeAdditional("routeProbe")

# create routeProbe
netedit.leftClick(referencePosition, 250, 120)

# create another routeProbe (for duplicated ID)
netedit.leftClick(referencePosition, 250, 220)

# go to inspect mode
netedit.inspectMode()

# inspect first routeProbe
netedit.leftClick(referencePosition, 310, 200)

# Change parameter 0 with a non valid value (Duplicated ID)
netedit.modifyAttribute(0, "routeProbe_3_1", False)

# Change parameter 0 with a non valid value (Invalid ID)
netedit.modifyAttribute(0, "Id with spaces", False)

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "correctID", False)

# Change parameter 1 with a non valid value (dummy edge)
netedit.modifyAttribute(1, "dummyEdge", False)

# Change parameter 1 with a valid value (different edge)
netedit.modifyAttribute(1, "gneE0", False)

# Change parameter 2 with a non valid value (dummy)
netedit.modifyAttribute(2, "dummy", False)

# Change parameter 2 with a non valid value (negative)
netedit.modifyAttribute(2, "-50", False)

# Change parameter 2 with a valid value
netedit.modifyAttribute(2, "150", False)

# Change parameter 3 with an non valid value
netedit.modifyAttribute(3, "%%%;:..&&%$%$", False)

# Change parameter 3 with a duplicated value
netedit.modifyAttribute(3, "customName", False)

# Change parameter 4 with a non valid value (invalid characters)
netedit.modifyAttribute(4, "$$$$$$$$", False)

# Change parameter 4 with a valid value
netedit.modifyAttribute(4, "OwnOutput.txt", False)

# Change parameter 5 with a non valid value (dummy)
netedit.modifyAttribute(5, "dummy", False)

# Change parameter 5 with a non valid value (negative)
netedit.modifyAttribute(5, "-5", False)

# Change parameter 5 with a valid value
netedit.modifyAttribute(5, "20", False)

# Check undos and redos
netedit.undo(referencePosition, 12)
netedit.redo(referencePosition, 12)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)

#
# Copyright 2014 SRC Computers, LLC  All Rights Reserved.
#
#       Manufactured in the United States of America.
#
# SRC Computers, LLC
# 4240 N Nevada Avenue
# Colorado Springs, CO 80907
# (v) (719) 262-0213
# (f) (719) 262-0223
#
# No permission has been granted to distribute this software
# without the express permission of SRC Computers, LLC
#
# This program is distributed WITHOUT ANY WARRANTY OF ANY KIND.
#

# ------------------------------------------
# User defines FILES, MAPFILES, and BIN here
# ------------------------------------------
FILES           = main.cpp
MAPFILES        = tst10100.mcpp
BIN             = tst10100

# --------------------------------------
# Supply additional tool parameters here
# --------------------------------------
CPPFLAGS        +=              # C Preprocessor flags
CXXFLAGS        +=              # C++ compiler flags
MCXXFLAGS       += --log        # Carte++ compiler flags
LDFLAGS         +=              # Loader flags
INCLUDES        +=              # Include file search path: -Ipath
LOADLIBES       +=              # Library search path: -Lpath
LDLIBS          +=              # Libraries: -lmylib

# -----------------------
# Default tool selections
# -----------------------
TEST_SUITE_MCXX ?= carte++
TEST_SUITE_CXX  ?= clang++
TEST_SUITE_LD   ?= $(TEST_SUITE_CXX)
MCXX             = $(TEST_SUITE_MCXX)
CXX              = $(TEST_SUITE_CXX)
LD               = $(TEST_SUITE_LD)

# --------------------------
# Default MAP type selection
# --------------------------
export MAPTARGET ?= map_m

# --------------------------
# Default make target
# --------------------------
default:        all

# --------------------------
# Test output clean up
# --------------------------
test_clean:
	rm -f res res_correct report.xml

clean:  test_clean

# -----------------------------------
# No modifications are required below
# -----------------------------------
APPRULES        ?= $(CARTE)/lib/AppRules.make
include $(APPRULES)

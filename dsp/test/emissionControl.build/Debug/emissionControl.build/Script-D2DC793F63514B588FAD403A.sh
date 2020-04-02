#!/bin/sh
make -C /Users/jkilgore/Projects/EmissionControlPort/dsp/test -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/CMakeScripts/emissionControl_postBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all

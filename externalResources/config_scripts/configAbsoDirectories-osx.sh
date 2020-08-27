#!/bin/bash

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/" ]; then
  mkdir "$HOME/Library/Application Support/EmissionControl2/"
fi

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/presets" ]; then
  mkdir "$HOME/Library/Application Support/EmissionControl2/presets"
fi

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/midi_presets" ]; then
  mkdir "$HOME/Library/Application Support/EmissionControl2/midi_presets"
fi

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/soundOutput" ]; then
  mkdir "$HOME/Library/Application Support/EmissionControl2/soundOutput"
fi

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/samples" ]; then
  cp -r "samples" "$HOME/Library/Application Support/EmissionControl2/"
fi

if [ ! -d "$HOME/Library/Application Support/EmissionControl2/configs" ]; then
  mkdir "$HOME/Library/Application Support/EmissionControl2/configs"
fi

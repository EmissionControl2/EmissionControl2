#!/bin/bash

if [ ! -d "$HOME/Music/EmissionControl2/" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/"
fi

if [ ! -d "$HOME/Music/EmissionControl2/presets" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/presets"
fi

if [ ! -d "$HOME/Music/EmissionControl2/midi_presets" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/midi_presets"
fi

if [ ! -d "$HOME/Music/EmissionControl2/soundOutput" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/soundOutput"
fi

if [ ! -d "$HOME/Music/EmissionControl2/samples" ]; then
  cp -r "samples" "$HOME/Music/EmissionControl2/"
fi

if [ ! -d "$HOME/Music/EmissionControl2/configs" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/configs"
fi

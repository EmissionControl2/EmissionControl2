#!/bin/base

if [ ! -d "$HOME/.config/EmissionControl2/presets" ]; then
  mkdir -p "$HOME/.config/EmissionControl2/presets"
fi

if [ ! -d "$HOME/Music/EmissionControl2/soundOutput" ]; then
  mkdir -p "$HOME/Music/EmissionControl2/soundOutput"
fi

if [ ! -d "$HOME/Music/EmissionControl2/samples" ]; then
  cp -r "/usr/local/share/EmissionControl2/samples" "$HOME/Music/EmissionControl2"
fi

if [ ! -d "$HOME/.config/EmissionControl2/configs" ]; then
  mkdir -p "$HOME/.config/EmissionControl2/configs"
fi

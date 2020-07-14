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

<<<<<<< HEAD
if [ ! -d "$HOME/.config/EmissionControl2/configs" ]; then
  mkdir -p "$HOME/.config/EmissionControl2/configs"
=======
if [ ! -d "$HOME/opt/EmissionControl20/soundOutput" ]; then
  mkdir "$HOME/opt/EmissionControl20/soundOutput"
fi

if [ ! -d "$HOME/opt/EmissionControl20/samples" ]; then
  cp -r "Resources/samples" "$HOME/opt/EmissionControl20/"
fi

if [ ! -d "$HOME/opt/EmissionControl20/configs" ]; then
  mkdir "$HOME/opt/EmissionControl20/configs"
>>>>>>> 4b025467834fa32927e87a0b79c525814a539413
fi

#!/bin/base

if [ ! -d "$HOME/opt/" ]; then
  mkdir "$HOME/opt/"
fi

if [ ! -d "$HOME/opt/EmissionControl20/" ]; then
  mkdir "$HOME/opt/EmissionControl20/"
fi

if [ ! -d "$HOME/opt/EmissionControl20/presets" ]; then
  mkdir "$HOME/opt/EmissionControl20/presets"
fi

if [ ! -d "$HOME/opt/EmissionControl20/soundOutput" ]; then
  mkdir "$HOME/opt/EmissionControl20/soundOutput"
fi

if [ ! -d "$HOME/opt/EmissionControl20/samples" ]; then
  cp -r "Resources/samples" "$HOME/opt/EmissionControl20/"
fi

if [ ! -d "$HOME/opt/EmissionControl20/configs" ]; then
  mkdir "$HOME/opt/EmissionControl20/configs"
fi

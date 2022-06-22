#! /bin/bash 

# Navigate to ec2 config directory
if [[ $XDG_CONFIG_HOME != '' ]]
then
	cd $XDG_CONFIG_HOME/EmissionControl2
else
	cd ~/.config/EmissionControl2
fi

# rename and move midi presets
for file in ./midi_presets*.json; do
 mv "$file" "./midi_presets/${file#*midi_presets}"
done

# rename and move sample presets
for file in ./sample_presets*.json; do
 mv "$file" "./sample_presets/${file#*sample_presets}"
done

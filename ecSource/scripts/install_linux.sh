#!/bin/bash

# Installs resources to your system! Needs to be run with sudo.
# The only files that absolutely need to be installed are samples, as this path is hardcoded in EC2. (/usr/share/emissioncontrol2)
# The rest are just good to have installed too.

if [ $(whoami) != root ]; then
    echo Please run this script as root or using sudo
    exit
fi

result=${PWD##*/}
if [ $result == "scripts" ]; then
    cd ..
elif [ $result == "EmissionControl2" ]; then
    cd ecSource/
fi

# install binary
if [ ! -f "/usr/bin/emissioncontrol2" ]; then
    echo "Installing binary to /usr/bin..."
    cp bin/EmissionControl2 "/usr/bin/emissioncontrol2"
else
    echo "binary already installed..."
fi

# create resource folder
if [ ! -d "/usr/share/emissioncontrol2" ]; then
    echo "creating /usr/share/emissioncontrol2..."
    mkdir /usr/share/emissioncontrol2
else
    echo "/usr/share/emissioncontrol2 already exists..."
fi

#install samples.
if [ ! -d "/usr/share/emissioncontrol2/samples" ]; then
    echo "Installing samples to /usr/share/emissioncontrol2/samples..."
    cp -r "bin/Resources/samples" "/usr/share/emissioncontrol2/"
else
    echo "samples already installed..."
fi

# install icon
if [ ! -f "/usr/share/pixmaps/emissioncontrol2.png" ]; then
    echo "Installing icon to /usr/share/pixmaps..."
    cp "../externalResources/icon/EmissionControl2.png" "/usr/share/pixmaps/emissioncontrol2.png"
else
    echo "icon already installed..."
fi

# install .desktop file for launcher compatibility etc.
if [ ! -f "/usr/share/applications/EmissionControl2.desktop" ]; then
    echo "Installing .desktop file to /usr/share/applications..."
    echo "[Desktop Entry]
Name=EmissionControl2
Comment=Launch EmissionControl2
Exec=emissioncontrol2
Icon=/usr/share/pixmaps/emissioncontrol2.png
Terminal=false
Type=Application
Categories=Audio;Music;
Name[en_US]=EmissionControl2" >>"/usr/share/applications/EmissionControl2.desktop"
else
    echo ".desktop file already installed..."
fi

#!/bin/bash

# Uninstall EC2 and resources from your system! Needs to be run with sudo.

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

# uninstall binary
if [ -f "/usr/bin/emissioncontrol2" ]; then
    echo "Removing binary from /usr/bin..."
    rm "/usr/bin/emissioncontrol2"
else
    echo "no binary installed..."
fi

# remove resource folder
if [ -d "/usr/share/emissioncontrol2" ]; then
    echo "Removing /usr/share/emissioncontrol2..."
    rm -rf "/usr/share/emissioncontrol2/"
else
    echo "/usr/share/emissioncontrol2 not found..."
fi

# uninstall icon
if [ -f "/usr/share/pixmaps/emissioncontrol2.png" ]; then
    echo "Removing icon to /usr/share/pixmaps..."
    rm "/usr/share/pixmaps/emissioncontrol2.png"
else
    echo "no icon installed.."
fi

# uninstall .desktop file for launcher compatibility etc.
if [ -f "/usr/share/applications/EmissionControl2.desktop" ]; then
    echo "Removing .desktop file to /usr/share/applications..."
    rm /usr/share/applications/EmissionControl2.desktop
else
    echo "no .desktop file installed..."
fi

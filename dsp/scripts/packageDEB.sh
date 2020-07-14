#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Error: No version number provided. Version number required in format <MajorVersion>.<MinorVersion>"
    exit 1
fi

# make sure we're in dsp

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "$Dir"


VERSION="$1""-1"
RELEASENAME="EmissionControl2_""$VERSION""_amd64"
BUILDLOCATION=$(cd ../deployment && pwd)

echo "Packaging $RELEASENAME..."

# make directory structure

BUILDDIR="../deployment/$RELEASENAME"

mkdir -p "$BUILDDIR/DEBIAN"
mkdir -p "$BUILDDIR/usr/local/bin"
mkdir -p "$BUILDDIR/usr/local/share/applications"
mkdir -p "$BUILDDIR/usr/local/share/EmissionControl2/"
mkdir -p "$BUILDDIR/usr/share/pixmaps"
mkdir -p "$BUILDDIR/etc/EmissionControl2"
mkdir -p "$BUILDDIR/usr/local/share/fonts/EmissionControl2"


# copy necessary files over

cd "$Dir"


cp -r "bin/Resources/samples" "$BUILDDIR/usr/local/share/EmissionControl2/"

cp -r "bin/Resources/config_scripts" "$BUILDDIR/usr/local/share/EmissionControl2/"

cp "bin/EmissionControl2" "$BUILDDIR/usr/local/bin"

cp "../externalResources/EmissionControl2.png" "$BUILDDIR/usr/share/pixmaps"

cp "bin/Resources/Fonts/Roboto-Medium.ttf" "$BUILDDIR/usr/local/share/fonts/EmissionControl2"

# Make desktop file

rm "$BUILDDIR/usr/local/share/applications/Emission Control 2.desktop"
echo "[Desktop Entry]
Name=Emission Control 2
Comment=Launch Emission Control 2
Exec=EmissionControl2
Icon=EmissionControl2.png
Terminal=false
Type=Application
Categories=Music;Synthesis;Sound;
Name[en_US]=Emission Control 2" >> "$BUILDDIR/usr/local/share/applications/Emission Control 2.desktop"

# make Debian control file 
rm "$BUILDDIR/DEBIAN/control"
echo "Package: EmissionControl2
Version: $VERSION
Maintainer: Jack Kilgore <jkilgore@ucsb.edu>, Rodney DuPlessis <rodney@rodneyduplessis.com>
Architecture: amd64
Depends: libsndfile1
Homepage: https://www.curtisroads.net
Description: Granular Synthesis" >> "$BUILDDIR/DEBIAN/control"

echo "Packaging .deb at $BUILDLOCATION..."

# package .deb
cd ../deployment
exec dpkg -b "$RELEASENAME" "$RELEASENAME.deb"

echo "Packaging Complete!"

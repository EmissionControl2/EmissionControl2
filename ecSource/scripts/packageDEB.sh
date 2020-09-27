#!/bin/bash
result=${PWD##*/}
if [ $result == "scripts" ]; then
  cd ..
elif [ $result == "EmissionControl2" ]; then
  cd ecSource/
fi

mkdir -p "../deployment/Linux/"

if [ $# -eq 0 ]; then
  echo "Error: No version number provided. Version number required in format <MajorVersion>.<MinorVersion>"
  exit 1
fi

# make sure we're in ecSource

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

cd "$Dir"

VERSION="$1""-1"
RELEASENAME="EmissionControl2_""$VERSION""_amd64"
BUILDLOCATION=$(cd ../deployment && pwd)

echo "Packaging $RELEASENAME..."

# make directory structure

BUILDDIR="../deployment/Linux/$RELEASENAME"

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

cp "bin/EmissionControl2" "$BUILDDIR/usr/local/bin"

cp "../externalResources/Icon/EmissionControl2.png" "$BUILDDIR/usr/share/pixmaps"

cp "bin/Resources/Fonts/Roboto-Medium.ttf" "$BUILDDIR/usr/local/share/fonts/EmissionControl2"

cp "bin/Resources/Fonts/ferrari.ttf" "$BUILDDIR/usr/local/share/fonts/EmissionControl2"

# Make desktop file

rm "$BUILDDIR/usr/local/share/applications/Emission Control 2.desktop"
echo "[Desktop Entry]
Name=Emission Control 2
Comment=Launch Emission Control 2
Exec=EmissionControl2
Icon=EmissionControl2.png
Terminal=false
Type=Application
Categories=Music;Synthesis;Sound;Granular;
Name[en_US]=Emission Control 2" >>"$BUILDDIR/usr/local/share/applications/Emission Control 2.desktop"

# make Debian control file
rm "$BUILDDIR/DEBIAN/control"
echo "Package: EmissionControl2
Version: $VERSION
Maintainer: Jack Kilgore <jkilgore@ucsb.edu>, Rodney DuPlessis <rodney@rodneyduplessis.com>
Architecture: amd64
Depends: libsndfile1
Homepage: https://github.com/jackkilgore/EmissionControl2
Description: An advanced granular synthesizer designed by Curtis Roads, Jack Kilgore, and Rodney DuPlessis (2020). Based on Emission Control by Curtis Roads and David Thall (2004-2008)." >>"$BUILDDIR/DEBIAN/control"

echo "Packaging .deb at $BUILDLOCATION..."

# package .deb
cd ../deployment/Linux
exec dpkg -b "$RELEASENAME" "$RELEASENAME.deb" &

cp ../../docs/EmissionControl2-Manual.pdf .

zip $RELEASENAME.zip "$RELEASENAME.deb" EmissionControl2-Manual.pdf

rm -rf "$RELEASENAME" "$RELEASENAME.deb" EmissionControl2-Manual.pdf

echo "Packaging Complete!"

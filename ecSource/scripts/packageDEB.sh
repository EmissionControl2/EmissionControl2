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
RELEASENAME="emissioncontrol2-""$VERSION""-amd64"
BUILDLOCATION=$(cd ../deployment && pwd)

echo "Packaging $RELEASENAME..."
echo "Don't forget to compile this on an older version of Debian for compatibility..."
echo "Also compile a version with just PulseAudio, for Pipewire compatibility."

# make directory structure

BUILDDIR="../deployment/Linux/$RELEASENAME"

mkdir -p "$BUILDDIR/DEBIAN"
mkdir -p "$BUILDDIR/usr/bin"
mkdir -p "$BUILDDIR/usr/share/doc/emissioncontrol2/"
mkdir -p "$BUILDDIR/usr/share/applications/"
mkdir -p "$BUILDDIR/usr/share/emissioncontrol2/"
mkdir -p "$BUILDDIR/usr/share/pixmaps/"

# copy necessary files over

cd "$Dir"

cp -r "bin/Resources/." "$BUILDDIR/usr/share/emissioncontrol2/"

objcopy --strip-debug --strip-unneeded bin/EmissionControl2 "$BUILDDIR/usr/bin/emissioncontrol2"

cp "../externalResources/icon/EmissionControl2.png" "$BUILDDIR/usr/share/pixmaps/emissioncontrol2.png"

cp ../docs/EmissionControl2-Manual.pdf "$BUILDDIR/usr/share/doc/emissioncontrol2/"

# Make .desktop file
echo "[Desktop Entry]
Name=EmissionControl2
Comment=Launch EmissionControl2
Exec=emissioncontrol2
Icon=/usr/share/pixmaps/emissioncontrol2.png
Terminal=false
Type=Application
Categories=Audio;Music;
Name[en_US]=EmissionControl2" >>"$BUILDDIR/usr/share/applications/EmissionControl2.desktop"

# make Debian control file
echo "Package: emissioncontrol2
Architecture: amd64
Section: sound
Priority: optional
Version:$VERSION
Maintainer:Rodney DuPlessis <rodney@rodneyduplessis.com>
Depends:libsndfile1, libc6
Homepage:https://github.com/EmissionControl2/EmissionControl2
Description:This package provides EmissionControl2, an advanced granular synthesizer.
 EmissionControl2 was designed by Curtis Roads, Jack Kilgore, and Rodney DuPlessis (2020).
 Based on Emission Control by Curtis Roads and David Thall (2004-2008)." >>"$BUILDDIR/DEBIAN/control"

# make copyright file
echo "Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: EmissionControl2
Source: https://github.com/EmissionControl2/EmissionControl2

Files: *
Copyright: 2020 Curtis Roads <clangtint@gmail.com>
   2020 Rodney DuPlessis <rodney@rodneyduplessis.com>
   2020 Jack Kilgore <jkilgore@ucsb.edu>
License: GPL-3+
 This program is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later
 version.
 .
 This program is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the implied
 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the GNU General Public License for more
 details.
 .
 You should have received a copy of the GNU General Public
 License along with this package; if not, write to the Free
 Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 Boston, MA  02110-1301 USA
 .
 On Debian systems, the full text of the GNU General Public
 License version 3 can be found in the file
 '/usr/share/common-licenses/GPL-3'" >>"$BUILDDIR/usr/share/doc/emissioncontrol2/copyright"
DATE="$(date +'%a, %d %b %Y %H:%M:%S %Z')"
echo "emissioncontrol2 (1.2) stable; urgency=medium
  * New upstream release
  -- Rodney DuPlessis <rodney@rodneyduplessis.com>  Sat, 2 Jun 2022 14:30:00 -0600
emissioncontrol2 (1.1) stable; urgency=medium
  * New upstream release
  -- Rodney DuPlessis <rodney@rodneyduplessis.com>  Sat, 27 Mar 2021 12:52:00 -0500
emissioncontrol2 (1.0) stable; urgency=high
  * Initial Release
 -- Rodney DuPlessis <rodney@rodneyduplessis.com>  Thu, 1 Oct 2020 20:48:30 -0800" >>"$BUILDDIR/usr/share/doc/emissioncontrol2/changelog.Debian"
gzip -9 -n "$BUILDDIR/usr/share/doc/emissioncontrol2/changelog.Debian"
echo "Packaging .deb at $BUILDLOCATION..."

# package .deb
cd ../deployment/Linux
fakeroot dpkg -b "$RELEASENAME" "$RELEASENAME.deb"

# add manual and zip it up
cp ../../docs/EmissionControl2-Manual.pdf .

zip $RELEASENAME.zip "$RELEASENAME.deb" EmissionControl2-Manual.pdf

# cleanup
# rm -rf "$RELEASENAME" "$RELEASENAME.deb" EmissionControl2-Manual.pdf

echo "Packaging Complete!"

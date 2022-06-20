# EmissionControl2

EmissionControl2 (EC2) is a standalone interactive real-time application for granular synthesis and sound file granulation. It is available for OSX, Linux, and Windows. 
<br><br>

# [DOWNLOAD EMISSIONCONTROL2](https://github.com/EmissionControl2/EmissionControl2/releases/latest)

<br><br>
![](docs/EC2_lightmode.png "This is what EC2 looks like")

Features include:

- Per-grain signal processing (envelope, waveform, amplitude, frequency, spatial position, filter center frequency and resonance)
- Granulation of multiple sound files simultaneously
- Up to 2048 simultaneous grains
- Synchronous and asynchronous grain emission
- Intermittency control
- Modulation control of all parameters with six LFOs (bipolar or unipolar waveforms)
- Real-time display of peak amplitude, active grains, waveform, scan range, scanner, and grain emission
- Scalable graphical user interface (GUI) and font size
- Easy mapping of parameters to any MIDI/OSC continuous controller
- Algorithmic control of granular processes via OSC scripts
- Unique filter design optimized for per-grain synthesis
- Unlimited user presets with smooth interpolation for gestural design
- Open source code and free to download and use

## Videos
### Controlling EmissionControl2 (lecture tutorial)
[![](docs/ControllingEmissionControl2_Screenshot.png)](https://vimeo.com/483296353)

### What people are doing with it
<a href="https://www.facebook.com/708680357/videos/10158420236745358/"><img src="docs/devineVideo_Screenshot.png" width="400" /></a>
<a href="https://www.youtube.com/watch?v=dCS2G6ASHUc"><img src="docs/BartonVideo_Screenshot.png" width="400" /></a>

## Mailing Lists

### New releases, major bug fixes, and other big news (low-volume list)
https://lists.sr.ht/~rondo/emissioncontrol2-announce

To subscribe, send a blank email to: ~rondo/emissioncontrol2-announce+subscribe@lists.sr.ht

### Discussion, questions, ideas, tips, sounds and video, and other general postings (higher-volume list; anyone can post here)
https://lists.sr.ht/~rondo/emissioncontrol2-discuss

To subscribe, send a blank email to: ~rondo/emissioncontrol2-discuss+subscribe@lists.sr.ht

To post to this list, send your post to: ~rondo/emissioncontrol2-discuss@lists.sr.ht

## Building
### Debian Linux

#### First, check the releases page to see if the pre-compiled installers work for you: https://github.com/EmissionControl2/EmissionControl2/releases/latest

- This project uses cmake to build so if you don't have cmake then install it (Minimum version: 3.13) and make sure your c and c++ compilers are defined in your environment.

- Run the following in a terminal to install the necessary libraries for building:

`sudo apt install libgtk-3-dev libasound2-dev libsndfile1-dev libfftw3-dev libjack-dev`
 
- git clone the repository and cd into the directory:

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

`cd EmissionControl2/ecSource`

- Run the configure script. You may add the argument "withjack" if you want JACK support. Note that compiling with JACK support can cause issues with Pipewire. If your system runs Pipewire, it is recommended to not compile with jack support.

`./scripts/configure.sh` --or-- (for jack support) `./scripts/configure.sh withjack`

- Run the build script:

`./scripts/build.sh`

- You can install ec2 by running the following script:

`sudo ./scripts/install_linux.sh` (this will put the binary in /usr/bin and set up resources in the paths where EC2 expects them to be)

To undo the files installed with install_linux.sh script, I've included the `scripts/uninstall_linux.sh` script as well.

### Arch Linux

- Must have cmake (3.13 or later), git, and base-devel packages installed.

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

`cd EmissionControl2/ecSource`

Run the configure script. You may add the argument "withjack" if you want JACK support. Note that compiling with JACK support can cause issues with Pipewire. If your system runs Pipewire, it is recommended to not compile with jack support.

`./scripts/configure.sh` --or-- (for jack support) `./scripts/configure.sh withjack`

- Run the build script:

`./scripts/build.sh`

- You can install ec2 by running the following script:

`sudo ./scripts/install_linux.sh` (this will put the binary in /usr/bin and set up resources in the paths where EC2 expects them to be)

To undo the files installed with install_linux.sh script, I've included the `scripts/uninstall_linux.sh` script as well.


### OS X

#### First, check the releases page to see if the pre-compiled installers work for you: https://github.com/EmissionControl2/EmissionControl2/releases/latest

You must have cmake installed (version 3.10 or later), and Xcode (hopefully we can get rid of this dependency soon).

You'll also need a few libraries installed: libsndfile flac libogg libvorbis (If you have homebrew, you can get them all with `brew install libsndfile flac libogg libvorbis`)

- First, clone the repo:

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

- cd into EmissionControl2/ecSource

`cd EmissionControl2/ecSource`

- run configure script:

`./scripts/configure.sh`

- run build script:

`./scripts/build.sh`

- EmissionControl2 is located in `EmissionControl2/ecSource/bin/`

### Windows (Visual Studio)

For installation through Visual Studio (taken from the allolib repo: https://github.com/AlloSphere-Research-Group/allolib):

    Install Visual Studio 2017 [ec2 devs: 2019 seems to work as well] Community Edition from https://visualstudio.microsoft.com/downloads/

    During installation options:

    a. Install "Desktop development with C++" workload

    b. Install Individual components: C++/CLI support, Git for Windows, Visual C++ Tools for Cmake

Install libsndfile: Aim your browser at http://www.mega-nerd.com/libsndfile/#Download. Download and install the 64-bit version: libsndfile-1.0.xx-w64-setup.exe.
    - Install this at the default location.

#### Compile Emission Control
- Note that you need Visual Studio installed for the below scripts to work.

- Clone the repo using the git bash terminal:

`git clone https://github.com/EmissionControl2/EmissionControl2.git`

- Open Visual Studio pointed at the directory `EmissionControl2\`

- In Visual Studio, open the Developer Power Shell.

- When first running the sh scripts, Visual Studio will ask what program would you like to open this file with. Choose `Git Bash`.

- Configure the project:

`.\ecSource\scripts\configure.sh`

- The above step will initially fail. This reason for this is that NFD is using an old version of Visual Studio (2010). To fix this problem, do the following.
     - In file explorer go to the folder `EmissionControl2\ecSource\external\nativefiledialog\build\vs2010\`
     - Open `NativeFileDialog.sln` in Visual Studio.
     - Double click on `Solution 'NativeFileDialog' (5 of 5 projects)` in the vertical window on the right side.
     - Agree to updating this project to the newest toolchain.
     - Run `.\ecSource\scripts\configure.sh` again from the Emission Control 2 Visual Studio Project.
     - Note that the following will only have to be done once per git clone.
     - Sidenote: If you have a better, more stable way, let me know -- jack


- Compile the project
`.\ecSource\scripts\build.sh`







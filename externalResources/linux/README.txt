Starting from v1.2:

1) IMPORTANT: If you are upgrading from EC2 before v1.2 and you want to keep your MIDI and SOUND FILE presets, you need to run the included script called "linux_preset_migrate.sh"!
    There was a bug found in the naming scheme of previous EC2 versions on Linux that is now fixed.
    The script will fix the naming issue in your presets and will make sure they're in the right directory.
    Note: if you get the following output, then you have no more presets with the naming issue:
        mv: cannot stat './midi_presets*.json': No such file or directory
        mv: cannot stat './sample_presets*.json': No such file or directory

2) There are two installers for EC2 on Linux. 
    The standard one includes support for JACK, PulseAudio, and ALSA. It checks for those sound engines in that order at startup and selects the first one it finds.
    This probably works for most users.

    Then there is the NOJACK version that is compiled without Jack support.
    Use this one if you have Pipewire on your system.
    This installer was created becuase of some strange problems that came up between JACK and Pipewire.
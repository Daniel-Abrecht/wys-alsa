# Wys
A daemon to bring up and take down PulseAudio loopbacks for phone call
audio.

Wys was written to manage call audio in the Librem 5 phone with a
Gemalto PLS8.  It may be useful for other systems.

Wys is pronounced "weece" to rhyme with "fleece".


## License
Wys is licensed under the GPLv3+.


## Dependencies

    sudo apt-get install libpulse-dev libglib2.0-dev


## Building
We use the meson and thereby Ninja.  The quickest way to get going is
to do the following:

    meson ../wys-build
    ninja -C ../wys-build
    ninja -C ../wys-build install


## Running
Wys is usually run as a systemd user service.  To run it by hand,
the program takes two options, --codec and --modem.  These should
specify the ALSA card names of the codec and modem ALSA devices.  That
is, the value of the "alsa.card_name" property of the device's
PulseAudio sink or source.

  $ wys --codec sgtl5000 --modem "SIMCom SIM7100"

You can also set the WYS_CODEC or WYS_MODEM environment variables with
the same information.

  $ export WYS_CODEC=sgtl5000
  $ export WYS_MODEM="SIMCom SIM7100"
  $ wys

The command-line options take precendence over the environment
variables.

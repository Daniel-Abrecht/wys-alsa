# Wys-alsa
Fork of wys (https://source.puri.sm/Librem5/wys), brings up and take down loopbacks for phone call audio.
But this fork uses alsaloop to do so. 

Wys was written to manage call audio in the Librem 5 phone with a
Gemalto PLS8.  It may be useful for other systems.

Wys is pronounced "weece" to rhyme with "fleece".


## License
Wys is licensed under the GPLv3+.


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

There is also a database of machine-specific configuration files in
the machine-conf/ sub-directory.  This database will be installed
under the installation prefix, in $prefix/share/wys/machine-conf.  The
machine name is the device tree model field for that machine, given by
/proc/device-tree/model.

A number of locations will be checked for Wys machine configuration
entries under a wys/machine-conf/ sub-directory in this order:

  $XDG_CONFIG_HOME         (default: $HOME/.config)
  $XDG_CONFIG_DIRS         (default: /etc/xdg)
  sysconfdir meson option  (default: $prefix/etc)
  datadir meson option     (default: $prefix/share)
  $XDG_DATA_DIRS           (default: /usr/local/share/:/usr/share/)

The precendence of the different configuration methods is as follows:

  (1) command line options
  (2) environment variables
  (3) machine configuration files.

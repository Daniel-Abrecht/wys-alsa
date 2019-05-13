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
the program takes no options so just run it with the program name:

  $ wys

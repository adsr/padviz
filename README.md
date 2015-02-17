padviz
======

padviz is a quick and dirty NES controller visualizer. It reads events from
`/dev/input/js0` as a
[hid-atari-retrobit](https://github.com/robmcmullen/hid-atari-retrobit) device
and visualizes button presses on the window.

### Building

    $ # Install gtk3 dev package
    $ sudo apt-get install libgtk-3-dev  # or equivalent
    $
    $ # Clone this repo
    $ git clone https://github.com/adsr/padviz.git
    $ cd padviz
    $
    $ # Build
    $ make
    $
    $ # Run
    $ ./padviz

### Screenshot
![Screenshot of padviz](http://i.imgur.com/4PIkJBS.png)

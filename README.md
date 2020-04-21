PowerMate hooks
===============

Adapted from Will Sowerbutts' [Griffin Powermate driver for Linux](https://sowerbutts.com/powermate/).


Install
-------

```
git clone git@github.com/paulherron/powermate-hooks.git
cd powermate-hooks
make
```


Usage
-----

The main script provided is `powermate-hooks`, which responds to activity on the Griffin PowerMate.

Plug in the PowerMate and run:

```
./powermate-hooks
```

You can place scripts in the `hooks/` directory as follows, and they'll be called at the appropriate times:

```
hooks/press-on.sh # Called when the button is pressed on
hooks/press-off.sh # Called when the button is pressed off
hooks/rotate.sh # Called as the dial is rotated, and the percentage rotation passed to the hook script
```

See inside the `hooks/` directory for some example scripts that create an internet radio when the PowerMate is plugged into a Raspberry Pi. Pressing the button toggles the radio on and off, and turning the dial controls the system volume with `amixer`.


Limitations
-----------

The `powermate-hooks` script was intended to make the PowerMate an interface for a Raspberry Pi-based internet radio, which explains some of the limitations:

* The script always starts in an 'off' state
* The concept of the PowerMate toggling on and off is hardcoded
* Rotation is always passed in percentages


Troubleshooting
---------------

If the PowerMate isn't automatically detected it can be specified as an argument. For example on Manjaro Linux:

```
find /dev/input -iname "*powerm*"
	# Outputs: "/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00
sudo ./powermate-hooks /dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00
```

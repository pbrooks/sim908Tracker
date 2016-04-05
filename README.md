# Sim908 Tracker (Arduino)
This is a WIP project for creating a tracker using a SIM908 chipset, it's based upon the [excellent guide from cooking-hacks](https://www.cooking-hacks.com/documentation/tutorials/geolocation-tracker-gprs-gps-geoposition-sim908-arduino-raspberry-pi).

Hardware being used during development includes:

	* Arduino Uno
	* Cooking-Hacks SIM908 shield

## Feature state

Currently the codebase is a cut down version of the cooking hacks Arduino example which communicates with an API via SMS, this will be expanded to HTTP.

Interations are currently very frequent, this is a battery drain and will be replaced with better use of sensor inputs.

In the original example - the Arduino converted the coordinates into degrees. This is offloaded to the API to handle raw SIM908 output.

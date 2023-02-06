# controlVoltageExamples

Files for creating Eurorack modules based on the controlVoltage repository: https://github.com/ianhattwick/controlVoltage

### Standard examples
Three examples so far:
* 8-step trigger sequencer
* Sloopy: dual slope generator
* dual LFO (missing?)
* Voxelist: Dual input quantizer

## Also examples of using USB Midi devices to generated sequences

### Requires
Note: all libraries available through library manager
EXCEPT: Control Voltage needs most recent version from Github

1. Arduino USB Host shield 
	* https://github.com/felis/USB_Host_Shield_2.0
2. Hitachi style 2x16 LCD display with I2C backpack
	* https://github.com/marcoschwartz/LiquidCrystal_I2C.git
3. Control Voltage library
	* https://github.com/ianhattwick/controlVoltage
4. Standard EEPROM library

### examples
1. Harpy - Launchkey controlling NiftyCase with melodic voice
2. Leonard - Launchpad 2 controlling NiftyCase with rhythmic voice
3. Tyrone - nanokontrol2 controlling Roland T-8



#https://forums.raspberrypi.com/viewtopic.php?t=143484
#Pulse light: https://raspberrypihq.com/making-a-led-blink-using-the-raspberry-pi-and-python/

import RPi.GPIO as GPIO    # Import Raspberry Pi GPIO library
from time import sleep     # Import the sleep function from the time module
GPIO.setwarnings(False)    # Ignore warning for now
GPIO.setmode(GPIO.BOARD)   # Use physical pin numbering
GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW)   # Set pin 8 to be an output pin and set initial value to low (off)

while True: # Run forever
    GPIO.output(8, GPIO.HIGH) # Turn on
    sleep(.01)                  # Sleep for .01 second
    GPIO.output(8, GPIO.LOW)  # Turn off
    sleep(.01)                  # Sleep for .01 second

    

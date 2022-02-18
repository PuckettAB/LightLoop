#https://forums.raspberrypi.com/viewtopic.php?t=143484
#Pulse light: https://raspberrypihq.com/making-a-led-blink-using-the-raspberry-pi-and-python/

from gpiozero import LED
led = LED(2) #5 volt output


for x in range(2,6):
    led.on()
    sleep(.01)
    led.off()
    sleep(.01)
#use 5 volts for laser
#use ground for laser

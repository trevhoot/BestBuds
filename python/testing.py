import device
from time import sleep

dev = device.Device()


dev.set_speed(1200)

while 1:
	print "ticks =", dev.read_ticks()
	print "current in =", dev.read_current()
	sleep(0.1)

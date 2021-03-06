
import usb.core
import time

class Device:
	def __init__(self):
		self.SET_TIMER = 0
		self.GET_STATE = 1
		self.SET_THRESHOLD = 2
		self.GET_VOLTAGE = 3
		self.SET_DUTY = 4
		self.GET_STEP_COUNT = 5
		self.SCAN = 6
		self.SET_SPEED = 7
		self.DEBUG = 8
		self.SET_RED = 9
		self.SET_GREEN = 10
		self.SET_BLUE = 11


		self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
		if self.dev is None:
			raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
		self.dev.set_configuration()
		self.speed = 0
		self.time = 0
		self.threshold = 0

	def close(self):
		self.dev = None

	def set_duty(self, duty):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_DUTY, int(duty), 0)
		except usb.core.USBError:
			print "Could not send SET_DUTY vendor request."

	def set_speed(self, speed):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_SPEED, int(speed), 0)
		except usb.core.USBError:
			print "Could not send SET_SPEED vendor request."

	def set_threshold(self, threshold):
		self.threshold = threshold
		try:
			self.dev.ctrl_transfer(0x40, self.SET_THRESHOLD, int(self.threshold), 0)
		except usb.core.USBError:
			print "Could not send SET_THRESHOLD vendor request."

	def set_red(self, redTarget):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_RED, int(redTarget), 0)
		except usb.core.USBError:
			print "Could not send SET_RED vendor request."
	def set_green(self, greenTarget):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_GREEN, int(greenTarget), 0)
		except usb.core.USBError:
			print "Could not send SET_GREEN vendor request."
	def set_blue(self, blueTarget):
		try:
			self.dev.ctrl_transfer(0x40, self.SET_BLUE, int(blueTarget), 0)
		except usb.core.USBError:
			print "Could not send SET_BLUE vendor request."

	def get_voltage(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_VOLTAGE, 0, 0, 5)
	   except usb.core.USBError:
		   print "Could not send GET_VOLTAGE vendor request."
	   else:
		   return int(ret[0])*256, int(ret[1])*256, int(ret[2])*256, int(ret[3])*256,  int(ret[4])*256

	def get_state(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_STATE, 0, 0, 5)
	   except usb.core.USBError:
		   print "Could not send GET_STATE vendor request."
	   else:
		   return int(ret[0]), int(ret[1]), int(ret[2]), int(ret[3]), int(ret[4]);

	def debug(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.DEBUG, 0, 0, 8)
	   except usb.core.USBError:
		   print "Could not send GET_STATE vendor request."
	   else:
		   return int(ret[0]) + 256*int(ret[1]), int(ret[2]) + 256*int(ret[3]), int(ret[4]) + 256*int(ret[5]), int(ret[6]) + 256*int(ret[7]);

	def get_step_count(self):
	   try:
		   ret = self.dev.ctrl_transfer(0xC0, self.GET_STEP_COUNT, 0, 0, 4)
	   except usb.core.USBError:
		   print "Could not send GET_STEP_COUNT vendor request."
	   else:
		   return int(ret[0]) + int(ret[1])*256, int(ret[2]) + int(ret[3])*256;

	def scan(self):
        	try:
        	    ret = self.dev.ctrl_transfer(0xC0, self.SCAN, 0, 0, 2)
        	except usb.core.USBError:
        	    print "Could not send SCAN vendor request."
		    return 0
        	else:
        	    return int(ret[0])+int(ret[1])*256
	


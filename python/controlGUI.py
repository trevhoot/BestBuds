from Tkinter import *
import device
import time
	

class GUI():
	def __init__(self):
		self.dev = device.Device()
		
		self.root = Tk()
		self.duty = DoubleVar()
		self.time = DoubleVar()
		self.speed = DoubleVar()
		self.speed = 1
		self.red = DoubleVar()
		self.red = 0
		self.green = DoubleVar()
		self.green = 0
		self.blue = DoubleVar()
		self.blue = 0
		self.dev.set_threshold(22000)

		self.var = 0
		
		scale1= Scale(self.root, variable = self.duty , from_ = 32768, to = 32900, orient = HORIZONTAL, label = "Position", command = self.set_duty)
		scale1.set(32768)
		scale1.pack(anchor = CENTER)
		
		scale3= Scale(self.root, variable = self.speed , from_ = 1, to = 10000, orient = HORIZONTAL, label = "Speed", command = self.dev.set_speed)
		scale3.pack(anchor = CENTER)
		scale3.set(7000)

		#scale4= Scale(self.root, variable = self.red , from_ = 0, to = 40000, orient = HORIZONTAL, label = "Red", command = self.dev.set_red)
		#scale4.pack(anchor = CENTER)
		#scale4.set(0)

		#scale5= Scale(self.root, variable = self.green , from_ = 0, to = 40000, orient = HORIZONTAL, label = "Green", command = self.dev.set_green)
		#scale5.pack(anchor = CENTER)
		#scale5.set(0)

		scale6= Scale(self.root, variable = self.blue , from_ = 0, to = 40000, orient = HORIZONTAL, label = "Blue", command = self.dev.set_blue)
		scale6.pack(anchor = CENTER)
		scale6.set(0)

		'''
		b0 = Button(self.root, text = "Ping", command = self.send_pulse)
		b0.pack(anchor=CENTER)

		b1 = Button(self.root, text = "Get Voltage", command = self.get_voltage)
		b1.pack(anchor=CENTER)

		b2 = Button(self.root, text = "Get State", command = self.get_state)
		b2.pack(anchor=CENTER)

		b3 = Button(self.root, text = "Get Step Count", command = self.get_step_count)
		b3.pack(anchor=CENTER)

		'''

		b3 = Button(self.root, text = "Debug", command = self.debug)
		b3.pack(anchor=CENTER)

		label = Label(self.root)
		label.pack()

		self.root.pack_propagate(0)
		self.root.mainloop()

	def set_duty(self, var):
		self.dev.set_duty(var)
		self.get_step_count()

	def do_it(self, var):
		print self.var, var

	def get_voltage(self):
		time = self.dev.get_voltage()
		print time

	def get_state(self):
		state = self.dev.get_state()
		print state

	def debug(self):
		print self.dev.debug()

	def get_step_count(self):
		stepcount, destination = self.dev.get_step_count()
		print 'step count = %d, destination is %d' %(stepcount, destination)

	def send_pulse(self):
		while 1:
			read = self.dev.scan()
			print read
			self.debug()
			time.sleep(0.4)
		

g = GUI()

from Tkinter import *
import device
	

class GUI():
	def __init__(self):
		self.dev = device.Device()
		
		self.root = Tk()
		self.duty = DoubleVar()
		self.time = DoubleVar()
		self.threshold = DoubleVar()
		self.threshold = 40000
		self.speed = DoubleVar()
		self.speed = 1000

		self.var = 0

		scale1= Scale(self.root, variable = self.duty , from_ = 32268, to = 33268, orient = HORIZONTAL, label = "Position", command = self.dev.set_duty)
		scale1.set(32768)
		scale1.pack(anchor = CENTER)

		scale2= Scale(self.root, variable = self.speed , from_ = 1000, to = 10000, orient = HORIZONTAL, label = "Speed", command = self.dev.set_speed)
		scale2.pack(anchor = CENTER)

		scale3= Scale(self.root, variable = self.threshold , from_ = 30000, to = 30001, orient = HORIZONTAL, label = "Threshold", command = self.dev.set_threshold)
		scale3.pack(anchor = CENTER)
		'''
		scale = Scale(self.root, variable = self.var, orient = HORIZONTAL, label = "Timer", command = self.do_it)
		scale.pack(anchor = CENTER)
		'''

		b1 = Button(self.root, text = "Get Voltage", command = self.get_voltage)
		b1.pack(anchor=CENTER)

		b2 = Button(self.root, text = "Get State", command = self.get_state)
		b2.pack(anchor=CENTER)

		b3 = Button(self.root, text = "Get Step Count", command = self.get_step_count)
		b3.pack(anchor=CENTER)

		label = Label(self.root)
		label.pack()

		self.root.pack_propagate(0)
		self.root.mainloop()


	def do_it(self, var):
		print self.var, var

	def get_voltage(self):
		time = self.dev.get_voltage()
		print time

	def get_state(self):
		state = self.dev.get_state()
		print state

	def get_step_count(self):
		steps = self.dev.get_step_count()
		print steps

g = GUI()

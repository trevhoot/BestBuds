import usb.core
import Tkinter as tk
import tkFileDialog
from Tkinter import *
import ctypes
import time
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter

def randrange(n, vmin, vmax):
    return (vmax-vmin)*np.random.rand(n) + vmin

class panTilt:

    def __init__(self):
        self.SEND = 0
        self.SET_VALS = 1
        self.GET_VALS = 2
        self.SCAN = 3
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None
    def send(self):
        try:
            self.dev.ctrl_transfer(0x40, self.SEND)
            print "Requested a pulse."
        except usb.core.USBError:
            print "Could not send SEND vendor request."
    def set_vals(self, val1, val2):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_VALS, int(val1), int(val2))
        except usb.core.USBError:
            print "Could not send SET_VALS vendor request."
    def get_vals(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_VALS, 0, 0, 4)
        except usb.core.USBError:
            print "Could not send GET_VALS vendor request."
        else:
            return [int(ret[0])+int(ret[1])*256, int(ret[2])+int(ret[3])*256]
    def scan(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.SCAN, 0, 0, 4)
        except usb.core.USBError:
            print "Could not send SCAN vendor request."
            return [4800,0]
        else:
            return [int(ret[0])+int(ret[1])*256, int(ret[2])+int(ret[3])*256]
        
def send_pulse():
    a = 0
    for x in range (0,3):
        read = irene.scan()
        # print read
        if read[0] != 0:
            a += read[0]
        else:
            a += 4800
        time.sleep(0.02)
    a_tick = a/3
    print a_tick
    josh.append(a_tick)
    # print josh
    return a_tick

def scan():
    distances = [0.025, 0.055]
    for x in range (0,len(josh)-2):
        distances.append(0.155+x*0.1)
        x += 1
    plt.plot(distances,josh)
    plt.xlabel('Distance (m)')
    plt.ylabel('Number of Ticks')
    plt.title('Ultrasonic Ranging System Calibration Curve')
    plt.show()

def plot():
    #hAngle = 0
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    for hAngle in range (0,151):
        x = []
        y = []
        #angle = 0
        for angle in range (0,151):
            irene.set_vals(hAngle/150.*65535,angle/150.*65535)
            hypo = send_pulse()/1600.
            if (hypo != 0):
                x.append(np.cos(angle/180.*(np.pi))*hypo*np.cos(hAngle/180.*(np.pi)))
                y.append(np.sin(angle/180.*(np.pi))*hypo*np.cos(hAngle/180.*(np.pi)))
                #print [hypo,hAngle]  
            z = np.sin(hAngle/180.*(np.pi))
            ax.plot(x, y, z, zdir = 'z')
            print hAngle
            ax.scatter(x, y, z)
            #angle += 5
        #hAngle += 5
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()

def test():
    hAngle = 0
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    while hAngle in range (0,91):
        x = []
        y = []
        for angle in range (0,10):
            irene.set_vals(hAngle/150.*65535,(90+angle)/150.*65535)
            hypo = send_pulse()/1600.
            x.append(np.cos(angle/180.*(np.pi))*hypo*np.cos(hAngle/180.*(np.pi)))
            y.append(np.sin(angle/180.*(np.pi))*hypo*np.cos(hAngle/180.*(np.pi)))
            print [hypo,hAngle]  
        z = np.sin(hAngle/180.*(np.pi))
        #ax.plot(x, y, z, zdir = 'z')
        ax.scatter(x, y, z)
        hAngle += 10
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    #ax = fig.add_subplot(1,2,2, projection='3d')
    #x, y = np.meshgrid(x, y)
    #surf = ax.plot_surface(x, y, z, rstride=1, cstride=1, cmap=cm.coolwarm,
    #        linewidth=0, antialiased=False)
    #fig.colorbar(surf, shrink=0.5, aspect=5)

    # ax = fig.add_subplot(1,3,3, projection='3d')
    # x,y,z = np.mesh(x,y,z);
    # ax.plot_wireframe(x,y,z, rstride=10, cstride=10)

    plt.show()
    
def get_position():
    print irene.get_vals()
        
def update_vals(something):
    irene.set_vals(w1.get()/150.*65535,w2.get()/150.*65535)
    
master = Tk()
w1 = Scale(master, from_=0, to=150, command=update_vals)
w1.pack()
w2 = Scale(master, from_=0, to=150, orient=HORIZONTAL, command=update_vals)
w2.pack()
Button(master, text='Send Pulse', command=send_pulse).pack()
Button(master, text='Scan', command=scan).pack()
Button(master, text='Plot', command=plot).pack()
Button(master, text='Test', command=test).pack()
irene = panTilt()
global josh # the global ticks list
josh = []
mainloop()


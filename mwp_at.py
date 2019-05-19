#:ts=4 number
#!/usr/bin/python

# Simple python example for mwp dbus

import sys
import dbus
import dbus.mainloop.glib

from gi.repository import GLib

# GLobal data for posits etc
posit_HOME = ""
posit_UAV  = ""
posit_SATS = ""
posit_STAT = ""


# Handlers for UAV posit
def loc_handler(*args):
    print('sig loc: ', args)

# Handler for Home posit
def home_handler(*args):
    print('sig home: ', args)

# Handler for number of sats chcanged
def SatsChanged_handler(*args):
  print ("Sattellites Changed: ", args)

# Handler for UAV state change - eg arming
def StateChanged_handler(*args):
	print("State Changed: ", args)

def MWP_GetDevices(mwp_obj):
	return mwp_obj.GetDevices()

# Get Current Connection Status
def MWP_GetConnStatus(*args):
	return mwp_obj.ConnectionStatus()

def MWP_Connect(mwp_obj):
	# Get Number of devices
	device_list = MWP_GetDevices(mwp_obj)

	# Are we Connected?
	constat = MWP_GetConnStatus(mwp_obj)

	# If we are just return true, if not, try to connect and if still not connected, return false
	if constat[1]:
		return(True)
	else:
		# Are we connected yet?
		constat = mwp.ConnectDevice(device_list[0])
		if constat[1]:
			return(True)
		else:
			return(False)

# Upload a mission to the FC
def UploadMission(mwp_obj, filename):
	# Are we Connected? If not, try and connect else fail
	constat = MWP_Connect(mwp_obj)
	if constat[1]:
		# Try to open mission
		mwp_mission_points = mwp_obj.Loadmission(filename)
		print("Loaded {} wth {} Mission Points".format(filename, str(mwp_mission_points)))
		# Upload Mission
		if mwp_mission_points > 0:
			nwpts = mwp_obj.UploadMission(True)
			print("Uploaded {} Waypoints to UAV".format(wpts))

# Initalise mwp and get devices
def MWP_Setup(mwp_obj):
	print("Getting MWP Device List")
	# Get List of Devices
	device_list = MWP_GetDevices(mwp)
	for dev in device_list:
		print("Found Device: {}".format(dev))

	return(device_list)

# Initialise the tracker post data
def MWP_InitTracker(mwp_obj):
	global posit_HOME, posit_UAV

	posit_HOME = mwp_obj.GetHome()
	posit_UAV  = mwp_obj.GetLocation()
	posit_SATS = mwp_obj.GetSats()
	print("Init Home: {}".format(posit_HOME))
	print("Init Vehicle: {}".format(posit_UAV))
	print("Init Sats: {}".format(posit_SATS))

# Connect to the signals we need for location data
def MWP_SignalSubscribe(mwp_obj):
	mwp_obj.connect_to_signal('LocationChanged', loc_handler)
	mwp_obj.connect_to_signal('HomeChanged', home_handler)
	mwp_obj.connect_to_signal('SatsChanged', SatsChanged_handler)
	mwp_obj.connect_to_signal('StateChanged', StateChanged_handler)

#####################################################################3
### MAIN
#####################################################################3
if __name__ == "__main__":
	dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

	# Attempting DBUS Connection to MWP
	print("Attempting DBUS Connection to MWP")
	try:
	    bus = dbus.SessionBus()
	    obj = bus.get_object("org.mwptools.mwp", "/org/mwptools/mwp")
	    mwp = dbus.Interface(obj, "org.mwptools.mwp")

	except dbus.DBusException as e:
	        print(str(e))
	        sys.exit(255)


	# Get List of MWP Devices
	MWP_Devices = MWP_Setup(mwp);	

	loop = GLib.MainLoop()
	loop.run()

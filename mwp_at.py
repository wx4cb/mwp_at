#!/usr/bin/python

# Simple python example for mwp dbus
import threading
import math
import sys
import dbus
import dbus.mainloop.glib
import time
import atexit
import pyproj
from gi.repository import GLib
import serial

from angles import normalize
from measurement.measures import Distance

# Global access to main loop
MainLoop = GLib.MainLoop()

# GLobal data for posits etc
posit_HOME = None
posit_UAV = None
posit_SATS = None
posit_STATE = 0     # Assume Disarmed
posit_POLAR = None
posit_VELOCITY = None

__CMD_UAV__ = "!UAV:"

# Tracker Serial Port object
TrackerSerial = None

def quit_handler(*args):
    global MainLoop

    try:
        MainLoop.quit()
    except:
        pass

    print("Hasta La Vista BABY.. I'll be back...")
    exit(0)

# Handlers for UAV posit
def loc_handler(*args):
    global posit_UAV

    #print('sig loc: ', args)
    # Get data
    posit_UAV = args

    # Procees data
    MWP_UpdateTracker()

# Handler for Home posit
def home_handler(*args):
    global posit_HOME
    #print('sig home: ', args)
    posit_HOME = args
    # send the new home posit to the Tracker
    homedata = args[0]
    homedata += " "
    homedata += args[1]
    homedata += " "
    homedata += args[2]
    MWP_SendCommand("HOME", homedata)

# Handler for number of sats changed
def SatsChanged_handler(*args):
    global posit_SATS
    posit_SATS = args
    print ("Satellites Changed: {:d} Sats - Fix Type: {:d}".format(posit_SATS[0], posit_SATS[1]))

# Handler for UAV state change - eg arming
def StateChanged_handler(*args):
    global posit_STATE
    #print("State Changed: ", args)
    posit_STATE = args
    # Procees data
    MWP_UpdateTracker()

def PolarChanged_handler(*args):
    global posit_POLAR
    print("Polar Changed: Range: {} Bearing: {} Azimuth: {}".format(args[0], args[1], args[2]))
    posit_POLAR = args
    # Procees data
    MWP_UpdateTracker()
def VelocityChanged_handler(*args):
    global posit_VELOCITY
    #print("Velocity Changed: ", args)
    posit_VELOCITY = args
    # Procees data
    MWP_UpdateTracker()

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
        return (True)
    else:
        # Are we connected yet?
        constat = mwp.ConnectDevice(device_list[0])
        if constat[1]:
            return (True)
        else:
            return (False)

# Get availabl states
# Available states ["DISARMED", "MANUAL", "ACRO", "HORIZON", "ANGLE", "CRUISE", "RTH", "LAND", "WP", "HEADFREE", "POSHOLD", "ALTHOLD", "LAUNCH", "AUTOTUNE", "UNDEFINED"]

def MWP_GetState(argument):
    switcher = {
        0: "DISARMED",
        1: "MANUAL",
        2: "ACRO",
        3: "HORIZON",
        4: "ANGLE",
        5: "CRUISE",
        6: "RTH",
        7: "LAND",
        8: "WP",
        9: "HEADFREE",
        10: "POSHOLD",
        11: "ALTHOLD",
        12: "LAUNCH",
        13: "AUTOTUNE",
        14: "UNDEFINED"
    }
    return(switcher.get(int(argument), "UNDEFINED"))

def MWP_GetFixType(argument):
    switcher = {
        -1: "INVALID",
        0: "NONE",
        1: "2D",
        2: "3D",
        3: 'DGPS'
    }
    return(switcher.get(argument, "INVALID"))

# Upload a mission to the FC
def MWP_UploadMission(mwp_obj, filename):
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

    return (device_list)

# Initialise the tracker post data
def MWP_InitTracker(mwp_obj):
    global posit_HOME
    global posit_UAV
    global posit_SATS
    global TrackerSerial
    global posit_STATE
    global posit_POLAR
    global posit_VELOCITY

    # Fill with initial values
    posit_HOME = mwp_obj.GetHome()
    posit_UAV = mwp_obj.GetLocation()
    posit_SATS = mwp_obj.GetSats()
    posit_STATE = mwp_obj.GetState()
    posit_POLAR = mwp_obj.GetPolarCoordinates()
    posit_VELOCITY = mwp_obj.GetVelocity()

    print("Init Home: {}".format(posit_HOME))
    print("Init Vehicle: {}".format(posit_UAV))
    print("Init Sats: {}".format(posit_SATS))
    print("Init Polar: {}".format(posit_POLAR))
    print("Init Velocity: {}".format(posit_POLAR))

    # send the new home posit to the Tracker
    homedata = str(posit_HOME[0])
    homedata += " "
    homedata += str(posit_HOME[1])
    homedata += " "
    homedata += str(posit_HOME[2])
    MWP_SendCommand("HOME", homedata)

def MWP_SendCommand(cmd, cmd_args):
    output = None
    output = "!"
    output += cmd
    output += ": "
    output += cmd_args

    print("Sending Command: {}".format(output))
    TrackerSerial.write(output)
    TrackerSerial.flush()


def MWP_UpdateTracker():
    global posit_HOME
    global posit_UAV
    global posit_SATS
    global TrackerSerial
    global posit_POLAR
    global posit_VELOCITY
    global posit_STATE

    # Command constants
    global __CMD_UAV__

    # Array Definitions
    ARRAYDEF_LAT=0
    ARRAYDEF_LONG=1
    ARRAYDEF_ALT=2
    ARRAYDEF_RANGE=0
    ARRAYDEF_BEARING=1
    ARRAYDEF_AZIMUTH=2
    ARRAYDEF_SPEED=0
    ARRAYDEF_COURSE=1
    ARRAYDEF_SATS=0
    ARRAYDEF_FIX=1

    bearing = posit_POLAR[ARRAYDEF_BEARING]-180
    azimuth = posit_POLAR[ARRAYDEF_AZIMUTH]

    homealt = Distance(m=posit_HOME[ARRAYDEF_ALT]).ft
    uavalt = Distance(m=posit_UAV[ARRAYDEF_ALT]).ft
    range = Distance(m=posit_POLAR[ARRAYDEF_RANGE]).ft

    if bearing < 0:
        bearing +=360

    #azimuth = 0 if azimuth > 90 else azimuth

    #bearing = normalize(bearing, 0, 360, b=True)
    #azimuth = normalize(azimuth, -90, 90, b=True)
    # Sort out fixes and states
    FixType = int(posit_SATS[ARRAYDEF_FIX])
    #   print("UAVSTATE: {0}\n\n".format(posit_STATE))
    if isinstance(posit_STATE, tuple):
        UAVState = int(posit_STATE[0])
    else:
        UAVState = int(posit_STATE)

    #print("Home: {:+.4f} {:+.4f}".format(
    #    posit_HOME[ARRAYDEF_LAT],
    #    posit_HOME[ARRAYDEF_LONG]))
    print("UAV ({} {}): {:+.4f} {:+.4f} Alt: {} sats: {} - Distance: {} ft - Elevation: {} deg - Bearing: {} - Velocity: {:.2f} Course: {:.2f}".format(
        MWP_GetFixType(FixType),
        #int(posit_SATS[ARRAYDEF_FIX]),
        MWP_GetState(int(UAVState)),
        posit_UAV[ARRAYDEF_LAT],
        posit_UAV[ARRAYDEF_LONG],
        int(uavalt),
        int(posit_SATS[ARRAYDEF_SATS]),
        int(range),
        azimuth,
        bearing,
        # Multiply m/s speed by 2.237 to get miles per hour
        posit_VELOCITY[ARRAYDEF_SPEED]*2.237, posit_VELOCITY[ARRAYDEF_COURSE]))

    # Send data to TrackerTTY
    cmd = str(posit_UAV[ARRAYDEF_LAT])
    cmd += " "
    cmd += str(posit_UAV[ARRAYDEF_LONG])
    cmd += " R:"
    cmd += str(int(range))
    cmd += " V:"
    cmd += str(posit_VELOCITY[ARRAYDEF_SPEED]*2.237)
    cmd += " A:"
    cmd += str(int(uavalt))
    cmd += " B:"
    cmd += str(int(bearing))
    cmd += " Z:"
    cmd += str(int(azimuth))
    MWP_SendCommand("UAV", cmd)

def valmap(value, istart, istop, ostart, ostop):
  return ostart + (ostop - ostart) * ((value - istart) / (istop - istart))

# Connect to the signals we need for location data
def MWP_SignalSubscribe(mwp_obj):
    print("Connecting to MWP DBUS Signals")
    # Position of UAV chcanged
    mwp_obj.connect_to_signal('LocationChanged', loc_handler)
    # Position of home changed
    mwp_obj.connect_to_signal('HomeChanged', home_handler)
    # Number of Satellites Changed
    mwp_obj.connect_to_signal('SatsChanged', SatsChanged_handler)
    # Arming/Mode State Change
    mwp_obj.connect_to_signal('StateChanged', StateChanged_handler)
    # MWP Closed
    mwp_obj.connect_to_signal('Quit',quit_handler)
    # Polar Change - bearing etc
    mwp_obj.connect_to_signal('PolarChanged', PolarChanged_handler)
    # Velocity Change - speed/course etc
    mwp_obj.connect_to_signal('VelocityChanged', VelocityChanged_handler)

def read_from_port(ser):
    while True:
        reading = ser.readline().decode()
        print(reading)
# MAIN
if __name__ == "__main__":


    # Check for serial ports on command line
    # format: mwp_at.py <tracker tty port>
    try:
        if len(sys.argv) < 2:
            print("Wrong amount of arguments: {}".format(len(sys.argv)))
            print("USAGE:")
            print("{} <tty port for Antenna Tracker Controller - usually /dev/ttyACM0".format(sys.argv[0]))
            exit(1)
        else:
            TrackerTTY = sys.argv[1]
            print("Using {} for Tracker Controller".format(TrackerTTY))
    except:
        raise

    # Create the maestro object and center channel
    try:
        TrackerSerial = serial.Serial(TrackerTTY, 115200, timeout=1)

    except:
        raise
        exit(1)

#    finally:
#        atexit(Maestro_obj.closeServo())

    print("Creating DBUS Loop")
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
    MWP_Devices = MWP_Setup(mwp)

    # Init the tracker subsystem
    MWP_InitTracker(mwp)

    # Subscribe tos MWP_SignalSubscribe
    MWP_SignalSubscribe(mwp)

    # Start Read Thread
    #try:
        #thread = threading.Thread(target=read_from_port, args=(TrackerSerial,))
        #thread.start()
    MainLoop.run()
    #except:
        #thread.join()

# Pololu Maestro Servo class
# baed on the work by martin saint: http://martinsant.net/?page_id=479
#
import serial
import time

class MaestroController:

    #Class constructor
    def __init__(self, usbPort):
        self.usbPort = usbPort
        # Set the lower, upper and range limits for each servo channel
        # min ms, center ms, max ms, servo throw
        self.ServoLimits = [[900, 1500, 2000, 180],
                            [1000, 1500, 2000, 180],
                            [1000, 1500, 2000, 180],
                            [1000, 1500, 2000, 180],
                            [1000, 1500, 2000, 180],
                            [1000, 1500, 2000, 180]  ]

        # Port that the maestro is connected to = usually TTYACM0/1
        try:
            self.sc = serial.Serial(self.usbPort, timeout=1)
        except serial.SerialException:
            print("Could not open port {}".format(self.usbPort))
            exit(1)

    # Class Destructor
    def closeServo(self):
        self.sc.close()

    def CenterServos(self):
        self.setAngle(0, 0)
        self.setAngle(1, 0)
        self.setAngle(2, 0)
        self.setAngle(3, 0)
        self.setAngle(4, 0)
        self.setAngle(5, 0)

    # Set an angle between -90 (full min to +90 full max, with 0 degrees as center position
    # This checks the servo limits above to make sure it doesn't go past those
    # converts the range that the servo can go so that it is always -90 to +90 degrees
    # with 0 degrees being "center".
    # for example, a -90 on a 270 degree servo is always going to be the max the servo can go.
    def setAngle(self, chan, angle):
        # Check limits
        if angle == 0:
            ms = self.ServoLimits[chan][1]
        elif angle == -90:
            ms = self.ServoLimits[chan][0]
        elif angle == 90:
            ms = self.ServoLimits[chan][2]
        else:
            # Get new range for angle
            old_min = -90
            old_max = 90
            new_max = self.ServoLimits[chan][2]
            new_min = self.ServoLimits[chan][0]

            OldRange = (old_max - old_min)
            NewRange = (new_max - new_min)

            ms = (((angle - old_min) * NewRange) / OldRange) + new_min

        # Set the position according to the calculated ms
        print("Set servo channel {} to {} degrees ({} ms)".format(chan, angle, ms))
        self.setPosition(chan, ms)

    def valmap(value, istart, istop, ostart, ostop):
        return ostart + (ostop - ostart) * ((value - istart) / (istop - istart))

    # set definitive angle via maestro
    # Alternative to above if you are using standard 180 degree servos
    # this is the direct controlled version via pololu commands
    def setDefAngle(self, n, angle):
        if angle > 180 or angle <0:
           angle=90
        bud=chr(0xFF)+chr(n)+chr(byteone)
        self.sc.write(bud)

    # Set the servo channel to a specified ms value
    def setPosition(self, servo, microsecs):
        microsecs = microsecs * 4
        poslo = (int(microsecs) & 0x7f)
        poshi = (int(microsecs) >> 7) & 0x7f
        chan  = servo &0x7f
        data =  chr(0xaa) + chr(0x0c) + chr(0x04) + chr(chan) + chr(poslo) + chr(poshi)
        self.sc.write(data)

    # get the current position of the servo as a hi/lo nibble pair
    def getPosition(self, servo):
        chan  = servo &0x7f
        data =  chr(0xaa) + chr(0x0c) + chr(0x10) + chr(chan)
        self.sc.write(data)
        w1 = ord(self.sc.read())
        w2 = ord(self.sc.read())
        return w1, w2

    # Get error codes as a hi/lo nibble pair
    def getErrors(self):
        data =  chr(0xaa) + chr(0x0c) + chr(0x21)
        self.sc.write(data)
        w1 = ord(self.sc.read())
        w2 = ord(self.sc.read())
        return w1, w2

    # Trigger a specified subrouting stored in the maestro
    def triggerScript(self, subNumber):
        data =  chr(0xaa) + chr(0x0c) + chr(0x27) + chr(0)
        self.sc.write(data)

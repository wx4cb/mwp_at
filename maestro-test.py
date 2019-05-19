import time
from maestro import MaestroController

# Create the servo controller
print("Creating servo object")
servos = MaestroController(usbPort = "/dev/ttyACM0")

# Make a list of random bearing angles
bearings=[-69,    -74,        38,        -70,        -2,        -18,        87,        -19,        88,        -44,        -85,        69,
            56,      48,        -71,        -8,        11,        -54,        77,        20,        -3,        8,        71,
            80,      66,        -9,        -59,        22,        30,        -78,        39,        85,        -15,        13,
            -16,    72,        -40,        -63,        -41,        -7,        64,        75,        68,        -12 ]


numelements = len(bearings)

# Set through the list and move the servo to specified angle using the servo limited angle function
for i in bearings:
    servos.setAngle(0, i)
    time.sleep(0.25)

# Reset servo position to center/home
servos.setAngle(0, 0)

# close the controller
servos.closeServo()

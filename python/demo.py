from client import *

oquam = Client('cnc', 60.0)

oquam.execute('homing')

oquam.execute('moveto', x=0.1, y=0.1, speed=0.8)

oquam.execute('travel',
              path=[[0.2, 0.1, 0.0],
                    [0.2, 0.1, 0.0],
                    [0.1, 0.2, 0.0],
                    [0.0, 0.0, 0.0]],
              speed=0.7)


import gpiozero as gpio
from time import sleep
import robotAI as ai
import cv2
import math
import numpy as np



#Motor activation function
def drive(speedR, speedL):
	if (speedR >= 0):
		motorR.forward(speedR)
	else:
		motorR.backward(-speedR)
	if (speedL >= 0):
		motorL.forward(speedL)
	else:
		motorL.backward(-speedL)



#Camera lines calculation
def mapLane(lines):
    if lines is None:
        print ('No line!')
        return 200

    leftAlphaV = []
    rightAlphaV = []

    for line in lines:
        for x1, y1, x2, y2 in line:
            mmm = (y2 - y1)/(x2 - x1)
            #if math.fabs(mmm) < 0.5:
            #    continue
            mmm = -mmm
            if mmm <= 0:
                alpha = math.atan(mmm)
                alpha = math.degrees(alpha)
                rightAlphaV.append(alpha)
            else:
                alpha = math.atan(mmm)
                alpha = math.degrees(alpha)
                leftAlphaV.append(alpha)

    sumLeftAlpha = 0.0
    sumRightAlpha = 0.0

    if len(leftAlphaV) != 0:
        for alpha in leftAlphaV:
            sumLeftAlpha = sumLeftAlpha + alpha
        leftAlpha = sumLeftAlpha/(len(leftAlphaV))
    else:
        leftAlpha = 0

    if len(rightAlphaV) != 0:
        for alpha in rightAlphaV:
            sumRightAlpha = sumRightAlpha + alpha
        rightAlpha = sumRightAlpha/(len(rightAlphaV))
    else:
        rightAlpha = 0

    alphaError = rightAlpha + leftAlpha

    print ('Right alpha is: %f' % rightAlpha)
    print ('Left alpha is: %f' % leftAlpha)
    print ('Alpha error is: %f' % alphaError)
    return alphaError



#Main function
#Motors setup
motorR = gpio.Motor(18, 23, 1)
motorL = gpio.Motor(17, 22, 1)

#Camera setup
cam = cv2.VideoCapture(0)

while(True):
    #Robot starts by viewing what's ahead:
    ret, frame = cam.read()
    blackwhite = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    dummy, threshold = cv2.threshold(blackwhite, 95, 255, cv2.THRESH_BINARY)
    cannyT = cv2.Canny(threshold, 100, 120)
    #calculate and apply ROI
    roiT = cannyT[350:405, 0:640]
    lines = cv2.HoughLinesP(roiT, 2, np.pi/180, 20, 20, 2)
    alphaError = mapLane(lines)
    if alphaError == 200:
            speedR = 0
            speedL = 0
    else:
            speedR, speedL = ai.robotAI(alphaError)
            speedR = speedR/5
            speedL = speedL/5
    print ('Right motor is: %f' % speedR)
    print ('Left motor is: %f' % speedL)
    drive(speedR, speedL)

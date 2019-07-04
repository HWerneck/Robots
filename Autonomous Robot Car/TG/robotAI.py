import numpy as np

def trimf(x, abc):
    assert len(abc) == 3, 'abc parameter must have exactly three elements.'
    a, b, c = np.r_[abc]     # Zero-indexing in Python
    assert a <= b and b <= c, 'abc requires the three elements a <= b <= c.'

    y = np.zeros(len(x))

    # Left side
    if a != b:
        idx = np.nonzero(np.logical_and(a < x, x < b))[0]
        y[idx] = (x[idx] - a) / float(b - a)

    # Right side
    if b != c:
        idx = np.nonzero(np.logical_and(b < x, x < c))[0]
        y[idx] = (c - x[idx]) / float(c - b)

    idx = np.nonzero(x == b)
    y[idx] = 1
    return y



def interp_membership(x, xmf, xx):
    # Nearest discrete x-values
    x1 = x[x <= xx][-1]
    x2 = x[x >= xx][0]

    idx1 = np.nonzero(x == x1)[0][0]
    idx2 = np.nonzero(x == x2)[0][0]

    xmf1 = xmf[idx1]
    xmf2 = xmf[idx2]

    if x1 == x2:
        xxmf = xmf[idx1]
    else:
        slope = (xmf2 - xmf1) / float(x2 - x1)
        xxmf = slope * (xx - x1) + xmf1

    return xxmf



def robotAI(laneError):
    #Variables and their universe:
    laneInput = np.arange(-90, 91, 1)
    motorRight = np.arange(-1, 1, 1)
    motorLeft = np.arange(-1, 1, 1)

    #Membership functions:
    #Input membership functions
    lane_leftCurve = trimf(laneInput, [-90, -90, 0])
    lane_middle = trimf(laneInput, [-90, 0, 90])
    lane_rightCurve = trimf(laneInput, [0, 90, 90])

    #Output right motor membership function:
    motorR_backward = trimf(motorRight, [-1, -1, 0])
    motorR_forward = trimf(motorRight, [0, 1, 1])

    #Output left motor membership function:
    motorL_backward = trimf(motorLeft, [-1, -1, 0])
    motorL_forward = trimf(motorLeft, [0, 1, 1])


    #Fuzzyfication
    fuzzy_laneR = interp_membership(laneInput, lane_rightCurve, laneError)
    fuzzy_laneM = interp_membership(laneInput, lane_middle, laneError)
    fuzzy_laneL = interp_membership(laneInput, lane_leftCurve, laneError)


    #Rules
    #1. If "laneInput" = "lane_middle", motorR = "motorR_forward" AND motorL = "motorL_forward"
    middle_motorR = fuzzy_laneM
    middle_motorL = fuzzy_laneM

    #2. If "laneInput" = "lane_rightCurve", motorR = "motorR_forward" AND motorL = "motorL_forward"
    rightCurve_motorR = -fuzzy_laneR
    rightCurve_motorL = fuzzy_laneR

    #3. If "laneInput" = "lane_leftCurve", motorR = "motorR_forward" AND motorL = "motorL_forward"
    leftCurve_motorR = fuzzy_laneL
    leftCurve_motorL = -fuzzy_laneL

    #Defuzzification
    #Right motor
    motorR = middle_motorR + rightCurve_motorR + leftCurve_motorR
    #Left motor
    motorL = middle_motorL + rightCurve_motorL + leftCurve_motorL

    return motorR, motorL

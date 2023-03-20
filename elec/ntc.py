#!/usr/bin/python3
import math

# Values from NTC datasheet
# Model B57861S0103J040 R/T N°8016
T0 = -273.15
R25 = 20. # kOhm
T = [-5., 0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 55., 60., 65.]
R = [4.232, 3.265, 2.539, 1.99, 1.571, 1.249, 1.0, 0.8057, 0.6531, 0.5327, 0.4369, 0.3603, 0.2986, 0.2488, 0.2083]

def ctn_value(t):
    if (t < T[0]) or (t > T[-1]):
        raise Exception("Model is only valid between %.0f°C and %.0f°C" % (T[0], T[-1]))

    for i in range(len(T)):
        if t <= T[i]:
            break

    if i == 0:
        i += 1

    Ta = T[i - 1] - T0
    Tb = T[i] - T0
    Ra = R[i - 1]
    Rb = R[i]

    K = Ta * Tb / (Tb - Ta) * math.log(Ra / Rb)

    return R25 * Ra * math.exp(K * (1 / (t - T0) - 1 / Ta))

def ctn_temperature(x):
    r = x / R25

    if (r > R[0]) or (r < R[-1]):
        raise Exception("Model is only valid between %.0fkOhm and %.0fkOhm" % (R25 * R[-1], R25 * R[0]))

    for i in range(len(R)):
        if r >= R[i]:
            break

    if r == 0:
        r += 1

    Ta = T[i - 1] - T0
    Tb = T[i] - T0
    Ra = R[i - 1]
    Rb = R[i]

    K = Ta * Tb / (Tb - Ta) * math.log(Ra / Rb)

    return 1 / (1 / Ta + math.log(r / Ra) / K) + T0

# Algorithm to find appropriate value for MP2672 resistor values
Tcold = 0
Thot = 50
resistor_standard = 96 # Possible standard E24, E48, E96, E192
Vcold = 0.71 # See MP2671 datasheet P10
Vhot = 0.345 # See MP2671 datasheet P10

def r_e(n, m, d):
    t = round(10 ** (2 + m / n)) / 100
    return t * 10 ** d

def closest(r):
    d = math.floor(math.log10(r))
    m = round((math.log10(r) - d) * resistor_standard)
    return (resistor_standard, m, d)

RL = ctn_value(Tcold)
RH = ctn_value(Thot)

Rt1 = (1 - Vcold) * (1 - Vhot) * (RL - RH) / ((1 - Vhot) * Vcold - (1 - Vcold) * Vhot) # See MP2671 datasheet P35
Rt2 = Vcold * Rt1 / (1 - Vcold) - RL                                                   # See MP2671 datasheet P35

#(n1, m1, d1) = closest(Rt1 * 1000)
#(n2, m2, d2) = closest(Rt2 * 1000)

def dist(Tc, Th):
    return abs(Tc - Tcold) + abs(Th - Thot)

d1 = math.floor(math.log10(Rt1 * 1000))
d2 = math.floor(math.log10(Rt2 * 1000))

first = True
Tcold_e = Thot
Thot_e = Tcold

for d1 in range(d1 - 2, d1 + 2):
    for d2 in range(d1 - 2, d1 + 2):
        for m1 in range(resistor_standard):
            for m2 in range(resistor_standard):
                _Rt1_e = r_e(resistor_standard, m1, d1) / 1000
                _Rt2_e = r_e(resistor_standard, m2, d2) / 1000

                _RL_e = Vcold / (1 - Vcold) * _Rt1_e - _Rt2_e
                _RH_e = Vhot / (1 - Vhot) * _Rt1_e - _Rt2_e

                try:
                    _Tcold_e = ctn_temperature(_RL_e)
                    _Thot_e = ctn_temperature(_RH_e)
                except:
                    continue

                if (_Tcold_e > Tcold) and (_Thot_e < Thot):
                    if first or (dist(_Tcold_e, _Thot_e) < dist(Tcold_e, Thot_e)):
                        Tcold_e = _Tcold_e
                        Thot_e = _Thot_e

                        Rt1_e = _Rt1_e
                        Rt2_e = _Rt2_e

print("Theoretical: Rt1 = %.2f kOhm | Rt2 = %.2f kOhm" % (Rt1, Rt2))
print("Closest in E%d serie: Rt1 = %.2f kOhm | Rt2 = %.2f kOhm" % (resistor_standard, Rt1_e, Rt2_e))
print("Actual temperature range [%.1f : %.1f]" % (Tcold_e, Thot_e))
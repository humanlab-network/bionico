#ifndef _CONTROL_CONFIG_H_
#define _CONTROL_CONFIG_H_

/* --- PERIOD CONFIGURATION--- */
// [us], Sampling period in microseconds
#define PERIOD_US 10000
// [s], Sampling Period for all the programm
#define PERIOD_SEC (PERIOD_US / 1000000.0)

/* --- EMG CLOSE CONFIGURATION--- */
// [V], Dead zone to filter noise : EMG signals under limit go to zero
#define EMG_CLOSE_MIN_VALUE 0.2
// [V], set the max value that EMG reach to scale command on it
#define EMG_CLOSE_MAX_VALUE 3.3

/* --- EMG OPEN CONFIGURATION--- */
// [V], Dead zone to filter noise : EMG signals under limit go to zero
#define EMG_OPEN_MIN_VALUE 0.2
// [V], set the max value that EMG reach to scale command on it
#define EMG_OPEN_MAX_VALUE 3.3

/* --- FILTERING--- */
// Using lowpass 1st order filter : alpha = dt / (tau + dt)
// [-], set the alpha coefficient for emg filter, with tau = 0.09sec
#define EMG_ALPHA_FILTER 0.1
// [-], set the alpha coefficient for battery high frequency filter, with tau = 0.03sec
#define VOLTAGE_ALPHA_HIGHFREQ_FILTER 0.25
// [-], set the alpha coefficient for battery low frequency filter, with tau = 30sec
#define VOLTAGE_ALPHA_LOWFREQ_FILTER 0.000333
// [-], set the alpha coefficient for current filter, with tau = 0.03sec
#define CURRENT_ALPHA_FILTER 0.25
// [-], set the alpha coefficient for current ratio filter, with tau = 0.03sec
#define CURRENT_RATIO_ALPHA_FILTER 0.25

/* --- COMMAND SIGNAL CONFIGURATION--- */
// [cmd_unit] maximum Command possible
#define MAX_CMD 100.0
// [cmd_unit], Command scale if needed (max = MAX_CMD)
#define SPEED_LIMIT_FLOAT (0.5 * MAX_CMD)
// [cmd_unit], Command scale if needed (max = MAX_CMD)
#define EMG_SCALE_FLOAT 2.0

/* --- CONTROL ALGORITHM CONFIGURATION--- */
// [cmd_unit] Maximum acceleration of command (exprimed by a delta between 2 cmd)
#define CMD_MAX_ACCEL_STEP (3.5 * MAX_CMD * PERIOD_SEC)
// [cmd_unit] Maximum deceleration of command (exprimed by a delta between 2 cmd)
#define CMD_MAX_DECEL_STEP (5.0 * MAX_CMD * PERIOD_SEC)
// [-] Stall ratio threshold to consider motor as quasi stalled
#define STALL_RATIO_THOLD 0.85
// [-] Command percentage to consider that user did not released muscles after a stall state of motor
#define STALL_CMD_MARGIN 1.1
// [period] Time to apply stall torque
#define STALL_DETECTION_TIME 5
// [period] Time to stay in "Long stall mode"
#define STALL_TIMER_LONG 75
// [period] Time to stay in "Short stall mode"
#define STALL_TIMER_SHORT 30

/* --- MOTOR THERMAL LIMITS --- */
// [°C], Stator temperature limit
#define STATOR_TEMP_LIMIT 70.0
// [°C], Estimation of outside temperature
#define ROTOR_TEMP_LIMIT 90.0
// [°C], Estimation of outside temperature
#define DELTA_TEMP_LIMIT 40.0

/* --- MOTOR GLOBAL CHARACTERISTICS --- */
// [Ohm], Motor resistance
#define R_MOT 0.65
// [A], Maximum continuous current of motor
#define SAFE_CURRENT 1.50
// [A], Minimum voltage of battery authorized to protect the battery
#define MIN_VOLTAGE 7.0

/* --- MOTOR THERMAL MODEL --- */
// [°C], Estimation of outside temperature
#define T_EXT 20.0
// [°C/W] Thermal resistance between rotor and stator
#define RTH_ROT 6.0
// [s] Thermal time constant of the rotor
#define TAU_ROT 9.0
// [°C/W] Thermal resistance between stator and the exterior
#define RTH_ST 7.0
// [s] Thermal time constant of the rotor
#define TAU_ST 750.0

#endif
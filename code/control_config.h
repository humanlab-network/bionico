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
#define EMG_CLOSE_MAX_VALUE 2.0

/* --- EMG OPEN CONFIGURATION--- */
// [V], Dead zone to filter noise : EMG signals under limit go to zero
#define EMG_OPEN_MIN_VALUE 0.2
// [V], set the max value that EMG reach to scale command on it
#define EMG_OPEN_MAX_VALUE 2.0

/* --- FILTERING--- */
// Using lowpass 1st order filter : alpha = dt / (tau + dt)
// [-], set the alpha coefficient for emg filter, with tau = 0.09sec
#define EMG_ALPHA_FILTER 0.2
// [-], set the alpha coefficient for battery high frequency filter, with tau = 0.03sec
#define VOLTAGE_ALPHA_HIGHFREQ_FILTER 0.25
// [-], set the alpha coefficient for battery low frequency filter, with tau = 30sec
#define VOLTAGE_ALPHA_LOWFREQ_FILTER 0.000333
// [-], set the alpha coefficient for current filter, with tau = 0.03sec
#define CURRENT_ALPHA_FILTER 0.25
// [-], set the alpha coefficient for current ratio filter, with tau = 0.03sec
#define CURRENT_RATIO_ALPHA_FILTER 0.25
// [-], set the alpha coefficient for command emg filter, with tau = 0.09sec
#define CMD_EMG_ALPHA_FILTER 0.1

/* --- COMMAND SIGNAL CONFIGURATION--- */
// [cmd_unit] maximum Command possible
#define MAX_CMD 100.0
// [cmd_unit], Command scale if needed (max = MAX_CMD)
#define SPEED_LIMIT_FLOAT (1.0 * MAX_CMD)
// [cmd_unit], Command scale if needed (max = MAX_CMD)
#define EMG_SCALE_FLOAT 1.0

/* --- CONTROL ALGORITHM CONFIGURATION--- */
// [cmd_unit] Maximum acceleration of command (exprimed by a delta between 2 cmd)
#define CMD_MAX_ACCEL_STEP (3.5 * MAX_CMD * PERIOD_SEC)
// [cmd_unit] Maximum deceleration of command (exprimed by a delta between 2 cmd)
#define CMD_MAX_DECEL_STEP (5.0 * MAX_CMD * PERIOD_SEC)
// [-] Stall ratio threshold to consider motor as quasi stalled
#define STALL_RATIO_THOLD 0.85
// [-] Command percentage to consider that user did not released muscles after a stall state of motor
#define STALL_CMD_MARGIN 1.05
//
#define EMG_DERIVATIVE_THRESHOLD 0.01
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
#define R_MOT 1.94
// [A], Maximum continuous current of motor
#define SAFE_CURRENT 1.50
// [A], Maximum stall current specified if virtual resistance is used
#define VIRTUAL_STALL_CURRENT 2.50
// [Ohm], Corresponding virtual resistance at voltage of 7.8V
#define VIRTUAL_R_MOT (7.8 / VIRTUAL_STALL_CURRENT)
// [A], Minimum voltage of battery authorized to protect the battery
#define MIN_VOLTAGE 7.0
// [Ah], Full capacity of the battery
#define BATTERY_FULL_CAPACITY 3.0

/* --- MOTOR THERMAL MODEL --- */
// [°C], Estimation of outside temperature
#define T_EXT 25.0
// [°C/W] Thermal resistance between rotor and stator
#define RTH_ROT 5.0
// [s] Thermal time constant of the rotor
#define TAU_ROT 6.8
// [°C/W] Thermal resistance between stator and the exterior
#define RTH_ST 15.0 // 20.0 if motor is alone
// [s] Thermal time constant of the rotor
#define TAU_ST 800.0 // 440.0 is motor is alone

/* --- LED SETTINGS --- */
#define LED_OFF_PWM 100
#define LED_ON_PWM_GREEN 90
#define LED_ON_PWM_RED 95

#endif
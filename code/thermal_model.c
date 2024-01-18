#include "thermal_model.h"

#include "control_config.h"

void run_thermal_model(float *rotor_temp, float *prev_rotor_temp, float *stator_temp, float *prev_stator_temp, float *motor_current)
{
    float dT_rotor = *motor_current * *motor_current * (R_MOT * (RTH_ROT / TAU_ROT)) - (*prev_rotor_temp - *prev_stator_temp) / TAU_ROT;
    *rotor_temp = *prev_rotor_temp + dT_rotor * PERIOD_SEC;

    float dT_stator = ((RTH_ST / TAU_ST) / RTH_ROT) * (*prev_rotor_temp - *prev_stator_temp) - (*prev_stator_temp - T_EXT) / TAU_ST;
    *stator_temp = *prev_stator_temp + dT_stator * PERIOD_SEC;
}
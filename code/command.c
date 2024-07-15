#include <math.h>
#include "control_config.h"
#include "signal.h"
#include "command.h"

void command_from_emg(float emg_close_filtered, float emg_open_filtered, float *command, int8_t *direction)
{
    float emg_open_cmd = 0.0;
    float emg_close_cmd = 0.0;

    // If the 2 EMG signals are under MIN Threshold, stop the motor
    if (emg_close_filtered < EMG_CLOSE_MIN_VALUE && emg_open_filtered < EMG_OPEN_MIN_VALUE)
    {
        *command = 0.0;
    }
    else
    {
        // Detect which EMG signal is stronger than the other, and constrain its boundaries
        if (emg_close_filtered >= EMG_CLOSE_MIN_VALUE)
        {
            emg_close_cmd = saturate(emg_close_filtered, EMG_CLOSE_MIN_VALUE, EMG_CLOSE_MAX_VALUE);
            // remap EMG signal linearly from [EMG_MIN; EMG_MAX] to [0; SPEED_LIMIT_FLOAT]
            emg_close_cmd = (emg_close_cmd - EMG_CLOSE_MIN_VALUE) * (SPEED_LIMIT_FLOAT / (EMG_CLOSE_MAX_VALUE - EMG_CLOSE_MIN_VALUE));
        } // else 0
        if (emg_open_filtered >= EMG_OPEN_MIN_VALUE)
        {
            emg_open_cmd = saturate(emg_open_filtered, EMG_OPEN_MIN_VALUE, EMG_OPEN_MAX_VALUE);
            // remap EMG signal linearly from [EMG_MIN; EMG_MAX] to [0; SPEED_LIMIT_FLOAT]
            emg_open_cmd = (emg_open_cmd - EMG_OPEN_MIN_VALUE) * (SPEED_LIMIT_FLOAT / (EMG_OPEN_MAX_VALUE - EMG_OPEN_MIN_VALUE));
        } // else 0

        if (emg_close_filtered > emg_open_filtered)
        {
            *command = emg_close_cmd;
            *direction = DIR_CLOSE;
        }
        else if (emg_open_filtered >= emg_close_filtered)
        {
            *command = emg_open_cmd;
            *direction = DIR_OPEN;
        }

#ifdef SQUARED_SHAPE
        // apply a "square shape", keeping same boundaries, to be more precise with
        // low EMG signals
        *command = (1.0 / SPEED_LIMIT_FLOAT) * SQR(*command);
#endif
#ifdef SQRT_SHAPE
        // apply a "square shape", keeping same boundaries, to be more precise with
        // low EMG signals
        *command = sqrt(1.0 * SPEED_LIMIT_FLOAT) * sqrt(*command);
#endif
    }
}
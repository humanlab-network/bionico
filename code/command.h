#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>

/**
 * Generate proportionnal command and direction from two EMG signals
 */
void command_from_emg(float emg_close_filtered, float emg_open_filtered, float *command, int8_t *direction);

#endif
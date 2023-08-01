#include <stdio.h>
#include "PID_controller.h"

float pid_update(pidInfo* pid_info) {
    float error = pid_info->aim - pid_info->current;
    pid_info->cumsum_error += error * pid_info->dt;
    float dericative = (error - pid_info->previous_error) / pid_info->dt;
    float output = pid_info->kp * error + pid_info->ki * pid_info->cumsum_error + pid_info->kd * dericative;
    pid_info->previous_error = error;
    pid_info->pwm = output;
    // printf("aim: %f, current: %f, error: %f, kp: %f, output: %f\n", pid_info->aim, pid_info->current, error, pid_info->kp, output);
    return output;
}

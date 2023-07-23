

typedef struct {
    float aim;
    float current;
    float kp;
    float ki;
    float kd;
    float cumsum_error;
    float previous_error;
    float dt;
} pidInfo;


float pid_update(pidInfo* pid_info);


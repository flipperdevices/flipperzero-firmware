#ifndef IMU_T_H   /* Include guard */
#define IMU_T_H

struct imu_t
{
    unsigned int address;
    bool (*begin)(void);
    void (*end)(void);
    int (*read)(double* vec);
    char* name;
};

#endif
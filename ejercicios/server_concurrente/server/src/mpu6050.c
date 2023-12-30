
#include "mpu6050.h"

// #define SENSOR_PATH "/dev/urandom"
#define SENSOR_PATH "/dev/MPU6050"



int read_sensor(char *buffer, double * coefFilter)
{
    int retVal = -1;
    ssize_t bytesRead;
    static int i = 0;
    float acc_modifier, gyro_modifier;
    float raw_accel_xout[12], raw_accel_yout[12], raw_accel_zout[12], raw_gyro_xout[12], raw_gyro_yout[12], raw_gyro_zout[12], raw_temp_out[12];
    float mva_accel_xout[12], mva_accel_yout[12], mva_accel_zout[12], mva_gyro_xout[12], mva_gyro_yout[12], mva_gyro_zout[12], mva_temp_out[12];
    float raw[7];
    int driver;
    char buff[PACKET_SIZE];

    static int filtroCargado = 0;

    if((driver = open(SENSOR_PATH, O_RDONLY)) == -1)
    {
        perror("Error while opening devchar ");
        goto error;
    } 

    // if((acc_modifier = ioctl(driver,0)/1) == -1) 
    // {
    //     perror("Error while getting acc_modifier "); 
    //     goto error;
    // }
    
    // if((gyro_modifier = ioctl(driver,1)/10) == -1) 
    // {
    //     perror("Error while getting gyro_modifier ");
    //     goto error;
    // }

    acc_modifier = 1;
    gyro_modifier = 1;
    

    if((bytesRead = read(driver, buff, sizeof(buff))) == -1)
    {
        perror("Error while reading devchar "); 
        goto error;
    }

    
    // Sets data
    raw[0] = raw_accel_xout[i] = (float) ((short int)((buff[0] << 8) | buff[1]) / acc_modifier);
    raw[1] = raw_accel_yout[i] = (float) ((short int)((buff[2] << 8) | buff[3]) / acc_modifier);
    raw[2] = raw_accel_zout[i] = (float) ((short int)((buff[4] << 8) | buff[5]) / acc_modifier);

    raw[3] = raw_temp_out[i] = (float) (((short int)((buff[6] << 8) | buff[7]) / 340) + 36.53);

    raw[4] = raw_gyro_xout[i] = (float) ((short int)((buff[8] << 8)  | buff[9])  / gyro_modifier);
    raw[5] = raw_gyro_yout[i] = (float) ((short int)((buff[10] << 8) | buff[11]) / gyro_modifier);
    raw[6] = raw_gyro_zout[i] = (float) ((short int)((buff[12] << 8) | buff[13]) / gyro_modifier);
    

    // printf("ACC X: %.5f - Filtrado: %.5f\n", raw[0], mva_accel_xout[11]);
    // printf("ACC Y: %.5f - Filtrado: %.5f\n", raw[1], mva_accel_yout[11]);
    // printf("ACC Z: %.5f - Filtrado: %.5f\n", raw[2], mva_accel_zout[11]);
    // printf("\n");
    // printf("TEMP: %.5f - Filtrado: %.5f\n", raw[3], mva_temp_out[11]);
    // printf("\n");
    // printf("GYRO X: %.5f - Filtrado: %.5f\n", raw[4], mva_gyro_xout[11]);
    // printf("GYRO Y: %.5f - Filtrado: %.5f\n", raw[5], mva_gyro_yout[11]);
    // printf("GYRO Z: %.5f - Filtrado: %.5f\n", raw[6], mva_gyro_zout[11]);

    // Updates index for filter input data
    i++;
    i%=12;
    if (i==0) filtroCargado = 1;

    // Gets filter output
    if (filtroCargado)
    {
        // sprintf(buffer, "%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n",raw[0], raw[1], raw[2], raw[4], raw[5], raw[5], raw[3], mva_accel_xout[11], mva_accel_yout[11], mva_accel_zout[11], mva_gyro_xout[11], mva_gyro_yout[11], mva_gyro_zout[11], mva_temp_out[11]);
        sprintf(buffer, "%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n%.5f\n",raw[0], raw[1], raw[2], raw[4], raw[5], raw[5], raw[3], 0, 0, 0, 0, 0, 0, 0);
        retVal = bytesRead;
    }else{
        retVal = 0;
    }


    error: close(driver);
    return retVal;
}
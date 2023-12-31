
#include "mpu6050.h"

#define SENSOR_PATH "/dev/MPU6050"


int read_sensor(data_t *datos, double * coefFilter)
{
    /*******************************************************************************
    * This is a low level read operation. 
    * 
    * @param  datos Array of data to be filled.
    * @param  coefFilter Array with all the filter coefficients.
    * 
    * @return None.
    ******************************************************************************/

    int retVal = -1;
    ssize_t bytesRead;
    static int i = 0;
    float raw_accel_xout[12], raw_accel_yout[12], raw_accel_zout[12], raw_gyro_xout[12], raw_gyro_yout[12], raw_gyro_zout[12], raw_temp_out[12];
    float mva_accel_xout[12], mva_accel_yout[12], mva_accel_zout[12], mva_gyro_xout[12], mva_gyro_yout[12], mva_gyro_zout[12], mva_temp_out[12];
    float raw[7];
    int mpu;
    char buff[PACKET_SIZE];

    static int filtroCargado = 0;

    // Open chardev
    if((mpu = open(SENSOR_PATH, O_RDONLY)) == -1)
    {
        perror("Error while opening devchar ");
        goto error;
    }

    // Reads raw data
    if((bytesRead = read(mpu, buff, sizeof(buff))) == -1)
    {
        perror("Error while reading devchar "); 
        goto error;
    }

    
    // Computes raw data
    raw[0] = raw_accel_xout[i] = (float) ((short int)((buff[0] << 8) | buff[1]) / 16384);
    raw[1] = raw_accel_yout[i] = (float) ((short int)((buff[2] << 8) | buff[3]) / 16384);
    raw[2] = raw_accel_zout[i] = (float) ((short int)((buff[4] << 8) | buff[5]) / 16384);

    raw[3] = raw_temp_out[i] = (float) (((short int)((buff[6] << 8) | buff[7]) / 340) + 36.53);

    raw[4] = raw_gyro_xout[i] = (float) ((short int)((buff[8] << 8)  | buff[9])  / 131);
    raw[5] = raw_gyro_yout[i] = (float) ((short int)((buff[10] << 8) | buff[11]) / 131);
    raw[6] = raw_gyro_zout[i] = (float) ((short int)((buff[12] << 8) | buff[13]) / 131);

    // Updates Filter
    fir_filter_neon(raw_accel_xout, coefFilter, mva_accel_xout, 12);
    fir_filter_neon(raw_accel_yout, coefFilter, mva_accel_yout, 12);
    fir_filter_neon(raw_accel_zout, coefFilter, mva_accel_zout, 12);

    fir_filter_neon(raw_temp_out, coefFilter, mva_temp_out, 12);

    fir_filter_neon(raw_gyro_xout, coefFilter, mva_gyro_xout, 12);
    fir_filter_neon(raw_gyro_yout, coefFilter, mva_gyro_yout, 12);
    fir_filter_neon(raw_gyro_zout, coefFilter, mva_gyro_zout, 12);


    // Updates index for filter input data
    i++;
    i%=12;
    if (i==0) filtroCargado = 1;

    // Returns data
    if (filtroCargado)
    {
        datos->rax  = raw[0];
        datos->ray  = raw[1];
        datos->raz  = raw[2];
        datos->rtmp = raw[3];
        datos->rgx  = raw[4];
        datos->rgy  = raw[5];
        datos->rgz  = raw[6];

        datos->fax  = mva_accel_xout[11];
        datos->fay  = mva_accel_yout[11];
        datos->faz  = mva_accel_zout[11];
        datos->ftmp = mva_temp_out[11];
        datos->fgx  = mva_gyro_xout[11];
        datos->fgy  = mva_gyro_yout[11];
        datos->fgz  = mva_gyro_zout[11];

        
        retVal = 1; // We only read one data frame
    }else{
        retVal = 0;
    }


    error: close(mpu);
    return retVal;
}
/*******************************************************
 * Initialization and Configuration for MPU Sensor Read *
 *******************************************************/

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "platform.h"
#include "xil_printf.h"
#include "xiic.h"
#include "xparameters.h"

// Commands for MPU initialization and data reading
u8 MPU_init[] = {0x6B, 0};
u8 MPU_read[] = {0x3B};

// Calibration constants for sensor data
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int AcXcal = -950, AcYcal = -300, AcZcal = 0;
int tcal = -1600;
int GyXcal = 480, GyYcal = 170, GyZcal = 210;
double t, tx, pitch, roll;

/*******************************************************************
 * Function to Calculate Pitch and Roll Angles from Accelerometer  *
 *******************************************************************/
void getAngle(int Ax, int Ay, int Az) {
    pitch = atan((double)Ax / sqrt(Ay * Ay + Az * Az)) * 180.0 / M_PI;
    roll = atan((double)Ay / sqrt(Ax * Ax + Az * Az)) * 180.0 / M_PI;
}

int main() {
    u8 DataBuffer[14];  // Buffer to store sensor data

    // Platform-specific initialization
    init_platform();

    // Send initialization command to MPU
    XIic_Send(XPAR_IIC_0_BASEADDR, 0x68, MPU_init, 2, XIIC_STOP);

    // Display startup messages
    printf("Hello World\n\r");
    printf("Successfully ran Hello World application\n\r");

    // Main loop for continuous sensor reading and display
    while (1) {
        // Send read command to MPU
        XIic_Send(XPAR_IIC_0_BASEADDR, 0x68, MPU_read, 1, XIIC_REPEATED_START);
        XIic_Recv(XPAR_IIC_0_BASEADDR, 0x68, DataBuffer, 14, XIIC_STOP);

        // Parse accelerometer data
        AcX = (DataBuffer[0] << 8) | DataBuffer[1];
        AcY = (DataBuffer[2] << 8) | DataBuffer[3];
        AcZ = (DataBuffer[4] << 8) | DataBuffer[5];
        // Parse temperature data
        Tmp = (DataBuffer[6] << 8) | DataBuffer[7];
        // Parse gyroscope data
        GyX = (DataBuffer[8] << 8) | DataBuffer[9];
        GyY = (DataBuffer[10] << 8) | DataBuffer[11];
        GyZ = (DataBuffer[12] << 8) | DataBuffer[13];

        // Adjust and convert temperature
        tx = Tmp + tcal;
        t = tx / 340 + 36.53;

        // Calculate pitch and roll from accelerometer data
        getAngle(AcX, AcY, AcZ);

        // Display calculated angles, adjusted sensor values, and temperature
        printf("Angle: Pitch = %.2f Roll = %.2f\n\r", pitch, roll);
        printf("Accelerometer: X = %d Y = %d Z = %d\n\r", (AcX + AcXcal), (AcY + AcYcal), (AcZ + AcZcal));
        printf("Temperature in Celsius = %.2f\n\r", t);
        printf("Gyroscope: X = %d Y = %d Z = %d\n\r", (GyX + GyXcal), (GyY + GyYcal), (GyZ + GyZcal));

        // Clear terminal screen and wait for a second
        printf("\033[H");
        sleep(1);
    }

    // Clean up and exit program
    cleanup_platform();
    return 0;
}


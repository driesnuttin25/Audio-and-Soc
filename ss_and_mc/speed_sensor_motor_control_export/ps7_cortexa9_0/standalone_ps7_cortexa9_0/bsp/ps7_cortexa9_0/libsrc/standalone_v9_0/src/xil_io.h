/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "speed_sensor.h"

int main()
{
    init_platform();
    unsigned int value = 0;
    print("Speed Sensor test \n\r");
	SPEED_SENSOR_mWriteReg(XPAR_SPEED_SENSOR_0_S00_AXI_BASEADDR,SPEED_SENSOR_S00_AXI_SLV_REG1_OFFSET,0b1111);

    for(;;)
    {
    	sleep_A9(1);
    	value = SPEED_SENSOR_mReadReg(XPAR_SPEED_SENSOR_0_S00_AXI_BASEADDR,SPEED_SENSOR_S00_AXI_SLV_REG2_OFFSET);
        printf("Pulses: [%d]\n\r",value);
    }
    cleanup_platform();
    return 0;
}

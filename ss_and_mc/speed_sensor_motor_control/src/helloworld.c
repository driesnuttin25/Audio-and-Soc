#include "xparameters.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "speed_sensor.h"

#define SCUGIC_DEVICE_ID 	XPAR_SCUGIC_SINGLE_DEVICE_ID
#define AXI_TIMER_DEVICE_ID XPAR_AXI_TIMER_0_DEVICE_ID
#define AXI_TIMER_IRPT_INTR XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR

XTmrCtr	xTmrCtr_Inst;
XScuGic xScuGic_Inst;

#define AXI_TIMER_CHANNEL_1	0
#define AXI_TIMER_CHANNEL_2	1

#define AXI_TIMER_PWM_TEST_SW	1

#define AXI_TIMER_PERIOD_US	10000

#define AXI_TIMER_PWM_HIGH_TIME_US	5000

void xTmrCtr_Int_Handler(void *CallBackRef, u8 TmrCtrNumber)
{
	//xil_printf("AXI Timer Int! Channel: %d\n", TmrCtrNumber+1);

}

u32 xTmr_US_To_RegValue(u32 US)
{
	u32 Value;
	Value = 50*US;
	return 0xFFFFFFFF - Value;
}

u32 xTmr_US_To_NS(u32 US)
{
	return US*1000;
}


int xTmrCtr_Init(XTmrCtr *xTmrCtr_Ptr, u32 DeviceId)
{
	int Status, DutyCycle;

	Status = XTmrCtr_Initialize(xTmrCtr_Ptr, DeviceId);
	if(Status != XST_SUCCESS)
		{
		return XST_FAILURE;
		}

		Status = XTmrCtr_SelfTest(xTmrCtr_Ptr, 2-1);
		if(Status != XST_SUCCESS)
			{
			return XST_FAILURE;
			}


		XTmrCtr_SetHandler(xTmrCtr_Ptr,xTmrCtr_Int_Handler,xTmrCtr_Ptr);

		XTmrCtr_SetResetValue(xTmrCtr_Ptr,AXI_TIMER_CHANNEL_1,xTmr_US_To_RegValue (AXI_TIMER_PERIOD_US));

		XTmrCtr_SetResetValue(xTmrCtr_Ptr, AXI_TIMER_CHANNEL_2,xTmr_US_To_RegValue (AXI_TIMER_PERIOD_US));

		XTmrCtr_SetOptions(xTmrCtr_Ptr, AXI_TIMER_CHANNEL_1, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION); XTmrCtr_SetOptions(xTmrCtr_Ptr, AXI_TIMER_CHANNEL_2, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

			#if AXI_TIMER_PWM_TEST_SW
			DutyCycle = XTmrCtr_PwmConfigure(xTmrCtr_Ptr, xTmr_US_To_NS (AXI_TIMER_PERIOD_US),xTmr_US_To_NS (AXI_TIMER_PWM_HIGH_TIME_US));
			xil_printf("AXI Timer PWM DutyCycle: %d%!\n",DutyCycle);
			#endif
			return XST_SUCCESS;
}

int xScuGic_Init(XScuGic *ScuGic_Ptr,XTmrCtr *xTmrCtr_Ptr)
{
	int Status;
	XScuGic_Config *intc_cfg_ptr;
	intc_cfg_ptr = XScuGic_LookupConfig(SCUGIC_DEVICE_ID);
	if(intc_cfg_ptr == NULL)
	{
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(ScuGic_Ptr, intc_cfg_ptr,intc_cfg_ptr->CpuBaseAddress);

	if(Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, ScuGic_Ptr);
	Xil_ExceptionEnable();
	XScuGic_Connect(ScuGic_Ptr, AXI_TIMER_IRPT_INTR, (Xil_ExceptionHandler)XTmrCtr_InterruptHandler, (void *)xTmrCtr_Ptr);
	XScuGic_Enable(ScuGic_Ptr, AXI_TIMER_IRPT_INTR);
	return XST_SUCCESS;
}

int main() {
    u32 Status;
    unsigned int value = 0;

    xil_printf("AXI Timer and Speed Sensor Test!\n");

    // Initialize and start the PWM timer
    Status = xTmrCtr_Init(&xTmrCtr_Inst, AXI_TIMER_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        xil_printf("AXI Timer Init Error!\n");
        return XST_FAILURE;
    }

    // Initialize the GIC for the timer interrupt
    Status = xScuGic_Init(&xScuGic_Inst, &xTmrCtr_Inst);
    if (Status != XST_SUCCESS) {
        xil_printf("AXI Timer Init Error!\n");
        return XST_FAILURE;
    }

    // Start the PWM channels
    XTmrCtr_Start(&xTmrCtr_Inst, AXI_TIMER_CHANNEL_1);
    XTmrCtr_Start(&xTmrCtr_Inst, AXI_TIMER_CHANNEL_2);

    // Enable PWM if required
    #if AXI_TIMER_PWM_TEST_SW
    XTmrCtr_PwmDisable(&xTmrCtr_Inst);
    XTmrCtr_PwmEnable(&xTmrCtr_Inst);
    #endif

    // Configure speed sensor register (assuming this is necessary for starting the sensor)
    SPEED_SENSOR_mWriteReg(XPAR_SPEED_SENSOR_0_S00_AXI_BASEADDR, SPEED_SENSOR_S00_AXI_SLV_REG1_OFFSET, 0b1111);

    // Main application loop
    while (1) {
        // Sleep for a certain period - this could be adjusted based on how often you need to read the sensor
        sleep_A9(1);

        // Read the pulse count from the speed sensor
        value = SPEED_SENSOR_mReadReg(XPAR_SPEED_SENSOR_0_S00_AXI_BASEADDR, SPEED_SENSOR_S00_AXI_SLV_REG2_OFFSET);
        xil_printf("Pulses: [%u]\n\r", value);
    }

    // Cleanup and exit (this part of the code will never be reached in a properly running embedded application)
    cleanup_platform();
    return 0;
}

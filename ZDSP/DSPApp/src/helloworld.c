#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "audio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "math.h"

#define UINT32_MAX_AS_FLOAT 4294967295.0f //(2^32 - 1
#define UINT_SCALED_MAX_VALUE 0xFFFFF // 2^24 =>24 bits audio codec maximum value is 0xFF FFFF

#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR

#define SAMPLE_RATE 		48000 // Sampling rate of audio input, is also used for generating an interrupt at this frequency. 48kHz.


volatile unsigned long u32DataL, u32DataR;


// Timer_ISR for sine generation (no LUT, our processor seems to be fast enough ;-) )
static void Timer_ISR(void * CallBackRef)
{
	const float32_t frequency = 1500; //audio frequency to generate
	const float32_t theta_increment = 2 * PI * frequency / SAMPLE_RATE;
	static float32_t theta = 0.0f;

	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
	XScuTimer_ClearInterruptStatus(TimerInstancePtr);

	theta += theta_increment ;
	if ( theta > 2* PI)
		theta -= 2* PI;

	//float sine_value = sinf(theta); // non CMSIS function comment out for using
	float32_t sine_value = arm_sin_f32(theta); // CMSIS function
	uint32_t scaled_value = (uint32_t)(((sine_value + 1.0f) * 0.5f) * UINT_SCALED_MAX_VALUE);
	Xil_Out32(I2S_DATA_TX_R_REG, scaled_value);
}

static int Timer_Intr_Setup(XScuGic * IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
	int Status;
	XScuGic_Config *IntcConfig;
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
	// Step 1: Interrupt Setup
	Xil_ExceptionInit();
	// Step 2: Interrupt Setup
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler,IntcInstancePtr);
	// Step 3: Interrupt Setup
	Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId, (Xil_ExceptionHandler)Timer_ISR, (void *)TimerInstancePtr);
	// Step 4: Interrupt Setup
	XScuGic_Enable(IntcInstancePtr, TimerIntrId);
	// Step 5:
	XScuTimer_EnableInterrupt(TimerInstancePtr);
	// Step 6: Interrupt Setup
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

int main()
{
	  int Status;
	  init_platform();
	  IicConfig(XPAR_XIICPS_0_DEVICE_ID);
	  AudioPllConfig();
	  AudioConfigureJacks();
	  LineinLineoutConfig();

	  XScuTimer Scu_Timer;
	  XScuTimer_Config *Scu_ConfigPtr;
	  XScuGic IntcInstance;

	  Scu_ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	  Status = XScuTimer_CfgInitialize(&Scu_Timer, Scu_ConfigPtr, Scu_ConfigPtr->BaseAddr);
	  Status = Timer_Intr_Setup(&IntcInstance, &Scu_Timer, XPS_SCU_TMR_INT_ID);
	  XScuTimer_LoadTimer(&Scu_Timer,(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)/(SAMPLE_RATE));
	  XScuTimer_EnableAutoReload(&Scu_Timer);
	  XScuTimer_Start(&Scu_Timer);
	  for(;;){

	  }
    cleanup_platform();
    return 0;
}

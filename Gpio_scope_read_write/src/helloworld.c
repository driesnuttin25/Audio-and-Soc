/* Hello there
Ok dus wa ge moet doen dries als ge GPIO pin wilt aanzetten is het volgende:

Define de GPIO device Id:

		#ifndef GPIO_DEVICE_ID
		#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
		#endif



Define de bank dat wordt gebruikt:

		#define GPIOPS_BANK XGPIOPS_BANK2



Zet een pointer:

		XGpioPs GpioPs;



Zet de direction en output afhankelijk van de bank.
Zoek hierbij in de xdc file om de pin te weten (15)
(15) pin = 16e bit van de 32 van de bank. 16e bit togglen van binary:
00000000000000001000000000000000 = 0x8000 in hex

		XGpioPs_SetDirection(&GpioPs, GPIOPS_BANK, 0x8000);
	    XGpioPs_SetOutputEnable(&GpioPs, GPIOPS_BANK, 0x8000);



Pointer naar de gpio_device:

		XGpioPs_Config *ConfigPtr;

    	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    	Status = XGpioPs_CfgInitialize(&GpioPs, ConfigPtr, ConfigPtr->BaseAddr);
    	if (Status != XST_SUCCESS){
    		return XST_FAILURE;
    	}



Writing en reading:

  		int currentState = XGpioPs_Read(&GpioPs, GPIOPS_BANK);
  		XGpioPs_Write(&GpioPs, GPIOPS_BANK, ~currentState);

*/
#include "audio.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xgpiops.h"

#ifndef GPIO_DEVICE_ID
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#endif

#define GPIOPS_BANK XGPIOPS_BANK2

#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE	0xFFFF
#define AMPLITUDE 200000
#define SAMPLE_RATE 48000

volatile unsigned long u32DataL, u32DataR;
volatile unsigned long u32Temp;
volatile int Timer_Intr_rcvd;

XGpioPs GpioPs;
// {GPIO_0_0_tri_io[15]}

static void Timer_ISR(void * CallBackRef)
{
  	    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
  		XScuTimer_ClearInterruptStatus(TimerInstancePtr);
  		u32DataL = Xil_In32(I2S_DATA_RX_L_REG);
  		Xil_Out32(I2S_DATA_TX_L_REG, u32DataL);

  		int currentState = XGpioPs_Read(&GpioPs, GPIOPS_BANK);
  		XGpioPs_Write(&GpioPs, GPIOPS_BANK, ~currentState);
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
    XGpioPs_Config *ConfigPtr;

    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    	Status = XGpioPs_CfgInitialize(&GpioPs, ConfigPtr, ConfigPtr->BaseAddr);
    	if (Status != XST_SUCCESS){
    		return XST_FAILURE;
    	}
    XGpioPs_SetDirection(&GpioPs, GPIOPS_BANK, 0x8000);
	XGpioPs_SetOutputEnable(&GpioPs, GPIOPS_BANK, 0x8000);
	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();

	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that
	//enables the HP jack too.
	AudioConfigureJacks();
	LineinLineoutConfig();

  print("Interrupt Audio Demo by www.cteq.eu\n\r" );
  print("=========================\n\r");

  XScuTimer Scu_Timer;
  XScuTimer_Config *Scu_ConfigPtr;
  XScuGic IntcInstance;

  Scu_ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
  Status = XScuTimer_CfgInitialize(&Scu_Timer, Scu_ConfigPtr, Scu_ConfigPtr->BaseAddr);
  Status = Timer_Intr_Setup(&IntcInstance, &Scu_Timer, XPS_SCU_TMR_INT_ID);
  XScuTimer_LoadTimer(&Scu_Timer,(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)/48000);
  XScuTimer_EnableAutoReload(&Scu_Timer);
  XScuTimer_Start(&Scu_Timer);

  for(;;){
  }
  cleanup_platform();
  return 0;
}
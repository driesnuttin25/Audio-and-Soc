#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"

#include "xparameters.h"
//#include "xiicps.h"
#include "xiic.h"
#include "xil_printf.h"

#include "lis2ds12.h"

/* The original code was
 * https://www.element14.com/community/thread/66802/l/i2csensor-app-source-code
 *
 */

/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */

//#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define IIC_DEVICE_ID		XPAR_AXI_IIC_0_DEVICE_ID
#define IIC_BASE_ADDRESS	XPAR_IIC_0_BASEADDR
/*
 * The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x55
#define IIC_SCLK_RATE		100000

XIic Iic;		/**< Instance of the IIC Device */

#define MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI  0x1D /* 0011101b for LIS2DS12 on MiniZed when SA0 is pulled high*/

#define TEST_BUFFER_SIZE 32
/*
 * The following buffers are used in this example to send and receive data
 * with the IIC.
 */
u8 SendBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Receiving Data */

int ByteCount;
int ByteCount;
u8 send_byte;
u8 write_data [256];
u8 read_data [256];
u8 i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI; //by default

u8 LIS2DS12_WriteReg(u8 Reg, u8 *Bufp, u16 len)
{
	write_data[0] = Reg;
	for (ByteCount = 1;ByteCount <= len; ByteCount++)
	{
		write_data[ByteCount] = Bufp[ByteCount-1];
	}
	ByteCount = XIic_Send(IIC_BASE_ADDRESS, i2c_device_addr, &write_data[0], (len+1), XIIC_STOP);
	//return(ByteCount);

	return XST_SUCCESS;
}

u8 LIS2DS12_ReadReg(uint8_t Reg, uint8_t *Bufp, uint16_t len)
{
	write_data[0] = Reg;
	ByteCount = XIic_Send(IIC_BASE_ADDRESS, i2c_device_addr, (u8*)&write_data, 1, XIIC_REPEATED_START);
	ByteCount = XIic_Recv(IIC_BASE_ADDRESS, i2c_device_addr, (u8*)Bufp, len, XIIC_STOP);
	//return(ByteCount);

	return XST_SUCCESS;
}


bool isSensorConnected()
{
	uint8_t who_am_i = 0;
	uint8_t send_byte;

	LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
	printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n", MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI, who_am_i);
	if (who_am_i != LIS2DS12_ACC_WHO_AM_I)
	{
		//maybe the address bit was changed, try the other one:
		LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
		printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n", MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI, who_am_i);
	}
	send_byte = 0x00; //No auto increment
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL2, &send_byte, 1);

	//Write 60h in CTRL1	// Turn on the accelerometer.  14-bit mode, ODR = 400 Hz, FS = 2g
	send_byte = 0x60;
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL1, &send_byte, 1);
	printf("CTL1 = 0x60 written\r\n");

	//Enable interrupt
	send_byte = 0x01; //Acc data-ready interrupt on INT1
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL4, &send_byte, 1);
	printf("CTL4 = 0x01 written\r\n");
	return true;

}
bool sensor_init()
{
	u8 who_am_i;
	i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_HI; //default
	LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
	printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n", i2c_device_addr, who_am_i);
	if (who_am_i != LIS2DS12_ACC_WHO_AM_I)
	{
		//maybe the address bit was changed, try the other one:
		i2c_device_addr = MINIZED_MOTION_SENSOR_ADDRESS_SA0_LO;
		LIS2DS12_ReadReg(LIS2DS12_ACC_WHO_AM_I_REG, &who_am_i, 1);
		printf("With I2C device address 0x%02x received WhoAmI = 0x%02x\r\n", i2c_device_addr, who_am_i);
	}
	send_byte = 0x00; //No auto increment
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL2, &send_byte, 1);

	//Write 60h in CTRL1	// Turn on the accelerometer.  14-bit mode, ODR = 400 Hz, FS = 2g
	send_byte = 0x60;
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL1, &send_byte, 1);
	printf("CTL1 = 0x60 written\r\n");

	//Enable interrupt
	send_byte = 0x01; //Acc data-ready interrupt on INT1
	LIS2DS12_WriteReg(LIS2DS12_ACC_CTRL4, &send_byte, 1);
	printf("CTL4 = 0x01 written\r\n");

#if (0)
	write_data[0] = 0x0F; //WhoAmI
	ByteCount = XIic_Send(IIC_BASE_ADDRESS, MAGNETOMETER_ADDRESS, (u8*)&write_data, 1, XIIC_REPEATED_START);
	ByteCount = XIic_Recv(IIC_BASE_ADDRESS, MAGNETOMETER_ADDRESS, (u8*)&read_data[0], 1, XIIC_STOP);
	printf("Received 0x%02x\r\n",read_data[0]);
	printf("\r\n"); //Empty line
	//for (int n=0;n<1400;n++) //118 ms is too little
	for (int n=0;n<1500;n++) //128 ms
	{
		printf(".");
	};
	printf("\r\n");
#endif

	return true;
}


int u16_2s_complement_to_int(u16 word_to_convert)
{
	u16 result_16bit;
	int result_14bit;
	int sign;

	if (word_to_convert & 0x8000)
	{ //MSB is set, negative number
		//Invert and add 1
		sign = -1;
		result_16bit = (~word_to_convert) + 1;
	}
	else
	{ //Positive number
		//No change
		sign = 1;
		result_16bit = word_to_convert;
	}
	//We are using it in 14-bit mode
	//All data is left-aligned.  So convert 16-bit value to 14-but value
	result_14bit = sign * (int)(result_16bit >> 2);
	return(result_14bit);
} //u16_2s_complement_to_int()



bool pollForAccel(int *x, int *y )
{
		int iacceleration_X;
		int iacceleration_Y;
		int iacceleration_Z;
		u8 read_value_LSB;
		u8 read_value_MSB;
		u16 accel_X;
		u16 accel_Y;
		u16 accel_Z;
		u8 accel_status;
		u8 data_ready;

		data_ready = 0;

		 //wait for DRDY
		LIS2DS12_ReadReg(LIS2DS12_ACC_STATUS, &accel_status, 1);
		data_ready = accel_status & 0x01; //bit 0 = DRDY
		if (!data_ready)
			return false;

		//wait for DRDY


		//Read X:
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_X_L, &read_value_LSB, 1);
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_X_H, &read_value_MSB, 1);
		accel_X = (read_value_MSB << 8) + read_value_LSB;
		iacceleration_X = u16_2s_complement_to_int(accel_X);
		//Read Y:
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Y_L, &read_value_LSB, 1);
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Y_H, &read_value_MSB, 1);
		accel_Y = (read_value_MSB << 8) + read_value_LSB;
		iacceleration_Y = u16_2s_complement_to_int(accel_Y);
		//Read Z:
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Z_L, &read_value_LSB, 1);
		LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_Z_H, &read_value_MSB, 1);
		accel_Z = (read_value_MSB << 8) + read_value_LSB;
		iacceleration_Z = u16_2s_complement_to_int(accel_Z);

		if (x)
		{
			*x = iacceleration_X;
		}

		if (y)
		{
			*y = iacceleration_Y;
		}
	//	printf("  Acceleration = X: %+5d, Y: %+5d, Z: %+5d\r\n",iacceleration_X, iacceleration_Y, iacceleration_Z);
//		printf("  Acceleration = X: %+5d, Y: %+5d, Z: %+5d\r",iacceleration_X, iacceleration_Y, iacceleration_Z);

		return true;
}

void read_temperature(void)
{
	int temp;
	u8 read_value;

	LIS2DS12_ReadReg(LIS2DS12_ACC_OUT_T, &read_value, 1);
	//Temperature is from -40 to +85 deg C.  So 125 range.  0 is 25 deg C.  +1 deg C/LSB.  So if value < 128 temp = 25 + value else temp = 25 - (256-value)
	if (read_value < 128)
	{
		temp = 25 + read_value;
	}
	else
	{
		temp = 25 - (256 - read_value);
	}
	printf("OUT_T register = 0x%02x -> Temperature = %i degrees C",read_value,temp);
	//printf("OUT_T register = 0x%02x -> Temperature = %i degrees C\r\n",read_value,temp);
} //read_temperature()

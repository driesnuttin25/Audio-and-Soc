# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\dries\IMU\eFPGA_I2C\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\dries\IMU\eFPGA_I2C\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {eFPGA_I2C}\
-hw {C:\Users\dries\IMU\design_1_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {C:/Users/dries/IMU}

platform write
platform generate -domains 
platform active {eFPGA_I2C}
bsp reload
bsp config stdin "ps7_uart_0"
bsp config stdout "ps7_uart_0"
bsp write
platform write
platform generate
platform active {eFPGA_I2C}
platform config -updatehw {C:/Users/dries/IMU/design_1_wrapper.xsa}
platform generate
platform active {eFPGA_I2C}
platform config -updatehw {C:/Users/dries/IMU/design_1_wrapper.xsa}
platform generate -domains 

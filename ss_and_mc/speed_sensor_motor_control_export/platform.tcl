# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\dries\ss_and_mc\speed_sensor_motor_control_export\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\dries\ss_and_mc\speed_sensor_motor_control_export\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {speed_sensor_motor_control_export}\
-hw {C:\Users\dries\ss_and_mc\speed_sensor_motor_control_export.xsa}\
-out {C:/Users/dries/ss_and_mc}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {hello_world}
platform generate -domains 
platform active {speed_sensor_motor_control_export}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
platform generate
domain active {zynq_fsbl}
bsp reload
domain active {standalone_ps7_cortexa9_0}
bsp reload
platform generate -domains 
platform generate -domains standalone_ps7_cortexa9_0,zynq_fsbl 
platform config -updatehw {C:/Users/dries/ss_and_mc/speed_sensor_motor_control_export.xsa}
platform config -updatehw {C:/Users/dries/ss_and_mc/speed_sensor_motor_control_export.xsa}
platform config -updatehw {C:/Users/dries/ss_and_mc/speed_sensor_motor_control_export.xsa}
platform generate -domains standalone_ps7_cortexa9_0,zynq_fsbl 
platform generate -domains standalone_ps7_cortexa9_0,zynq_fsbl 
platform clean
platform clean
platform active {speed_sensor_motor_control_export}
platform config -updatehw {C:/Users/dries/ss_and_mc/speed_sensor_motor_control_export.xsa}
platform clean
platform generate
platform clean
platform generate
platform generate -domains standalone_ps7_cortexa9_0,zynq_fsbl 

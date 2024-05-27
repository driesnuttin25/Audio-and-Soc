----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Dries Nuttin
-- 
-- Create Date: 19.03.2024 08:45:02
-- Design Name: joy_it speedsensor AXI block
-- Module Name: speed_sensor_Core1 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;    
use IEEE.NUMERIC_STD.ALL;

entity speed_sensor_Core1 is
    Port (
        Clk : in STD_LOGIC; -- Clock input
        Reset : in STD_LOGIC; -- Active high reset input
        Enable : in STD_LOGIC; -- Enable input for counting
        Sensor_Pin : in STD_LOGIC; -- Pulse input from the sensor
        Count : out STD_LOGIC_VECTOR(31 downto 0) -- 32-bit output for the pulse count
    );
end speed_sensor_Core1;

architecture Behavioral of speed_sensor_Core1 is
    signal pulse_count : unsigned(31 downto 0) := (others => '0');
    signal last_sensor_state : STD_LOGIC := '0'; -- To detect rising edges
begin

    process(Clk)
    begin
        if rising_edge(Clk) then
            if Reset = '1' then
                pulse_count <= (others => '0'); -- Reset pulse count
            elsif Enable = '1' then
                -- Check for rising edge on the Sensor_Pin
                if Sensor_Pin = '1' and last_sensor_state = '0' then
                    pulse_count <= pulse_count + 1;
                end if;
                last_sensor_state <= Sensor_Pin; -- Update last sensor state
            end if;
        end if;
    end process;

    -- Assign the pulse count to the output
    Count <= std_logic_vector(pulse_count);

end Behavioral;

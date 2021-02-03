--Autor: Adrián Boros [xboros03@stud.fit.vutbr.cz]

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

--Entity
entity ledc8x8 is
port ( 
	SMCLK: in std_logic;	
	RESET: in std_logic;	
	ROW: out std_logic_vector(0 to 7);	
	LED: out std_logic_vector(0 to 7)	
);
end ledc8x8;

architecture main of ledc8x8 is
	signal counter: STD_LOGIC_VECTOR (7 downto 0) := (others => '0');	--signal pre citac
	signal state_counter: STD_LOGIC_VECTOR (20 downto 0) := (others => '0');	--signal na zmenu stavov
	signal ce: STD_LOGIC;	--povolovaci signal
	signal LED_state: STD_LOGIC_VECTOR(1 downto 0) := "00";	--stav signalu
	signal row_counter: STD_LOGIC_VECTOR (7 downto 0) := "10000000";
	signal LED_active: STD_LOGIC_VECTOR (7 downto 0) := "11111111";

begin
	--citac na znizenie frekvencie a na zmenu stavu
	CE_generator: process (RESET, SMCLK)
	begin
		if RESET = '1' then	--asynchronny reset
			counter <= (others => '0');
			state_counter <= (others => '0');
			LED_state <= "00";	--pociatocny stav
		elsif rising_edge(SMCLK) then	--nastupna hrana
			counter <= counter + 1;
			state_counter <= state_counter + 1;
			if(state_counter = "111000010000000000000") then
				LED_state <= LED_state + 1;
				state_counter <= (others => '0');
			end if;
		end if;
	end process;
	ce <= '1' when counter = "11111111" else '0';
	
	--Rotacia riadkov(prechadzaju sa vsetky riadky)
	ROW_control: process (RESET, SMCLK)
	begin
		if RESET = '1' then
			row_counter <= "10000000";
		elsif rising_edge(SMCLK) then
			if ce = '1' then
				row_counter <= row_counter(0) & row_counter(7 downto 1);	--konkatenacia na posunutie jednotky
			end if;
		end if;
	end process;
	
	--vyber LED aby sa vytvorili inicialky mena
	set_LEDS: process (row_counter,LED_state)
	begin
		if(LED_state = "00") then	--inicialka A
			case row_counter is
				when "10000000" => LED_active <= "11100011";
				when "01000000" => LED_active <= "11011101";
				when "00100000" => LED_active <= "11011101";
				when "00010000" => LED_active <= "11000001";
				when "00001000" => LED_active <= "11011101";
				when "00000100" => LED_active <= "11011101";
				when "00000010" => LED_active <= "11011101";
				when "00000001" => LED_active <= "11011101";
				when others => LED_active <= "11111111";
			end case;
		elsif(LED_state = "01") or (LED_state = "11") then	--vsetky LED zhasnute
			case row_counter is
				when "10000000" => LED_active <= "11111111";
				when "01000000" => LED_active <= "11111111";
				when "00100000" => LED_active <= "11111111";
				when "00010000" => LED_active <= "11111111";
				when "00001000" => LED_active <= "11111111";
				when "00000100" => LED_active <= "11111111";
				when "00000010" => LED_active <= "11111111";
				when "00000001" => LED_active <= "11111111";
				when others => LED_active <= "11111111";
			end case;
		elsif(LED_state = "10") then	--inicialka B
			case row_counter is
				when "10000000" => LED_active <= "11000011";
				when "01000000" => LED_active <= "11011101";
				when "00100000" => LED_active <= "11011101";
				when "00010000" => LED_active <= "11000011";
				when "00001000" => LED_active <= "11011101";
				when "00000100" => LED_active <= "11011101";
				when "00000010" => LED_active <= "11011101";
				when "00000001" => LED_active <= "11000011";
				when others => LED_active <= "11111111";
			end case;
		end if;	
	end process;
	
	--nastavenie ROW a LED aktualnymi hodnotami LED_active a row_counter
	LED <= LED_active;
	ROW <= row_counter;

end main;

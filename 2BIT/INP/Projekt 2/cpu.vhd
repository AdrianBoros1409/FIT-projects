-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2018 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Adrian Boros <xboros03@stud.fit.vutbr.cz>
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='1') / zapis do pameti (DATA_RDWR='0')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 type fsm_state is (
	fetch,	-- Nacitanie instrukcie
	instr_decode,	-- Dekodovanie instrukcie
	increm_ptr, -- Inkrementacia hodnoty ukazatela
	decrem_ptr, -- Inkrementacia hodnoty ukazatela
	increm_value_1, 	-- Inkrementacia hodnoty aktualnej bunky
	increm_value_2, 		
	decrem_value_1, 	-- Dekrementacia hodinovy aktualnej bunky
	decrem_value_2, 	
	start_while_1, 		-- Zaciatok cyklu while
	start_while_2, 
	start_while_3, 
	start_while_4,
	end_while_1, 		-- Koniec cyklu while
	end_while_2, 
	end_while_3, 
	end_while_4,
	end_while_5,
	end_while_6,
	comment,	-- Komentar
	comment2,
	comment3,
	hexa_char,		-- Hexadecimalny znak
	putchar, -- Vypis hodnoty aktualnej bunky
	putchar_1,
	getchar,  --  Nacitanie hodnoty do aktualnej bunky
	return_state, -- Zastavenie vykonavania programu
	others_state -- Ostatne
 );
 
 -- Program counter (PC)
 signal PC_inc : std_logic;
 signal PC_dec : std_logic;
 signal PC_reg : std_logic_vector(11 downto 0);
 
 -- Pointer to data register (PTR)
 signal PTR_inc : std_logic;
 signal PTR_dec : std_logic;
 signal PTR_reg : std_logic_vector(9 downto 0);
 
 -- Counter of start and end of while (CNT)
 signal CNT_inc : std_logic;
 signal CNT_dec : std_logic;
 signal CNT_reg : std_logic_vector(7 downto 0);
 
 signal TMP_reg : std_logic_vector(7 downto 0);
 
 signal fsm_pres_state  : fsm_state;
 signal fsm_next_state  : fsm_state;
 
 -- Multiplexor MX k volbe hodnoty zapisanej do pamati dat
 signal mx_write_select : std_logic_vector(1 downto 0) := "00";
 
begin

 ----------------------------------------
 -- 		       PC		
 ----------------------------------------
 pc_proc: process (RESET, CLK, PC_dec, PC_inc, PC_reg)
    begin
		if RESET = '1' then
			PC_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if PC_inc = '1' then
				PC_reg <= PC_reg + 1;
			elsif PC_dec = '1' then
				PC_reg <= PC_reg - 1;
			end if;
		end if;
    end process;
 
 CODE_ADDR <= PC_reg;
 
----------------------------------------
-- 		       	  CNT		
----------------------------------------
 cnt_proc: process (RESET, CLK, CNT_inc, CNT_dec, CNT_reg)
	begin
		if RESET = '1' then
			CNT_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if CNT_inc = '1' then
				CNT_reg <= CNT_reg + 1;
			elsif CNT_dec = '1' then
				CNT_reg <= CNT_reg - 1;
			end if;
		end if;
    end process;
	
----------------------------------------
-- 		       	  PTR		
----------------------------------------
 ptr_proc: process (RESET, CLK, PTR_inc, PTR_dec)
	begin
		if RESET = '1' then
			PTR_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if PTR_inc = '1' then
				PTR_reg <= PTR_reg + 1;
			elsif PTR_dec = '1' then
				PTR_reg <= PTR_reg - 1;
			end if;
		end if;
    end process;
	
 DATA_ADDR <= PTR_reg;
 
----------------------------------------
-- 		       	  FSM		
----------------------------------------	
 fsm_pstate_proc: process(CLK, RESET, EN)
    begin
		if RESET = '1' then
			fsm_pres_state <= fetch;
		elsif (CLK'event) and (CLK = '1') then
			if EN = '1' then
				fsm_pres_state <= fsm_next_state;
			end if;
		end if;
    end process;
	
----------------------------------------
-- 		       	  MX		
----------------------------------------	
 mx_select_write_proc: process (IN_DATA, DATA_RDATA, mx_write_select)
	begin
			case mx_write_select is
				when "00" =>
					-- zapis hodnoty zo vstupu
					DATA_WDATA <= IN_DATA;
				when "01" =>
					-- zapis aktualnej bunky zvysenej o 1
					DATA_WDATA <= DATA_RDATA + 1;
				when "10" =>
					-- zapis aktualnej bunky znizenej o 1
					DATA_WDATA <= DATA_RDATA - 1;
				when "11" =>
					-- zapis hodnoty ktora je sucastou instrukcie
					DATA_WDATA <= TMP_reg;
				when others =>	
					DATA_WDATA <= (others => '0');
			end case;
	end process;
	
	
 fsm_nstate_proc: process(CODE_DATA, IN_VLD, OUT_BUSY, DATA_RDATA, CNT_reg, fsm_pres_state)
 begin
	OUT_WE <= '0';
	IN_REQ <= '0';
	CODE_EN <= '1';
	DATA_EN <= '0';
	DATA_RDWR <= '0';
	mx_write_select <= "00";
	PTR_inc <= '0';
	PTR_dec <= '0';
	CNT_inc <= '0';
	CNT_dec <= '0';
	PC_inc <= '0';
	PC_dec <= '0';
	
	case fsm_pres_state is
	  -- Nacitanie instrukcie
	  when fetch => 
	    CODE_EN <= '1';
		fsm_next_state <= instr_decode;
	  
	  -- Dekoder
	  when instr_decode =>
	    case CODE_DATA is
		  when X"3E" =>
		    fsm_next_state <= increm_ptr;
		  when X"3C" =>
		    fsm_next_state <= decrem_ptr;
		  when X"2B" =>
		    fsm_next_state <= increm_value_1;
          when X"2D" =>
		    fsm_next_state <= decrem_value_1;			
	      when X"5B" =>
		    fsm_next_state <= start_while_1;
		  when X"5D" =>
		    fsm_next_state <= end_while_1;
		  when X"2E" =>
		    fsm_next_state <= putchar;
		  when X"2C" =>
		    fsm_next_state <= getchar;
		  when X"23" =>
			fsm_next_state <= comment;
		  when X"30" | X"31" | X"32" | X"33" | X"34" | X"35" | X"36" | X"37" | 
			   X"38" | X"39" | X"41" | X"42" | X"43" | X"44" | X"45" | X"46" =>
			fsm_next_state <= hexa_char;
		  when X"00" =>
		    fsm_next_state <= return_state;	
		  when others =>
			fsm_next_state <= others_state;
		end case;

	  -- Inkrementacia hodnoty ukazatela
	  when increm_ptr =>
	    PTR_inc <= '1';
		PC_inc <= '1';
		fsm_next_state <= fetch;
		
	  -- Dekrementacia hodnoty ukazatela	  
	  when decrem_ptr =>
		PTR_dec <= '1';
		PC_inc <= '1';
		fsm_next_state <= fetch;
		
	  -- Inkrementacia hodnoty aktualnej bunky	
	  when increm_value_1 =>
		DATA_EN <= '1';
		DATA_RDWR <= '1';
		fsm_next_state <= increm_value_2;
	  
	  when increm_value_2 =>
		mx_write_select <= "01";
		DATA_EN <= '1';
		DATA_RDWR <= '0';
		PC_inc <= '1';
		fsm_next_state <= fetch;
		
	  -- Dekrementacia hodnoty aktualnej bunky	
      when decrem_value_1 =>
		DATA_EN <= '1';
		DATA_RDWR <= '1';
		fsm_next_state <= decrem_value_2;
		
	  when decrem_value_2 =>
		mx_write_select <= "10";
		PC_inc <= '1';
		DATA_EN <= '1';
		DATA_RDWR <= '0';
		fsm_next_state <= fetch;
	  
	  -- Vypis hodnoty aktualnej bunky
	  when putchar =>
	    if(OUT_BUSY = '1') then
		  fsm_next_state <= putchar;
		else
		  DATA_EN <= '1';   
		  DATA_RDWR <= '1';
		  fsm_next_state <= putchar_1;
		end if;
		
	  when putchar_1 =>	
		OUT_WE <= '1';
		OUT_DATA <= DATA_RDATA;
		PC_inc <= '1';
		fsm_next_state <= fetch;

	  -- Nacitanie hodnoty do aktualnej bunky
	  when getchar =>
	    IN_REQ <= '1';
		if(IN_VLD = '1') then 
		  mx_write_select <= "00";
		  DATA_EN <= '1';
		  DATA_RDWR <= '0';
		  PC_inc <= '1';
		  fsm_next_state <= fetch;
		else
		  fsm_next_state <= getchar;
		end if;
		
	  -- Zaciatok cyklu while	
	  when start_while_1 =>
		PC_inc <= '1';
		DATA_EN <= '1';
		DATA_RDWR <= '1';
		fsm_next_state <= start_while_2;
		
	  when start_while_2 =>
	    if(DATA_RDATA = (DATA_RDATA'range => '0')) then
			CNT_inc <= '1';
			fsm_next_state <= start_while_3;
		else
			fsm_next_state <= fetch;
		end if;
	  
	  when start_while_3 =>
	    if(CNT_reg = (CNT_reg'range => '0')) then
			fsm_next_state <= fetch;
		else
			CODE_EN <= '1';
			fsm_next_state <= start_while_4;
		end if;
	
	  when start_while_4 =>
		if(CODE_DATA = X"5B") then
			CNT_inc <= '1';
		elsif(CODE_DATA = X"5D") then
			CNT_dec <= '1';
		end if;
		PC_inc <= '1';
		fsm_next_state <= start_while_3;
	
	  -- Koniec cyklu while
	  when end_while_1 =>
		DATA_EN <= '1';
		DATA_RDWR <= '1';
		fsm_next_state <= end_while_2;
	
	  when end_while_2 =>
		if(DATA_RDATA = (DATA_RDATA'range => '0')) then 
			PC_inc <= '1';
			fsm_next_state <= fetch;
		else
			fsm_next_state <= end_while_3;
		end if;
	   
	   when end_while_3 =>
		CNT_inc <= '1';
		PC_dec <= '1';
		fsm_next_state <= end_while_4;
			
	  when end_while_4 =>
		if(CNT_reg = (CNT_reg'range => '0')) then
			fsm_next_state <= fetch;
		else
			CODE_EN <= '1';
			fsm_next_state <= end_while_5;
		end if;
		
	  when end_while_5 =>
		if(CODE_DATA = X"5D") then
			CNT_inc <= '1';
		elsif(CODE_DATA = X"5B") then
			CNT_dec <= '1';
		end if;	
		fsm_next_state <= end_while_6;
		
	  when end_while_6 =>
		if(CNT_reg = (CNT_reg'range => '0')) then	
			PC_inc <= '1';
		else
			PC_dec <= '1';
		end if;
		fsm_next_state <= end_while_4;
	   
	  -- Komentar  
	  when comment =>
		PC_inc <= '1';
		fsm_next_state <= comment2;
	
	  when comment2 =>
		CODE_EN <= '1';
		fsm_next_state <= comment3;
	
	  when comment3 =>	
		if (CODE_DATA = X"23") then
			PC_inc <= '1';
			fsm_next_state <= fetch;
		else
			fsm_next_state <= comment;
		end if;
	  
	  -- Hexadecimalny znak
	  when hexa_char =>
		DATA_EN <= '1';
		PC_inc <= '1';
		mx_write_select <= "11";
		TMP_reg <= CODE_DATA(3 downto 0) & "0000";
		fsm_next_state <= fetch;
	  
	  -- Zastavenie vykonavania programu
	  when return_state =>
		fsm_next_state <= return_state;
		
	  -- Ostatne stavy	
	  when others_state =>
		PC_inc <= '1';
		fsm_next_state <= fetch;
	  
	end case;
	
 end process;
	
end behavioral;
 

-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Hung Do (xdohun00)
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
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
type STATE is (
    IDLE,
	FETCH0,		-- nacte prikaz
	FETCH1,		-- nacte prikaz
	DECODE,		-- dekoduje prikaz
	EXECUTE 	-- provede prikaz
);

type EXEC_STATE is (
    loop_sbetween, loop_ebetween,
    loop_snop1, loop_snop2, loop_snop3, loop_snop4,
    loop_enop1, loop_enop2, loop_enop3, loop_enop4,
    pointer_inc,
    pointer_dec,
    val_inc1, val_inc2, val_inc3,
    val_dec1, val_dec2, val_dec3,
    putchar1, putchar2,
    getchar1, getchar2, getchar3,
    loop_s1, loop_s2, loop_s3, 
    loop_e1, loop_e2, loop_e3,
    break1, break2, break_nop1, break_nop2, break_nop3, break_nop4,
    ret,
    undef
);

-- stav radice
signal curr_state : STATE   := IDLE;
-- mezistavy ruznych prikazu
signal exe_st : EXEC_STATE  := undef;

-- prace s Program Counter
-- 12bit == 4KiB ROM pametu
signal pc      : std_logic_vector(11 downto 0) := (others=>'0');
signal pc_inc  : std_logic := '0';
signal pc_dec  : std_logic := '0';

-- pocet vnorenrych cyklu
signal cnt     : integer := 0;
signal cnt_inc : std_logic := '0';
signal cnt_dec : std_logic := '0';

-- prace s Pointer (Address)
-- 10-bit ukazatel do pameti RAM
signal ptr     : std_logic_vector(9 downto 0)  := (others=>'0');
signal ptr_inc : std_logic := '0';
signal ptr_dec : std_logic := '0';

-- multiplexor
signal reg_mlt : std_logic_vector(1 downto 0)  := "00";

begin
    -- radic
    fsm: process (CLK, RESET, CODE_DATA, IN_DATA, IN_VLD, OUT_BUSY)
    begin
        if (RESET='1') then
            reg_mlt    <= "00";
            pc_inc     <= '0';
            pc_dec     <= '0';
            cnt_inc    <= '0';
            cnt_dec    <= '0';
            ptr_inc    <= '0';
            ptr_dec    <= '0';

            curr_state <= IDLE;
            CODE_EN    <= '0';
            DATA_WREN  <= '0';
            DATA_EN    <= '0';
            IN_REQ     <= '0';
            OUT_WREN   <= '0';
        elsif rising_edge(CLK) then
            if (EN='1') then
                case curr_state is
                    when IDLE => 
                        reg_mlt <= "00";
                        pc_inc  <= '0';
                        pc_dec  <= '0';
                        cnt_inc <= '0';
                        cnt_dec <= '0';
                        ptr_inc <= '0';
                        ptr_dec <= '0';
                        curr_state <= FETCH0;

                    when FETCH0 =>
                        CODE_EN <= '1';
                        pc_inc  <= '0';
                        pc_dec  <= '0';
                        cnt_inc <= '0';
                        cnt_dec <= '0';
                        ptr_inc <= '0';
                        ptr_dec <= '0';
                        OUT_WREN <= '0';
                        curr_state <= FETCH1;

                    when FETCH1 =>
                        CODE_EN <= '0';
                        curr_state <= DECODE;

                    -- dekodovani operace
                    when DECODE =>
                        case (CODE_DATA) is
                            when X"3E" =>  exe_st <= pointer_inc;
                            when X"3C" =>  exe_st <= pointer_dec;
                            when X"2B" =>  exe_st <= val_inc1;
                            when X"2D" =>  exe_st <= val_dec1;
                            when X"5B" =>  exe_st <= loop_s1;
                            when X"5D" =>  exe_st <= loop_e1;
                            when X"2E" =>  exe_st <= putchar1;
                            when X"2C" =>  exe_st <= getchar1;
                            when X"7E" =>  exe_st <= break1;
                            when X"00" =>  exe_st <= ret;
                            when others => exe_st <= undef;
                        end case;
                        curr_state <= EXECUTE;

                    -- provadeni operace
                    when EXECUTE => 
                        case exe_st is
                            -- znak >; ptr++
                            when pointer_dec =>
                                ptr_dec <= '1';
                                pc_inc  <= '1';
                                curr_state   <= FETCH0;

                            -- znak <; ptr--
                            when pointer_inc =>
                                ptr_inc <= '1';
                                pc_inc  <= '1';
                                curr_state   <= FETCH0;

                            -- znak +; (*ptr)++
                            when val_inc1 =>    -- cteni z RAM
                                DATA_EN   <= '1';
                                DATA_WREN <= '0';
                                exe_st    <= val_inc2;
                            when val_inc2 =>    -- zapis do RAM
                                reg_mlt   <= "01";
                                --DATA_EN   <= '0';
                                DATA_WREN <= '1';
                                exe_st    <= val_inc3;
                            when val_inc3 =>    -- navrat do puvodniho stavu
                                DATA_EN   <= '0';
                                pc_inc    <= '1';
                                curr_state     <= FETCH0;

                            -- znak -; (*ptr)--
                            when val_dec1 =>    -- cteni z RAM
                                DATA_EN   <= '1';
                                DATA_WREN <= '0';
                                exe_st    <= val_dec2;
                            when val_dec2 =>    -- nacteni do registru
                                reg_mlt   <= "10";
                                DATA_WREN <= '1';
                                exe_st    <= val_dec3;
                            when val_dec3 =>    -- navrat do puvodniho stavu
                                DATA_EN   <= '0';
                                pc_inc    <= '1';
                                curr_state     <= FETCH0;

                            -- znak [; 
                            -- while_label: 
                            --     while (*ptr)
                            when loop_s1 =>
                                pc_inc    <= '1';
                                DATA_EN   <= '1';
                                DATA_WREN <= '0';
                                exe_st    <= loop_sbetween;
                            when loop_sbetween =>
                                DATA_EN <= '0';
                                exe_st  <= loop_s2;
                                pc_inc    <= '0';
                            when loop_s2 =>
                                if (DATA_RDATA=X"00") then
                                    cnt_inc <= '1';
                                    CODE_EN <= '1';
                                    exe_st  <= loop_snop1;
                                else
                                    curr_state <= FETCH0;
                                end if;
                                DATA_EN <= '0';
                            when loop_snop1 =>
                                exe_st <= loop_snop2;
                                cnt_inc <= '0';
                            when loop_snop2 =>
                                exe_st <= loop_s3;
                            when loop_s3 =>
                                if (cnt=0) then
                                    CODE_EN <= '0';
                                    pc_dec <= '1';
                                    curr_state <= FETCH0;
                                else
                                    if (CODE_DATA=X"5B") then       -- [
                                        cnt_inc <= '1';
                                        -- pc_inc <= '1';
                                    elsif (CODE_DATA=X"5D") then    -- ]
                                        cnt_dec <= '1';
                                    end if;
                                    pc_inc <= '1';
                                    exe_st <= loop_snop3;
                                end if; -- if (cnt=0)
                            when loop_snop3 =>
                                cnt_inc <= '0';
                                cnt_dec <= '0';
                                pc_inc  <= '0';
                                pc_dec  <= '0';
                                exe_st <= loop_snop4;
                            when loop_snop4 =>
                                exe_st <= loop_s3;

                            -- znka ]; goto while_label
                            when loop_e1 =>
                                DATA_EN   <= '1';
                                DATA_WREN <= '0';
                                exe_st    <= loop_ebetween;
                            when loop_ebetween =>
                                DATA_EN  <= '0';
                                exe_st    <= loop_e2;
                            when loop_e2 =>
                                -- DATA_EN   <= '0';
                                if (DATA_RDATA=X"00") then
                                    pc_inc <= '1';
                                    curr_state  <= FETCH0;
                                else
                                    cnt_inc <= '1';
                                    CODE_EN <= '1';
                                    pc_dec  <= '1';
                                    exe_st  <= loop_enop1;
                                end if;
                            when loop_enop1 =>    -- nop
                                exe_st <= loop_enop2;
                                pc_dec <= '0';
                                cnt_inc <= '0';
                            when loop_enop2 =>
                                exe_st <= loop_e3;
                            when loop_e3 =>
                                pc_dec    <= '0';
                                if (cnt=0) then
                                    curr_state   <= FETCH0;
                                    pc_inc  <= '1';
                                    pc_dec  <= '0';
                                    CODE_EN <= '0';
                                else
                                    if (CODE_DATA=X"5B") then       -- [
                                        cnt_dec <= '1';
                                    elsif (CODE_DATA=X"5D") then    -- ]
                                        cnt_inc <= '1';
                                    end if;
                                    pc_dec <= '1';
                                    exe_st <= loop_enop3;
                                end if; -- if (cnt=0)
                            when loop_enop3 =>
                                pc_inc  <= '0';
                                pc_dec  <= '0';
                                cnt_inc <= '0';
                                cnt_dec <= '0';
                                exe_st <= loop_enop4;
                            when loop_enop4 =>
                                exe_st <= loop_e3;

                            -- znak .; putchar(*ptr)
                            when putchar1 =>
                                if (OUT_BUSY='0') then
                                    DATA_EN   <= '1';
                                    DATA_WREN <= '0';
                                    exe_st   <= putchar2;
                                end if;
                            when putchar2 =>
                                DATA_EN   <= '0';
                                OUT_WREN  <= '1';
                                pc_inc    <= '1';
                                curr_state     <= FETCH0;

                            -- znak ,; *ptr = getchar()
                            when getchar1 =>
                                IN_REQ <= '1';
                                exe_st <= getchar2;
                            when getchar2 =>
                                if (IN_VLD='1') then
                                    DATA_EN   <= '1';
                                    DATA_WREN <= '1';
                                    reg_mlt   <= "11";
                                    exe_st <= getchar3;
                                end if;
                            when getchar3 =>
                                DATA_EN <= '0';
                                pc_inc    <= '1';
                                curr_state     <= FETCH0;

                            -- znak ~; break
                            when break1 =>
                                cnt_inc <= '1';
                                pc_inc  <= '1';
                                CODE_EN <= '1';
                                exe_st  <= break_nop1;
                            when break_nop1 =>  -- nop
                                cnt_inc <= '0';
                                pc_inc <= '0';
                                exe_st <= break_nop2;
                            when break_nop2 =>  -- nop
                                -- CODE_EN <= '0';
                                exe_st <= break2;
                            when break2 =>
                                if (cnt=0) then
                                    pc_inc <= '0';
                                    pc_dec <= '1';
                                    curr_state  <= FETCH0;
                                else
                                    if (CODE_DATA=X"5B") then       -- [
                                        cnt_inc <= '1';
                                    elsif (CODE_DATA=X"5D") then    -- ]
                                        cnt_dec <= '1';
                                    else    -- nenacte se zadna zavorka
                                        cnt_inc <= '0';
                                        cnt_dec <= '0';
                                    end if;
                                    pc_inc <= '1';
                                    exe_st <= break_nop3;
                                end if; -- if (cnt=0)
                            when break_nop3 =>
                                pc_inc <= '0';
                                cnt_inc <= '0';
                                cnt_dec <= '0';
                                exe_st <= break_nop4;
                            when break_nop4 =>
                                exe_st <= break2;

                            -- znak null; return;
                            when ret => 
                                curr_state  <= IDLE;
                                exe_st <= ret;

                            -- ostatni; komentar
                            when undef =>
                                curr_state  <= FETCH0;
                                pc_inc <= '1';
                        end case; -- case exe_st is 
                end case; -- case curr_state is
            end if; -- if (EN='1')
        end if; -- if (RESET='1')
    end process;

    -- registr adresy v pameti ROM
    pc_reg: process (CLK, RESET)
    begin
        if (RESET='1') then
            pc <= (others=>'0');
        elsif rising_edge(CLK) then
            -- incrementuje nebo dekrementuje ukazatel do pameti ROM
            if (pc_inc='1') then
                pc <= pc + 1;
            elsif (pc_dec='1') then
                pc <= pc - 1;
            end if;
        end if;

    end process;

    -- citac cyklu
    cnt_reg: process (CLK, RESET)
    begin
        if (RESET='1') then
            cnt <= 0;
        elsif rising_edge(CLK) then
            if (cnt_inc='1') then
                cnt <= cnt + 1;
            elsif (cnt_dec='1') then
                cnt <= cnt - 1;
            end if;
        end if;
    end process;

    -- registr adresy v pameti RAM
    ptr_reg: process (CLK, RESET)
    begin
        if (RESET='1') then
            ptr <= (others=>'0');
        elsif rising_edge(CLK) then
            if (ptr_inc='1') then
                ptr <= ptr + 1;
            elsif (ptr_dec='1') then
                ptr <= ptr - 1;
            end if; -- if (ptr_inc='1')

        end if;
    end process;

    -- zapis adresy na vystup
    DATA_ADDR <= ptr;
    -- zapis na vystupni port CODE_ADDR
    CODE_ADDR <= pc;
    -- zapis na LCD display
    OUT_DATA <= DATA_RDATA;

    with reg_mlt select
    DATA_WDATA <= DATA_RDATA + 1 when "01",
                  DATA_RDATA - 1 when "10",
                  IN_DATA        when "11",
                  (others=>'X')  when others;
                  
end behavioral;


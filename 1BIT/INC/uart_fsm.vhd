-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): Hung Do
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK        : in std_logic;
   RST        : in std_logic;
   DIN        : in std_logic;
   CNT        : in std_logic_vector(3 downto 0);
   CNT_END    : in std_logic;
   RST_CNT    : out std_logic;
   CNT_ENABLE : out std_logic;
   DOUT_VLD   : out std_logic
);
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
-- Vycet moznych stavu
type STATES is (
  WAIT_START_BIT,     -- ceka na 0
  START_CONFIRM,      -- ceka 8 cyklu
  LOAD_BITS,          -- ceka 16 cyklu
  WAIT_STOP_BIT,      -- ceka na bit 1
  SEND_DATA           -- odesle DOUT, DOUT_VLD; vraci se na WAIT_START_BIT
);

signal curr_state : STATES := WAIT_START_BIT;
---------------
begin
  process (CLK) begin
    if rising_edge(CLK) then
      if (RST='1') then
        curr_state  <= WAIT_START_BIT;
        DOUT_VLD    <= '0';
        CNT_ENABLE  <= '0';
        RST_CNT     <= '0';
      else
        case curr_state is

          when WAIT_START_BIT   =>
            DOUT_VLD <= '0';
            if (DIN='0') then
              curr_state <= START_CONFIRM;
              CNT_ENABLE <= '1';
            end if;

          when START_CONFIRM =>
            if (CNT="0111") then
              if (DIN='0') then
                curr_state <= LOAD_BITS;
                RST_CNT   <= '1';
              else
                curr_state <= WAIT_START_BIT;
                CNT_ENABLE <= '0';
              end if;
            end if;

          when LOAD_BITS        => 
            RST_CNT       <= '0';
            -- kontrola, zda bylo nactenych 8 bitu
            if (CNT_END='1') then
              curr_state  <= WAIT_STOP_BIT;
              CNT_ENABLE  <= '0';
            else
              -- povoleni pro necteni hodnoty
              CNT_ENABLE  <= '1';
            end if;

          when WAIT_STOP_BIT    =>
            if (DIN='1') then
              curr_state <= SEND_DATA;
            end if;

          when SEND_DATA        => 
            DOUT_VLD   <= '1';
            curr_state <= WAIT_START_BIT; -- posle data a vrati se do puvodni pozice
        end case;
      end if; -- if (RST='1')

    end if; -- if rising_edge(CLK)
  end process;
end behavioral;

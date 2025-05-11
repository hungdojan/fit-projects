-- uart.vhd: UART controller - receiving part
-- Author(s): Hung Do
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
  CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
-- casovac
signal cnt       : std_logic_vector(3 downto 0) := "0000";
-- pocet nactenych bitu
signal bits      : std_logic_vector(3 downto 0) := "0000";
-- nactene bity
signal regist    : std_logic_vector(7 downto 0) := "00000000";
-- logicka hodnota udavajici konec nacitani
signal cnt_end   : std_logic := '0';
-- vyresetovani casovace (nastavuje hodnotu 0001)
signal rst_tick  : std_logic;
-- povoluje casovac
signal cnt_enable: std_logic;
-- odesilani zpravy
signal valid_bit : std_logic;
begin

  FSM: entity work.UART_FSM(behavioral)
  port map (
    CLK         => CLK,
    RST         => RST,
    DIN         => DIN,
    CNT         => cnt,
    CNT_END     => cnt_end,
    RST_CNT     => rst_tick,
    CNT_ENABLE  => cnt_enable,
    DOUT_VLD    => valid_bit
  );

  -- casovac
  process (CLK, cnt_enable, rst_tick, RST) begin
    if (RST='1') then
      cnt   <= "0000";
    elsif (CLK'event and CLK='1') then
      if (rst_tick='1') then
        cnt <= "0001";
      elsif (cnt_enable='1') then
        cnt <= cnt + 1;
      else
        cnt <= "0000";
      end if;
    end if;
  end process;

  -- nacitani DIN
  process (CLK, RST) begin
    if (RST='1') then
      regist  <= "00000000";
      bits    <= "0000";
      cnt_end <= '0';
    elsif (CLK'event and CLK='1') then
      if (bits(3)='1') then
        cnt_end <= '1';
        bits <= "0000";
      elsif (cnt="1111") then
        bits <= bits + 1;
        regist <= DIN & regist(7 downto 1);
      else
        cnt_end <= '0';
      end if;
    end if;
  end process;

  -- Posilani na DOUT
  process (CLK) begin
    if (CLK'event and CLK='1') then
      if (RST='1' or valid_bit='0') then
        DOUT_VLD <= '0';
        DOUT     <= "00000000";
      elsif (valid_bit='1') then  -- posilani prijateho signalu
        DOUT_VLD <= '1';
        DOUT     <= regist;
      end if;
    end if;
  end process;

end behavioral;

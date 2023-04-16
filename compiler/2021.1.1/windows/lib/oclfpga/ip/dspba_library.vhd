-- (C) 2012 Altera Corporation. All rights reserved.
-- Your use of Altera Corporation's design tools, logic functions and other
-- software and tools, and its AMPP partner logic functions, and any output
-- files any of the foregoing (including device programming or simulation
-- files), and any associated documentation or information are expressly subject
-- to the terms and conditions of the Altera Program License Subscription
-- Agreement, Altera MegaCore Function License Agreement, or other applicable
-- license agreement, including, without limitation, that your use is for the
-- sole purpose of programming logic devices manufactured by Altera and sold by
-- Altera or its authorized distributors.  Please refer to the applicable
-- agreement for further details.

library IEEE;
use IEEE.std_logic_1164.all;
use work.dspba_library_package.all;

entity dspba_delay is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1';
        reset_kind : string := "ASYNC"
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin   : in  std_logic_vector(width-1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_delay;

architecture delay of dspba_delay is
    type delay_array is array (depth downto 0) of std_logic_vector(width-1 downto 0);
    signal delay_signals : delay_array;
begin
    delay_signals(depth) <= xin;

    delay_block: if 0 < depth generate
    begin
        delay_loop: for i in depth-1 downto 0 generate
        begin
            sync_reset: if reset_kind = "ASYNC" generate
                process(clk, aclr)
                begin
                    if aclr=reset_high then
                        delay_signals(i) <= (others => '0');
                    elsif clk'event and clk='1' then
                        if ena='1' then
                            delay_signals(i) <= delay_signals(i + 1);
                        end if;
                    end if;
                end process;
            end generate;

            async_reset: if reset_kind = "SYNC" generate
                process(clk)
                begin
                    if clk'event and clk='1' then
                        if aclr=reset_high then
                            delay_signals(i) <= (others => '0');
                        elsif ena='1' then
                            delay_signals(i) <= delay_signals(i + 1);
                        end if;
                    end if;
                end process;
            end generate;

            no_reset: if reset_kind = "NONE" generate
                process(clk)
                begin
                    if clk'event and clk='1' then
                        if ena='1' then
                            delay_signals(i) <= delay_signals(i + 1);
                        end if;
                    end if;
                end process;
            end generate;
        end generate;
    end generate;

    xout <= delay_signals(0);
end delay;

library IEEE;
use IEEE.std_logic_1164.all;
use work.dspba_library_package.all;

entity dspba_mux2 is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
		xinsel  : in  std_logic_vector(0 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_mux2;

architecture mux2 of dspba_mux2 is
begin
	mux2genclk: if depth = 1 generate
		mux2proc: PROCESS (xin0, xin1, xinsel, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					CASE (xinsel) IS
						WHEN "0" => xout <= xin0;
						WHEN "1" => xout <= xin1;
						WHEN OTHERS => xout <= (others => '0');
					END CASE;
				end if;
			end if;
		END PROCESS mux2proc;
	end generate mux2genclk;
	
	mux2gencomb: if depth = 0 generate
		mux2proc2: process(xin0, xin1, xinsel)
		begin
			CASE (xinsel) IS
				WHEN "0" => xout <= xin0;
				WHEN "1" => xout <= xin1;
				WHEN OTHERS => xout <= (others => '0');
			END CASE;	
		end process mux2proc2;
	end generate mux2gencomb;
end mux2;

library IEEE;
use IEEE.std_logic_1164.all;
use work.dspba_library_package.all;

entity dspba_mux3 is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
		xin2  : in  std_logic_vector(width-1 downto 0);
		xinsel  : in  std_logic_vector(1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_mux3;

architecture mux3 of dspba_mux3 is
begin
	mux3genclk: if depth = 1 generate
		mux3proc: PROCESS (xin0, xin1, xin2, xinsel, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					CASE (xinsel) IS
						WHEN "00" => xout <= xin0;
						WHEN "01" => xout <= xin1;
						WHEN "10" => xout <= xin2;
						WHEN OTHERS => xout <= (others => '0');
					END CASE;
				end if;
			end if;
		END PROCESS mux3proc;
	end generate mux3genclk;
	
	mux3gencomb: if depth = 0 generate
		mux3proc2: process(xin0, xin1, xin2, xinsel)
		begin
			CASE (xinsel) IS
				WHEN "00" => xout <= xin0;
				WHEN "01" => xout <= xin1;
				WHEN "10" => xout <= xin2;
				WHEN OTHERS => xout <= (others => '0');
			END CASE;	
		end process mux3proc2;
	end generate mux3gencomb;
end mux3;

library IEEE;
use IEEE.std_logic_1164.all;
use work.dspba_library_package.all;

entity dspba_mux4 is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
		xin2  : in  std_logic_vector(width-1 downto 0);
		xin3  : in  std_logic_vector(width-1 downto 0);
		xinsel  : in  std_logic_vector(1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_mux4;

architecture mux4 of dspba_mux4 is
begin
	mux4genclk: if depth = 1 generate
		mux4proc: PROCESS (xin0, xin1, xin2, xin3, xinsel, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					CASE (xinsel) IS
						WHEN "00" => xout <= xin0;
						WHEN "01" => xout <= xin1;
						WHEN "10" => xout <= xin2;
						WHEN "11" => xout <= xin3;
						WHEN OTHERS => xout <= (others => '0');
					END CASE;
				end if;
			end if;
		END PROCESS mux4proc;
	end generate mux4genclk;
	
	mux4gencomb: if depth = 0 generate
		mux4proc2: process(xin0, xin1, xin2, xin3, xinsel)
		begin
			CASE (xinsel) IS
				WHEN "00" => xout <= xin0;
				WHEN "01" => xout <= xin1;
				WHEN "10" => xout <= xin2;
				WHEN "11" => xout <= xin3;
				WHEN OTHERS => xout <= (others => '0');
			END CASE;	
		end process mux4proc2;
	end generate mux4gencomb;
end mux4;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intadd_u is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intadd_u;

architecture intadd_u of dspba_intadd_u is
begin
	intadd_u_genclk: if depth = 1 generate
		intadd_u_proc: PROCESS (xin0, xin1, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) + UNSIGNED(xin1));					
				end if;
			end if;
		END PROCESS intadd_u_proc;
	end generate intadd_u_genclk;
	
	intadd_u_gencomb: if depth = 0 generate
			xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) + UNSIGNED(xin1));				
	end generate intadd_u_gencomb;
end intadd_u;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intadd_s is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intadd_s;

architecture intadd_s of dspba_intadd_s is
begin
	intadd_s_genclk: if depth = 1 generate
		intadd_s_proc: PROCESS (xin0, xin1, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					xout <= STD_LOGIC_VECTOR(SIGNED(xin0) + SIGNED(xin1));					
				end if;
			end if;
		END PROCESS intadd_s_proc;
	end generate intadd_s_genclk;
	
	intadd_s_gencomb: if depth = 0 generate
			xout <= STD_LOGIC_VECTOR(SIGNED(xin0) + SIGNED(xin1));				
	end generate intadd_s_gencomb;
end intadd_s;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intsub_u is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intsub_u;

architecture intsub_u of dspba_intsub_u is
begin
	intsub_u_genclk: if depth = 1 generate
		intsub_u_proc: PROCESS (xin0, xin1, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) - UNSIGNED(xin1));					
				end if;
			end if;
		END PROCESS intsub_u_proc;
	end generate intsub_u_genclk;
	
	intsub_u_gencomb: if depth = 0 generate
			xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) - UNSIGNED(xin1));				
	end generate intsub_u_gencomb;
end intsub_u;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intsub_s is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intsub_s;

architecture intsub_s of dspba_intsub_s is
begin
	intsub_s_genclk: if depth = 1 generate
		intsub_s_proc: PROCESS (xin0, xin1, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					xout <= STD_LOGIC_VECTOR(SIGNED(xin0) - SIGNED(xin1));					
				end if;
			end if;
		END PROCESS intsub_s_proc;
	end generate intsub_s_genclk;
	
	intsub_s_gencomb: if depth = 0 generate
			xout <= STD_LOGIC_VECTOR(SIGNED(xin0) - SIGNED(xin1));				
	end generate intsub_s_gencomb;
end intsub_s;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intaddsub_u is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
		xins  : in  std_logic_vector(0 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intaddsub_u;

architecture intaddsub_u of dspba_intaddsub_u is
begin
	intaddsub_u_genclk: if depth = 1 generate
		intaddsub_u_proc: PROCESS (xin0, xin1, xins, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					IF (xins = "1") THEN
						xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) + UNSIGNED(xin1));
					ELSE
						xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) - UNSIGNED(xin1));
					END IF;
				end if;
			end if;
		END PROCESS intaddsub_u_proc;
	end generate intaddsub_u_genclk;
	
	intaddsub_u_gencomb: if depth = 0 generate
    intaddsub_u_proc_comb: PROCESS (xin0, xin1, xins)
    BEGIN
        IF (xins = "1") THEN
            xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) + UNSIGNED(xin1));
        ELSE
            xout <= STD_LOGIC_VECTOR(UNSIGNED(xin0) - UNSIGNED(xin1));
        END IF;
    END PROCESS;			
	end generate intaddsub_u_gencomb;
end intaddsub_u;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use work.dspba_library_package.all;

entity dspba_intaddsub_s is
    generic (
        width : natural := 8;
        depth : natural := 1;
        reset_high : std_logic := '1'
    );
    port (
        clk   : in  std_logic;
        aclr  : in  std_logic;
        ena   : in  std_logic := '1';
        xin0  : in  std_logic_vector(width-1 downto 0);
		xin1  : in  std_logic_vector(width-1 downto 0);
		xins  : in  std_logic_vector(0 downto 0);
        xout  : out std_logic_vector(width-1 downto 0)
    );
end dspba_intaddsub_s;

architecture intaddsub_s of dspba_intaddsub_s is
begin
	intaddsub_s_genclk: if depth = 1 generate
		intaddsub_s_proc: PROCESS (xin0, xin1, xins, clk, aclr, ena)
		BEGIN
			if (aclr=reset_high) then 
				xout <= (others => '0');
			elsif (clk'event and clk='1') then 
				if (ena = '1') then 
					IF (xins = "1") THEN
						xout <= STD_LOGIC_VECTOR(SIGNED(xin0) + SIGNED(xin1));
					ELSE
						xout <= STD_LOGIC_VECTOR(SIGNED(xin0) - SIGNED(xin1));
					END IF;
				end if;
			end if;
		END PROCESS intaddsub_s_proc;
	end generate intaddsub_s_genclk;
	
	intaddsub_s_gencomb: if depth = 0 generate
    intaddsub_s_proc_comb: PROCESS (xin0, xin1, xins)
    BEGIN
        IF (xins = "1") THEN
            xout <= STD_LOGIC_VECTOR(SIGNED(xin0) + SIGNED(xin1));
        ELSE
            xout <= STD_LOGIC_VECTOR(SIGNED(xin0) - SIGNED(xin1));
        END IF;
    END PROCESS;			
	end generate intaddsub_s_gencomb;
end intaddsub_s;
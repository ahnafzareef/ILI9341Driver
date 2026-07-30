# XILINX ILI9341 Driver Library with XPT2026 Support.

Hello! This is a minimal **C Driver Library** for the **2.8 inch ILI9341 SPI TFT display**. I made this project because I was in the midst of working on a RTL project incorporating Chess Minimax algorithm logic to make a chess trainer. But unfortunately there was no existing libraries or support for this display, in addition to this, on the ARTY S7-25, using Microblaze with DDR3 and Caching enabled for both I-Cache and D-Cache caused too large of a memory footprint. So this runs entirely on BRAM. It is also plain C, so its small.


## Video Demo

## Features
- **Display:** hardware + software reset, full ILI9341 init sequence with RGB565 (16-bit per 2 Bytes) colour. If you're looking to adapt to a ILI9488 there are existing libraries or simply change the logic for RGB888 and change the initialization sequence.
- **Drawing:** `ili9341_fill_screen`, `ili9341_fill_rect`, `writePixel`, `drawPixel`, `ili9341_setRotation` (4 Orientations)
- **Text:** built-in 5x5 font, `ili9341_drawChar`, `ili9341_drawText` with scaling and also background colours!
- **Images:** `draw_image` for RGB565 bitmaps, if you want to use this please generate your own RGB565 bitmap just as I did my pikachu!
-  **Touch (Optional):** Fully optional module, XPT2046 read, IRQ based touch detection, sampling + averaging + calibration.
-  **BRAM-only: ** ALL BRAM, no DDR needed!

## Hardware

Maybe it'll help you to see the SOC I used to set this up, below are the images:

<img width="2555" height="1054" alt="image" src="https://github.com/user-attachments/assets/58647950-fceb-49a6-8157-c2595521c41c" />

This library drives the display through the standard Xilinx IP, the reference block design is:

- **MicroBlaze** + local BRAM (no MIG/DDR, no caches)
- **AXI Quad SPI** — Standard SPI mode
- **AXI GPIO** — for the DC / RST control lines (and touch CS + IRQ if using touch)
- **AXI UART Lite** — optional, for debug prints
- **Clocking Wizard** + **Processor System Reset**

### Pin Mapping

It'll also maybe help you to see the pin diagram, below is the image:


<img width="1176" height="1381" alt="image" src="https://github.com/user-attachments/assets/7ec589c4-20ae-45bd-b285-77ca1963397f" />



| Signal | Connects to | Notes |
|--------|-------------|-------|
| SPI SCK | AXI Quad SPI `sck` | shared display + touch |
| SPI MOSI | AXI Quad SPI `io0_o` | shared display + touch |
| SPI MISO | AXI Quad SPI `io1_i` | **standard mode → MISO is io1_i, not io0_i** |
| Display CS | <!-- TODO: your pin --> | |
| Display DC | AXI GPIO ch1 bit 1 (mask `0x02`) | command/data select |
| Display RST | AXI GPIO ch1 bit 0 (mask `0x01`) | |
| Touch CS | AXI GPIO ch1 bit 2 (mask `0x04`) | via GPIO slice |
| Touch IRQ | AXI GPIO ch2 bit 0 (mask `0x01`) | **input** — dual channel enabled |
| Backlight | 3.3V | tie high (or a GPIO for brightness) |

### Constraints (XDC)

```
set_property -dict { PACKAGE_PIN F14   IOSTANDARD LVCMOS33 } [get_ports { sys_clock }]; #IO_L13P_T2_MRCC_15 Sch=uclk
create_clock -add -name sys_clk_pin -period 83.333 -waveform {0 41.667} [get_ports { sys_clock }];

## Pmod Header JD
## the ports I'm using are named after the relative io in the pmod diagram.
set_property -dict { PACKAGE_PIN V15   IOSTANDARD LVCMOS33 } [get_ports { io1_rst }]; #IO_L20N_T3_A07_D23_14 Sch=jd1/ck_io[33]
set_property -dict { PACKAGE_PIN U12   IOSTANDARD LVCMOS33 } [get_ports { io2_dc }]; #IO_L21P_T3_DQS_14 Sch=jd2/ck_io[32]
set_property -dict { PACKAGE_PIN V13   IOSTANDARD LVCMOS33 } [get_ports { io3_mosi }]; #IO_L21N_T3_DQS_A06_D22_14 Sch=jd3/ck_io[31]
set_property -dict { PACKAGE_PIN T12   IOSTANDARD LVCMOS33 } [get_ports { io4_sck }]; #IO_L22P_T3_A05_D21_14 Sch=jd4/ck_io[30]
set_property -dict { PACKAGE_PIN T13   IOSTANDARD LVCMOS33 } [get_ports { io7_cs }]; #IO_L22N_T3_A04_D20_14 Sch=jd7/ck_io[29]
set_property -dict { PACKAGE_PIN R11   IOSTANDARD LVCMOS33 } [get_ports { io8_miso }]; #IO_L23P_T3_A03_D19_14 Sch=jd8/ck_io[28]
set_property -dict { PACKAGE_PIN T11   IOSTANDARD LVCMOS33 } [get_ports { io9_tcs }]; #IO_L23N_T3_A02_D18_14 Sch=jd9/ck_io[27]
set_property -dict { PACKAGE_PIN U11   IOSTANDARD LVCMOS33 } [get_ports { io10_irq }]; #IO_L24P_T3_A01_D17_14 Sch=jd10/ck_io[26]
```

## Getting it to work

### 1. Build platform on Vitis
### 2. Add all the files from `src/` into your Vitis Application
### 3. Build and run!

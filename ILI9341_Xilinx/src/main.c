#include "ILI9341_XIL.h"
#include "xparameters.h"

XSpi Spi;
XGpio Gpio;

int main()
{
    // --- SPI setup ---
    XSpi_Config *spiCfg = XSpi_LookupConfig(XPAR_XBRAM_0_BASEADDR);
    XSpi_CfgInitialize(&Spi, spiCfg, spiCfg->BaseAddress);
    XSpi_SetOptions(&Spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
    XSpi_SetSlaveSelect(&Spi, 0x01);
    XSpi_Start(&Spi);
    XSpi_IntrGlobalDisable(&Spi);

    // --- GPIO setup ---
    XGpio_Initialize(&Gpio, XPAR_XBRAM_0_BASEADDR);
    XGpio_SetDataDirection(&Gpio, 1, 0x0); // channel 1, all outputs

    // --- display ---
    ili9341_t display;
    ili9341_init(&display, &Spi, &Gpio, 0x01, 0x02, 1);

    // ili9341_fill_screen(&display, ILI9341_RED);
    // ili9341_fill_rect(&display, 0,0, 100, 250, ILI9341_CYAN);
    // drawPixel(&display, 160, 120, ILI9341_GREENYELLOW);
    // ili9341_drawChar(&display, 'C', 0, 0, ILI9341_DARKGREEN, 10, ILI9341_LIGHTGREY);
    ili9341_drawText(&display, "Hello World", 0, 0, ILI9341_WHITE, 2, ILI9341_BLACK);

    ili9341_drawText(&display, "bomboclat", 120, 120, ILI9341_WHITE, 2, ILI9341_BLACK);

    while (1)
    {
    }
    return 0;
}
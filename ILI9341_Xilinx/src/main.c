#include "ILI9341_XIL.h"
#include "xparameters.h"

XSpi  Spi;
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
    XGpio_SetDataDirection(&Gpio, 1, 0x0);   // channel 1, all outputs

    // --- display ---
    ili9341_t display;
    ili9341_init(&display, &Spi, &Gpio, 0x01, 0x02, 1);
    
    
    ili9341_fill_screen(&display, ILI9341_RED);
    

    while (1) {}
    return 0;
}
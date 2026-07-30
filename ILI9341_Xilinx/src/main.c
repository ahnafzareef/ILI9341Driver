#include "ILI9341_XIL.h"
#include "xparameters.h"
#include "xpt2046.h"
#include "pikachu.h"
#include <sleep.h>
XSpi Spi;
XGpio Gpio;

int main()
{
    // --- SPI setup ---
    XSpi_Config *spiCfg = XSpi_LookupConfig(XPAR_AXI_QUAD_SPI_0_BASEADDR);
    XSpi_CfgInitialize(&Spi, spiCfg, spiCfg->BaseAddress);
    XSpi_SetOptions(&Spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
    XSpi_SetSlaveSelect(&Spi, 0x01);
    XSpi_Start(&Spi);
    XSpi_IntrGlobalDisable(&Spi);

    // --- GPIO setup ---
    XGpio_Initialize(&Gpio, XPAR_AXI_GPIO_0_BASEADDR);
    XGpio_SetDataDirection(&Gpio, 1, 0x0); // channel 1, all outputs

    // --- display ---
    ili9341_t display;
    ili9341_init(&display, &Spi, &Gpio, 0x01, 0x02, 1);

    // --- touch ---
    xpt2046_t touch;
    xpt2046_init(&touch, &Spi, &Gpio, 1, 0x04, 0x01, 2);

    // ili9341_fill_screen(&display, ILI9341_RED);
    // ili9341_fill_rect(&display, 0,0, 100, 250, ILI9341_CYAN);
    // drawPixel(&display, 160, 120, ILI9341_GREENYELLOW);
    // ili9341_drawChar(&display, 'C', 0, 0, ILI9341_DARKGREEN, 10, ILI9341_LIGHTGREY);
    // ili9341_drawText(&display, "Hello World", 0, 0, ILI9341_WHITE, 2, ILI9341_BLACK);

    // ili9341_drawText(&display, "testing tesing", 120, 120, ILI9341_WHITE, 2, ILI9341_BLACK);

    // ili9341_drawImage(&display, 10, 10, PIKACHU_WIDTH, PIKACHU_HEIGHT, pikachu);

    uint16_t tx, ty;
    uint16_t px, py;
    while (1)
    {
        if (xpt2046_readPointCalibrated(&touch, &px, &py))
        {
            drawPixel(&display, px, py, ILI9341_WHITE);
        }
        usleep(10000);
    }

    while (1)
    {
    }
    return 0;
}
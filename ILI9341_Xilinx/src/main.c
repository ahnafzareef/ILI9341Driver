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

    // ---- Title ----
    ili9341_drawText(&display, "Ahnaf Zareef", 6, 6, ILI9341_CYAN, 2, ILI9341_BLACK);
    ili9341_drawText(&display, "ILI9341 Driver Library", 6, 28, ILI9341_WHITE, 1, ILI9341_BLACK);

    // ---- Two Pikachus: normal + inverted colours ----
    static uint16_t pikachu_inv[PIKACHU_WIDTH * PIKACHU_HEIGHT];
    for (int i = 0; i < PIKACHU_WIDTH * PIKACHU_HEIGHT; i++)
        pikachu_inv[i] = ~pikachu[i];

    ili9341_drawImage(&display, 20, 50, PIKACHU_WIDTH, PIKACHU_HEIGHT, pikachu);
    ili9341_drawImage(&display, 130, 50, PIKACHU_WIDTH, PIKACHU_HEIGHT, pikachu_inv);

    // ---- Row of different-sized rectangles ----
    ili9341_fill_rect(&display, 10, 115, 20, 20, ILI9341_RED);
    ili9341_fill_rect(&display, 40, 115, 30, 30, ILI9341_GREEN);
    ili9341_fill_rect(&display, 80, 115, 40, 40, ILI9341_BLUE);
    ili9341_fill_rect(&display, 130, 115, 50, 50, ILI9341_YELLOW);
    ili9341_fill_rect(&display, 190, 115, 40, 40, ILI9341_MAGENTA);

    // ---- Touch region label + box (bottom half) ----
    ili9341_drawText(&display, "Touch below to draw:", 6, 180, ILI9341_WHITE, 1, ILI9341_BLACK);
    ili9341_fill_rect(&display, 0, 195, ILI9341_WIDTH, ILI9341_HEIGHT - 195, ILI9341_DARKGREY);

    // ---- Live touch loop ----
    uint16_t px, py;
    while (1)
    {
        if (xpt2046_readPointCalibrated(&touch, &px, &py))
        {
            if (py >= 195)
            { // only inside the draw box
                ili9341_fill_rect(&display, px, py, 4, 4, ILI9341_CYAN);
            }
        }
        usleep(5000);
    }

    return 0;
}
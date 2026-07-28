#include "ILI9341_XIL.h"
#include <stdio.h>
#include "sleep.h"
#include <stdlib.h>

void ili9341_init(ili9341_t *display, XSpi *Spi, XGpio *Gpio, u32 rst_mask, u32 dc_mask, unsigned gpio_channel)
{
    // Update Variable States
    display->spi = Spi;
    display->gpio = Gpio;
    display->rst_mask = rst_mask;
    display->dc_mask = dc_mask;
    display->gpio_channel = gpio_channel;
    display->height = ILI9341_HEIGHT;
    display->width = ILI9341_WIDTH;

    if (Spi->IsStarted != XIL_COMPONENT_IS_STARTED)
        XSpi_Start(Spi);

    // Turn on SPI Transmitter. By default AXI Quad has transmitter off. Turn on
    // The Control Register has bit 8 as the Inhibit.
    u32 controlRegister = XSpi_GetControlReg(Spi);
    controlRegister &= ~(1 << 8); // Clearing Bit 8.
    XSpi_SetControlReg(Spi, controlRegister);

    initDisplay(display);
} // ili9341_init

// For Filling WHOLE Screen.
void ili9341_fill_screen(ili9341_t *display, uint16_t colour)
{
    ili9341_fill_rect(display, 0, 0, display->width, display->height, colour);

} // ili9341_fill_screen

void setPinHi(ili9341_t *display, u32 mask)
{
    XGpio_DiscreteSet(display->gpio, display->gpio_channel, mask);
}
void setPinLo(ili9341_t *display, u32 mask)
{
    XGpio_DiscreteClear(display->gpio, display->gpio_channel, mask);
}

void writeCommand(ili9341_t *display, uint8_t command)
{
    setPinLo(display, display->dc_mask); // Command Mode
    XSpi_Transfer(display->spi, &command, NULL, 1);
}

void writeData(ili9341_t *display, uint8_t data)
{
    setPinHi(display, display->dc_mask); // Data Mode
    XSpi_Transfer(display->spi, &data, NULL, 1);
}

void setAddressWindow(ili9341_t *display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    writeCommand(display, 0x2A);         // Column Address Set
    setPinHi(display, display->dc_mask); // Data Mode, next bytes read as data.

    // send the four coordinates, x0,y0,x1,y1
    uint8_t data[4];
    data[0] = x0 >> 8;   // hi byte of x0
    data[1] = x0 & 0xFF; // lo byte of x0
    data[2] = x1 >> 8;   // hi byte of x1
    data[3] = x1 & 0xFF; // lo byte of x1

    XSpi_Transfer(display->spi, data, NULL, 4);

    writeCommand(display, 0x2B); // Rows
    setPinHi(display, display->dc_mask);

    data[0] = y0 >> 8;   // hi byte of y0
    data[1] = y0 & 0xFF; // lo byte of y0
    data[2] = y1 >> 8;   // hi byte of y1
    data[3] = y1 & 0xFF; // lo byte of y1
    XSpi_Transfer(display->spi, data, NULL, 4);

    writeCommand(display, 0x2C); // Memory Write
} // setAddressWindow

void ili9341_fill_rect(ili9341_t *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    setAddressWindow(display, x, y, x + w - 1, y + h - 1);
    setPinHi(display, display->dc_mask); // Data Mode

    int totalPixels = w * h;

    u8 hi = colour >> 8;   // hi
    u8 lo = colour & 0xFF; // low

    for (int i = 0; i < totalPixels; i++)
    {
        XSpi_Transfer(display->spi, &hi, NULL, 1);
        XSpi_Transfer(display->spi, &lo, NULL, 1);
    }
}
// write pixel is to set colour wherever the address window is.
void writePixel(ili9341_t *display, uint16_t colour)
{
    setPinHi(display, display->dc_mask); // Data Mode

    u8 hi = colour >> 8;   // hi
    u8 lo = colour & 0xFF; // low

    XSpi_Transfer(display->spi, &hi, NULL, 1);
    XSpi_Transfer(display->spi, &lo, NULL, 1);
} // writePixel
// draw pixel draws at this coordinate.
void drawPixel(ili9341_t *display, uint16_t x, uint16_t y, uint16_t colour)
{
    setAddressWindow(display, x, y, x, y); // Set to one pixel
    writePixel(display, colour);
}
void ILI9341_reset(ili9341_t *display)
{
    // Reset the display
    setPinHi(display, display->rst_mask);
    usleep(20000); // 20ms
    setPinLo(display, display->rst_mask);
    usleep(20000); // 20ms
    setPinHi(display, display->rst_mask);
    usleep(150000); // 150ms
}
void initDisplay(ili9341_t *display)
{

    // Need Hardware Reset + software reset before init.
    ILI9341_reset(display);

    writeCommand(display, 0x01); // Software Reset
    usleep(150000);              // 150ms

    // init sequence (From STM32 ILI9341 Driver Library)

    // POWER CONTROL A
    writeCommand(display, 0xCB);
    writeData(display, 0x39);
    writeData(display, 0x2C);
    writeData(display, 0x00);
    writeData(display, 0x34);
    writeData(display, 0x02);

    // POWER CONTROL B
    writeCommand(display, 0xCF);
    writeData(display, 0x00);
    writeData(display, 0xC1);
    writeData(display, 0x30);

    // DRIVER TIMING CONTROL A
    writeCommand(display, 0xE8);
    writeData(display, 0x85);
    writeData(display, 0x00);
    writeData(display, 0x78);

    // DRIVER TIMING CONTROL B
    writeCommand(display, 0xEA);
    writeData(display, 0x00);
    writeData(display, 0x00);

    // POWER ON SEQUENCE CONTROL
    writeCommand(display, 0xED);
    writeData(display, 0x64);
    writeData(display, 0x03);
    writeData(display, 0x12);
    writeData(display, 0x81);

    // PUMP RATIO CONTROL
    writeCommand(display, 0xF7);
    writeData(display, 0x20);

    // POWER CONTROL,VRH[5:0]
    writeCommand(display, 0xC0);
    writeData(display, 0x23);

    // POWER CONTROL,SAP[2:0];BT[3:0]
    writeCommand(display, 0xC1);
    writeData(display, 0x10);

    // VCM CONTROL
    writeCommand(display, 0xC5);
    writeData(display, 0x3E);
    writeData(display, 0x28);

    // VCM CONTROL 2
    writeCommand(display, 0xC7);
    writeData(display, 0x86);

    // MEMORY ACCESS CONTROL
    writeCommand(display, 0x36);
    writeData(display, 0x48);

    // PIXEL FORMAT
    writeCommand(display, 0x3A);
    writeData(display, 0x55);

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    writeCommand(display, 0xB1);
    writeData(display, 0x00);
    writeData(display, 0x18);

    // DISPLAY FUNCTION CONTROL
    writeCommand(display, 0xB6);
    writeData(display, 0x08);
    writeData(display, 0x82);
    writeData(display, 0x27);

    // 3GAMMA FUNCTION DISABLE
    writeCommand(display, 0xF2);
    writeData(display, 0x00);

    // GAMMA CURVE SELECTED
    writeCommand(display, 0x26);
    writeData(display, 0x01);

    // POSITIVE GAMMA CORRECTION
    writeCommand(display, 0xE0);
    writeData(display, 0x0F);
    writeData(display, 0x31);
    writeData(display, 0x2B);
    writeData(display, 0x0C);
    writeData(display, 0x0E);
    writeData(display, 0x08);
    writeData(display, 0x4E);
    writeData(display, 0xF1);
    writeData(display, 0x37);
    writeData(display, 0x07);
    writeData(display, 0x10);
    writeData(display, 0x03);
    writeData(display, 0x0E);
    writeData(display, 0x09);
    writeData(display, 0x00);

    // NEGATIVE GAMMA CORRECTION
    writeCommand(display, 0xE1);
    writeData(display, 0x00);
    writeData(display, 0x0E);
    writeData(display, 0x14);
    writeData(display, 0x03);
    writeData(display, 0x11);
    writeData(display, 0x07);
    writeData(display, 0x31);
    writeData(display, 0xC1);
    writeData(display, 0x48);
    writeData(display, 0x08);
    writeData(display, 0x0F);
    writeData(display, 0x0C);
    writeData(display, 0x31);
    writeData(display, 0x36);
    writeData(display, 0x0F);

    // EXIT SLEEP
    writeCommand(display, 0x11);
    usleep(120000); // 120ms

    // TURN ON DISPLAY
    writeCommand(display, 0x29);
}
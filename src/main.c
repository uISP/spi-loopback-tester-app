#include <arch/antares.h>
#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <generated/usbconfig.h>
#include <arch/vusb/usbportability.h>
#include <arch/vusb/usbdrv.h>


#define CSN_PIN   (1<<CONFIG_CSN_PIN)
#define  CE_PIN   (1<<CONFIG_CE_PIN)
#define SPI_PORTX PORTB
#define SPI_DDRX  DDRB
#define SPI_MOSI  3
#define SPI_MISO  4
#define SPI_SCK   5
#define SPI_SS    2


static uint8_t spi_xfer(uint8_t data)
{
	uint8_t report;
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	report = SPDR;
	return report;
}


ANTARES_INIT_LOW(spi_init)
{
	SPI_DDRX &= ~(1<<SPI_MISO);
	SPI_DDRX |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	SPI_PORTX |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_MISO)|(1<<SPI_SS);
	
	
	SPCR = ((1<<SPE)|               /* SPI Enable */
		(0<<SPIE)|              /* SPI Interrupt Enable */
		(0<<DORD)|              /* Data Order (0:MSB first / 1:LSB first) */
		(1<<MSTR)|              /* Master/Slave select */   
		(0<<SPR1)|(0<<SPR0)|    /* SPI Clock Rate */
		(0<<CPOL)|              /* Clock Polarity (0:SCK low / 1:SCK hi when idle) */
		(0<<CPHA));             /* Clock Phase (0:leading / 1:trailing edge sampling) */
	
	SPSR = (1<<SPI2X);              /* Double Clock Rate */
}


char msg[32];
uchar   usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;
	if (rq->bRequest == 0) {
		uint8_t tmp = spi_xfer(rq->wValue.bytes[0]);
		sprintf(msg, "tx 0x%x rx 0x%x result: %s\n", rq->wValue.bytes[0], tmp, 
			(tmp == rq->wValue.bytes[0]) ? "PASS" : "EPIC FAIL");
		usbMsgPtr = msg;
		return strlen(msg)+1;
	}
	return 0;
}


inline void usbReconnect()
{
	DDRD=0xff;
	_delay_ms(250);
	DDRD=0;
}

ANTARES_INIT_LOW(io_init)
{
	DDRC=1<<2;
	PORTC=0xff;
 	usbReconnect();
}

ANTARES_INIT_HIGH(uinit)
{
  	usbInit();
}


ANTARES_APP(usb_app)
{
	usbPoll();
}

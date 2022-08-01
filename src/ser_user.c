
#include <stdint.h>
#include "serial.h"
#include "ser_user.h"
#include "hardware_user.h"

// serial baudrate
#define	SER_BAUD		115200

// TX Fifo (soft-fifo read by TX complete interrupt)
static uint8_t  TxFifo[SER_TX_FIFO_SIZE];

// RX Fifo (soft-fifo write by RX complete interrupt)
static uint8_t  RxFifo[SER_RX_FIFO_SIZE];



// Init the serial interface
void  SER_UserInit ( void)
{
	SERInit_t  setup;
	
	
	setup.prescaler = ( HW_CPU_CLOCK_HZ + 8 * SER_BAUD) / ( 16 * SER_BAUD);
	setup.databits = 8;
	setup.stopbits = 1;
	setup.parity = SER_PARITY_NONE;
	
	setup.pTxFifo = &TxFifo;
	setup.pRxFifo = &RxFifo;
	
	setup.TxFifoSize = SER_TX_FIFO_SIZE;
	setup.RxFifoSize = SER_RX_FIFO_SIZE;
	
	SER_Initialize ( SER_PORT1, &setup);
}


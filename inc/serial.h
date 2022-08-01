
#ifndef _SERIAL_H_
#define _SERIAL_H_

//
//	serial.h
//
//	-------------------------------------------------------------------
//! @addtogroup SERIAL
//! <h3> API Functions for serial port access </h3>
//!
//! Functions for serial interaction. The data is buffered by
//! software fifos. The functions will target to the main() level.
//! When used on interrupts, the user must provide own wrapper functions
//! for interrupt safety.
//!
//! <h3> Targets: </h3>
//! - PCAN-Router FD
//!
//	-------------------------------------------------------------------
//
//	Copyright (C) PEAK-System Technik GmbH, Darmstadt
//	more Info at http://www.peak-system.com 
//
//! @{


////////////////////////////////////////////////////////////
//! @name port-handles
//! Use one of these port handles to access a serial port
//! @{
#define	SER_PORT1			0		//!< Serial Port 1
/*! @}*/


////////////////////////////////////////////////////////////
//! @name errors
//! A function returns one of these errors:
//! @{
#define	SER_ERR_OK				0		//!< OK, no error
#define	SER_ERR_ILLHANDLE		1		//!< invalid port handle
#define	SER_ERR_RX_EMPTY		2		//!< RX fifo is empty
#define	SER_ERR_RX_OVERRUN	3		//!< RX fifo overrun since last read
#define	SER_ERR_TX_SPACE		4		//!< no space on TX fifo
#define	SER_ERR_ILLSETUP		5		//!< invalid value in setup
#define	SER_ERR_ILLPARAMVAL	6		//!< invalid parameter
/*! @}*/


////////////////////////////////////////////////////////////
//! @name parity
//! options for initialization used by SER_Init()
//! @{
#define	SER_PARITY_NONE			0		//!< no parity
#define	SER_PARITY_EVEN			1		//!< even parity
#define	SER_PARITY_ODD			2		//!< odd parity
/*! @}*/


////////////////////////////////////////////////////////////
//! @name types
//! Basetypes for serial functions.
//! @{
#define	SERResult_t		uint32_t			//!< status type for API functions
#define	SERHandle_t		uint8_t			//!< SER handle type
/*! @}*/


////////////////////////////////////////////////////////////
//! @name structures
//! structures for serial interaction
//! @{

//! @brief
//! structure for serial initialization
typedef struct {

	uint32_t		prescaler;			//!< prescaler value for serial port
	
	void		*pTxFifo;			//!< pointer for TX buffer
	void		*pRxFifo;			//!< pointer for RX buffer
	
	uint8_t		databits;			//!< number of databits ( 5..8)
	uint8_t		stopbits;			//!< number of stopbits ( 1..2)
	uint8_t		parity;				//!< mode of parity ( see SER_PARITY_..)
	uint8_t		TxFifoSize;			//!< size of TX buffer space
	uint8_t		RxFifoSize;			//!< size of RX buffer space
	
} SERInit_t;


/*! @}*/


#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////
//! @name functions
//! This section will describe the API functions. The functions will target
//! to the main() level. 
//! @{


//! @brief
//! Initialize a serial port
//!
//! @param		hPort		port to initialize
//! @param		setup		structure with setup values
//!
//! @return		one error of SER_ERR_...
SERResult_t	 SER_Initialize (	SERHandle_t  hPort, SERInit_t  *setup);


//! @brief
//! Send data over serial. The function will write the complete buffer or nothing.
//! If TX fifo has not enough space for the buffer the function will return SER_ERR_TX_SPACE.
//!
//! @param		hPort			port to write data to
//! @param		buffer			user buffer with data to send
//! @param		buffsize		number of bytes to send ( 1..255)
//!
//! @return		one error of SER_ERR_...
SERResult_t	 SER_Write ( SERHandle_t  hPort, void  *buffer, uint8_t  buffsize);


//! @brief
//! Read data from RX fifo. If fifo is empty SER_ERR_RX_EMPTY is returned. If an overrun
//! has occoured since last read SER_ERR_RX_OVERRUN is returned. The user buffer is filled
//! with one byte representing an overrun-counter.
//!
//! @param		hPort			port to read data from fifo
//! @param		buffer		user buffer to write data to
//! @param		buffsize		size of the user buffer ( 1..255)
//! @param		bytesread	actual bytes read from fifo to user buffer
//!
//! @return		one error of SER_ERR_...
SERResult_t  SER_Read ( SERHandle_t  hPort, void  *buffer, uint8_t  buffsize, uint8_t  *bytesread);


//! @brief
//! Reset TX fifo (soft-fifo).
//!
//! @param		hPort		serial port
//!
//! @return		one error of SER_ERR_...
SERResult_t  SER_ResetTX ( SERHandle_t  hPort);


//! @brief
//! Reset RX fifo (soft-fifo).
//!
//! @param		hPort		serial port
//!
//! @return		one error of SER_ERR_...
SERResult_t  SER_ResetRX ( SERHandle_t  hPort);


/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif


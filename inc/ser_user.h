
#ifndef _SER_USER_H_
#define _SER_USER_H_

// size for TX Fifo
#define	SER_TX_FIFO_SIZE		255

// size for RX Fifo
#define	SER_RX_FIFO_SIZE		60

// user protos

#ifdef __cplusplus
extern "C" {
#endif


void  SER_UserInit ( void);


#ifdef __cplusplus
}
#endif


#endif


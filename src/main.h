#ifndef  _MAIN_H_
#define  _MAIN_H_
  
#define	USER_TEXT_SIZE		70
#define	USER_TX_SIZE		70

#define _TYPE				0x01
#define _STX				0x02
#define _ETX				0x03
#define _CMD				0x02
#define _CNT				0x03
#define _DUMMY				0x05

//#define MAX 		50                      //버퍼 사이즈 5초
#define MAX 		10                      //버퍼 사이즈 1초
//#define CAN_CNT	160+4+1             //can data 160 + nid + count
#define CAN_CNT		64+4+1                  //can data 64 + nid + count <-- voltage  can data 48 + nid + count <-- temp 
#define VOLT_CNT	64
#define TEMP_CNT	48

#define	MSG_100_PERIOD_US		100000          //0.1초
#define VOLTAGE_CAN_LENGTH      64
#define TEMP_CAN_LENGTH         48

#include <stdint.h>
#include "can.h"
#include "can_user.h"
#include "hardware.h"
#include "serial.h"
#include "ser_user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ARMCM4_FP.h"
#include "lpc407x_8x_177x_8x.h"
#include "systime.h"

typedef struct{
	uint8_t data[CAN_CNT];
}__CAN_DATA;

typedef struct{
    uint8_t type;
    uint8_t cmd;
    uint8_t dlc;
    uint8_t data[USER_TEXT_SIZE];

    int8_t temp;
    uint8_t uart_ready;
    uint8_t uart_read;
}__UART_DATA;

typedef enum{
    idle_status, stx_status, type_status, cmd_status, count_status, data_staus, etx_status, parsing_status, sending_status
}_UART_STATUS;

int data_ok_parsing(uint8_t *data, uint8_t dlc);
void set_mac_address(uint8_t *data, uint8_t dlc);
void create_enable_wbms(uint8_t *data, uint8_t dlc);
void init_mac_address(void);
void standby_mode(void);
void wakeup_mode(void);
void en_queue(void);
uint8_t * de_queue(void);
int is_empty(void);
int is_full(void);
void clear_queue(void);
void set_can_id(uint8_t *data, uint8_t dlc);
void send_direct(uint8_t *data, uint8_t dlc);
void set_sampling_time(uint8_t *data, uint8_t dle);
#endif


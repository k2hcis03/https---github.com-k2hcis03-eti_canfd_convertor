
#ifndef  _MAIN_H_
#define  _MAIN_H_

#define	USER_TEXT_SIZE		64
#define	USER_TX_SIZE		64

#define TYPE				0x01
#define STX					0x02
#define ETX					0x03
#define CMD					0x02
#define CNT					0x03
#define DUMMY				0x05

//#define MAX 		50          //버퍼 사이즈 5초
#define MAX 		10          //버퍼 사이즈 1초
//#define CAN_CNT		160+4+1             //can data 160 + nid + count
#define CAN_CNT		80+4+1             //can data 80 + nid + count

#include <stdint.h>
#include "can.h"
#include "can_user.h"
#include "hardware.h"
#include "serial.h"
#include "ser_user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#endif


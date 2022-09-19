/*
2022-08-01
@K2H
- LED2 기능 추가 : 컨버터에서 CAN으로 데이터 전송 시, LED 토글
*/
#include "main.h"

// abstract:
// Here we will forward CAN data to the serial port. Additionally we receive some data from
// the serial port to fill up a CAN message for transmission.


// size for user text buffer received from partner like terminal program or other device

int front=-1;
int rear=-1;
uint32_t can_id = 0x200;

// identifier is needed by PEAK-Flash.exe -> do not delete
const char Ident[] __attribute__ ((used)) = { "PCAN-Router_FD"};
uint8_t queue_data[MAX][CAN_CNT];

// variables for LED toggle
static uint8_t LED_toggleCAN1;
static uint8_t LED_toggleCAN2;

// variables for serial reception
uint8_t battery_ok = 0;
uint8_t mac_ok = 0;
static char user_tx[USER_TX_SIZE];
__CAN_DATA can_data;
__UART_DATA uart_data;

#if 0
// main_greeting()
// transmit a message at module start
static void  main_greeting ( void)
{
	CANTxMsg_t  Msg;
	
	
	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN8_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD;
	Msg.id       = 0x123;
	
	Msg.data32[0] = 0x67452301;
	Msg.data32[1] = 0xEFCDAB89;
	
	// overwrite byte 0 with FPGA version
	Msg.data8[0] = FPGA_VERSION;
	
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}
#endif

//
int is_empty(void)
{
    if(front==rear){//front와 rear가 같으면 큐는 비어있는 상태 
        return 1;
	}
    else{
		return 0;
	}
}
int is_full(void)
{
    int tmp=(rear+1)%MAX; //원형 큐에서 rear+1을 MAX로 나눈 나머지값이
    if(tmp==front){ //front와 같으면 큐는 가득차 있는 상태 
        return 1;
	}
    else{
        return 0;
	}
}

void clear_queue(void)
{
    front=-1;
	rear=-1;
}

void en_queue(void)
{
    if(is_full()){
        //SER_Write ( SER_PORT1, "full", 4);		
	}
    else{
		rear = (rear+1)%MAX;
		memcpy(queue_data[rear], can_data.data, 160);
		//queue[rear]=value;
	}
 
}
uint8_t * de_queue(void)
{
    if(is_empty()){
		//printf("Queue is Empty.\n");
		//SER_Write ( SER_PORT1, "empty", 4);	
		return NULL;		
	}
    else{
        front = (front+1)%MAX;
        //return queue[front];
		return queue_data[front];
    }
}
//
int data_ok_parsing(uint8_t *data, uint8_t dlc)
{
	uint8_t i = 0;
	uint8_t ready[] = {0x00, 0x65, 0x5d, 0x03, 0x00, 0x01, 0x00, 0x00};

	for (i = 0; i < dlc; i++){
		if (data[i] != ready[i]){
			break;
		}
	}
	return ((i == dlc) ? 1 : 0);
}

void init_mac_address(void)
{
	uint8_t mac[] = {0x17, 0xAF, 0x01, 0x00, 0x00, 0x00, 0x10};

	CANTxMsg_t  Msg;
	
	mac_ok = 0;

	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN7_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < Msg.dlc; i++){
		Msg.data8[i] = mac[i];
	}
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}

void send_direct(uint8_t *data, uint8_t dlc)
{
	CANTxMsg_t  Msg;

	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = data[4]; //dlc;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
#if 0
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < dlc; i++){
		Msg.data8[i] = data[i];
	}
#else
	Msg.id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];		
	
	for (uint8_t i = 0; i < Msg.dlc; i++){
		Msg.data8[i] = data[5+i];
	}
#endif
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}

void set_mac_address(uint8_t *data, uint8_t dlc)
{
	CANTxMsg_t  Msg;

	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN8_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < dlc; i++){
		Msg.data8[i] = data[i];
	}
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}

void set_can_id(uint8_t *data, uint8_t dlc)
{
	can_id = (uint32_t) (data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]);
}

void create_enable_wbms(uint8_t *data, uint8_t dlc)
{
	//uint8_t start[] = {0x02, 0x02, 0x01, 0x39, 0x03, 0x01};

	CANTxMsg_t  Msg;
	
	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN6_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < Msg.dlc; i++){
		Msg.data8[i] = data[i];
	}
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}

void standby_mode(void)
{
	uint8_t data[] = {0x0a, 0x01};

	CANTxMsg_t  Msg;
	
	//mac_ok = 0;

	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN2_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < Msg.dlc; i++){
		Msg.data8[i] = data[i];
	}
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}

void wakeup_mode(void)
{
	uint8_t data[] = {0x20, 0x01};

	CANTxMsg_t  Msg;
	
	//mac_ok = 0;

	Msg.bufftype = CAN_BUFFER_TX_MSG;
	Msg.dlc      = CAN_LEN2_DLC;
	Msg.msgtype  = CAN_MSGTYPE_STANDARD | CAN_MSGTYPE_FDF;
	Msg.id       = 0x10;
	
	for (uint8_t i = 0; i < Msg.dlc; i++){
		Msg.data8[i] = data[i];
	}
	// Send Msg
	CAN_Write ( CAN_BUS1, &Msg);
}
// main()
// entry point from startup
int  main ( void)
{
	uint8_t toggle_first = 0;
	uint8_t uart_status = idle_status;
	uint8_t led1_toggle = 0;
	uint8_t led2_toggle = 0;
	// init hardware and timer 0. Timer 0 is free running
	// with 1 us resolution without any IRQ.
	HW_Init();
	
	// init CAN
	CAN_UserInit();
	
	// init serial
	SER_UserInit();
	
	// set green LEDs for CAN1 and CAN2
	HW_SetLED ( HW_LED_CAN1, HW_LED_GREEN);
	HW_SetLED ( HW_LED_CAN2, HW_LED_GREEN);
	
	// send the greeting message
	
	// init variables
	user_tx[0] = STX;
	user_tx[1] = 0xB5;
	// main loop
	HW_SetLED ( HW_LED_CAN2, HW_LED_ORANGE);
	while ( 1)
	{
		CANRxMsg_t  RxMsg;
		//CANTxMsg_t  TxMsg;
		SERResult_t  serstat;
		uint8_t  ser_read;
		uint8_t * status;
		uint8_t temp;

		// process messages from CAN1
		if ( CAN_UserRead ( CAN_BUS1, &RxMsg) == CAN_ERR_OK)
		{
			// message received from CAN1
			if ((led1_toggle++ % 5) == 0){
				LED_toggleCAN1 ^= 1;
			}

			if ( LED_toggleCAN1)  
			{
				HW_SetLED ( HW_LED_CAN1, HW_LED_ORANGE);
			}
			   
			else
			{
				HW_SetLED ( HW_LED_CAN1, HW_LED_GREEN);
			}   
			// catch ID  to send on serial as ascii
			// 전원 인가 응답 및 장비 응답
			//if ( RxMsg.id == 0x20 && (RxMsg.msgtype == CAN_MSGTYPE_EXTENDED || RxMsg.msgtype == CAN_MSGTYPE_STANDARD || RxMsg.msgtype == CAN_MSGTYPE_FDF))
			if ( RxMsg.id == 0x20)
			{
				if (data_ok_parsing(RxMsg.data8, RxMsg.dlc)){
					battery_ok = 1;
				}
				// 장비에서 MAC Address 설정이 완료되면 응답
				if (RxMsg.dlc == 1){			
					mac_ok = 1;
				}
			}else if( RxMsg.id == can_id && RxMsg.dlc == CAN_LEN64_DLC ){
				if (toggle_first == 0){
					toggle_first = 1;

					can_data.data[0] = RxMsg.id & 0x000000FF;
					can_data.data[1] = (RxMsg.id >> 8) & 0x000000FF;
					can_data.data[2] = (RxMsg.id >> 16) & 0x000000FF;
					can_data.data[3] = (RxMsg.id >> 24) & 0x000000FF;

					can_data.data[4] = CAN_CNT;
					memcpy(&can_data.data[5], &RxMsg.data8[0], 64);				//64개
				}
			}else if( RxMsg.id == (can_id+1) && RxMsg.dlc == CAN_LEN16_DLC){
				if (toggle_first == 1){
					toggle_first = 0;
					memcpy(&can_data.data[64+5], &RxMsg.data8[0], 16);			//16개
					en_queue();
				}else{
					toggle_first = 0;
				}
			}else if( RxMsg.id >= 0x400 && RxMsg.id <= 0x410){
			
			}else{
				//SER_Write ( SER_PORT1, &RxMsg.id, 4);
				//SER_Write ( SER_PORT1, &RxMsg.data8, RxMsg.dlc);
			}
		}
		
		
		// process messages from CAN2
		if ( CAN_UserRead ( CAN_BUS2, &RxMsg) == CAN_ERR_OK)
		{
			// message received from CAN2
			LED_toggleCAN2 ^= 1;

			if ( LED_toggleCAN2)
			{
				HW_SetLED ( HW_LED_CAN2, HW_LED_ORANGE);
			}
			
			else
			{
				HW_SetLED ( HW_LED_CAN2, HW_LED_GREEN);
			}
			
			// forward message to CAN1
			CAN_Write ( CAN_BUS1, &RxMsg);
		}
		// check input from serial. Modify code for your own needs. Check buffer for overflows if needed.
		serstat = SER_Read ( SER_PORT1, &temp, 1, &ser_read);
		
		if (uart_status == idle_status){
			uart_data.uart_ready = 0;

			if (led2_toggle){
				HW_SetLED ( HW_LED_CAN2, HW_LED_ORANGE);		//명령어가 들어오면 LED는 오렌지 색
			}else{
				HW_SetLED ( HW_LED_CAN2, HW_LED_GREEN);			//명령어가 들어오면 LED는 그린 색
			}
			

			if (serstat == SER_ERR_OK && ser_read > 0 ){
				if (temp == STX){
					uart_status = stx_status;
				}else{
					uart_status = idle_status;
				}
			}
		}else if(uart_status == stx_status){
			if (serstat == SER_ERR_OK && ser_read > 0 ){
				if (temp == 0xB4){
					uart_status = cmd_status;
					uart_data.type = temp;
				}else{
					uart_status = idle_status;
				}
			}
		}else if(uart_status == cmd_status){
			if (serstat == SER_ERR_OK && ser_read > 0 ){
				uart_status = count_status;
				uart_data.cmd = temp;
			}
		}else if(uart_status == count_status){
			if (serstat == SER_ERR_OK && ser_read > 0 ){
				uart_status = data_staus;
				uart_data.dlc = temp;
				uart_data.temp = 0;

				if (uart_data.dlc == 0){
					uart_status = etx_status;
				}
			}
		}else if(uart_status == data_staus){
			if (serstat == SER_ERR_OK && ser_read > 0 ){
				uart_status = data_staus;
				uart_data.data[uart_data.temp++] = temp;
				if (uart_data.temp == uart_data.dlc){
					uart_status = etx_status;
				}else if(uart_data.temp >= 30){		//데이터가 30개 이상이면 idle상태로 들어감
					uart_status = idle_status;
				}
			}
		}else if(uart_status == etx_status){
			if (serstat == SER_ERR_OK && ser_read > 0 ){
				if (temp == ETX){
					uart_data.uart_ready = 1;
				}else{
					uart_data.uart_ready = 0;
				}
				uart_status = idle_status;
				led2_toggle = !led2_toggle;
			}
		}
		if ( uart_data.uart_ready ){
			switch (uart_data.cmd){
				case 0xC0:										//전원 인가 확인
					user_tx[2] = 0xC0;
					user_tx[3] = 2;
					user_tx[4] = battery_ok;
					user_tx[5] = mac_ok;
					user_tx[6] = ETX;
					SER_Write ( SER_PORT1, user_tx, DUMMY+2);
					break;
				case 0xC1:										//MAC 주소 초기화
					user_tx[2] = 0xC1;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					init_mac_address();
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC2:										//MAC 주소 설정	
					user_tx[2] = 0xC2;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					set_mac_address(uart_data.data, uart_data.dlc);
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC3:										//데이터 전송 15초뒤에 데이터 수신
					user_tx[2] = 0xC3;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					create_enable_wbms(uart_data.data, uart_data.dlc);
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC4:										//STAND BY 모드 진입
					user_tx[2] = 0xC4;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					standby_mode();
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC5:										//WAKE UP 모드 진입
					user_tx[2] = 0xC5;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					wakeup_mode();
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC6:										//CIU 데이터 전송
					status = de_queue();

					if (status != NULL){
						user_tx[2] = 0xC6;
						user_tx[3] = CAN_CNT;
						SER_Write ( SER_PORT1, user_tx, DUMMY-1);
						
						SER_Write ( SER_PORT1, status, CAN_CNT);	//status is pointer for data
						user_tx[4] = ETX;
						SER_Write ( SER_PORT1, &user_tx[4], 1);
					}else{
						user_tx[2] = 0xC6;
						user_tx[3] = 0x00;
						SER_Write ( SER_PORT1, user_tx, DUMMY);
					}
					break;
				case 0xC7:										//QUEUE 클리어
					user_tx[2] = 0xC7;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					clear_queue();
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xC8:										//재전송
					user_tx[2] = 0xC6;
					user_tx[3] = CAN_CNT;
					SER_Write ( SER_PORT1, user_tx, DUMMY-1);
					
					SER_Write ( SER_PORT1, status, CAN_CNT);
					user_tx[4] = ETX;
					SER_Write ( SER_PORT1, &user_tx[4], 1);
					break;
				case 0xC9:										//CAN ID 설정
					user_tx[2] = 0xC9;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					set_can_id(uart_data.data, uart_data.dlc);
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
				case 0xCA:										//CAN Direct 송신
					user_tx[2] = 0xCA;
					user_tx[3] = 0;
					user_tx[4] = ETX;
					send_direct(uart_data.data, uart_data.dlc);
					SER_Write ( SER_PORT1, user_tx, DUMMY);
					break;
			}
		}
	}
}


#ifndef  _SYSTIME_H_
#define  _SYSTIME_H_


// get the current time
#define	SYSTIME_NOW		LPC_TIM0->TC

// max for this MCU
#define	SYSTIME_MAX		0xFFFFFFFF

// calc a timediff
#define	SYSTIME_DIFF( _First, _Second)		(( _First <= _Second ? ( _Second - _First) : (( SYSTIME_MAX - _First) + _Second)))

// this is our basetype for time calculations
#define	SYSTIME_t	uint32_t


// function protos

void  SYSTIME_Wait ( SYSTIME_t  micros);


#endif


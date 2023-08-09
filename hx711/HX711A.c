/*
Lib for Hx711 24bit adc for weight scales
C++ / AVR microcontroller.
2016-11-21 by P vd Hoeven.

How it works:
Declaring an object of the THx711 type will call the constructor and
initialise the hx711 interface.
*/
#include "header.h"
#include "HX711A.hpp"
#include <util/atomic.h>

//===========================================================================
THx711 Hx711;

//===========================================================================
THx711::THx711( void) {
	HX711_CLK_PORT &= ~HX711_CLK_BIT;	// Output, high, (Power down mode).
	HX711_CLK_DDR	|= HX711_CLK_BIT;
	HX711_DATA_DDR &= ~HX711_DATA_BIT;	// Input.

	SetFunction( Func_Power_Down);

//	static uint16_t WeightDelay;		// Delay untill next measurement [ms].
//	static uint16_t WeightCount;		// Number of measurements to make.
	CallOffset = 37730;	// HX711 has 24 bits...
	CallMul = 0.0321;		//


//	digitalWrite( _pin_slk, 1);
//	delayMicroseconds( 100);
//	digitalWrite( _pin_slk, 0);

//	averageValue( );
//	this->setOffset( averageValue( ));
//	this->setScale( );
}

//===========================================================================
bool THx711::GetBit( void) {
// Read a single bit from hx711 hardware.
	bool Bit;

	ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
		// Atomic because hx711 will power down if clk is high for > 60us.
		HX711_CLK_PORT |= HX711_CLK_BIT;
		asm volatile( "nop\n\n");	// Works without nops, but more robust.
		HX711_CLK_PORT &= ~HX711_CLK_BIT;
	}
	Bit = ( HX711_DATA_PIN & HX711_DATA_BIT) ? true : false;
	return Bit;
}

//===========================================================================
uint8_t THx711::GetByte( void) {
// Shift a byte in from hx711.
	uint8_t Byte = 0;
	uint8_t Bit;

	for( Bit = 0x80; Bit; Bit >>= 1) {
		if( GetBit( )) {
			Byte |= Bit;
		}
	}
	return Byte;
}

//===========================================================================
int32_t THx711::GetValue( void) {
/* Read a raw value from hx711.
First read the 24 data bits.
Then toggle CLK an extra 1, 2 or 3 times to "program" hx711 function.
*/
	union {
		long Value;			// Measurement value from hx711.
		uint8_t Value_Byte[4];
	};

	Value_Byte[ 2] = GetByte( );
	Value_Byte[ 1] = GetByte( );
	Value_Byte[ 0] = GetByte( );
//	Value_Byte[ 2] ^= 0x80;
	Value_Byte[ 3] = (Value_Byte[ 2] & 0x80)? 0xFF : 0x00;	// Sign extend.

	switch( Func) {
	case Func_Unknown:
	case Func_Power_Down:
		// hx711 will power down if clk is high for > 60us.
		HX711_CLK_PORT |= HX711_CLK_BIT;
		break;
	case Func_A_128:	GetBit( );	// 27 pulses.
		// No Break.
	case Func_B_32:		GetBit( );	// 26 pulses
		// No Break.
	case Func_A_64:		GetBit( );	// 25 pulses.
		// No Break.
	}
	return Value;
}

//===========================================================================
void THx711::SetZero( void) {
	CallOffset = Measure();
}

//===========================================================================
//void THx711::setOffset( long offset) {
//	_offset = offset;
//}

//===========================================================================
/*long THx711::averageValue( uint8_t times) {
	long sum = 0;
	for ( uint8_t i = 0; i < times; i++){
		sum += getValue( );
	}
	return sum / times;
}
*/
//===========================================================================
//void THx711::setScale( float scale){
//	_scale = scale;
//}

//============================================================================
bool THx711::IsReady( ) {
// Check if HX711 data conversion has finished.

	if( HX711_CLK_PORT & HX711_CLK_BIT) {
		// hx711 is in power down mode.
		return false;
	}

	if( HX711_DATA_PORT & HX711_DATA_BIT) {
		// If clk is low, data bit is high during conversion.
		return false;
	}
	// Conversion has finished and data bit reads low.
	return true;
}

//============================================================================
float THx711::Measure( void) {
	// Do a measurement and return it as a floating pointnumber.

	if( Func == Func_Power_Down) {
		Func = Func_A_128;// SetFunction( );
	}
	if ( !IsReady( )) {
		return LastValue;
	}else {
		LastValue = GetValue();
	}

//	return static_cast<long>( ++value);
	return (CallOffset + LastValue) * CallMul;
}

//============================================================================
uint8_t THx711::Measure( char *pDest) {
	// Format a measurement in a string.
	long Value = Hx711.Measure() * 1000;

	PutS8Hex( pDest, Value);
	pDest[8] = ' ';
	SPrintFixed( pDest + 8, Value, 12, 3);
	pDest[20] = '\n';
	return 21;
}

//============================================================================
void THx711::SetFunction( E_FUNC Input) {

	switch( Input) {
	case Func_A_128:
	case Func_B_32:
	case Func_A_64:
		Func = Input;
		Measure();		// Dummy measurement to set the function.
		break;
	case Func_Power_Down:
	case Func_Unknown:
		Func = Func_Power_Down;
		// hx711 will power down if clk is high for > 60us.
		HX711_CLK_PORT |= HX711_CLK_BIT;
		break;
	}
}

/*
//============================================================================
double THx711::get_value( uint8_t times) {
	return read_average( times) - Offset;
}

//============================================================================
float THx711::get_units( uint8_t times) {
	return get_value( times) / Scale;
}

//============================================================================
void THx711::tare( uint8_t times) {
	double sum = read_average( times);
	set_offset( sum);
}

//============================================================================
void THx711::Set_Calbibration( float Value) {
	Cal = Value;
}

//============================================================================
float THx711::get_scale( void) {
	return Cal;
}

//============================================================================
void THx711::set_offset( long Value) {
	Offset = Value;
}

//============================================================================
long THx711::get_offset( void) {
	return Offset;
}

*/

/*===========================================================================
	End of all code.	Below this line is only comment.
-----------------------------------------------------------------------------
2017-07-26 When Ck stays low Data has a 77us high pulse every 11ms.
2017-07-26 When deliberately sending to few (19) clock pulses and the last
	bit is a "1" the HX711 times out after 20ms and makes data low.
Bug: 2016-11-21 Datasheet introduction claims that channel B has a fixed gain
	of 32. However, the timing diagram on page 5 shows a gain of 32 or 64 for
	channel B. Page 5 also suggests channel B is for "battery level detection".

===========================================================================*/


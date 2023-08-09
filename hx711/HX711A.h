#pragma once

#include <inttypes.h>

//===========================================================================
enum E_FUNC{
	Func_Unknown,
	Func_A_128,
	Func_B_32,
	Func_A_64,
	Func_Power_Down,
};

//===========================================================================
class THx711 {
	public:
	THx711( void);	// Constructor, does initialisation.
	//		virtual ~THx711();

	void SetFunction( enum E_FUNC= Func_A_128);
	void SetZero( void);
	// Set number of measurements to average.
	//		void SetAaverage(uint8_t Value = 10);

	// Measure a value, return last measured value.
	float Measure( void);
	uint8_t Measure( char * pDest);

	//		void Set_Calbibrate(float Calibrate = 1.f);
	//		float Get_Calbirate( );

	// set OFFSET, the value that's subtracted from the actual
	// reading (tare weight)
	//		void set_offset(long offset = 0);

	// get the current OFFSET
	//		long get_offset();
	private:
	enum E_FUNC Func;
	int32_t LastValue;	// Last measured value.
	//		int32_t Offset;	// Raw adc offset compensation.
	int32_t CallOffset;	// Add offset to make sure zero is zero.
	int32_t CallMul;	// Multiply to get right value at high loads.
	int32_t Tare;	// Tare, temporary offset.
	uint8_t Average;	// Number of measurements to average. (Max 255).
	bool GetBit( void);		// Get a single bit from hx711.
	uint8_t GetByte( void);	// Get a single byte from hx711.
	int32_t GetValue( void);// Get a raw value from hx711.

	bool IsReady ( void);
};

//===========================================================================
extern THx711 Hx711;
//===========================================================================


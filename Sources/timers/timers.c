

void timer_init(void) 
{
	TSCR2 = 0x00;
	TSCR2_TOI = TIMER_OVERFLOW_INT;
	TSCR2_TCRE = TIMER_RESET_ENABLE;
	TSCR2 |= TIMER_PRESCALER;	//Prescaler: 4 - de 0000 a FFFF: tarda 10ms en overflow
	TSCR1_TFFCA = FLAG_AUTOCLR;
	TSCR1_TEN = TIMER_START; // TEN
}

void oc_init(void)
{
	OC0_DISCONNECTED;
	TIOS |= OC_CHANNEL;	// Reserve OC channel
	TIE &= !OC_INT_ENABLE;	// set desired interrupts
	OC_FLAG_CLR;	// Flag clear
}

void ic_init(void)
{
	IC1_FALLING_EDGE;
	TIOS &= !IC_CHANNEL;	// Reserve IC channel
	TIE = IC_INT_ENABLE;
	
	IC_FLAG_CLR;

}
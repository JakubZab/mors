#include "MKL05Z4.h"
#include	"pit.h"



void PIT_Init(void)
{
	uint32_t tsv;
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;			// Wlaczenie sygnalu zegara do modulu PIT
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;				// Wlaczenie modulu PIT
	tsv=BUS_CLOCK;											// Przerwanie co 1s
	//tsv=BUS_CLOCK/2;											// Przerwanie co 0.5s
	//tsv=BUS_CLOCK/10;										// Przerwanie co 100ms
	//tsv=BUS_CLOCK/100;									// Przerwanie co 10ms
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(tsv);		// Zaladowanie wartosci startowej
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK;		// Odblokowanie przerwania i wystartowanie licznika
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);	
}

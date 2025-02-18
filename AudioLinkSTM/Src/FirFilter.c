#include "FirFilter.h"

// static float FIR_IMPULSE_RESPONSE[FIR_FILTER_LENGTH] = {}; // FIR filter coefficients;
static float FIR_IMPULSE_RESPONSE_LOW_PASS[] =
{
	0.045066907137521421,
	0.045820575643761983,
	0.046501173159741213,
	0.047106642713001412,
	0.047635151455958924,
	0.048085097286934234,
	0.048455114642117027,
	0.048744079435647437,
	0.048951113128342105,
	0.049075585909023091,
	0.049117118975902395,
	0.049075585909023091,
	0.048951113128342105,
	0.048744079435647437,
	0.048455114642117027,
	0.048085097286934234,
	0.047635151455958924,
	0.047106642713001412,
	0.046501173159741213,
	0.045820575643761983,
	0.045066907137521421
};

static float FIR_IMPULSE_RESPONSE_BAND_PASS_5K[] =
{
	-0.000311341691536864,
	-0.000314904795981970,
	0.000508327471209295,
	0.001886713825960096,
	0.002669278280304561,
	0.001628902830305447,
	-0.001285223879872146,
	-0.004212533294060990,
	-0.003964774370143720,
	0.002107278500657691,
	-0.002546211988778623,
	0.042125896162662775,
	0.075005720377210566,
	0.063022777008648959,
	0.001168622691895534,
	-0.077692228319504730,
	-0.121860302795318648,
	-0.097149587276850802,
	-0.014025157134872221,
	0.076045894155919530,
	0.114385708484295065,
	0.076045894155918003,
	-0.014025157134872086,
	-0.097149587276850857,
	-0.121860302795318842,
	-0.077692228319504536,
	0.001168622691895495,
	0.063022777008649139,
	0.075005720377210594,
	0.042125896162662685,
	-0.002546211988778749,
	0.002107278500657585,
	-0.003964774370143736,
	-0.004212533294060951,
	-0.001285223879872101,
	0.001628902830305452,
	0.002669278280304540,
	0.001886713825960078,
	0.000508327471209286,
	-0.000314904795981983,
	-0.000311341691536884,
};

static float FIR_IMPULSE_RESPONSE_BAND_PASS_10K[] =
{
	0.000748334746194504,
	-0.000000000000000003,
	-0.002350362931001288,
	0.000000000000000005,
	0.004358176149758669,
	-0.000000000000000020,
	-0.006737982136279914,
	0.000000000000000018,
	0.009327852951630281,
	-0.000000000000000015,
	-0.037140910102565948,
	-0.000033058508785831,
	0.076391418046722726,
	-0.000009196971980083,
	-0.104252824980979569,
	0.000082940104585621,
	0.125391365825438861,
	-0.000230135089701787,
	-0.138773826528781424,
	0.000841885548709210,
	0.144772647754071920,
	0.000841885548709205,
	-0.138773826528779842,
	-0.000230135089701790,
	0.125391365825438805,
	0.000082940104585621,
	-0.104252824980979375,
	-0.000009196971980086,
	0.076391418046722739,
	-0.000033058508785830,
	-0.037140910102566094,
	-0.000000000000000014,
	0.009327852951630248,
	0.000000000000000014,
	-0.006737982136279903,
	-0.000000000000000016,
	0.004358176149758612,
	0.000000000000000008,
	-0.002350362931001275,
	-0.000000000000000003,
	0.000748334746194465,
};

void FirFilter_Init(FirFilter *fir, uint8_t type)
{
    fir -> type=type;

    /*Limpiar el buffer del filtro*/
	for (uint8_t n = 0; n < ((fir -> type == LOW_PASS) ? LOW_PASS_LENGTH : BAND_PASS_LENGHT); n++) {
        fir->buf[n] = 0;
    }

    /* Reseteamos elindice del buffer */
    fir -> bufIndex = 0;

    /* Limpiamos la salida del filtro */
    fir -> out = 0.0f;
}
/* 
    *fir => puntero a la estructura "fir"
    inp => ultima entrada al filtro 
*/
float FirFilter_Update(FirFilter *fir, float inp)
{
    /* Se guarda laultima muestra en el buffer */
    fir->buf[fir->bufIndex] = inp;

    /* Incremento el indice del buffer */
    fir->bufIndex++;

    /* Se compruba que el tamaño es menor que el tamaño maximo */
    if(fir->bufIndex == ((fir -> type == LOW_PASS) ? LOW_PASS_LENGTH : BAND_PASS_LENGHT))
    {
        fir->bufIndex = 0;
    }

    /* A partir de aqui se computa la siguiente salida del filtro realizando la convolucion */
    /* Se empieza con la salida a cero */
    fir-> out = 0.0f;

    /* Definimos un indice para la suma de la convolucion */
    uint8_t sumIndex = fir->bufIndex;

    /* Creamos un bucle que englobe a toda la longitud del filtro (numero de coeficientes) */
    for(uint8_t n = 0; n < ((fir -> type == LOW_PASS) ? LOW_PASS_LENGTH : BAND_PASS_LENGHT); n++)
    {
        /* Comprobamos que el indice sea mayor que cero y en ese caso decrementamos */
        if(sumIndex > 0) {
            sumIndex--;
        } else {
            sumIndex = ((fir -> type == LOW_PASS) ? LOW_PASS_LENGTH : BAND_PASS_LENGHT) - 1;
        }

        /* Multiplicamos la respuesta al impulso con la muestra y lo añadimos a la salida */
        if(fir -> type == LOW_PASS){
        	fir->out += FIR_IMPULSE_RESPONSE_LOW_PASS[n] * fir->buf[sumIndex];

        }else if(fir -> type == BAND_PASS_5K){
            fir->out += FIR_IMPULSE_RESPONSE_BAND_PASS_5K[n] * fir->buf[sumIndex];

        }else{
            fir->out += FIR_IMPULSE_RESPONSE_BAND_PASS_10K[n] * fir->buf[sumIndex];

        }
    }

    return fir->out;
}

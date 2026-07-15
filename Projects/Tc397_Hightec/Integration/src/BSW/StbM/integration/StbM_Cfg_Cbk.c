/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/

#include "StbM.h"

//#include "Gpt.h"

/*
**********************************************************************************************************************
* Variables
**********************************************************************************************************************
*/

#if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )

void GPT_Get_ElapsedValue_Cbk (const TickType * Lastupdation_of_timebase,
                                              TickType * l_Deltatick_uo)
{
    uint64 value_now,value_in;
    value_now = Gpt_Read_Timer64_Ticks();
    value_in = *Lastupdation_of_timebase;
    /*MR12 RULE 10.3 VIOLATION: l_Deltatick_uo is of type TickType. Hence the conversion is necessary in this case*/
    *l_Deltatick_uo = value_now - value_in;
}

void GPT_Get_CounterValue_Cbk (TickType * l_tick)
{
    /*MR12 RULE 10.3 VIOLATION: l_tick is of type TickType. Hence the conversion is necessary in this case*/
    *l_tick=  Gpt_Read_Timer64_Ticks();
}

uint32 GPT_Tick_To_Ns_Cbk (TickType ticks)
{
    uint32 lNanoseconds_u32;
    /*MR12 RULE 10.4,10.8 VIOLATION: ticks to nanoseconds conversion is done depending on the frequency*/
    lNanoseconds_u32=(uint32)(ticks*12.5);

    return lNanoseconds_u32;
}

#endif/* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/

/*
**********************************************************************************************************************
* End of the file
**********************************************************************************************************************
*/

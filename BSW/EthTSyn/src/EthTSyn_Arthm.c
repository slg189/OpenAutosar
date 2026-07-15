

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED
#include "EthTSyn_Prv.h"

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

/**
 ***************************************************************************************************
 * \module description
 *  Adds two timestamps
 *
 * Parameter In:
 * \Param Time1
 * \Param Time2
 *
 * Parameter Out:
 * \Param TimeAdd
 *
 ***************************************************************************************************
 */
void EthTSyn_AddTi( const Eth_TimeIntDiffType *Time1,
                    const Eth_TimeIntDiffType *Time2,
                          Eth_TimeIntDiffType *TimeAdd )
{
    /* local variable declaration */
    uint64 lTime1;
    uint64 lTime2;
    uint64 lTimeAdd;

    /* local variable initialisation */
    lTime1   =   (uint64)( ( ( ( uint64 )( Time1->diff.secondsHi ) ) << ETHTSYN_THIRTYTWO ) | ( Time1->diff.seconds ) );
    lTime2   =   (uint64)( ( ( ( uint64 )( Time2->diff.secondsHi ) ) << ETHTSYN_THIRTYTWO ) | ( Time2->diff.seconds ) );
    lTimeAdd =   ETHTSYN_ZERO;

    /* Check if Ns part is within the limit and sHi is zero */
    if( Time1->sign == Time2->sign )
    {
        TimeAdd->diff.nanoseconds = Time1->diff.nanoseconds + Time2->diff.nanoseconds;
        lTimeAdd                  = lTime1+lTime2;
        if( ETHTSYN_NS_UPPRLIM < TimeAdd->diff.nanoseconds )
        {
            lTimeAdd                  += ETHTSYN_ONE;
            TimeAdd->diff.nanoseconds -= ETHTSYN_NANOSEC;
        }
        TimeAdd->sign = Time1->sign;
    }
    else
    {
        if( lTime1 == lTime2 )
        {
            lTimeAdd = ETHTSYN_ZERO;
            if( Time1->diff.nanoseconds >= Time2->diff.nanoseconds )
            {
                TimeAdd->diff.nanoseconds = Time1->diff.nanoseconds - Time2->diff.nanoseconds;
                TimeAdd->sign             = Time1->sign;
            }
            else
            {
                TimeAdd->diff.nanoseconds = Time2->diff.nanoseconds - Time1->diff.nanoseconds;
                TimeAdd->sign             = Time2->sign;
            }
        }
        else if( lTime1 > lTime2 )
        {
            if( Time1->diff.nanoseconds >= Time2->diff.nanoseconds )
            {
                TimeAdd->diff.nanoseconds = Time1->diff.nanoseconds - Time2->diff.nanoseconds;
                lTimeAdd                  = lTime1 - lTime2;
                TimeAdd->sign             = Time1->sign;
            }
            else
            {
                TimeAdd->diff.nanoseconds = ( Time1->diff.nanoseconds + ETHTSYN_NANOSEC ) - Time2->diff.nanoseconds;
                lTimeAdd                  = ( lTime1 - ETHTSYN_ONE ) - lTime2;
                TimeAdd->sign             = Time1->sign;
            }
        }
        else
        {
            if( Time1->diff.nanoseconds > Time2->diff.nanoseconds )
            {
                TimeAdd->diff.nanoseconds = ( Time2->diff.nanoseconds + ETHTSYN_NANOSEC ) - Time1->diff.nanoseconds;
                lTimeAdd                  = ( lTime2 - ETHTSYN_ONE ) - lTime1;
                TimeAdd->sign             = Time2->sign;
            }
            else
            {
                TimeAdd->diff.nanoseconds = Time2->diff.nanoseconds - Time1->diff.nanoseconds;
                lTimeAdd                  = lTime2 - lTime1;
                TimeAdd->sign             = Time2->sign;
            }
        }
    }
    /* Initialising seconds and secondsHi of TimeAdd from local variable */
    TimeAdd->diff.secondsHi =   ( uint16 )( ( ( lTimeAdd ) & ( ETHTSYN_HIGHER_DOUBLE_WORD_MASK ) ) >> ETHTSYN_THIRTYTWO );
    TimeAdd->diff.seconds   =   ( uint32 )( ( lTimeAdd ) & ( ETHTSYN_LOWER_DOUBLE_WORD_MASK ) );
}

/**
 ***************************************************************************************************
 * \module description
 *  Subtracts two timestamps
 *
 * Parameter In:
 * \Param Time1
 * \Param Time2
 *
 * Parameter Out:
 * \Param  TimeDiff
 *
 ***************************************************************************************************
 */
void EthTSyn_SubTi( const Eth_TimeStampType   *Time1,
                    const Eth_TimeStampType   *Time2,
                          Eth_TimeIntDiffType *TimeDiff )
{
    /* local variable declaration */
    uint64 lTime1;
    uint64 lTime2;
    uint64 lTimeDiff;

    /* local variable initialisation */
    lTime1    =   (uint64)( ( ( ( uint64 )( Time1->secondsHi ) ) << ETHTSYN_THIRTYTWO ) | ( Time1->seconds ) );
    lTime2    =   (uint64)( ( ( ( uint64 )( Time2->secondsHi ) ) << ETHTSYN_THIRTYTWO ) | ( Time2->seconds ) );
    lTimeDiff =   ETHTSYN_ZERO;

    if( lTime1 == lTime2 )
    {
        lTimeDiff = ETHTSYN_ZERO;
        if( Time1->nanoseconds >= Time2->nanoseconds )
        {
            TimeDiff->diff.nanoseconds  = Time1->nanoseconds - Time2->nanoseconds;
            TimeDiff->sign              = TRUE;
        }
        else
        {
            TimeDiff->diff.nanoseconds  = Time2->nanoseconds - Time1->nanoseconds;
            TimeDiff->sign              = FALSE;
        }
    }
    else if( lTime1 > lTime2 )
    {
        if( Time1->nanoseconds >= Time2->nanoseconds )
        {
            TimeDiff->diff.nanoseconds  = Time1->nanoseconds - Time2->nanoseconds;
            lTimeDiff                   = lTime1 - lTime2;
            TimeDiff->sign              = TRUE;
        }
        else
        {
            TimeDiff->diff.nanoseconds = ( Time1->nanoseconds + ETHTSYN_NANOSEC ) - Time2->nanoseconds;
            lTimeDiff                  = ( lTime1 - ETHTSYN_ONE ) - lTime2;
            TimeDiff->sign             = TRUE;
        }
    }
    else
    {
        if( Time1->nanoseconds > Time2->nanoseconds )
        {
            TimeDiff->diff.nanoseconds  = ( Time2->nanoseconds + ETHTSYN_NANOSEC ) - Time1->nanoseconds;
            lTimeDiff                   = ( lTime2 - ETHTSYN_ONE ) - lTime1;
            TimeDiff->sign              = FALSE;
        }
        else
        {
            TimeDiff->diff.nanoseconds  = Time2->nanoseconds - Time1->nanoseconds;
            lTimeDiff                   = lTime2 - lTime1;
            TimeDiff->sign              = FALSE;
        }
    }
    /* Initialising seconds and secondsHi of TimeDiff from local variable */
    TimeDiff->diff.secondsHi =   ( uint16 )( ( ( lTimeDiff ) & ( ETHTSYN_HIGHER_DOUBLE_WORD_MASK ) ) >> ETHTSYN_THIRTYTWO );
    TimeDiff->diff.seconds   =   ( uint32 )( ( lTimeDiff ) & ( ETHTSYN_LOWER_DOUBLE_WORD_MASK ) );

}

/**
 ***************************************************************************************************
 * \module description
 *  Averages two timestamps
 *
 * Parameter In:
 * \Param Time1
 * \Param Time2
 *
 * Parameter Out:
 * \Param  TimeAvg
 *
 ***************************************************************************************************
 */
void EthTSyn_AvgTi( const Eth_TimeIntDiffType *Time1,
                    const Eth_TimeIntDiffType *Time2,
                          Eth_TimeIntDiffType *TimeAvg )
{
    /* local variable declaration */
    uint64 lTimeAvg;

    /* local variable initialisation */
    lTimeAvg =   ETHTSYN_ZERO;

    /* Add the two timestamps */
    EthTSyn_AddTi( Time1, Time2, TimeAvg );

    lTimeAvg   =   (uint64)( ( ( ( uint64 )( TimeAvg->diff.secondsHi ) ) << ETHTSYN_THIRTYTWO ) | ( TimeAvg->diff.seconds ) );

    if( ( TimeAvg->diff.seconds & ETHTSYN_ONE ) == ETHTSYN_ONE )
    {
        lTimeAvg                  -= ETHTSYN_ONE;
        TimeAvg->diff.nanoseconds += ETHTSYN_NANOSEC;
    }
    /* Find the average */
    lTimeAvg                  = lTimeAvg >> 1;
    TimeAvg->diff.nanoseconds = TimeAvg->diff.nanoseconds >> 1;

    /* Initialising seconds and secondsHi of TimeAvg from local variable */
    TimeAvg->diff.secondsHi =   ( uint16 )( ( ( lTimeAvg ) & ( ETHTSYN_HIGHER_DOUBLE_WORD_MASK ) ) >> ETHTSYN_THIRTYTWO );
    TimeAvg->diff.seconds   =   ( uint32 )( ( lTimeAvg ) & ( ETHTSYN_LOWER_DOUBLE_WORD_MASK ) );
}

/**
 ***************************************************************************************************
 * \module description
 *  Converts bit field to time
 *
 * Parameter In:
 * \Param Value_u64 : Value that has to be converted to Eth_TimeIntDiffType
 *                    Resolution should be nanoseconds or higher
 * \Param Factor_u8 : Factor through which the value has to be right shifted
 * Parameter Out:
 * \Param  Time_pst : Pointer to the final Eth_TimeIntDiffType type time value
 *
 ***************************************************************************************************
 */
void EthTSyn_CnvToEthTi( uint64               Value_u64,
                         uint8                Factor_u8,
                         Eth_TimeIntDiffType *Time_pst  )
{
    /* Local variable declaration */
    uint64 lTime_u64;

    /* Convert from Bitfield to Time */
    lTime_u64 = Value_u64 >> Factor_u8;

    /* Obtain nanoseconds part of time */
    Time_pst->diff.nanoseconds = (uint32)( lTime_u64 % ETHTSYN_NANOSEC );

    /* Obtain Seconds part of time */
    lTime_u64 = lTime_u64 / ETHTSYN_NANOSEC;

    /* Initialising seconds and secondsHi of Time */
    Time_pst->diff.secondsHi = ( uint16 )( ( ( lTime_u64 ) & ( ETHTSYN_HIGHER_DOUBLE_WORD_MASK) ) >> ETHTSYN_THIRTYTWO );
    Time_pst->diff.seconds   = ( uint32 )(   ( lTime_u64 ) & ( ETHTSYN_LOWER_DOUBLE_WORD_MASK ) );

    /* Initialise the sign */
    Time_pst->sign = TRUE;

}



/**
 ***************************************************************************************************
 * \module description
 *  Converts bit field to time
 *
 * Parameter In:
 * \Param Time_st       : Eth_TimeIntDiffType time structure that has to be converted to hex value of resolution nanoseconds or higher
 * \Param Factor_u8     : Factor through which the hex value has to be left shifted
 *
 * Parameter Out:
 * \Param Value_pu64    : Pointer to the hex value
 *
 ***************************************************************************************************
 */
void EthTSyn_CnvEthTitoHex( Eth_TimeIntDiffType  Time_st,
                            uint8                Factor_u8,
                            uint64              *Value_pu64 )
{
    /* Local variable declaration */
    uint64 lTime_u64;

    /* Convert to nanoseconds */

    /* Or SecondsHi and Seconds */
    lTime_u64 =  ( ( (uint64)( Time_st.diff.secondsHi ) ) << ETHTSYN_THIRTYTWO ) | (uint64)( Time_st.diff.seconds );

    /* Multiply Seconds with 10^9 to obtain number of nanoseconds */
    lTime_u64 = lTime_u64 * ETHTSYN_NANOSEC;

    /* Add nanoseconds value of the time structure with converted nanoseconds value of the seconds */
    lTime_u64 = (uint64)( lTime_u64 + Time_st.diff.nanoseconds );

    /* Convert nanoseconds to Bitfield by left shifting with the factor */
    *Value_pu64 = lTime_u64 << Factor_u8;
}

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

#endif /* ETHTSYN_CONFIGURED */

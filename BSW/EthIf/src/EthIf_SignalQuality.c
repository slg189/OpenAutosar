

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"

#if ( ETHIF_ETHIFTRCV_EXIST == STD_ON )
#include "EthTrcv.h"
#endif

#if ( ETHIF_ETHIFSWITCH_EXIST == STD_ON )
#include "EthSwt.h"
#endif

#include "EthIf_Prv.h"

/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"

#if( ETHIF_ENABLE_SIGNALQUALITY == STD_ON )
/* Signal quality check counter */
/* This is a global static variable because it is not desirable to allow direct access to it outside the file */
static uint32 EthIf_SignalQualityCheckCntr_u32 = 0U;
#endif

#define ETHIF_STOP_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \module description
 * Function called from EthIf_MainFunctionState to poll signal quality for Trcv/switch port.
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#if( ETHIF_ENABLE_SIGNALQUALITY == STD_ON )
void EthIf_Prv_SignalQualityProcessing( void )
{
    /* Declare local variables */
    Std_ReturnType                          lStdRetVal_en;
    uint32                                  lSignalQuality_u32;
    EthIf_SignalQualityMeasurement_tst      *lSignalQualityMeasurement_pst;
    uint8                                   lLoopIdx_u8;
#if ( ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON )
    uint8                                   lRefSwitchTableIdx_u8;
#endif

    /* Initialize local variables */
    lSignalQuality_u32  = 0U;

    /* Increment the signal quality counter and check if it is expired */
    EthIf_SignalQualityCheckCntr_u32++;

    if(EthIf_SignalQualityCheckCntr_u32 == ETHIF_SIGNALQUALITY_CHECKPERIOD)
    {

#if( ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON )
        /* Loop through all transceivers configured */
        for(lLoopIdx_u8 = 0; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfTransceiver_cu8; lLoopIdx_u8++)
        {
            /* Initiaize lStdRetVal_en for every iteration */
            lStdRetVal_en = E_NOT_OK;

            /* Calculate the signal quality only if link state is ETHIF_LINKSTATE_ACTIVE */
            if( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].TrcvLinkState_en == ETHIF_LINKSTATE_ACTIVE )
            {
                /* Call Trcv API to poll signal quality */
                lStdRetVal_en = EthTrcv_GetPhySignalQuality( EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].EthTrcvIdx_cu8,
                                                             &lSignalQuality_u32 );
            }

            /* If the signal quality is successfully polled from the Trcv */
            if( lStdRetVal_en == E_OK )
            {
                lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicTrcvTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st);

                /* Enter critical section: A critical section is required here as EthIf_GetTrcvSignalQuality()/EthIf_ClearTrcvSignalQuality() can interrupt this function, */
                /* and leads to inconsistent data. */
                 SchM_Enter_EthIf_TrcvSignalQuality();

                /* Set a flag to indicate stored signal is valid and update the current signal as ActualSignalQuality */
                lSignalQualityMeasurement_pst->SignalQualityValid_b = TRUE;
                lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality = lSignalQuality_u32;

                /* If the current signal quality is higher than the highest stored signal quality, update HighestSignalQuality */
                if( lSignalQuality_u32 > lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality )
                {
                    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality = lSignalQuality_u32;
                }

                /* If the current signal quality is lower than the lowest stored signal quality, update LowestSignalQuality */
                if ( lSignalQuality_u32 < lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality )
                {
                    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality = lSignalQuality_u32;
                }

                /* Exit critical section */
                SchM_Exit_EthIf_TrcvSignalQuality();
            }
        }
#endif

#if( ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON )
        /* Loop through all configured EthIf switch ports */
        for(lLoopIdx_u8 = 0; lLoopIdx_u8 < EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPorts_cu8; lLoopIdx_u8++)
        {
            /* Initiaize lStdRetVal_en for every iteration */
            lStdRetVal_en = E_NOT_OK;

            /* Calculate the signal quality only if link state is ETHIF_LINKSTATE_ACTIVE */
            if( EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].PortLinkState_en == ETHIF_LINKSTATE_ACTIVE )
            {
                /* Get the index of static switch table for the switch port */
                lRefSwitchTableIdx_u8 = EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].RefSwitchTableIdx_cu8;

                /* Call switch API to poll signal quality */
                lStdRetVal_en = EthSwt_GetPortSignalQuality( EthIf_CfgPtr_pco->EthIf_StaticSwitchTable_cpcst[lRefSwitchTableIdx_u8].EthSwtIdx_cu8,
                                                             EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].EthSwtPortIdx_cu8,
                                                             &lSignalQuality_u32 );
            }

            /* If the signal quality is successfully polled from the EthSwt */
            if( lStdRetVal_en == E_OK )
            {
                lSignalQualityMeasurement_pst = &(EthIf_CfgPtr_pco->EthIf_DynamicSwtPortTable_cpst[lLoopIdx_u8].SignalQualityMeasurement_st);

                /* Enter critical section: A critical section is required here as EthIf_GetSwitchPortSignalQuality()/EthIf_ClearSwitchPortSignalQuality() can interrupt this function, */
                /* and leads to inconsistent data. */
                SchM_Enter_EthIf_SwtPortSignalQuality();

                /* Set a flag to indicate stored signal is valid and update the current signal as ActualSignalQuality */
                lSignalQualityMeasurement_pst->SignalQualityValid_b = TRUE;
                lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.ActualSignalQuality = lSignalQuality_u32;

                /* If the current signal quality is higher than the highest stored signal quality, update HighestSignalQuality */
                if( lSignalQuality_u32 > lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality )
                {
                    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.HighestSignalQuality = lSignalQuality_u32;
                }

                /* If the current signal quality is lower than the lowest stored signal quality, update LowestSignalQuality */
                if ( lSignalQuality_u32 < lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality )
                {
                    lSignalQualityMeasurement_pst->EthIfSignalQualityResultType_st.LowestSignalQuality = lSignalQuality_u32;
                }

                /* Exit critical section */
                SchM_Exit_EthIf_SwtPortSignalQuality();
            }
        }
#endif
        /* Reset the signal quality counter */
        EthIf_SignalQualityCheckCntr_u32 = 0U;
    }
}
#endif


#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"


#endif /* ETHIF_CONFIGURED */


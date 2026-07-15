

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED
# if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )

#if (ETHTSYN_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /* (ETHTSYN_DEV_ERROR_DETECT == STD_ON) */

#include "EthTSyn_Prv.h"

/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

#  if (ETHTSYN_PDELAY_INIT_SM_ENABLED == STD_ON)
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposePdelayReqMsg()
 *
 * \Function Description
 * Composes Pdelay_Req message
 *
 * Parameters (in) :
 * \param BufPtr_pu8  - Buffer pointer
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ComposePdelayReqMsg( uint8 *BufPtr_pu8 )
{
    /* Initialise the reserved field of Pdelay request message to Zero */
    *( BufPtr_pu8 + 34 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 35 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 36 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 37 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 38 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 39 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 40 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 41 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 42 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 43 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 44 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 45 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 46 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 47 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 48 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 49 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 50 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 51 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 52 ) = ETHTSYN_ZERO;
    *( BufPtr_pu8 + 53 ) = ETHTSYN_ZERO;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReceivePdelayResp()
 *
 * \Function Description
 * Handles received Pdelay_Resp message
 *
 * Parameters (in) :
 * \param DataPtr_pu8    - Pointer to the received data
 * \param TimeStamp_pst  - Pointer to current time stamp
 * \param MsgLen_u16     - Length of Pdely Response message
 * \param LenByte_u16    - Length of received data
 * \param IdxDomain_u8   - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ReceivePdelayResp( const uint8             *DataPtr_pu8,
                                const Eth_TimeStampType *TimeStamp_pst,
                                      uint16             MsgLen_u16,
                                      uint16             LenByte_u16,
                                      uint8              IdxDomain_u8 )
{
    /* Local Variable declaration */
    EthTSyn_SrcPortId_tst *lsntSrcPortId_pst;
    EthTSyn_SrcPortId_tst  lrcvdReqPortId_st;
    uint32                 lPdelayResp_RespFolwUpTout_u32;
    uint16                 lrcvdSeqId_u16;
    uint16                 lsntSeqId_u16;
    uint8                  lCnt_u8;
    boolean                lClkIdMatch_b;

    /* Local Variable Initialization */
    lClkIdMatch_b = TRUE;

    /* The Time Domain matches to one of the configured Time Domains */
    if( ( ETHTSYN_ZERO           == IdxDomain_u8 )
    &&  ( ETHTSYN_ZERO           == DataPtr_pu8[4] )
    &&  ( ETHTSYN_PDELAY_MSG_LEN == MsgLen_u16 )
    &&  ( LenByte_u16            >= MsgLen_u16 ) )
    {
        lsntSrcPortId_pst               = &( EthTSyn_RamDataTyp_ast[0].VarHeader_st.SourcePortIdentity_st );
        lPdelayResp_RespFolwUpTout_u32  = EthTSyn_TimeDomainCfg_pcst[0].PortCfg_pcst->PdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16;
        lsntSeqId_u16                   = EthTSyn_RamDataTyp_ast[0].VarHeader_st.SequenceId_au16[ETHTSYN_PDREQ_HRD_IDX];

        /* Update the local datastructures with the received Ingress Timestamp */
        EthTSyn_PdelayInitiator_st.PdelayRespIngressTimeStamp_st = *TimeStamp_pst;

        /* Extract Sequence Id */
        lrcvdSeqId_u16                         = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[30] ) << 8U  ) | ( DataPtr_pu8[31] ) );
        lrcvdReqPortId_st.ClockIdentity_au8[0] = DataPtr_pu8[44];
        lrcvdReqPortId_st.ClockIdentity_au8[1] = DataPtr_pu8[45];
        lrcvdReqPortId_st.ClockIdentity_au8[2] = DataPtr_pu8[46];
        lrcvdReqPortId_st.ClockIdentity_au8[3] = DataPtr_pu8[47];
        lrcvdReqPortId_st.ClockIdentity_au8[4] = DataPtr_pu8[48];
        lrcvdReqPortId_st.ClockIdentity_au8[5] = DataPtr_pu8[49];
        lrcvdReqPortId_st.ClockIdentity_au8[6] = DataPtr_pu8[50];
        lrcvdReqPortId_st.ClockIdentity_au8[7] = DataPtr_pu8[51];
        lrcvdReqPortId_st.PortNumber_u16       = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[52] ) << 8U  ) | ( DataPtr_pu8[53] ) );

        for( lCnt_u8 = 0U; lCnt_u8 < ETHTSYN_CLK_ID_LEN; lCnt_u8++ )
        {
            if( lrcvdReqPortId_st.ClockIdentity_au8[lCnt_u8] != lsntSrcPortId_pst->ClockIdentity_au8[lCnt_u8] )
            {
                lClkIdMatch_b = FALSE;
            }
        }

        if( ( ETHTSYN_TX_PDELAY_REQ_SENT_E == EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en )
        &&  ( lrcvdSeqId_u16 == lsntSeqId_u16 )
        &&  ( TRUE == lClkIdMatch_b ) )
        {
            /* TRACE [SWS_EthTSyn_00164]: If Pdelay timeout is being observed, initialise the counter with maximum EthTSynPdelayRespAndRespFollowUpTimeout */
            if ( ETHTSYN_ZERO != lPdelayResp_RespFolwUpTout_u32 )
            {
                EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = lPdelayResp_RespFolwUpTout_u32;
            }
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                = ETHTSYN_RX_PDELAY_RESP_E;
            EthTSyn_PdelayInitiator_st.RxReceiptTimeStamp_st.secondsHi    = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[34] ) << 8U  ) | DataPtr_pu8[35] );
            EthTSyn_PdelayInitiator_st.RxReceiptTimeStamp_st.seconds      = (uint32)( ( ( uint32 )DataPtr_pu8[36] << 24U ) | ( ( uint32 )DataPtr_pu8[37] << 16U ) | ( ( uint32 )DataPtr_pu8[38] << 8U ) | DataPtr_pu8[39] );
            EthTSyn_PdelayInitiator_st.RxReceiptTimeStamp_st.nanoseconds  = (uint32)( ( ( uint32 )DataPtr_pu8[40] << 24U ) | ( ( uint32 )DataPtr_pu8[41] << 16U ) | ( ( uint32 )DataPtr_pu8[42] << 8U ) | DataPtr_pu8[43] );
        }
    }
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReceivePdelayRespFollowUp()
 *
 * \Function Description
 * Handles received Pdelay_Resp_Follow_Up message
 *
 * Parameters (in) :
 * \param DataPtr_pu8    - Pointer to the received data
 * \param MsgLen_u16     - Length of Pdely Response Follow-Up message
 * \param LenByte_u16    - Length of received data
 * \param IdxDomain_u8   - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ReceivePdelayRespFollowUp( const uint8  *DataPtr_pu8,
                                              uint16  MsgLen_u16,
                                              uint16  LenByte_u16,
                                              uint8   IdxDomain_u8 )
{
    /* Local Variable declaration */
    EthTSyn_SrcPortId_tst *lsntSrcPortId_pst;
    EthTSyn_SrcPortId_tst  lrcvdReqPortId_tst;
    uint16                 lrcvdSeqId_u16;
    uint16                 lsntSeqId_u16;
    uint8                  lCnt_u8;
    boolean                lClkIdMatch_b;

    /* Local Variable Initialization */
    lClkIdMatch_b = TRUE;

    /* The Time Domain matches to one of the configured Time Domains */
    if( ( ETHTSYN_ZERO           == IdxDomain_u8 )
    &&  ( ETHTSYN_ZERO           == DataPtr_pu8[4] )
    &&  ( ETHTSYN_PDELAY_MSG_LEN == MsgLen_u16 )
    &&  ( LenByte_u16            >= MsgLen_u16 ) )
    {
        lsntSrcPortId_pst = &( EthTSyn_RamDataTyp_ast[0].VarHeader_st.SourcePortIdentity_st );
        lsntSeqId_u16     = EthTSyn_RamDataTyp_ast[0].VarHeader_st.SequenceId_au16[ETHTSYN_PDREQ_HRD_IDX];

        lrcvdSeqId_u16                          = ( ( uint16 )( ( uint16 )DataPtr_pu8[30] << 8U  ) | DataPtr_pu8[31] );
        lrcvdReqPortId_tst.ClockIdentity_au8[0] = DataPtr_pu8[44];
        lrcvdReqPortId_tst.ClockIdentity_au8[1] = DataPtr_pu8[45];
        lrcvdReqPortId_tst.ClockIdentity_au8[2] = DataPtr_pu8[46];
        lrcvdReqPortId_tst.ClockIdentity_au8[3] = DataPtr_pu8[47];
        lrcvdReqPortId_tst.ClockIdentity_au8[4] = DataPtr_pu8[48];
        lrcvdReqPortId_tst.ClockIdentity_au8[5] = DataPtr_pu8[49];
        lrcvdReqPortId_tst.ClockIdentity_au8[6] = DataPtr_pu8[50];
        lrcvdReqPortId_tst.ClockIdentity_au8[7] = DataPtr_pu8[51];
        lrcvdReqPortId_tst.PortNumber_u16       = ( ( uint16 )( ( uint16 )DataPtr_pu8[52] << 8U  ) | DataPtr_pu8[53] );

        for( lCnt_u8 = 0U; lCnt_u8 < ETHTSYN_CLK_ID_LEN; lCnt_u8++ )
        {
            if( lrcvdReqPortId_tst.ClockIdentity_au8[lCnt_u8] != lsntSrcPortId_pst->ClockIdentity_au8[lCnt_u8] )
            {
                lClkIdMatch_b = FALSE;
            }
        }

        if( ( ETHTSYN_RX_PDELAY_RESP_E == EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en )
        &&  ( lrcvdSeqId_u16 == lsntSeqId_u16 )
        &&  ( lrcvdReqPortId_tst.PortNumber_u16 == EthTSyn_RamDataTyp_ast[0].VarHeader_st.SourcePortIdentity_st.PortNumber_u16 )
        &&  ( TRUE == lClkIdMatch_b ) )
        {
            if( ETHTSYN_ZERO != EthTSyn_TimeDomainCfg_pcst[0].PortCfg_pcst->PdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16 )
            {
                EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = ETHTSYN_ZERO;
            }
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                       = ETHTSYN_RX_PDELAY_RESP_FOLLOW_UP_E;
            EthTSyn_PdelayInitiator_st.RxResponseOriginTimeStamp_st.secondsHi    = (uint16)( ( uint16 )( ( uint16 )DataPtr_pu8[34] << 8U  ) | DataPtr_pu8[35] );
            EthTSyn_PdelayInitiator_st.RxResponseOriginTimeStamp_st.seconds      = (uint32)( ( ( uint32 )DataPtr_pu8[36] << 24U ) | ( ( uint32 )DataPtr_pu8[37] << 16U ) | ( ( uint32 )DataPtr_pu8[38] << 8U ) | DataPtr_pu8[39] );
            EthTSyn_PdelayInitiator_st.RxResponseOriginTimeStamp_st.nanoseconds  = (uint32)( ( ( uint32 )DataPtr_pu8[40] << 24U ) | ( ( uint32 )DataPtr_pu8[41] << 16U ) | ( ( uint32 )DataPtr_pu8[42] << 8U ) | DataPtr_pu8[43] );
        }
    }
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_CalPdelay()
 *
 * \Function Description
 * Calculates pdelay and updates the global variable
 *
 * Parameters (in) :
 * None
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
static void EthTSyn_CalPdelay( void )
{
    /* Local Variable declaration */
    EthTSyn_PdelayInitiator_tst *lPdelayInitiator_pst;
    Eth_TimeIntDiffType          lslaveToMstDelay_st;
    Eth_TimeIntDiffType          lmstToSlaveDelay_st;
    Eth_TimeIntDiffType          lEthTSyn_Pdelay_st;
    uint32                       lPdelayThd;
    uint32                       lPdelayThd_Nanoseconds;
    uint32                       lPdelayThd_Seconds;

    /* Local variable Initialization */
    lPdelayInitiator_pst = &EthTSyn_PdelayInitiator_st;
    lPdelayThd           = EthTSyn_TimeDomainCfg_pcst[0].PortCfg_pcst->PdelayCfg_pcst->PdelayLtcyThd_u32;

    /* Calculate Master to  Slave Delay */
    EthTSyn_SubTi( &lPdelayInitiator_pst->PdelayRespIngressTimeStamp_st,  &lPdelayInitiator_pst->RxResponseOriginTimeStamp_st, &lmstToSlaveDelay_st );

    /* Calculate Slave to Master Delay */
    EthTSyn_SubTi( &lPdelayInitiator_pst->RxReceiptTimeStamp_st,  &lPdelayInitiator_pst->PdelayReqEgressTimeStamp_st, &lslaveToMstDelay_st );

    /* Take an average of the delays and Update the global variable Pdelay */
    EthTSyn_AvgTi( &lmstToSlaveDelay_st, &lslaveToMstDelay_st, &lEthTSyn_Pdelay_st );

    /* Obtain the seconds part of the configured threshold value */
    lPdelayThd_Seconds = lPdelayThd / ETHTSYN_NANOSEC;

    /* Obtain the nanoseconds part of the configured threshold value */
    lPdelayThd_Nanoseconds = lPdelayThd % ETHTSYN_NANOSEC;

    /* Compare the measured Pdelay value and configured threshold value*/
    if ( ( lPdelayThd_Seconds >  lEthTSyn_Pdelay_st.diff.seconds )
    || ( ( lPdelayThd_Seconds == lEthTSyn_Pdelay_st.diff.seconds ) && ( lPdelayThd_Nanoseconds > lEthTSyn_Pdelay_st.diff.nanoseconds ) ) )
    {
        EthTSyn_Pdelay_ast[ETHTSYN_ZERO] = lEthTSyn_Pdelay_st;
    }
    else
    {
        lEthTSyn_Pdelay_st = EthTSyn_Pdelay_ast[ETHTSYN_ZERO];
    }
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_RunPdelayInitiatorSM()
 *
 * \Function Description
 * State transitions of EthTSyn_Pdelay_ReqStMach_en SM
 *
 * Parameters (in) :
 * \param IdxDomain_u8 - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_RunPdelayInitiatorSM( uint8 IdxDomain_u8 )
{
    const EthTSyn_PdelayCfg_tst *lPdelayCfg_pcst;

    /* Local Variable initialization */
    lPdelayCfg_pcst = EthTSyn_TimeDomainCfg_pcst[0].PortCfg_pcst->PdelayCfg_pcst;

    /* TRACE[SWS_EthTSyn_00164]: A value of ZERO deactivates Pdelay measurement */
    /* Pdelay_req is sent only if Tx period for Pdelay_Req is not ZERO */
    if( ( ETHTSYN_ZERO == IdxDomain_u8 ) && ( ETHTSYN_ZERO != lPdelayCfg_pcst->PdelayReqTxPerd_u32 ) )
    {
        if( ETHTSYN_ZERO != EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX] )
        {
            /*Decrement the counter every main cycle*/
            EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX]--;
        }

        /* Check that Pdelay main function counter and debounce counter are zero before transmitting Pdelay request message */
        if( ( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX] )
# if (ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON)
        &&  ( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_DEB_IDX] )
# endif
          )
        {
            /* Periodically transmit Pdelay_Req message */
            EthTSyn_Transmit( ETHTSYN_PDELAY_REQ_TYPE, ETHTSYN_ZERO, ETHTSYN_ZERO );
            /* Set Pdelay Initiator state machine to Pdelay Request Sent and Pdelay main fuction counter to maximum value */
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en                         = ETHTSYN_TX_PDELAY_REQ_E;
            EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDREQ_MAINFUN_IDX] = lPdelayCfg_pcst->PdelayReqTxPerd_u32;

            /* TRACE[SWS_EthTSyn_00164]: If Pdelay timeout is being observed, set the Pdelay counter to EthTSynPdelayRespAndRespFollowUpTimeout */
            if( ETHTSYN_ZERO != lPdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16)
            {
                EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] = lPdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16;
            }
        }
        /*If Pdelay timeout is being observed and PdelayReq has been sent or PdelayResp has been received */
        else if( ( ETHTSYN_ZERO != lPdelayCfg_pcst->PdelayResp_RespFolwUpTout_u16)
              && ( ( ETHTSYN_TX_PDELAY_REQ_SENT_E == EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en )|| ( ETHTSYN_RX_PDELAY_RESP_E == EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en ) ) )
        {
            /*Decrement the counter every main cycle*/
            EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX]--;
            /* If Pdelay timeout occurs before receiving PdelayResp message, reset the Pdelay Initiator state machine to ETHTSYN_TX_PDELAY_REQ_INIT_E*/
            if ( ETHTSYN_ZERO ==  EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_PDRESP_PDRESPFUP_MAINFUN_IDX] )
            {
                EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en = ETHTSYN_TX_PDELAY_REQ_INIT_E;
            }
        }
        /*If PdelayRespFollowUp has been received, calculate Pdelay and RateRatio and set initiator state machine to INIT */
        else if( ETHTSYN_RX_PDELAY_RESP_FOLLOW_UP_E == EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en )
        {
            /* Calculate Pdelay and RateRatio on successful transmission and reception of Pdelay messages */
            EthTSyn_CalPdelay();
            EthTSyn_PdelayInitiator_st.Pdelay_ReqStMach_en = ETHTSYN_TX_PDELAY_REQ_INIT_E;
        }
        else
        {
            /* Do Nothing */
        }
    }
}
#  endif

#  if (ETHTSYN_PDELAY_RESP_SM_ENABLED == STD_ON)
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposePdelayRespMsg()
 *
 * \Function Description
 * Composes Pdelay_Resp message
 *
 * Parameters (in) :
 * \param BufPtr_pu8 - Buffer pointer
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ComposePdelayRespMsg( uint8 *BufPtr_pu8 )
{
    /* Local variable declaration */
    Eth_TimeStampType     *lReceiveReceiptTimeStamp_pst;
    EthTSyn_SrcPortId_tst *lRequestingPortIdentity_pst;

    /* Local variable initialisation */
    lReceiveReceiptTimeStamp_pst = &EthTSyn_PdelayResponder_st.TxPdelayresp_st.ReceiveReceiptTimeStamp_st;
    lRequestingPortIdentity_pst  = &EthTSyn_PdelayResponder_st.TxPdelayresp_st.RequestingPortIdentity_st;

    *( BufPtr_pu8 + 34 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->secondsHi & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 35 ) = (uint8)( lReceiveReceiptTimeStamp_pst->secondsHi & 0x00FFU );
    *( BufPtr_pu8 + 36 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->seconds & 0xFF000000U ) >> 24U );
    *( BufPtr_pu8 + 37 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->seconds & 0x00FF0000U ) >> 16U );
    *( BufPtr_pu8 + 38 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->seconds & 0x0000FF00U ) >> 8U );
    *( BufPtr_pu8 + 39 ) = (uint8)( lReceiveReceiptTimeStamp_pst->seconds & 0x000000FFU );
    *( BufPtr_pu8 + 40 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->nanoseconds & 0xFF000000U ) >> 24U );
    *( BufPtr_pu8 + 41 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->nanoseconds & 0x00FF0000U ) >> 16U );
    *( BufPtr_pu8 + 42 ) = (uint8)( ( lReceiveReceiptTimeStamp_pst->nanoseconds & 0x0000FF00U ) >> 8U );
    *( BufPtr_pu8 + 43 ) = (uint8)( lReceiveReceiptTimeStamp_pst->nanoseconds & 0x000000FFU );
    *( BufPtr_pu8 + 44 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[0] );
    *( BufPtr_pu8 + 45 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[1] );
    *( BufPtr_pu8 + 46 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[2] );
    *( BufPtr_pu8 + 47 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[3] );
    *( BufPtr_pu8 + 48 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[4] );
    *( BufPtr_pu8 + 49 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[5] );
    *( BufPtr_pu8 + 50 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[6] );
    *( BufPtr_pu8 + 51 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[7] );
    *( BufPtr_pu8 + 52 ) = (uint8)( ( lRequestingPortIdentity_pst->PortNumber_u16 & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 53 ) = (uint8)( lRequestingPortIdentity_pst->PortNumber_u16 & 0x00FFU );
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposePdelayRespFollowUpMsg()
 *
 * \Function Description
 * Composes Pdelay_Resp_Follow_Up message
 *
 * Parameters (in) :
 * \param BufPtr_pu8 - Buffer pointer
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ComposePdelayRespFollowUpMsg( uint8 *BufPtr_pu8 )
{
    /* Local variable declaration */
    Eth_TimeStampType     *lResponseOriginTimeStamp_pst;
    EthTSyn_SrcPortId_tst *lRequestingPortIdentity_pst;

    /* Local variable initialisation */
    lResponseOriginTimeStamp_pst = &EthTSyn_PdelayResponder_st.TxPdelayrespfollowup_st.ResponseOriginTimeStamp_st;
    lRequestingPortIdentity_pst  = &EthTSyn_PdelayResponder_st.TxPdelayresp_st.RequestingPortIdentity_st;

    *( BufPtr_pu8 + 34 ) = (uint8)( ( lResponseOriginTimeStamp_pst->secondsHi & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 35 ) = (uint8)( lResponseOriginTimeStamp_pst->secondsHi & 0x00FFU );
    *( BufPtr_pu8 + 36 ) = (uint8)( ( lResponseOriginTimeStamp_pst->seconds & 0xFF000000U ) >> 24U );
    *( BufPtr_pu8 + 37 ) = (uint8)( ( lResponseOriginTimeStamp_pst->seconds & 0x00FF0000U ) >> 16U );
    *( BufPtr_pu8 + 38 ) = (uint8)( ( lResponseOriginTimeStamp_pst->seconds & 0x0000FF00U ) >> 8U );
    *( BufPtr_pu8 + 39 ) = (uint8)( lResponseOriginTimeStamp_pst->seconds & 0x000000FFU );
    *( BufPtr_pu8 + 40 ) = (uint8)( ( lResponseOriginTimeStamp_pst->nanoseconds & 0xFF000000U ) >> 24U );
    *( BufPtr_pu8 + 41 ) = (uint8)( ( lResponseOriginTimeStamp_pst->nanoseconds & 0x00FF0000U ) >> 16U );
    *( BufPtr_pu8 + 42 ) = (uint8)( ( lResponseOriginTimeStamp_pst->nanoseconds & 0x0000FF00U ) >> 8U );
    *( BufPtr_pu8 + 43 ) = (uint8)( lResponseOriginTimeStamp_pst->nanoseconds & 0x000000FFU );
    *( BufPtr_pu8 + 44 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[0] );
    *( BufPtr_pu8 + 45 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[1] );
    *( BufPtr_pu8 + 46 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[2] );
    *( BufPtr_pu8 + 47 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[3] );
    *( BufPtr_pu8 + 48 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[4] );
    *( BufPtr_pu8 + 49 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[5] );
    *( BufPtr_pu8 + 50 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[6] );
    *( BufPtr_pu8 + 51 ) = (uint8)( lRequestingPortIdentity_pst->ClockIdentity_au8[7] );
    *( BufPtr_pu8 + 52 ) = (uint8)( ( lRequestingPortIdentity_pst->PortNumber_u16 & 0xFF00U ) >> 8U );
    *( BufPtr_pu8 + 53 ) = (uint8)( lRequestingPortIdentity_pst->PortNumber_u16 & 0x00FFU );
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReceivePdelayReq()
 *
 * \Function Description
 * Handles received Pdelay_Req message
 *
 * Parameters (in) :
 * \param DataPtr_pu8   - Pointer to the received data
 * \param TimeStamp_pst - Pointer to current time stamp
 * \param MsgLen_u16    - Length of Pdely Request message
 * \param LenByte_u16   - Length of received data
 * \param IdxDomain_u8  - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_ReceivePdelayReq( const uint8             *DataPtr_pu8,
                               const Eth_TimeStampType *TimeStamp_pst,
                                     uint16             MsgLen_u16,
                                     uint16             LenByte_u16,
                                     uint8              IdxDomain_u8 )
{
    /* Local variable declaration */
    EthTSyn_SrcPortId_tst *lRequestingPortIdentity_pst;

    /* Local variable initialisation */
    lRequestingPortIdentity_pst  = &EthTSyn_PdelayResponder_st.TxPdelayresp_st.RequestingPortIdentity_st;

    /* Check for the State machine and update the Pdelay_Resp internal buffers and the Time Domain matches to one of the configured Time Domains */
    if( ( ETHTSYN_ZERO                  == IdxDomain_u8 )
    &&  ( ETHTSYN_ZERO                  == DataPtr_pu8[4] )
    &&  ( ETHTSYN_TX_PDELAY_RESP_INIT_E == EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en )
    &&  ( ETHTSYN_PDELAY_MSG_LEN        == MsgLen_u16 )
    &&  ( LenByte_u16                   >= MsgLen_u16 ) )
    {

        /* Update the local datastructures with the received Ingress Timestamp */
        EthTSyn_PdelayResponder_st.TxPdelayresp_st.ReceiveReceiptTimeStamp_st = *TimeStamp_pst ;
        EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_E;

        /* Update Pdelay_Resp's requesting port Id and SeqId */
        lRequestingPortIdentity_pst->ClockIdentity_au8[0] = DataPtr_pu8[20];
        lRequestingPortIdentity_pst->ClockIdentity_au8[1] = DataPtr_pu8[21];
        lRequestingPortIdentity_pst->ClockIdentity_au8[2] = DataPtr_pu8[22];
        lRequestingPortIdentity_pst->ClockIdentity_au8[3] = DataPtr_pu8[23];
        lRequestingPortIdentity_pst->ClockIdentity_au8[4] = DataPtr_pu8[24];
        lRequestingPortIdentity_pst->ClockIdentity_au8[5] = DataPtr_pu8[25];
        lRequestingPortIdentity_pst->ClockIdentity_au8[6] = DataPtr_pu8[26];
        lRequestingPortIdentity_pst->ClockIdentity_au8[7] = DataPtr_pu8[27];
        lRequestingPortIdentity_pst->PortNumber_u16       = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[28] ) << 8U  ) | ( DataPtr_pu8[29] ) );
        EthTSyn_RamDataTyp_ast[0].VarHeader_st.SequenceId_au16[ETHTSYN_PDRESP_HRD_IDX] = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[30] ) << 8U  ) | ( DataPtr_pu8[31] ) );
     }
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_RunPdelayResponderSM()
 *
 * \Function Description
 * State transitions of EthTSyn_Pdelay_RespStMach_en SM
 *
 * Parameters (in) :
 * \param IdxDomain_u8  - Index of the current configured Global Time Domain
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ***************************************************************************************************
 */
void EthTSyn_RunPdelayResponderSM( uint8 IdxDomain_u8 )
{
    if( ( ETHTSYN_ZERO == IdxDomain_u8 )
# if (ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON)
    /* Check that debounce counter value is zero before transmitting Pdelay Response and Resonse Follow-Up message */
    &&  ( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[0].MainFunTimer_au32[ETHTSYN_DEB_IDX] )
# endif
    )
    {
        /* Function need to be updated to use main function counter instade of PTP timer for cyclic */
        if( ETHTSYN_TX_PDELAY_RESP_E == EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en )
        {
            /* Transmit Pdelay_Resp message */
            EthTSyn_Transmit( ETHTSYN_PDELAY_RESP_TYPE, ETHTSYN_ZERO, ETHTSYN_ZERO );
            EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en =  ETHTSYN_TX_PDELAY_RESP_SENT_E;
        }
        else if( ETHTSYN_TX_PDELAY_RESP_FOLLOW_UP_E == EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en )
        {
            /* Transmit Pdelay_Resp_Follow_Up message */
            EthTSyn_Transmit( ETHTSYN_PDELAY_RESP_FOLLOW_UP_TYPE, ETHTSYN_ZERO, ETHTSYN_ZERO );
            EthTSyn_PdelayResponder_st.Pdelay_RespStMach_en = ETHTSYN_TX_PDELAY_RESP_INIT_E;
        }
        else
        {
            /* Do Nothing */
        }
    }
}
#  endif


#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

# endif
#endif /* ETHTSYN_CONFIGURED */

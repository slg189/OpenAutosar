

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED
# if (ETHTSYN_GLOBAL_TIME_MASTER == STD_ON)

#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
#include "Crc.h"
#  endif

#  if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
#include "Det.h"
#  endif /* (ETHTSYN_DEV_ERROR_DETECT == STD_ON) */

#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
#include "rba_BswSrv.h"
#  endif

#include "EthTSyn_Prv.h"

/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeSyncMsg()
 *
 * \Function Description
 * Composes Sync message
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

void EthTSyn_ComposeSyncMsg( uint8 *BufPtr_pu8 )
{
    /* Initialise the reserved field of Sync message to Zero */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF)
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
#  else
    /* Copy the entire Sync message from global buffer into driver buffer */
    /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
    (void)rba_BswSrv_MemCopy( &BufPtr_pu8[ETHTSYN_ZERO], &EthTSyn_TimBrdg_st.TxSyncBuf_au8[ETHTSYN_ZERO], ETHTSYN_SYNC_MSG_LEN );
#  endif
}

#   if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_CalculateTimeCRCOne()
 *
 * \Function Description
 * Calculates CRC_Time_1
 *
 * Parameters (in) :
 * \param BufPtr_pu8    - Buffer Pointer
 * \param DataId_u8     - DataId value
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
static void EthTSyn_CalculateTimeCRCOne( uint8 *BufPtr_pu8,
                                         uint8  DataId_u8,
                                         uint8  IdxDomain_u8 )
{
    /* Local Variable declaration */
    EthTSyn_VarMsgHdr_tst *lVarHdr_pst;
    uint8                  lLocalbuf_au8[ETHTSYN_FOURTEEN];
    uint8                  lCrcFlagsTxValidated_u8;
    uint8                  lLen_u8;
    uint8                  lCRCVal_u8;

    /* Local Variable Initialisation */
    lVarHdr_pst             = &( EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st );
# if( ETHTSYN_TIME_BRIDGE_SM ==STD_ON )
    lCrcFlagsTxValidated_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[EthTSyn_Master_ast[EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8].IdxMstCfg_u8].PortRole_pcst->GlbTimeMst_pcst->CrcFlagsTxValidated_u8;
# else
    lCrcFlagsTxValidated_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->CrcFlagsTxValidated_u8;
# endif
    lLen_u8                 = ETHTSYN_ZERO;

    lLocalbuf_au8[ETHTSYN_ZERO] = lCrcFlagsTxValidated_u8;
    lLen_u8++;

    /* Write MessageLength to local buffer if flag for domainNumber is set */
    if ( ETHTSYN_SUBTLV_MESSAGE_LENGTH == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_MESSAGE_LENGTH ) )
    {
        lLocalbuf_au8[lLen_u8]             = (uint8)( ( lVarHdr_pst->MessageLength_u16 & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_ONE] = (uint8)( lVarHdr_pst->MessageLength_u16 & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLen_u8                           += ETHTSYN_TWO;
    }

    /* Write correctionField to local buffer if flag for correctionField is set */
    if ( ETHTSYN_SUBTLV_CORRECTION_FIELD == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_CORRECTION_FIELD ) )
    {
        lLocalbuf_au8[lLen_u8]                = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_EIGHTH_BYTE_MASK ) >> ETHTSYN_FIFTYSIX );
        lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SEVENTH_BYTE_MASK ) >> ETHTSYN_FORTYEIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_TWO]    = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SIXTH_BYTE_MASK ) >> ETHTSYN_FORTY );
        lLocalbuf_au8[lLen_u8+ETHTSYN_THREE]  = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIFTH_BYTE_MASK ) >> ETHTSYN_THIRTYTWO );
        lLocalbuf_au8[lLen_u8+ETHTSYN_FOUR]   = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
        lLocalbuf_au8[lLen_u8+ETHTSYN_FIVE]   = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
        lLocalbuf_au8[lLen_u8+ETHTSYN_SIX]    = (uint8)( ( lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_SEVEN]  = (uint8)(   lVarHdr_pst->CorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIRST_BYTE_MASK );
        lLen_u8                              += ETHTSYN_EIGHT;
    }

    /* Write sequenceId to local buffer if flag for sequenceId is set */
    if ( ETHTSYN_SUBTLV_SEQUENCE_ID == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_SEQUENCE_ID ) )
    {
        lLocalbuf_au8[lLen_u8]             = (uint8)( ( lVarHdr_pst->SequenceId_au16[ETHTSYN_SYNC_HRD_IDX] & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_ONE] = (uint8)( lVarHdr_pst->SequenceId_au16[ETHTSYN_SYNC_HRD_IDX] & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLen_u8                           += ETHTSYN_TWO;
    }

    /* Read dataIDValue */
    lLocalbuf_au8[lLen_u8] = DataId_u8;
    lLen_u8++;

    /* Calculate CRC */
    /* TRACE[SWS_EthTSyn_00096]:The function Crc_CalculateCRC8H2F() shall be used to calculate the CRC if configured */
    lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), lLen_u8, ETHTSYN_CRC_START_VALUE, TRUE );

    /* Write CRC value into buffer */
    *( BufPtr_pu8 + ETHTSYN_FOUR ) = lCRCVal_u8;
}
#  endif

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF)
#   if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeStatusSubTLV()
 *
 * \Function Description
 * Composes status sub TLV
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param DataId_u8        - DataId value
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
static void EthTSyn_ComposeStatusSubTLV( uint8 *BufPtr_pu8,
                                         uint8  DataId_u8,
                                         uint8  IdxDomain_u8,
                                         uint8  IdxGlbTimMst_u8 )
{
    /* Local Variable declaration */
    EthTSyn_TxCrcSecured_ten lTxCrcValidated_en;
    uint8                    lCRCVal_u8;

    /* Local Variable Initialisation */
    lTxCrcValidated_en = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TxCrcValidated_en;

    /* Write Status Sub TLV fields into buffer */
    /* Write Length field */
    *( BufPtr_pu8 + ETHTSYN_ONE ) = ETHTSYN_TWO;
    /* Write Status field */
    *( BufPtr_pu8 + ETHTSYN_TWO ) = EthTSyn_Master_ast[IdxGlbTimMst_u8].TxFollowup_st.SubTLV_st.StatusSubTLV_st.Status_u8;

    /* Write Type field and CRC/Reserved field */
    /* Check that Master state machine is configured with CRC Support */
    if( ETHTSYN_CRC_SUPPORTED == lTxCrcValidated_en )
    {
        /* Write Type of the Status Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_ZERO )  = ETHTSYN_STATUS_WITH_CRC;
        /* Write Data ID value into buffer */
        *( BufPtr_pu8 + ETHTSYN_THREE ) = DataId_u8;

        /* Calculate CRC */
        /*TRACE[SWS_EthTSyn_00096]:The function Crc_CalculateCRC8H2F() shall be used to calculate the CRC if configured */
        lCRCVal_u8                      = Crc_CalculateCRC8H2F( ( BufPtr_pu8 + ETHTSYN_TWO ), ETHTSYN_TWO, ETHTSYN_CRC_START_VALUE, TRUE);
        *( BufPtr_pu8 + ETHTSYN_THREE ) = lCRCVal_u8;
    }
    else
    {
        /* Write Type of the Status Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_ZERO )  = ETHTSYN_STATUS_WITHOUT_CRC;
        /* Write Zero for reserved field of Status Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_THREE ) = ETHTSYN_ZERO;
    }
}
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeUserDataSubTLV()
 *
 * \Function Description
 * Composes UserData sub TLV
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param DataId_u8        - DataId value
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
static void EthTSyn_ComposeUserDataSubTLV( uint8 *BufPtr_pu8,
                                           uint8  DataId_u8,
                                           uint8  IdxDomain_u8,
                                           uint8  IdxGlbTimMst_u8 )
{
    /* Local Variable declaration */
    StbM_UserDataType        *lUserData_pst;
    EthTSyn_TxCrcSecured_ten  lTxCrcValidated_en;
    uint8                     lCRCVal_u8;

    /* Local Variable Initialisation */
    lUserData_pst      = &EthTSyn_Master_ast[IdxGlbTimMst_u8].TxFollowup_st.SubTLV_st.UserDataSubTLV_st.UserData_st;
    lTxCrcValidated_en = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TxCrcValidated_en;

    /* Write UserData Sub TLV fields into buffer */
    /* Write Length field */
    *( BufPtr_pu8 + ETHTSYN_ONE )      = ETHTSYN_FIVE;
    /* Write UserData fields */
    *( BufPtr_pu8 + ETHTSYN_TWO )      = lUserData_pst->userDataLength;
    *( BufPtr_pu8 + ETHTSYN_THREE )    = lUserData_pst->userByte0;
    *( BufPtr_pu8 + ETHTSYN_FOUR )     = lUserData_pst->userByte1;
    *( BufPtr_pu8 + ETHTSYN_FIVE )     = lUserData_pst->userByte2;

    /* Read UserData Sub TLV fields */
    /* Write Type field and CRC/Reserved field */
    if( ETHTSYN_CRC_SUPPORTED == lTxCrcValidated_en )
    {
        /* Write Type of the UserData Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_ZERO ) = ETHTSYN_USERDATA_WITH_CRC;
        /* Write Data ID value into buffer */
        *( BufPtr_pu8 + ETHTSYN_SIX )  = DataId_u8;

        /* Calculate CRC */
        /*TRACE[SWS_EthTSyn_00096]:The function Crc_CalculateCRC8H2F() shall be used to calculate the CRC if configured */
        lCRCVal_u8                    = Crc_CalculateCRC8H2F( ( BufPtr_pu8 + ETHTSYN_TWO ), ETHTSYN_FIVE, ETHTSYN_CRC_START_VALUE, TRUE);
        /* Write CRC value into buffer */
        *( BufPtr_pu8 + ETHTSYN_SIX ) = lCRCVal_u8;
    }
    else
    {
        /* Write Type of the UserData Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_ZERO ) = ETHTSYN_USERDATA_WITHOUT_CRC;
        /* Write Zero for reserved field of UserData Sub TLV into buffer */
        *( BufPtr_pu8 + ETHTSYN_SIX )  = ETHTSYN_ZERO;
    }
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_CalculateTimeCRCZero()
 *
 * \Function Description
 * Calculates CRC_Time_0
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param DataId_u8        - DataId value
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
static void EthTSyn_CalculateTimeCRCZero( uint8 *BufPtr_pu8,
                                          uint8  DataId_u8,
                                          uint8  IdxDomain_u8,
                                          uint8  IdxGlbTimMst_u8 )
{
    /* Local Variable declaration */
    EthTSyn_VarMsgHdr_tst *lVarHdr_pst;
    Eth_TimeStampType     *lPreciseOriginTimeStamp_pst;
    uint8                  lLocalbuf_au8[ETHTSYN_TWENTYTHREE];
    uint8                  lCrcFlagsTxValidated_u8;
    uint8                  lLen_u8;
    uint8                  lCRCVal_u8;

    /* Local Variable Initialisation */
    lVarHdr_pst                 = &( EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st );
    lPreciseOriginTimeStamp_pst = &EthTSyn_Master_ast[IdxGlbTimMst_u8].TxFollowup_st.PreciseOriginTimeStamp_st;
    lCrcFlagsTxValidated_u8     = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->CrcFlagsTxValidated_u8;
    lLen_u8                     = ETHTSYN_ZERO;

    lLocalbuf_au8[ETHTSYN_ZERO] = lCrcFlagsTxValidated_u8;
    lLen_u8++;

    /* Write domain number to local buffer if flag for domain number is set */
    if ( ETHTSYN_SUBTLV_DOMAIN_NUMBER == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_DOMAIN_NUMBER )  )
    {
        lLocalbuf_au8[lLen_u8]  = lVarHdr_pst->DomainNumber_u8;
        lLen_u8++;
    }

    /* Write sourcePortIdentity to local buffer if flag for sourcePortIdentity is set */
    if ( ETHTSYN_SUBTLV_SOURCEPORT_IDENTITY == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_SOURCEPORT_IDENTITY ) )
    {
        lLocalbuf_au8[lLen_u8]                = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_ZERO];
        lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_ONE];
        lLocalbuf_au8[lLen_u8+ETHTSYN_TWO]    = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_TWO];
        lLocalbuf_au8[lLen_u8+ETHTSYN_THREE]  = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_THREE];
        lLocalbuf_au8[lLen_u8+ETHTSYN_FOUR]   = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_FOUR];
        lLocalbuf_au8[lLen_u8+ETHTSYN_FIVE]   = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_FIVE];
        lLocalbuf_au8[lLen_u8+ETHTSYN_SIX]    = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_SIX];
        lLocalbuf_au8[lLen_u8+ETHTSYN_SEVEN]  = lVarHdr_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_SEVEN];
        lLocalbuf_au8[lLen_u8+ETHTSYN_EIGHT]  = (uint8)( ( lVarHdr_pst->SourcePortIdentity_st.PortNumber_u16 & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_NINE]   = (uint8)( lVarHdr_pst->SourcePortIdentity_st.PortNumber_u16 & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLen_u8                              += ETHTSYN_TEN;
    }

    /* Write preciseOriginTimestamp to local buffer if flag for preciseOriginTimestamp is set */
    if ( ETHTSYN_SUBTLV_PRECISE_ORIGIN_TIMESTAMP == ( lCrcFlagsTxValidated_u8 & ETHTSYN_SUBTLV_PRECISE_ORIGIN_TIMESTAMP ) )
    {
        lLocalbuf_au8[lLen_u8]                = (uint8)( ( lPreciseOriginTimeStamp_pst->secondsHi & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = (uint8)( lPreciseOriginTimeStamp_pst->secondsHi & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLocalbuf_au8[lLen_u8+ETHTSYN_TWO]    = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
        lLocalbuf_au8[lLen_u8+ETHTSYN_THREE]  = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
        lLocalbuf_au8[lLen_u8+ETHTSYN_FOUR]   = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_FIVE]   = (uint8)( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLocalbuf_au8[lLen_u8+ETHTSYN_SIX]    = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
        lLocalbuf_au8[lLen_u8+ETHTSYN_SEVEN]  = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
        lLocalbuf_au8[lLen_u8+ETHTSYN_EIGHT]  = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
        lLocalbuf_au8[lLen_u8+ETHTSYN_NINE]   = (uint8)( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_FIRST_BYTE_MASK );
        lLen_u8                              += ETHTSYN_TEN;
    }

    /* Read dataIDValue */
    lLocalbuf_au8[lLen_u8] = DataId_u8;
    lLen_u8++;

    /* Calculate CRC */
    /* TRACE[SWS_EthTSyn_00096]:The function Crc_CalculateCRC8H2F() shall be used to calculate the CRC if configured */
    lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), lLen_u8, ETHTSYN_CRC_START_VALUE, TRUE);

    /* Write CRC value into buffer */
    *( BufPtr_pu8 + ETHTSYN_THREE ) = lCRCVal_u8;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeTimeSubTLV()
 *
 * \Function Description
 * Composes Time sub TLV
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param DataId_u8        - DataId value
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
LOCAL_INLINE void EthTSyn_ComposeTimeSubTLV( uint8 *BufPtr_pu8,
                                             uint8  DataId_u8,
                                             uint8  IdxDomain_u8,
                                             uint8  IdxGlbTimMst_u8 )
{
    /* Read Status Sub TLV fields */
    *( BufPtr_pu8 + ETHTSYN_ZERO ) = ETHTSYN_TIME_WITH_CRC;
    *( BufPtr_pu8 + ETHTSYN_ONE )  = ETHTSYN_THREE;
    *( BufPtr_pu8 + ETHTSYN_TWO )  = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->CrcFlagsTxValidated_u8;

    /* Calculate CRC_Time_0 */
    EthTSyn_CalculateTimeCRCZero( BufPtr_pu8, DataId_u8, IdxDomain_u8, IdxGlbTimMst_u8 );

    /* Calculate CRC_Time_1 */
    EthTSyn_CalculateTimeCRCOne( BufPtr_pu8, DataId_u8, IdxDomain_u8 );
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeAutosarTLVHdr()
 *
 * \Function Description
 * Composes Autosar TLV Header
 *
 * Parameters (in) :
 * \param BufPtr_pu8   - Buffer Pointer
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
static void EthTSyn_ComposeAutosarTLVHdr( uint8 *BufPtr_pu8,
                                          uint8  IdxDomain_u8 )
{
    /* Local Variable declaration */
    uint16 lLength_u16;

    /* Local Variable Initialisation */
    /* Subtract FollowUp message length and Four bytes from Autosar TLV header from total length */
    lLength_u16 = EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st.MessageLength_u16 - ( ETHTSYN_SYNC_FUP_MSG_LEN + ETHTSYN_FOUR );

    /*Assign Autosar TLV Header fields */
    *( BufPtr_pu8 + ETHTSYN_ZERO )   = (uint8) ( ( (uint16)( ETHTYSN_FOLLOWUP_TLV_TYPE & ETHTSYN_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + ETHTSYN_ONE )    = (uint8) (   (uint16)( ETHTYSN_FOLLOWUP_TLV_TYPE & ETHTSYN_FIRST_BYTE_MASK ) );
    *( BufPtr_pu8 + ETHTSYN_TWO )    = (uint8) ( ( lLength_u16 & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + ETHTSYN_THREE )  = (uint8) (   lLength_u16 & ETHTSYN_WORD_FIRST_BYTE_MASK );
    *( BufPtr_pu8 + ETHTSYN_FOUR )   = (uint8) ( ( ETHTYSN_AR_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + ETHTSYN_FIVE )   = (uint8) ( ( ETHTYSN_AR_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + ETHTSYN_SIX )    = (uint8) (   ETHTYSN_AR_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_FIRST_BYTE_MASK );
    *( BufPtr_pu8 + ETHTSYN_SEVEN )  = (uint8) ( ( ETHTYSN_AR_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + ETHTSYN_EIGHT )  = (uint8) ( ( ETHTYSN_AR_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + ETHTSYN_NINE )   = (uint8) (   ETHTYSN_AR_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_FIRST_BYTE_MASK );
}
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeAutosarTLV()
 *
 * \Function Description
 * Composes Autosar TLV
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
static void EthTSyn_ComposeAutosarTLV( uint8 *BufPtr_pu8,
                                       uint8  IdxDomain_u8,
                                       uint8  IdxGlbTimMst_u8 )
{
    /* Local Variable declaration */
    EthTSyn_VarMsgHdr_tst *lVarHeader_pst;
    uint8                 *lBufPtr_pu8;
    uint8                  lDataIDVal_u8;
    uint8                  lTLVFolwUpSubTLV_u8;

    /* Local Variable Initialisation */
    lVarHeader_pst = &EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st;

    /* Check that Master state machine is configured with CRC Support */
    if( ETHTSYN_CRC_SUPPORTED == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TxCrcValidated_en )
    {
        /* TRACE[SWS_EthTSyn_00097]:The DataID shall be calculated as DataID = DataIDList[Follow_Up.sequenceId mod 16] */
        lDataIDVal_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].FolwUpDataIDList_pcst[( lVarHeader_pst->SequenceId_au16[ETHTSYN_SYNC_HRD_IDX] ) % ETHTSYN_SIXTEEN];
    }
    else
    {
        lDataIDVal_u8 = ETHTSYN_ZERO;
    }

    lTLVFolwUpSubTLV_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TLVFolwUpSubTLV_u8;

    /* Update Buffer Pointer to Point to the beginning of sub TLVs after TLV header */
    lBufPtr_pu8 = BufPtr_pu8 + ETHTSYN_TEN;

    /* Increment message length by Ten for fields of TLV header */
    lVarHeader_pst->MessageLength_u16 += ETHTSYN_TEN;

    /* TRACE[SWS_EthTSyn_00077]:If EthTSynMessageCompliance is set to FALSE and EthTSynTLVFollowUpStatusSubTLV is set to TRUE, the Time Master shall send a Follow_Up with Status sub TLV */
    if( ETHTSYN_MASTER_STATUS_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_STATUS_SUB_TLV ) )
    {
        /* Update message length by length of Status sub TLV */
        lVarHeader_pst->MessageLength_u16 += ETHTSYN_STATUS_LENGTH;

        /* Call a function to compose Status Sub TLV */
        EthTSyn_ComposeStatusSubTLV( lBufPtr_pu8, lDataIDVal_u8, IdxDomain_u8, IdxGlbTimMst_u8 );

        /* Update the buffer pointer */
        lBufPtr_pu8 += ETHTSYN_STATUS_LENGTH;
    }
    /* TRACE[SWS_EthTSyn_00079]:If EthTSynMessageCompliance is set to FALSE and EthTSynTLVFollowUpUserDataSubTLV is set to TRUE, the Time Master shall send a Follow_Up with UserData sub TLV */
    /* TRACE[SWS_EthTSyn_00153]:If StbM_UserDataType.userDataLength is set to 0 the complete AUTOSAR Sub-TLV: UserData shall be excluded from the message */
    if( ( ETHTSYN_MASTER_USERDATA_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_USERDATA_SUB_TLV ) )
    && ( ETHTSYN_ZERO != EthTSyn_Master_ast[IdxGlbTimMst_u8].TxFollowup_st.SubTLV_st.UserDataSubTLV_st.UserData_st.userDataLength ) )
    {
        /* Update message length by length of UserData sub TLV */
        lVarHeader_pst->MessageLength_u16 += ETHTSYN_USERDATA_LENGTH;

        /* Call a function to compose UserData Sub TLV */
        EthTSyn_ComposeUserDataSubTLV( lBufPtr_pu8, lDataIDVal_u8, IdxDomain_u8, IdxGlbTimMst_u8 );

        /* Update the buffer pointer */
        lBufPtr_pu8 += ETHTSYN_USERDATA_LENGTH;

    }
    /* TRACE[SWS_EthTSyn_00075]:If EthTSynMessageCompliance is set to FALSE and EthTSynTLVFollowUpTimeSubTLV is set to
     * TRUE, the Time Master shall send a Follow_Up with Time Secured sub TLV */
    if( ETHTSYN_MASTER_TIME_SUB_TLV == ( lTLVFolwUpSubTLV_u8 & ETHTSYN_MASTER_TIME_SUB_TLV ) )
    {
        /* Update message length by length of Time sub TLV */
        lVarHeader_pst->MessageLength_u16 += ETHTSYN_TIME_LENGTH;

        /* Call a function to compose Time Sub TLV */
        EthTSyn_ComposeTimeSubTLV( lBufPtr_pu8, lDataIDVal_u8, IdxDomain_u8, IdxGlbTimMst_u8 );

        /* Update the buffer pointer */
        lBufPtr_pu8 += ETHTSYN_TIME_LENGTH;
    }

    /*Call a function to compose Autosar TLV header */
    EthTSyn_ComposeAutosarTLVHdr( BufPtr_pu8, IdxDomain_u8 );
}
#   endif
#  endif

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
/**
 ****************************************************************************************************************
 * \Function Name : EthTSyn_UpdateCorrectionField()
 *
 * \Function Description
 * This API is used to update the correction field in the followup header when time bridge feature is enabled
 *
 * Parameters (in) :
 * \param BufPtr_pu8    - Buffer Pointer
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
 ****************************************************************************************************************
 */
static void EthTSyn_UpdateCorrectionField ( uint8 *BufPtr_pu8,
                                            uint8  IdxDomain_u8 )
{
    /* Local Variable declaration */
#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    EthSwt_MgmtObjectType *lMstMgmtObj_pst;
#   else
    Eth_TimeIntDiffType    lUplinkToTx_st;
    Eth_TimeIntDiffType    lTim_st;
#   endif
    uint64                *lCorrnField_pu64;
    Eth_TimeIntDiffType    lResidenceTi_st;
    uint8                  lTrsmIdx_u8;
    uint8                  lIdxMstCfg_u8;
    uint8                  lPortNr_u8;

    /* Local Variable Initialisation */
#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    lMstMgmtObj_pst     = EthTSyn_MstMgmtObject_ast[EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8].MstEthSwt_MgmtObject_pst;
#   endif
    lCorrnField_pu64    = &( EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st.CorrectionField_u64 );

#   ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    if( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8 )
    {
        /* Convert the Switch Uplink to Tx Residence time to Eth_TimeIntDiffType format */
        EthTSyn_CnvToEthTi( ( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ), ETHTSYN_ZERO, &lUplinkToTx_st );

        /* Add Switch Rx to Uplink Residence time to Uplink to Tx time */
        EthTSyn_AddTi( &lUplinkToTx_st, &EthTSyn_SlaveMgmtObject_ast[ETHTSYN_ZERO].SlaveTiStamp_st, &lTim_st );

        /* Add the residence times of Master and Slave ports to difference between Ingress of Sync and Egress of Sync at driver  */
        EthTSyn_AddTi( &lTim_st, &EthTSyn_TimBrdg_st.MstDrvOffsetTiStamp_st, &lResidenceTi_st );

        /* Convert the time obtained to 64 bit value */
        EthTSyn_CnvEthTitoHex( lResidenceTi_st, ETHTSYN_SIXTEEN, lCorrnField_pu64 );

        /* Add correction field from the FollowUp header */
        (*lCorrnField_pu64) = (*lCorrnField_pu64) + EthTSyn_Slave_ast[0].RxFollowup_st.CorrectionField_u64;
    }

#   else

    /* Obtain the difference between Egress of Sync message at Master Port and Ingress of Sync message at Slave port */
    EthTSyn_SubTi( &lMstMgmtObj_pst->EgressTimestamp, &EthTSyn_SlaveMgmtObject_ast[ETHTSYN_ZERO].SlaveTiStamp_st.diff, &lResidenceTi_st );

    /* Convert the time obtained to 64 bit value */
    EthTSyn_CnvEthTitoHex( lResidenceTi_st, ETHTSYN_SIXTEEN, lCorrnField_pu64 );

    /* Add correction field from the FollowUp header */
    (*lCorrnField_pu64) = (*lCorrnField_pu64) + EthTSyn_Slave_ast[0].RxFollowup_st.CorrectionField_u64;
#   endif

    /* Clear lower 16 bits of the correction field for storing port number */
    ( *lCorrnField_pu64 )   = ( *lCorrnField_pu64 ) & ( ETHTSYN_CORRN_FIE_PORT_MASK );

    /* Store the port number in a local variable as well as in the Buffer */
    lTrsmIdx_u8             = EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8;
    lIdxMstCfg_u8           = EthTSyn_Master_ast[lTrsmIdx_u8].IdxMstCfg_u8;
    lPortNr_u8              = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxMstCfg_u8].SwtMngtEthSwtPort_u8;

    /* Update last byte to have Port number */
    ( *lCorrnField_pu64 )   = ( *lCorrnField_pu64 ) | ( (uint64) lPortNr_u8 );

    /* Update correction field in buffer */
    *( BufPtr_pu8 + 8 )     = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_EIGHTH_BYTE_MASK  ) >> ETHTSYN_FIFTYSIX );
    *( BufPtr_pu8 + 9 )     = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_SEVENTH_BYTE_MASK ) >> ETHTSYN_FORTYEIGHT );
    *( BufPtr_pu8 + 10 )    = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_SIXTH_BYTE_MASK   ) >> ETHTSYN_FORTY );
    *( BufPtr_pu8 + 11 )    = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_FIFTH_BYTE_MASK   ) >> ETHTSYN_THIRTYTWO );
    *( BufPtr_pu8 + 12 )    = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_FOURTH_BYTE_MASK  ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 13 )    = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_THIRD_BYTE_MASK   ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 14 )    = (uint8)( ( ( *lCorrnField_pu64 ) & ETHTSYN_DOUBLE_WORD_SECOND_BYTE_MASK  ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 15 )    = lPortNr_u8;
}
#  endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ComposeFollowUpMsg()
 *
 * \Function Description
 * Composes Follow_Up message
 *
 * Parameters (in) :
 * \param BufPtr_pu8       - Buffer Pointer
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
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
void EthTSyn_ComposeFollowUpMsg( uint8 *BufPtr_pu8,
                                 uint8  IdxDomain_u8,
                                 uint8  IdxGlbTimMst_u8 )
{
    /* local variable declaration */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF)
    Eth_TimeStampType            *lPreciseOriginTimeStamp_pst;
#   if( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    uint8                        *lBufPtr_pu8;
#   endif
#  else
#   if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    const EthTSyn_PortCfg_tst    *lPortCfg_pcst;
    const EthTSyn_GlbTimeMst_tst *lGlbTimeMst_pcst;
    uint16                        lSeqId_u16;
    uint8                         lTrsmIdx_u8;
    uint8                         lIdxMstCfg_u8;
#   endif
#  endif

    /* local variable initialisation */
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
    lPreciseOriginTimeStamp_pst  = &EthTSyn_Master_ast[IdxGlbTimMst_u8].TxFollowup_st.PreciseOriginTimeStamp_st;
#  else
   (void) IdxGlbTimMst_u8;
#  endif

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )

    *( BufPtr_pu8 + 34 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->secondsHi & ETHTSYN_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 35 ) = (uint8)  ( lPreciseOriginTimeStamp_pst->secondsHi & ETHTSYN_FIRST_BYTE_MASK  );
    *( BufPtr_pu8 + 36 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 37 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_THIRD_BYTE_MASK  ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 38 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 39 ) = (uint8)  ( lPreciseOriginTimeStamp_pst->seconds & ETHTSYN_WORD_FIRST_BYTE_MASK  );
    *( BufPtr_pu8 + 40 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 41 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_THIRD_BYTE_MASK  ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 42 ) = (uint8)( ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 43 ) = (uint8)  ( lPreciseOriginTimeStamp_pst->nanoseconds & ETHTSYN_WORD_FIRST_BYTE_MASK  );

    /* Sync FollowUp TLV */
    *( BufPtr_pu8 + 44 ) = (uint8)( ( (uint16)( ETHTYSN_FOLLOWUP_TLV_TYPE & ETHTSYN_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 45 ) = (uint8)(   (uint16)( ETHTYSN_FOLLOWUP_TLV_TYPE & ETHTSYN_FIRST_BYTE_MASK  ) );
    *( BufPtr_pu8 + 46 ) = (uint8)( ( (uint16)( ETHTYSN_FOLLOWUP_TLV_LENGTH & ETHTSYN_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 47 ) = (uint8)(   (uint16)( ETHTYSN_FOLLOWUP_TLV_LENGTH & ETHTSYN_FIRST_BYTE_MASK  ) );
    *( BufPtr_pu8 + 48 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_THIRD_BYTE_MASK  ) ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 49 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 50 ) = (uint8)(   (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGAID & ETHTSYN_WORD_FIRST_BYTE_MASK  ) );
    *( BufPtr_pu8 + 51 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_THIRD_BYTE_MASK  ) ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 52 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 53 ) = (uint8)(   (uint32)( ETHTYSN_FOLLOWUP_TLV_ORGASUBTYPE & ETHTSYN_WORD_FIRST_BYTE_MASK  ) );
    *( BufPtr_pu8 + 54 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_RATEOFFSET & ETHTSYN_WORD_FOURTH_BYTE_MASK ) ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 55 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_RATEOFFSET & ETHTSYN_WORD_THIRD_BYTE_MASK  ) ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 56 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_RATEOFFSET & ETHTSYN_WORD_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 57 ) = (uint8)(   (uint32)( ETHTYSN_FOLLOWUP_TLV_RATEOFFSET & ETHTSYN_WORD_FIRST_BYTE_MASK  ) );
    *( BufPtr_pu8 + 58 ) = (uint8)( ( (uint16)( ETHTYSN_FOLLOWUP_TLV_TIMEBASEIND & ETHTSYN_SECOND_BYTE_MASK ) )>> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 59 ) = (uint8)(   (uint16)( ETHTYSN_FOLLOWUP_TLV_TIMEBASEIND & ETHTSYN_FIRST_BYTE_MASK  ) );

    *( BufPtr_pu8 + 60 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 61 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 62 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 63 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 64 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 65 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 66 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 67 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 68 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 69 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 70 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );
    *( BufPtr_pu8 + 71 ) = (uint8)( ETHTYSN_FOLLOWUP_TLV_PHASECHG );

    *( BufPtr_pu8 + 72 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_FREQCHG & ETHTSYN_WORD_FOURTH_BYTE_MASK ) ) >> ETHTSYN_TWENTYFOUR );
    *( BufPtr_pu8 + 73 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_FREQCHG & ETHTSYN_WORD_THIRD_BYTE_MASK  ) ) >> ETHTSYN_SIXTEEN );
    *( BufPtr_pu8 + 74 ) = (uint8)( ( (uint32)( ETHTYSN_FOLLOWUP_TLV_FREQCHG & ETHTSYN_WORD_SECOND_BYTE_MASK ) ) >> ETHTSYN_EIGHT );
    *( BufPtr_pu8 + 75 ) = (uint8)(   (uint32)( ETHTYSN_FOLLOWUP_TLV_FREQCHG & ETHTSYN_WORD_FIRST_BYTE_MASK  ) );

#   if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )

    /* Assign the next buffer address to a local pointer */
    lBufPtr_pu8 = BufPtr_pu8 + ETHTSYN_SYNC_FUP_MSG_LEN;

    /* Compose Autosar TLV only if any of the sub TLVs are configured */
    if( ETHTSYN_ZERO != EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->TLVFolwUpSubTLV_u8 )
    {
        EthTSyn_ComposeAutosarTLV( lBufPtr_pu8, IdxDomain_u8, IdxGlbTimMst_u8 );
    }
#   endif
#  else
    /* Copy the entire FollowUp message from global buffer to driver buffer */
    /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
    (void)rba_BswSrv_MemCopy( &BufPtr_pu8[ETHTSYN_ZERO], &EthTSyn_TimBrdg_st.TxFollowUpBuf_au8[ETHTSYN_ZERO], EthTSyn_TimBrdg_st.SyncFupLen_u16 );

    /* Update the correction Field with Port number and residence time */
    EthTSyn_UpdateCorrectionField( BufPtr_pu8, IdxDomain_u8 );

#   if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )

    lTrsmIdx_u8             = EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8;
    lIdxMstCfg_u8           = EthTSyn_Master_ast[lTrsmIdx_u8].IdxMstCfg_u8;
    lPortCfg_pcst           = &EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxMstCfg_u8];
    lGlbTimeMst_pcst        = lPortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst;

    /* If time sub TLV is present re-calculate it and update the buffer contents */
    if( ( ETHTSYN_ZERO          != EthTSyn_TimBrdg_st.TiSubTLVLen_u16 )
    &&  ( ETHTSYN_CRC_SUPPORTED == lGlbTimeMst_pcst->TxCrcValidated_en ) )
    {
        /* Obtain the sequence id from reading into the message */
        lSeqId_u16 = ( ( uint16 )( ( ( uint16 )BufPtr_pu8[ETHTSYN_THIRTY] ) << ETHTSYN_EIGHT ) | ( BufPtr_pu8[ETHTSYN_THIRTYONE] ) );

        /* Store the obtained sequence id in the RAM data structure */
        EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st.SequenceId_au16[ETHTSYN_SYNC_HRD_IDX] = lSeqId_u16;

        /* Obtain the message length from reading into the message */
        EthTSyn_RamDataTyp_ast[IdxDomain_u8].VarHeader_st.MessageLength_u16 = ( ( uint16 )( ( ( uint16 )BufPtr_pu8[ETHTSYN_TWO] ) << ETHTSYN_EIGHT ) | ( BufPtr_pu8[ETHTSYN_THREE] ) );

        /* Calculate crc time one again due to the update in correction field */
        EthTSyn_CalculateTimeCRCOne( &BufPtr_pu8[ EthTSyn_TimBrdg_st.TiSubTLVLen_u16 ], EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].FolwUpDataIDList_pcst[ lSeqId_u16 % ETHTSYN_SIXTEEN ], IdxDomain_u8 );
    }
    (void) IdxGlbTimMst_u8;
#   endif
#  endif
#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_ON )
    (void) IdxDomain_u8;
#  endif
}

#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_OFF )
#   ifdef ETHTSYN_SWITCH
/**
 ********************************************************************************************
 * \Function Name : EthTSyn_TxFollowUpSwtEnad()
 *
 * \Function Description
 * This API is used to transmit the follow-Up frame when Switch Port Reference is configured
 *
 * Parameters (in) :
 * \param IdxDomain_u8     - Index of the current configured Global Time Domain
 * \param IdxGlbTimMst_u8  - Index of the Master Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: None
 * Return Type : void
 ********************************************************************************************
 */
static void EthTSyn_TxFollowUpSwtEnad ( uint8 IdxDomain_u8,
                                        uint8 IdxGlbTimMst_u8 )
{
    /* Local Variable declaration */
    EthTSyn_SyncStMach_ten  *lSync_Mst_pen;
    uint8                    lIdxMstPort_u8;

    /* Local variable Initialization */
    lSync_Mst_pen    = &EthTSyn_Master_ast[IdxGlbTimMst_u8].Sync_Mst_en;

#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME

    /* Call EthTSyn_Transmit with ETHTSYN_FOLLOW_UP_TYPE in a for loop for Master Ports if message to be sent is FollowUp and TxConfirmation of Sync has been successful*/
    for( lIdxMstPort_u8 = ETHTSYN_ZERO; lIdxMstPort_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lIdxMstPort_u8++ )
    {
        /* Transmit the FollowUp message with proper correction field only if the ownership of the management object is given to EthTSyn */
        if( ( NULL_PTR                              != EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst )
        &&  ( ETHSWT_MGMT_OBJ_OWNED_BY_UPPER_LAYER  == EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Ownership ) )
        {
            /* Transmit FollowUp only if Egress time stamp is valid */
            if( E_OK == EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Validation.EgressTimestampValid )
            {
                /* Increment for every successful FollowUp transmission */
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8 = lIdxMstPort_u8;

                /* Transmit Follow-up message */
                EthTSyn_Transmit( ETHTSYN_FOLLOW_UP_TYPE, IdxDomain_u8, IdxGlbTimMst_u8 );
            }
            /* Decrement the counter for number of successful transmissions */
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].nrTrsm_u8--;

            /* Set the ownership to unused so switch can use this datastructure when EthIf_GetTxMgmtObject is called */
            EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;

            /* Set the Management object pointer to null pointer after transmitting the message for that port */
            EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst = NULL_PTR;
        }
    }

    /* Check that number of successful transmissions index is ZERO before changing state of Master State Machine */
    if( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[IdxDomain_u8].nrTrsm_u8 )
    {
        /* Set State of Sync Master State Machine to Follow-Up Sent */
        (* lSync_Mst_pen ) = ETHTSYN_FOLLOW_UP_E;
    }

#    else

    /* Call EthTSyn_Transmit with ETHTSYN_FOLLOW_UP_TYPE in a for loop for Master Ports if message to be sent is FollowUp and TxConfirmation of Sync has been successful*/
    for( lIdxMstPort_u8 = ETHTSYN_ZERO; lIdxMstPort_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lIdxMstPort_u8++ )
    {
        /* Increment for every successful FollowUp transmission */
        EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8 = lIdxMstPort_u8;

        /* Transmit Follow-up message */
        EthTSyn_Transmit( ETHTSYN_FOLLOW_UP_TYPE, IdxDomain_u8, IdxGlbTimMst_u8 );
    }

    /* Set State of Sync Master State Machine to Follow-Up Sent */
    (* lSync_Mst_pen )      = ETHTSYN_FOLLOW_UP_E;

#    endif
}
#   endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_RunSyncMstSM()
 *
 * \Function Description
 * State transitions of EthTSyn_Sync_Mst_en SM
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
void EthTSyn_RunSyncMstSM( uint8 IdxDomain_u8 )
{
    /* Local Variable declaration */
    const EthTSyn_TimeDomainCfg_tst *lEthTSyn_TimeDomainCfg_pst;
    EthTSyn_RamDataTyp_tst          *lEthTSyn_RamDataTyp_pst;
    EthTSyn_SyncStMach_ten          *lSync_Mst_pen;
    uint32                           lSyncTxPerd_u32;
    Std_ReturnType                   lRetVal_en;
    StbM_TimeBaseStatusType          lSyncTimeBaseSt_st;
    StbM_TimeBaseStatusType          lOffsetTimeBaseSt_st;
#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
    uint8                            lTimeBasUpdCntr_u8;
#   endif
    uint8                            lIdxGlbTimMst_u8;

    /* Initialize local pointers */
    lEthTSyn_RamDataTyp_pst    = &EthTSyn_RamDataTyp_ast[IdxDomain_u8];
    lEthTSyn_TimeDomainCfg_pst = &EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8];

    if ( NULL_PTR == lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst )
    {
        /* Return to the calling context if Time Master is not configured */
        return;
    }

    /* Index of the current global time master */
    lIdxGlbTimMst_u8   = lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->GlbTimMstIdx_u8;

    /* Local variable Initialization */
    lSyncTxPerd_u32    = lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->SyncTxPerd_u32;
    lSync_Mst_pen      = &EthTSyn_Master_ast[lIdxGlbTimMst_u8].Sync_Mst_en;
#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
    lTimeBasUpdCntr_u8 = ETHTSYN_ZERO;
#   endif

    /* Get TimeBase ID, SyncTimeBase and OffsetTimeBase from StbM */
    lRetVal_en = StbM_GetTimeBaseStatus( lEthTSyn_TimeDomainCfg_pst->StbM_SyncdTimeBasId_u16, &lSyncTimeBaseSt_st, &lOffsetTimeBaseSt_st );

    if ( E_OK != lRetVal_en )
    {
#   if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
        /* Report DET: Development Error: invalid return value */
        ( void )Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RUNSYNCMSTSM, ETHTSYN_E_INV_RETURNVALUE );
#   endif
    }
    else
    {
        /* Set the Domain number in RamData Structure to the configured domain number */
        lEthTSyn_RamDataTyp_pst->VarHeader_st.DomainNumber_u8 = lEthTSyn_TimeDomainCfg_pst->GlbTimeDomId_u8;

#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
        /* Check if Immediate Time Sync bit is set to TRUE for transmission of Sync immediately */
        if ( TRUE == lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->ImdtTimeSync_b )
        {
            /* StbM_GetTimeBaseUpdateCounter returns if time base update counter for a paticular time base id changes  */
            lTimeBasUpdCntr_u8 = StbM_GetTimeBaseUpdateCounter( lEthTSyn_TimeDomainCfg_pst->StbM_SyncdTimeBasId_u16 );
        }
#   endif
        /* Check if GLOBAL_TIME_BASE bit is set and EthTSynGlobalTimeTxPeriod is not equal to zero */
        if ( ( ETHTSYN_EIGHT   == ( lSyncTimeBaseSt_st & ETHTSYN_GLOBAL_TIME_BASE_BIT_MASK ) )
        &&   ( lSyncTxPerd_u32 != ETHTSYN_ZERO ) )
        {
            /* Send Sync messages with cycle EthTSynGlobalTimeTxPeriod */
            if ( ETHTSYN_ZERO != lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] )
            {
                /*Decrement the counter every main cycle*/
                lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]--;
            }
            /* Check is transmission is Asynchronous or Synchronous */
            /* Transmit Sync or Follow-up frame only when Debounce counter value is zero as Debounce counter takes highest priority over other counters*/
            /* If transmission is Asynchronous, Check that StbM has reported a change in timeBaseUpdateCounter before transmitting Sync immediately */
            /* If transmission is Synchronous, Check status of State Machine and if Debounce counter is zero before transmission of Sync message */
            if (
#   if (ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON)
                 ( ETHTSYN_ZERO                  == lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX] ) &&
#   endif
#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
                 ( ( EthTSyn_Master_ast[lIdxGlbTimMst_u8].TimeBasUpdCntr_u8   != lTimeBasUpdCntr_u8 ) ||
#   endif
                   (  ( ETHTSYN_ZERO             == lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] )
                   && ( ( ETHTSYN_SYNC_INIT_E    == (* lSync_Mst_pen ) ) || ( ETHTSYN_FOLLOW_UP_E == (* lSync_Mst_pen ) ) ) )
#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
                   )
#   endif
               )
            {
                /* Transmit Sync message */
                (* lSync_Mst_pen ) = ETHTSYN_SYNC_PREP_E;
                EthTSyn_Transmit( ETHTSYN_SYNC_TYPE, IdxDomain_u8, lIdxGlbTimMst_u8 );
#   if (ETHTSYN_IMMEDIATE_TIME_SYNC == STD_ON)
                /* Check if StbM has reported a change in timeBaseUpdateCounter before loading counter to max value */
                if ( EthTSyn_Master_ast[lIdxGlbTimMst_u8].TimeBasUpdCntr_u8 != lTimeBasUpdCntr_u8 )
                {
                    /* Reset Main function Sync timer to the configured cyclic message resume counter value */
                    lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] =  lEthTSyn_TimeDomainCfg_pst->PortCfg_pcst->PortRole_pcst->GlbTimeMst_pcst->CyclicMsgResuCntr_u32;
                    EthTSyn_Master_ast[lIdxGlbTimMst_u8].TimeBasUpdCntr_u8 = lTimeBasUpdCntr_u8;
                }
                else
#   endif
                {
                    /* Reset the Main function Sync timer to max value */
                    lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] =  lSyncTxPerd_u32;
                }
            }
            /* Check status of State Machine and if Debounce counter is zero before transmission of Follow-up message */
            else if(  ( ETHTSYN_SYNC_SENT_E == (* lSync_Mst_pen ) )
#   if (ETHTSYN_GLOBAL_TIME_DEBOUNCE_TIME == STD_ON)
                   && ( ETHTSYN_ZERO == lEthTSyn_RamDataTyp_pst->MainFunTimer_au32[ETHTSYN_DEB_IDX] )
#   endif
                   )
            {
#   ifdef ETHTSYN_SWITCH
                /* Transmit Follow-Up Frame depending on port number */
                EthTSyn_TxFollowUpSwtEnad( IdxDomain_u8, lIdxGlbTimMst_u8 );
#   else
                /* Transmit Follow-up message */
                (* lSync_Mst_pen )      = ETHTSYN_FOLLOW_UP_E;
                EthTSyn_Transmit( ETHTSYN_FOLLOW_UP_TYPE, IdxDomain_u8, lIdxGlbTimMst_u8 );
#   endif
                /* Reset correction field value in the variable header datastructure */
                lEthTSyn_RamDataTyp_pst->VarHeader_st.CorrectionField_u64 = ETHTSYN_ZERO;
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }/* DET Check */
}
#  endif

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

# endif
#endif /* ETHTSYN_CONFIGURED */

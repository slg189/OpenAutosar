

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "EthTSyn.h"

#ifdef ETHTSYN_CONFIGURED

# if ( ETHTSYN_GLOBAL_TIME_SLAVE == STD_ON )

#  if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
#include "Crc.h"
#  endif

#  if (ETHTSYN_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#  endif /* (ETHTSYN_DEV_ERROR_DETECT == STD_ON) */

#include "EthIf.h"

#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
#include "rba_BswSrv.h"
#  endif

#include "EthTSyn_Prv.h"

#include "EthTSyn_Cfg_SchM.h"

#ifdef ETHTSYN_DEBUG_AND_TEST
#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
Eth_TimeStampType EthTSyn_StbMTimArry_ast[ETHTSYN_DEBUG_ARRAY_SIZE] = {0};
uint8 index = 0U;
#define ETHTSYN_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthTSyn_MemMap.h"
#endif
/*
 ***************************************************************************************************
 * public functions
 ***************************************************************************************************
 */

#define ETHTSYN_START_SEC_CODE
#include "EthTSyn_MemMap.h"

#  if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReadTimeSubTLV()
 *
 * \Function Description
 * Reads time sub TLV frames
 *
 * Parameters (in) :
 * \param HdrPtr_pu8        - Pointer to followup header
 * \param DataPtr_pu8       - Pointer to the sub TLV
 * \param LenTot_u16        - Total length of the Time Sub TLV
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgLen_pu16       - Length of sub TLVs until this sub TLV
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ReadTimeSubTLV( const uint8 *HdrPtr_pu8,
                                              const uint8 *DataPtr_pu8,
                                                   uint16 *MsgLen_pu16,
                                                   uint16  LenTot_u16,
                                                   uint8   IdxGlbTimSlv_u8 )
{
    /* Local Variable declaration */
    EthTSyn_TimeSubTLV_tst *lTimeSubTLV_pst;
    const uint8            *lDataPtr_pu8;
    Std_ReturnType          lRetVal_en;

    /* Local Variable Initialisation */
    lTimeSubTLV_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.TimeSubTLV_st;
    lRetVal_en      = E_OK;
    /* let lDataPtr_pu8 point to beginning of the sub TLV */
    lDataPtr_pu8    = &DataPtr_pu8[(*MsgLen_pu16)];

    /* Place a length check to see that the sub TLV doesn't cross the total length limit */
    if( LenTot_u16 >= ( ( *MsgLen_pu16 ) + lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO ) )
    {
        /*Place a length check to confirm that the received sub TLV is of valid length*/
        /*Write into sub TLV datastructure only for the first time sub TLV if more than one are received */
        if( ( ETHTSYN_THREE == lDataPtr_pu8[ETHTSYN_ONE] ) && ( ETHTSYN_ZERO == lTimeSubTLV_pst->Type_u8 ) )
        {
            /* Read messageLength, domainNumber, correctionField, sourcePortIdentity, sequenceId,
             * Type, CRC_Flags, CRC_Time_0, CRC_Time_1 into datastructure if length field is validated*/
            lTimeSubTLV_pst->Msglen_u16                                   = ( ( uint16 )( ( ( uint16 )HdrPtr_pu8[ETHTSYN_TWO] ) << ETHTSYN_EIGHT  )
                                                                                        | ( HdrPtr_pu8[ETHTSYN_THREE] ) );
            lTimeSubTLV_pst->domnum_u8                                    = HdrPtr_pu8[ETHTSYN_FOUR];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[0]   = HdrPtr_pu8[ETHTSYN_TWENTY];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[1]   = HdrPtr_pu8[ETHTSYN_TWENTYONE];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[2]   = HdrPtr_pu8[ETHTSYN_TWENTYTWO];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[3]   = HdrPtr_pu8[ETHTSYN_TWENTYTHREE];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[4]   = HdrPtr_pu8[ETHTSYN_TWENTYFOUR];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[5]   = HdrPtr_pu8[ETHTSYN_TWENTYFIVE];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[6]   = HdrPtr_pu8[ETHTSYN_TWENTYSIX];
            lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[7]   = HdrPtr_pu8[ETHTSYN_TWENTYSEVEN];
            lTimeSubTLV_pst->SourcePortIdentity_st.PortNumber_u16         = ( ( uint16 )( ( ( uint16 )HdrPtr_pu8[ETHTSYN_TWENTYEIGHT] ) << ETHTSYN_EIGHT )
                                                                                        | ( HdrPtr_pu8[ETHTSYN_TWENTYNINE] ) );
            lTimeSubTLV_pst->SequenceId_u16                               = ( ( uint16 )( ( ( uint16 )HdrPtr_pu8[ETHTSYN_THIRTY] ) << ETHTSYN_EIGHT )
                                                                                        | ( HdrPtr_pu8[ETHTSYN_THIRTYONE] ) );
            lTimeSubTLV_pst->Type_u8                                      = lDataPtr_pu8[ETHTSYN_ZERO];
            lTimeSubTLV_pst->CRC_TimeFlags_u8                             = lDataPtr_pu8[ETHTSYN_TWO];
            lTimeSubTLV_pst->CRC_Time0_u8                                 = lDataPtr_pu8[ETHTSYN_THREE];
            lTimeSubTLV_pst->CRC_Time1_u8                                 = lDataPtr_pu8[ETHTSYN_FOUR];
#  if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
            EthTSyn_TimBrdg_st.TiSubTLVLen_u16 = (* MsgLen_pu16) + ETHTYSN_LENGTH_TILL_TLV_HEADER;
#  endif
            ( *MsgLen_pu16 )                                             += ETHTSYN_TIME_LENGTH;

            /* Obtain the length at which time sub TLV starts */
        }
        else
        {
            /* If length is not validated for time subTLV, skip the sub TLV */
            ( *MsgLen_pu16 )  += lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO;
        }
    }
    else
    {
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReadStatusSubTLV()
 *
 * \Function Description
 * Reads status sub TLV frames
 *
 * Parameters (in) :
 * \param DataPtr_pu8       - Pointer to the sub TLV
 * \param LenTot_u16        - Total length of the Status Sub TLV
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgLen_pu16       - Length of sub TLVs until this sub TLV
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ReadStatusSubTLV( const uint8  *DataPtr_pu8,
                                                      uint16 *MsgLen_pu16,
                                                      uint16  LenTot_u16,
                                                      uint8   IdxGlbTimSlv_u8 )
{
    /* Local Variable declaration */
    EthTSyn_StatusSubTLV_tst *lStatusSubTLV_pst;
    const uint8              *lDataPtr_pu8;
    Std_ReturnType            lRetVal_en;

    /* Local Variable Initialisation */
    lStatusSubTLV_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.StatusSubTLV_st;
    lRetVal_en        = E_OK;

    /* let lDataPtr_pu8 point to beginning of the sub TLV */
    lDataPtr_pu8 = &DataPtr_pu8[(*MsgLen_pu16)];

    /* Place a length check to see that the sub TLV doesn't cross the total length limit */
    if( LenTot_u16 >= ( ( *MsgLen_pu16 ) + lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO ) )
    {
        /*Place a length check to confirm that the received sub TLV is of valid length*/
        /*Write into sub TLV datastructure only for the first status sub TLV if more than one are received */
        if( ( ETHTSYN_TWO == lDataPtr_pu8[ETHTSYN_ONE] ) && ( ETHTSYN_ZERO == lStatusSubTLV_pst->Type_u8 ) )
        {
            /* Read Type, Status and CRC_Status into datastructure */
            lStatusSubTLV_pst->Type_u8               = lDataPtr_pu8[ETHTSYN_ZERO];
            lStatusSubTLV_pst->Status_u8             = lDataPtr_pu8[ETHTSYN_TWO];
            lStatusSubTLV_pst->CRC_Status_u8         = lDataPtr_pu8[ETHTSYN_THREE];
            (*MsgLen_pu16)                          += ETHTSYN_STATUS_LENGTH;
        }
        else
        {
            /* If length is not validated for status subTLV, skip the sub TLV */
            (*MsgLen_pu16) += lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO;
        }
    }
    else
    {
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReadUserDataSubTLV()
 *
 * \Function Description
 * Reads UserData sub TLV frames
 *
 * Parameters (in) :
 * \param DataPtr_pu8       - Pointer to the sub TLV
 * \param LenTot_u16        - Total length of the UserData Sub TLV
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * \param MsgLen_pu16       - Length of sub TLVs until this sub TLV
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */

static Std_ReturnType EthTSyn_ReadUserDataSubTLV( const uint8   *DataPtr_pu8,
                                                        uint16  *MsgLen_pu16,
                                                        uint16   LenTot_u16,
                                                        uint8    IdxGlbTimSlv_u8 )
{
    /* Local Variable declaration */
    EthTSyn_UserDataSubTLV_tst *lUserDataSubTLV_pst;
    const uint8                *lDataPtr_pu8;
    Std_ReturnType              lRetVal_en;

    /* Local Variable Initialisation */
    lUserDataSubTLV_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.UserDataSubTLV_st;
    lRetVal_en          = E_OK;

    /* let lDataPtr_pu8 point to beginning of the sub TLV */
    lDataPtr_pu8 = &DataPtr_pu8[(*MsgLen_pu16)];

    /* Place a length check to see that the sub TLV doesn't cross the total length limit */
    /*Write into sub TLV datastructure only for the first UserData sub TLV if more than one are received */
    if( LenTot_u16 >= ( ( *MsgLen_pu16 ) + lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO ) )
    {
        /*Place a length check to confirm that the received sub TLV is of valid length*/
        if( ( ETHTSYN_FIVE == lDataPtr_pu8[ETHTSYN_ONE] ) && ( ETHTSYN_ZERO == lUserDataSubTLV_pst->Type_u8 ) )
        {
            /* Read Type, UserDataLength, UserByte_0, UserByte_1, UserByte_2 and CRC_UserData into
             * datastructure */
            lUserDataSubTLV_pst->Type_u8                     = lDataPtr_pu8[ETHTSYN_ZERO];
            lUserDataSubTLV_pst->UserData_st.userDataLength  = lDataPtr_pu8[ETHTSYN_TWO];
            lUserDataSubTLV_pst->UserData_st.userByte0       = lDataPtr_pu8[ETHTSYN_THREE];
            lUserDataSubTLV_pst->UserData_st.userByte1       = lDataPtr_pu8[ETHTSYN_FOUR];
            lUserDataSubTLV_pst->UserData_st.userByte2       = lDataPtr_pu8[ETHTSYN_FIVE];
            lUserDataSubTLV_pst->CRC_UserData_u8             = lDataPtr_pu8[ETHTSYN_SIX];
            (*MsgLen_pu16)                                  += ETHTSYN_USERDATA_LENGTH;
        }
        else
        {
            /* If length is not validated for UserData subTLV, skip the sub TLV */
            (*MsgLen_pu16) += lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO;
        }
    }
    else
    {
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}

#if  0
/*OFS not released*/
/**
 ***************************************************************************************************
 * \module description
 * Reads OFS sub TLV frames
 *
 * Parameter In:
 * \param DataPtr_pu8   Pointer to the sub TLV
 * \param LenTot_u16    Total length of the Offset Sub TLV
 *
 * Parameter InOut:
 * \param MsgLen_pu16   Length of sub TLVs until this sub TLV
 *
 * \return  None
 *
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ReadOFSSubTLV( const uint8  *DataPtr_pu8,
                                                   uint16 *MsgLen_pu16,
                                                   uint16  LenTot_u16 )
{
    /* Local Variable declaration */
    EthTSyn_OFSSubTLV_tst *lOFSSubTLV_pst;
    const uint8           *lDataPtr_pu8;
    Std_ReturnType         lRetVal_en;

    /*local variable initialisation */
    lRetVal_en = E_OK;

    /* let lDataPtr_pu8 point to beginning of the sub TLV */
    lDataPtr_pu8 = &DataPtr_pu8[(*MsgLen_pu16)];

    /* Place a length check to see that the sub TLV doesn't cross the total length limit */
    if( LenTot_u16 >= ( ( *MsgLen_pu16 ) + lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO ) )
    {
        /*Place a length check to confirm that the received sub TLV is of valid length*/
        /*Write into sub TLV datastructure only for the sixteen OFS sub TLVs received */
        if( ( ETHTSYN_SEVENTEEN == lDataPtr_pu8[ETHTSYN_ONE] ) &&  ( ETHTSYN_SIXTEEN < EthTSyn_RxFollowup_st.SubTLV_st.nrOFSSubTLV_u32 ) )
        {
            /* Read Type, OfsTimeDomain, OfsTimeSec, OfsTimeNSec, Status,
             * UserDataLength, UserByte_0, UserByte_1, UserByte_2 and CRC_UserData into datastructure*/
            lOFSSubTLV_pst                              = &EthTSyn_RxFollowup_st.SubTLV_st.OFSSubTLV_st[EthTSyn_RxFollowup_st.SubTLV_st.nrOFSSubTLV_u32];
            lOFSSubTLV_pst->Type_u8                     = lDataPtr_pu8[ETHTSYN_ZERO];
            lOFSSubTLV_pst->OFS_TimeDomain_u8           = lDataPtr_pu8[ETHTSYN_TWO];
            lOFSSubTLV_pst->OFS_Time_st.secondsHi       = ( ( uint16 )( ( ( uint16 )lDataPtr_pu8[ETHTSYN_THREE] ) << ETHTSYN_EIGHT  )
                                                                                  | lDataPtr_pu8[ETHTSYN_FOUR] );
            lOFSSubTLV_pst->OFS_Time_st.seconds         = (uint32)( ( ( uint32 )lDataPtr_pu8[ETHTSYN_FIVE] << ETHTSYN_TWENTYFOUR )
                                                                  | ( ( uint32 )lDataPtr_pu8[ETHTSYN_SIX] << ETHTSYN_SIXTEEN )
                                                                  | ( ( uint32 )lDataPtr_pu8[ETHTSYN_SEVEN] << ETHTSYN_EIGHT )
                                                                  | lDataPtr_pu8[ETHTSYN_EIGHT] );
            lOFSSubTLV_pst->OFS_Time_st.nanoseconds     = (uint32)( ( ( uint32 )lDataPtr_pu8[ETHTSYN_NINE] << ETHTSYN_TWENTYFOUR )
                                                                  | ( ( uint32 )lDataPtr_pu8[ETHTSYN_TEN] << ETHTSYN_SIXTEEN )
                                                                  | ( ( uint32 )lDataPtr_pu8[ETHTSYN_ELEVEN] << ETHTSYN_EIGHT )
                                                                  | lDataPtr_pu8[ETHTSYN_TWELVE] );
            lOFSSubTLV_pst->Status_u8                   = lDataPtr_pu8[ETHTSYN_THIRTEEN];
            lOFSSubTLV_pst->UserData_st.userDataLength  = lDataPtr_pu8[ETHTSYN_FOURTEEN];
            lOFSSubTLV_pst->UserData_st.userByte0       = lDataPtr_pu8[ETHTSYN_FIFTEEN];
            lOFSSubTLV_pst->UserData_st.userByte1       = lDataPtr_pu8[ETHTSYN_SIXTEEN];
            lOFSSubTLV_pst->UserData_st.userByte2       = lDataPtr_pu8[ETHTSYN_SEVENTEEN];
            lOFSSubTLV_pst->CRC_OFS_u8                  = lDataPtr_pu8[ETHTSYN_EIGHTEEN];
            (*MsgLen_pu16)                             += ETHTSYN_OFS_LENGTH;
            EthTSyn_RxFollowup_st.SubTLV_st.nrOFSSubTLV_u32++;
        }
        else
        {
            /* If length is not validated for OFS subTLV, skip the sub TLV */
            (*MsgLen_pu16)  += lDataPtr_pu8[ETHTSYN_ONE] + ETHTSYN_TWO;
        }
    }
    else
    {
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}
#endif
#  endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReceiveSync()
 *
 * \Function Description
 * Handles received Sync message
 *
 * Parameters (in) :
 * \param DataPtr_pu8    - Pointer to the received data
 * \param TimeStamp_pst  - Pointer to current time stamp
 * \param MsgLen_u16     - Length of Sync message
 * \param LenByte_u16    - Length of received data
 * \param IdxDomain_u8   - Index of the current configured Global Time Domain
 * \param IdxPort_u8     - Index of the received port index
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
void EthTSyn_ReceiveSync( const uint8             *DataPtr_pu8,
                          const Eth_TimeStampType *TimeStamp_pst,
                                uint16             MsgLen_u16,
                                uint16             LenByte_u16,
                                uint8              IdxDomain_u8,
                                uint8              IdxPort_u8 )
{
    /* Local Variable declaration */
    uint32         lSync_SyncFolwUpTout_u32;
    uint8          lIdxGlbTimSlv_u8;
# if ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON )
    Std_ReturnType lRetVal_en;
# endif

    if( NULL_PTR == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst )
    {
        /* Return to the calling context if Time Slave is not configured for the passed Domain Index*/
        return;
    }

    /* Index of the current global time slave */
    lIdxGlbTimSlv_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[IdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;

    /* Local Variable Initialisation */
    lSync_SyncFolwUpTout_u32 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[IdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst->Sync_SyncFolwUpTout_u32;
# if ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON )
    lRetVal_en = E_NOT_OK;
# endif

    /* The Time Domain matches to one of the configured Time Domains */
    if( ( DataPtr_pu8[4]        == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].GlbTimeDomId_u8 )
    &&  ( ( ETHTSYN_SYNC_INIT_E == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en ) || ( ETHTSYN_SYNC_PREP_E  == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en ) )
    &&  ( ETHTSYN_SYNC_MSG_LEN  == MsgLen_u16 )
    &&  ( LenByte_u16           >= MsgLen_u16 ) )
    {
# if ( ETHTSYN_MASTER_SLAVE_CONFLICT_DETECTION == STD_ON )
        /* Function call to do the DET check */
        lRetVal_en = EthTSyn_ReceiveSyncDetChk( DataPtr_pu8, lIdxGlbTimSlv_u8 );

        /* Condition checks: If there is no DET error when DET compiler macro is switched On. */
        if( E_OK == lRetVal_en  )
# endif
        {
            /* Assign the TimeStamp obtained from function parameter to correct datastructures based on Hardware Time Stamp or Software Time Stamp */
            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].SyncIngressTimeStamp_st = *TimeStamp_pst;

            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_PREP_E;

            /* TRACE[SWS_EthTSyn_00025]: A value of ZERO deactivates Sync_FollowUp timeout observation */
            /* Initialize Main Function Sync timer with Follow-Up TimeOut value if timeout observation is enabled */
            if( ETHTSYN_ZERO != lSync_SyncFolwUpTout_u32 )
            {
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = lSync_SyncFolwUpTout_u32;
            }

            /* Store the SeqId of the received Sync message */
            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxSyncSeqId_u16 = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[30] ) << 8U  ) | DataPtr_pu8[31] );

# if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
            /* Copy Sync message to global buffer */
            /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
            (void)rba_BswSrv_MemCopy( &EthTSyn_TimBrdg_st.TxSyncBuf_au8[ETHTSYN_ZERO], &DataPtr_pu8[ETHTSYN_ZERO], ETHTSYN_SYNC_MSG_LEN );

            /* Set Sync message Received variable to Message Received */
            EthTSyn_TimBrdg_st.SyncMsgRxd_u8 = ETHTSYN_MSG_RXD;
# endif
        }
    }
# if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
    else
    {
        EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
    }
# endif
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ReceiveFollowUp()
 *
 * \Function Description
 * Handles received Follow_Up message
 *
 * Parameters (in) :
 * \param DataPtr_pu8    - Pointer to the received data
 * \param MsgLen_u16     - Length of Follow-Up message
 * \param LenByte_u16    - Length of received data
 * \param IdxDomain_u8   - Index of the current configured Global Time Domain
 * \param IdxPort_u8     - Index of the received port index
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
void EthTSyn_ReceiveFollowUp( const uint8  *DataPtr_pu8,
                                    uint16  MsgLen_u16,
                                    uint16  LenByte_u16,
                                    uint8   IdxDomain_u8,
                                    uint8   IdxPort_u8 )
{
    /* Local Variable declaration */
    uint16                   lFollowUpSeqId_u16;
    EthTSyn_FollowUpMsg_tst *lRxFollowup_pst;
    uint8                    lIdxGlbTimSlv_u8;
# if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
    const uint8             *lDataPtr_pu8;
    uint16                   lLen_u16;
    uint16                   lTotalLen_u16;
    Std_ReturnType           lRetVal_en;
# endif
# if( ( ETHTSYN_TIME_BRIDGE_SM == STD_ON ) && !defined( ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME ) )
    uint8                    lMstPortNum_u8;
# endif

    if ( NULL_PTR == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[IdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst )
    {
        /* Return to the calling context if Time Slave is not configured for the passed Domain Index*/
        return;
    }
    /* Index of the current global time slave */
    lIdxGlbTimSlv_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[IdxPort_u8].PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;
    /* Local Variable Initialisation */
    lRxFollowup_pst  = &EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st;
# if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
    lLen_u16         = ETHTSYN_ZERO;
    lRetVal_en       = E_OK;
# endif

    /* The Time Domain matches to one of the configured Time Domains */
    if( ( DataPtr_pu8[4]       == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].GlbTimeDomId_u8 )
    &&  ( ETHTSYN_SYNC_MSG_LEN <= MsgLen_u16 )
    &&  ( LenByte_u16          >= MsgLen_u16 ) )
    {
        /* Extract the SeqId of received Follow_Up message */
        lFollowUpSeqId_u16 = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[30] ) << 8U  ) | ( DataPtr_pu8[31] ) );
        if( ETHTSYN_SYNC_PREP_E == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en )
        {
            /* Check the state machine and if the SeqIds match and retrieve the timestamp received*/
            if( lFollowUpSeqId_u16 == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxSyncSeqId_u16 )
            {
                EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_FOLLOW_UP_E;
                lRxFollowup_pst->CorrectionField_u64     = (uint64)( ( ( uint64 )DataPtr_pu8[ETHTSYN_EIGHT] << ETHTSYN_FIFTYSIX ) | ( ( uint64 )DataPtr_pu8[ETHTSYN_NINE] << ETHTSYN_FORTYEIGHT )
                        | ( ( uint64 )DataPtr_pu8[ETHTSYN_TEN] << ETHTSYN_FORTY ) | ( ( uint64 )DataPtr_pu8[ETHTSYN_ELEVEN] << ETHTSYN_THIRTYTWO )
                        | ( ( uint64 )DataPtr_pu8[ETHTSYN_TWELVE] << ETHTSYN_TWENTYFOUR )| ( ( uint64 )DataPtr_pu8[ETHTSYN_THIRTEEN] << ETHTSYN_SIXTEEN )
                        | ( ( uint64 )DataPtr_pu8[ETHTSYN_FOURTEEN] <<ETHTSYN_EIGHT ) | DataPtr_pu8[ETHTSYN_FIFTEEN] );
                lRxFollowup_pst->PreciseOriginTimeStamp_st.secondsHi   = ( ( uint16 )( ( ( uint16 )DataPtr_pu8[34] ) << 8U  ) | ( DataPtr_pu8[35] ) );
                lRxFollowup_pst->PreciseOriginTimeStamp_st.seconds     = (uint32)( ( ( uint32 )DataPtr_pu8[36] << 24U ) | ( ( uint32 )DataPtr_pu8[37] << 16U ) | ( ( uint32 )DataPtr_pu8[38] <<8U ) | DataPtr_pu8[39] );
                lRxFollowup_pst->PreciseOriginTimeStamp_st.nanoseconds = (uint32)( ( ( uint32 )DataPtr_pu8[40] << 24U ) | ( ( uint32 )DataPtr_pu8[41] << 16U ) | ( ( uint32 )DataPtr_pu8[42] <<8U ) | DataPtr_pu8[43] );

# if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
                lRxFollowup_pst->TLVHdr_b = FALSE;
#  if 0
                /*OFS not released*/
                lRxFollowup_pst->SubTLV_st.nrOFSSubTLV_u32 = ETHTSYN_ZERO;
#  endif

                /* Check that TLV header is not empty and that type is equal to three*/
                if( ( LenByte_u16   >  ETHTSYN_EIGHTYSIX )
                &&  ( ETHTSYN_ZERO  == DataPtr_pu8[ETHTSYN_SEVENTYSIX] )
                &&  ( ETHTSYN_THREE == DataPtr_pu8[ETHTSYN_SEVENTYSEVEN] )
                &&  ( ETHTSYN_SIX   < ( ( (uint16) ( ( uint16 )DataPtr_pu8[ETHTSYN_SEVENTYEIGHT] << ETHTSYN_EIGHT ) ) | DataPtr_pu8[ETHTSYN_SEVENTYNINE] ) ) )
                {
                    /* Initialise the flag to TRUE to say that TLV header exists */
                    lRxFollowup_pst->TLVHdr_b = TRUE;

                    /*Length of sub TLVs */
                    lTotalLen_u16 = ( ( ( uint16 ) ( ( uint16 )DataPtr_pu8[ETHTSYN_SEVENTYEIGHT] << ETHTSYN_EIGHT ) ) | DataPtr_pu8[ETHTSYN_SEVENTYNINE] )-ETHTSYN_SIX;

                    lDataPtr_pu8 = &DataPtr_pu8[ETHTYSN_LENGTH_TILL_TLV_HEADER];

                    /* In while loop, parse all sub TLVs received using switch-case */
                    while( lTotalLen_u16 > lLen_u16 )
                    {
                        switch( lDataPtr_pu8[lLen_u16] )
                        {
                            /* Case when received Follow-Up frame contains a Time Sub-TLV */
                            case ETHTSYN_TIME_WITH_CRC:
                                lRetVal_en = EthTSyn_ReadTimeSubTLV( DataPtr_pu8, lDataPtr_pu8, &lLen_u16, lTotalLen_u16, lIdxGlbTimSlv_u8 );
                                break;
                                /* Case when received Follow-Up frame contains a Status Sub-TLV */
                            case ETHTSYN_STATUS_WITH_CRC:
                            case ETHTSYN_STATUS_WITHOUT_CRC:
                                lRetVal_en = EthTSyn_ReadStatusSubTLV( lDataPtr_pu8, &lLen_u16, lTotalLen_u16, lIdxGlbTimSlv_u8 );
                                break;
                                /* Case when received Follow-Up frame contains a UserData Sub-TLV */
                            case ETHTSYN_USERDATA_WITH_CRC:
                            case ETHTSYN_USERDATA_WITHOUT_CRC:
                                lRetVal_en = EthTSyn_ReadUserDataSubTLV( lDataPtr_pu8, &lLen_u16, lTotalLen_u16, lIdxGlbTimSlv_u8 );
                                break;
#  if 0
                                /*OFS not released*/
                                /* Case when received Follow-Up frame contains an Offset Sub-TLV */
                            case ETHTSYN_OFS_WITH_CRC:
                            case ETHTSYN_OFS_WITHOUT_CRC:
                                lRetVal_en = EthTSyn_ReadOFSSubTLV( lDataPtr_pu8, &lLen_u16, lTotalLen_u16 );
                                break;
#  endif
                            default:
                                /* If the Type of Sub-TLV is unknown, skip the sub TLV */
                                lLen_u16 += lDataPtr_pu8[lLen_u16 + ETHTSYN_ONE] + ETHTSYN_TWO;
                                break;
                        }
                        /* If length check fails, re-Initialise the State machine and main fuction timer */
                        if( E_NOT_OK == lRetVal_en )
                        {
                            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en                                  = ETHTSYN_SYNC_INIT_E;
                            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]   = ETHTSYN_ALLFFL;
#  if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
                            EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;
#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                            for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
                            {
                                /* Set the owner of EthSwt Management object back to unused */
                                EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership   = ETHSWT_MGMT_OBJ_UNUSED;
                            }
#   endif

#   if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
                            /* Set the owner of EthSwt Management object back to unused */
                            EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
#   endif
#  endif
                            break;
                        } /* If return value is E_NOT_OK */
                    } /* End of While loop */
                }
                else
                {
                    /* Do Nothing */
                }

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )

                /* Assign the length of Fup frame to a variable in datastructure */
                EthTSyn_TimBrdg_st.SyncFupLen_u16   = MsgLen_u16;

                /* Copy FollowUp message to global buffer */
                /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
                (void)rba_BswSrv_MemCopy( &EthTSyn_TimBrdg_st.TxFollowUpBuf_au8[ETHTSYN_ZERO], &DataPtr_pu8[ETHTSYN_ZERO], MsgLen_u16 );
#  endif
# endif
            }
            /* If Slave state machine is not in PREP state and Sequence IDs do not match, Re-Initialise the State machine and main fuction timer */
            else
            {
                EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en                                  = ETHTSYN_SYNC_INIT_E;
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]   = ETHTSYN_ALLFFL;

# if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
                EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;

#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
                {
                    /* Set the owner of EthSwt Management object back to unused */
                    EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership   = ETHSWT_MGMT_OBJ_UNUSED;
                }
#  endif

# endif

# if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
                /* Set the owner of EthSwt Management object back to unused */
                EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
# endif
            }
        }
        else
        {
            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en                                  = ETHTSYN_SYNC_INIT_E;
            EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]   = ETHTSYN_ALLFFL;

# if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
            EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;
# endif
        }
    }
    /* If the time domains do not match, re-Initialise the State machine and main fuction timer */
    else
    {
        EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en                                  = ETHTSYN_SYNC_INIT_E;
        EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]   = ETHTSYN_ALLFFL;

# if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
        EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;

#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
        for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
        {
            /* Set the owner of EthSwt Management object back to unused */
            EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership   = ETHSWT_MGMT_OBJ_UNUSED;
        }
#  endif

# endif

# if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
        /* Set the owner of EthSwt Management object back to unused */
        EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
# endif
    }
}

#  if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ValidateTimeCRCZero()
 *
 * \Function Description
 * Validates CRC0 for AUTOSAR time sub TLVs
 *
 * Parameters (in) :
 * \param DataId_u8         - DataIdValue considered for CRC
 * \param IdxDomain_u8      - Index of the current configured Global Time Domain
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ValidateTimeCRCZero ( uint8 DataId_u8,
                                                    uint8 IdxDomain_u8,
                                                    uint8 IdxGlbTimSlv_u8)
{
    /* Local variable declaration */
    const EthTSyn_GlbTimeSlave_tst *lGlbTimeSlave_pcst;
    EthTSyn_TimeSubTLV_tst         *lTimeSubTLV_pst;
    Eth_TimeStampType              *lPreciseOriginTimeStamp_pst;
    EthTSyn_RxCrcValidated_ten      lRxCrcValidated_en;
    Std_ReturnType                  lRetVal_en;
    uint8                           lLocalbuf_au8[ETHTSYN_TWENTYTHREE];
    uint8                           lCrcFlagsRxValidated_u8;
    uint8                           lLen_u8;
    uint8                           lCRCVal_u8;
    uint8                           lIdxSlaveCfg_u8;

    /* Local variable Initialisation */
    lTimeSubTLV_pst             = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.TimeSubTLV_st;
    lPreciseOriginTimeStamp_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.PreciseOriginTimeStamp_st;
    lIdxSlaveCfg_u8             = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lGlbTimeSlave_pcst          = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst;
    lRxCrcValidated_en          = lGlbTimeSlave_pcst->RxCrcValidated_en;
    lCrcFlagsRxValidated_u8     = lGlbTimeSlave_pcst->CrcFlagsRxValidated_u8;
    lRetVal_en                  = E_OK;
    lLen_u8                     = ETHTSYN_ZERO;

    /* If time sub TLV is CRC is supported */
    if( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
    {
        /* Read CRC_Time_Flags into data block */
        lLocalbuf_au8[ETHTSYN_ZERO] = lTimeSubTLV_pst->CRC_TimeFlags_u8;
        lLen_u8++;

        /* Read domain number if flag for domain number is set */
        if ( ETHTSYN_SUBTLV_DOMAIN_NUMBER == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_DOMAIN_NUMBER )  )
        {
            lLocalbuf_au8[lLen_u8]  = lTimeSubTLV_pst->domnum_u8;
            lLen_u8++;
        }

        /* Read sourcePortIdentity if flag for sourcePortIdentity is set */
        if ( ETHTSYN_SUBTLV_SOURCEPORT_IDENTITY == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_SOURCEPORT_IDENTITY ) )
        {
            lLocalbuf_au8[lLen_u8]                = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_ZERO];
            lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_ONE];
            lLocalbuf_au8[lLen_u8+ETHTSYN_TWO]    = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_TWO];
            lLocalbuf_au8[lLen_u8+ETHTSYN_THREE]  = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_THREE];
            lLocalbuf_au8[lLen_u8+ETHTSYN_FOUR]   = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_FOUR];
            lLocalbuf_au8[lLen_u8+ETHTSYN_FIVE]   = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_FIVE];
            lLocalbuf_au8[lLen_u8+ETHTSYN_SIX]    = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_SIX];
            lLocalbuf_au8[lLen_u8+ETHTSYN_SEVEN]  = lTimeSubTLV_pst->SourcePortIdentity_st.ClockIdentity_au8[ETHTSYN_SEVEN];
            lLocalbuf_au8[lLen_u8+ETHTSYN_EIGHT]  = (uint8)( ( lTimeSubTLV_pst->SourcePortIdentity_st.PortNumber_u16 & 0xFF00U ) >> ETHTSYN_EIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_NINE]   = (uint8)( lTimeSubTLV_pst->SourcePortIdentity_st.PortNumber_u16 & 0x00FFU );
            lLen_u8                              += ETHTSYN_TEN;
        }

        /* Read preciseOriginTimestamp if flag for preciseOriginTimestamp is set */
        if ( ETHTSYN_SUBTLV_PRECISE_ORIGIN_TIMESTAMP == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_PRECISE_ORIGIN_TIMESTAMP ) )
        {
            lLocalbuf_au8[lLen_u8]                = (uint8)( ( lPreciseOriginTimeStamp_pst->secondsHi & 0xFF00U ) >> ETHTSYN_EIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = (uint8)( lPreciseOriginTimeStamp_pst->secondsHi & 0x00FFU );
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
        lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), lLen_u8, ETHTSYN_CRC_START_VALUE, TRUE);

        if( lCRCVal_u8 != lTimeSubTLV_pst->CRC_Time0_u8 )
        {
            lRetVal_en = E_NOT_OK;
        }
    }
    return lRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ValidateTimeCRCOne()
 *
 * \Function Description
 * Validates CRC1 for AUTOSAR time sub TLVs
 *
 * Parameters (in) :
 * \param DataId_u8         - DataIdValue considered for CRC
 * \param IdxDomain_u8      - Index of the current configured Global Time Domain
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */

static Std_ReturnType EthTSyn_ValidateTimeCRCOne ( uint8 DataId_u8,
                                                   uint8 IdxDomain_u8,
                                                   uint8 IdxGlbTimSlv_u8 )
{
    /* Local variable declaration */
    const EthTSyn_GlbTimeSlave_tst *lGlbTimeSlave_pcst;
    EthTSyn_TimeSubTLV_tst         *lTimeSubTLV_pst;
    EthTSyn_RxCrcValidated_ten      lRxCrcValidated_en;
    Std_ReturnType                  lRetVal_en;
    uint64                          lCorrectionField_u64;
    uint8                           lLocalbuf_au8[ETHTSYN_TWENTYTHREE];
    uint8                           lCrcFlagsRxValidated_u8;
    uint8                           lLen_u8;
    uint8                           lCRCVal_u8;
    uint8                           lIdxSlaveCfg_u8;

    /* Local variable Initialisation */
    lTimeSubTLV_pst         = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.TimeSubTLV_st;
    lCorrectionField_u64    = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.CorrectionField_u64;
    lIdxSlaveCfg_u8         = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lGlbTimeSlave_pcst      = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst;
    lRxCrcValidated_en      = lGlbTimeSlave_pcst->RxCrcValidated_en;
    lCrcFlagsRxValidated_u8 = lGlbTimeSlave_pcst->CrcFlagsRxValidated_u8;
    lRetVal_en              = E_OK;
    lLen_u8                 = ETHTSYN_ZERO;

    /* If time sub TLV is CRC is supported */
    if( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
    {
        /* Read CRC_Time_Flags into data block */
        lLocalbuf_au8[ETHTSYN_ZERO] = lTimeSubTLV_pst->CRC_TimeFlags_u8;
        lLen_u8++;

        /* Read domainNumber if flag for domainNumber is set */
        if ( ETHTSYN_SUBTLV_MESSAGE_LENGTH == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_MESSAGE_LENGTH ) )
        {
            lLocalbuf_au8[lLen_u8]             = (uint8)( ( lTimeSubTLV_pst->Msglen_u16 &0xFF00U ) >> ETHTSYN_EIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_ONE] = (uint8)( lTimeSubTLV_pst->Msglen_u16 &0x00FFU );
            lLen_u8                           += ETHTSYN_TWO;
        }

        /* Read correctionField if flag for correctionField is set */
        if ( ETHTSYN_SUBTLV_CORRECTION_FIELD == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_CORRECTION_FIELD ) )
        {
            lLocalbuf_au8[lLen_u8]                = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_EIGHTH_BYTE_MASK ) >> ETHTSYN_FIFTYSIX );
            lLocalbuf_au8[lLen_u8+ETHTSYN_ONE]    = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SEVENTH_BYTE_MASK ) >> ETHTSYN_FORTYEIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_TWO]    = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SIXTH_BYTE_MASK ) >> ETHTSYN_FORTY );
            lLocalbuf_au8[lLen_u8+ETHTSYN_THREE]  = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIFTH_BYTE_MASK ) >> ETHTSYN_THIRTYTWO );
            lLocalbuf_au8[lLen_u8+ETHTSYN_FOUR]   = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FOURTH_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
            lLocalbuf_au8[lLen_u8+ETHTSYN_FIVE]   = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_THIRD_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
            lLocalbuf_au8[lLen_u8+ETHTSYN_SIX]    = (uint8)( ( lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_SECOND_BYTE_MASK ) >> ETHTSYN_EIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_SEVEN]  = (uint8)(   lCorrectionField_u64 & ETHTSYN_DOUBLE_WORD_FIRST_BYTE_MASK );
            lLen_u8                              += ETHTSYN_EIGHT;
        }

        /* Read sequenceId if flag for sequenceId is set */
        if ( ETHTSYN_SUBTLV_SEQUENCE_ID == ( lCrcFlagsRxValidated_u8 & ETHTSYN_SUBTLV_SEQUENCE_ID ) )
        {
            lLocalbuf_au8[lLen_u8]             = (uint8)( ( lTimeSubTLV_pst->SequenceId_u16 &0xFF00U ) >> ETHTSYN_EIGHT );
            lLocalbuf_au8[lLen_u8+ETHTSYN_ONE] = (uint8)( lTimeSubTLV_pst->SequenceId_u16 &0x00FFU );
            lLen_u8                           += ETHTSYN_TWO;
        }

        /* Read dataIDValue */
        lLocalbuf_au8[lLen_u8] = DataId_u8;
        lLen_u8++;

        /* Calculate CRC */
        lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), lLen_u8, ETHTSYN_CRC_START_VALUE, TRUE );

        if( lCRCVal_u8 != lTimeSubTLV_pst->CRC_Time1_u8 )
        {
            lRetVal_en = E_NOT_OK;
        }
    }
    return lRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ValidateStatusCRC()
 *
 * \Function Description
 * Validates CRC for status sub TLV
 *
 * Parameters (in) :
 * \param DataId_u8         - DataIdValue considered for CRC
 * \param IdxDomain_u8      - Index of the current configured Global Time Domain
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ValidateStatusCRC( uint8 DataId_u8,
                                                 uint8 IdxDomain_u8,
                                                 uint8 IdxGlbTimSlv_u8 )
{
    /* Local variable declaration */
    EthTSyn_StatusSubTLV_tst   *lStatusSubTLV_pst;
    EthTSyn_RxCrcValidated_ten  lRxCrcValidated_en;
    Std_ReturnType              lRetVal_en;
    uint8                       lLocalbuf_au8[ETHTSYN_TWO];
    uint8                       lCRCVal_u8;
    uint8                       lIdxSlaveCfg_u8;

    /* Local variable Initialisation */
    lStatusSubTLV_pst   = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.StatusSubTLV_st;
    lIdxSlaveCfg_u8     = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lRxCrcValidated_en  = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst->RxCrcValidated_en;
    lRetVal_en          = E_OK;

    /* If Status sub TLV is CRC is supported */
    if( ( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
    &&  ( ETHTSYN_STATUS_WITH_CRC == lStatusSubTLV_pst->Type_u8 ) )
    {
        /* Make a datablock of status and DataId */
        lLocalbuf_au8[ETHTSYN_ZERO]   = lStatusSubTLV_pst->Status_u8;
        lLocalbuf_au8[ETHTSYN_ONE]    = DataId_u8;

        /*Calculate CRC*/
        lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), ETHTSYN_TWO, ETHTSYN_CRC_START_VALUE, TRUE);

        /* If CRC is validated, use the status field information from sub TLV */
        if( lCRCVal_u8 == lStatusSubTLV_pst->CRC_Status_u8  )
        {
            lStatusSubTLV_pst->isRead_b = TRUE;
        }
        /* If CRC is not validated, reject the entire followUp frame */
        else
        {
            lRetVal_en = E_NOT_OK;
        }
    }
    /* If Status sub TLV is not CRC supported */
    else if( ( ETHTSYN_CRC_IGNORED  == lRxCrcValidated_en )
    ||       ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en )
    ||       ( ( ETHTSYN_CRC_NOT_VALIDATED == lRxCrcValidated_en ) && ( ETHTSYN_STATUS_WITHOUT_CRC == lStatusSubTLV_pst->Type_u8 ) ) )
    {
        lStatusSubTLV_pst->isRead_b = TRUE;
    }
    else
    {
        /* Reject the entire followUp frame */
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ValidateUserDataCRC()
 *
 * \Function Description
 * Validates CRC for UserData sub TLV
 *
 * Parameters (in) :
 * \param DataId_u8         - DataIdValue considered for CRC
 * \param IdxDomain_u8      - Index of the current configured Global Time Domain
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ValidateUserDataCRC( uint8 DataId_u8,
                                                   uint8 IdxDomain_u8,
                                                   uint8 IdxGlbTimSlv_u8 )
{
    /* Local variable declaration */
    EthTSyn_UserDataSubTLV_tst  *lUserDataSubTLV_pst;
    EthTSyn_RxCrcValidated_ten   lRxCrcValidated_en;
    Std_ReturnType               lRetVal_en;
    uint8                        lLocalbuf_au8[ETHTSYN_FIVE];
    uint8                        lCRCVal_u8;
    uint8                        lIdxSlaveCfg_u8;

    /* Local variable Initialisation */
    lUserDataSubTLV_pst = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st.UserDataSubTLV_st;
    lIdxSlaveCfg_u8     = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lRxCrcValidated_en  = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst->RxCrcValidated_en;
    lRetVal_en          = E_OK;

    /* If UserData sub TLV is CRC is supported */
    if( ( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
    &&  ( ETHTSYN_USERDATA_WITH_CRC == lUserDataSubTLV_pst->Type_u8 ) )
    {
        /* Make a datablock using UserDataLength, UserByte0, UserByte1, UserByte2 */
        lLocalbuf_au8[ETHTSYN_ZERO]   = lUserDataSubTLV_pst->UserData_st.userDataLength;
        lLocalbuf_au8[ETHTSYN_ONE]    = lUserDataSubTLV_pst->UserData_st.userByte0;
        lLocalbuf_au8[ETHTSYN_TWO]    = lUserDataSubTLV_pst->UserData_st.userByte1;
        lLocalbuf_au8[ETHTSYN_THREE]  = lUserDataSubTLV_pst->UserData_st.userByte2;
        lLocalbuf_au8[ETHTSYN_FOUR]   = DataId_u8;

        /*Calculate CRC*/
        lCRCVal_u8 = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), ETHTSYN_FIVE, ETHTSYN_CRC_START_VALUE, TRUE);

        if( lCRCVal_u8 == lUserDataSubTLV_pst->CRC_UserData_u8 )
        {
            lUserDataSubTLV_pst->isRead_b = TRUE;
        }
        else
        {
            lRetVal_en  = E_NOT_OK;
        }
    }
    /* If UserData sub TLV is not CRC supported */
    else if( ( ETHTSYN_CRC_IGNORED  == lRxCrcValidated_en )
    ||       ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en )
    ||       ( ( ETHTSYN_CRC_NOT_VALIDATED == lRxCrcValidated_en ) && ( ETHTSYN_USERDATA_WITHOUT_CRC == lUserDataSubTLV_pst->Type_u8 ) ) )
    {
        lUserDataSubTLV_pst->isRead_b = TRUE;
    }
    else
    {
        /* Reject the entire followUp frame */
        lRetVal_en = E_NOT_OK;
    }
    return lRetVal_en;
}

#if 0
/*OFS not released*/
/**
 ***************************************************************************************************
 * \module description
 * Validates CRC for OFS sub TLVs
 *
 * Parameter In:
 * \param DataId_u8     DataIdValue considered for CRC
 * \param IdxDomain_u8  Stores the value of domain index to be used
 *
 * \return  lRetVal_en
 *
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ValidateOFSCRC( uint8 DataId_u8,
                                              uint8 IdxDomain_u8 )
{
    /* Local variable declaration */
    EthTSyn_SubTLV_tst         *lSubTLV_pst;
    EthTSyn_OFSSubTLV_tst      *lOFSSubTLV_pst;
    StbM_TimeStampType          lOFSStbMTimeStamp_st;
    StbM_UserDataType           lOFSUserData_st;
    EthTSyn_RxCrcValidated_ten  lRxCrcValidated_en;
    Std_ReturnType              lRetVal_en;
    uint8                       lLocalbuf_au8[ETHTSYN_SEVENTEEN];
    uint8                       lCRCVal_u8;
    uint8                       lOFSnum_u8;

    /* Local variable Initialisation */
    lSubTLV_pst         = &EthTSyn_RxFollowup_st.SubTLV_st;
    lRxCrcValidated_en  = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst->RxCrcValidated_en;
    lRetVal_en          = E_OK;
    lOFSSubTLV_pst      = &lSubTLV_pst->OFSSubTLV_st[ETHTSYN_ZERO];

    for( lOFSnum_u8 = ETHTSYN_ZERO; ( ( lOFSnum_u8 < lSubTLV_pst->nrOFSSubTLV_u32 ) && ( E_OK == lRetVal_en ) ); lOFSnum_u8++ )
    {
        lOFSSubTLV_pst = &lSubTLV_pst->OFSSubTLV_st[lOFSnum_u8];

        /* If OFS sub TLV is CRC is supported */
        if( ( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
        &&  ( ETHTSYN_STATUS_WITH_CRC == lOFSSubTLV_pst->Type_u8 ) )
        {

            if( ( lOFSSubTLV_pst->OFS_TimeDomain_u8 > ETHTSYN_FIFTEEN ) && ( lOFSSubTLV_pst->OFS_TimeDomain_u8 == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].GlbTimeDomId_u8 )
            &&  ( lOFSSubTLV_pst->OFS_Time_st.nanoseconds <= ETHTSYN_MAX_NANOSECOND ) )
            {
                lLocalbuf_au8[ETHTSYN_ZERO]     = lOFSSubTLV_pst->OFS_TimeDomain_u8;
                lLocalbuf_au8[ETHTSYN_ONE]      = (uint8) ( lOFSSubTLV_pst->OFS_Time_st.secondsHi & 0xFF00U );
                lLocalbuf_au8[ETHTSYN_TWO]      = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.secondsHi  & 0x00FFU ) >> ETHTSYN_EIGHT );
                lLocalbuf_au8[ETHTSYN_THREE]    = (uint8) ( lOFSSubTLV_pst->OFS_Time_st.seconds & ETHTSYN_FOURTH_BYTE_MASK );
                lLocalbuf_au8[ETHTSYN_FOUR]     = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.seconds & ETHTSYN_THIRD_BYTE_MASK ) >> ETHTSYN_EIGHT );
                lLocalbuf_au8[ETHTSYN_FIVE]     = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.seconds & ETHTSYN_SECOND_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
                lLocalbuf_au8[ETHTSYN_SIX]      = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.seconds & ETHTSYN_FIRST_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
                lLocalbuf_au8[ETHTSYN_SEVEN]    = (uint8) ( lOFSSubTLV_pst->OFS_Time_st.nanoseconds & ETHTSYN_FOURTH_BYTE_MASK );
                lLocalbuf_au8[ETHTSYN_EIGHT]    = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.nanoseconds & ETHTSYN_THIRD_BYTE_MASK ) >> ETHTSYN_EIGHT );
                lLocalbuf_au8[ETHTSYN_NINE]     = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.nanoseconds & ETHTSYN_SECOND_BYTE_MASK ) >> ETHTSYN_SIXTEEN );
                lLocalbuf_au8[ETHTSYN_TEN]      = (uint8) ( ( lOFSSubTLV_pst->OFS_Time_st.nanoseconds & ETHTSYN_FIRST_BYTE_MASK ) >> ETHTSYN_TWENTYFOUR );
                lLocalbuf_au8[ETHTSYN_ELEVEN]   = lOFSSubTLV_pst->Status_u8;
                lLocalbuf_au8[ETHTSYN_TWELVE]   = lOFSSubTLV_pst->UserData_st.userDataLength;
                lLocalbuf_au8[ETHTSYN_THIRTEEN] = lOFSSubTLV_pst->UserData_st.userByte0;
                lLocalbuf_au8[ETHTSYN_FOURTEEN] = lOFSSubTLV_pst->UserData_st.userByte1;
                lLocalbuf_au8[ETHTSYN_FIFTEEN]  = lOFSSubTLV_pst->UserData_st.userByte2;
                lLocalbuf_au8[ETHTSYN_SIXTEEN]  = DataId_u8;

                /*Calculate CRC*/
                lCRCVal_u8      = Crc_CalculateCRC8H2F( ( &lLocalbuf_au8[ETHTSYN_ZERO] ), ETHTSYN_SEVENTEEN, ETHTSYN_CRC_START_VALUE, TRUE);

                if( lCRCVal_u8 == lOFSSubTLV_pst->CRC_OFS_u8 )
                {
                    lOFSSubTLV_pst->isRead_b = TRUE;
                }
                else
                {
                    lRetVal_en  = E_NOT_OK;
                }
            }
            else
            {
               /* Do Nothing */
            }
        }
        else if( ( ETHTSYN_CRC_IGNORED  == lRxCrcValidated_en )
        ||       ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en )
        ||       ( ( ETHTSYN_CRC_NOT_VALIDATED == lRxCrcValidated_en ) && ( ETHTSYN_STATUS_WITHOUT_CRC == lOFSSubTLV_pst->Type_u8 ) ) )
        {
            lOFSSubTLV_pst->isRead_b = TRUE;
        }
        else
        {
            /* Do Nothing */
        }

        /* Set type to zero so OFS sub TLV datstructure is not reused */
        lOFSSubTLV_pst->Type_u8 = ETHTSYN_ZERO;
    }
    if ( E_OK == lRetVal_en )
    {
        for( lOFSnum_u8 = ETHTSYN_ZERO; lOFSnum_u8 < lSubTLV_pst->nrOFSSubTLV_u32 ; lOFSnum_u8++ )
        {
            if( TRUE == lOFSSubTLV_pst->isRead_b)
            {
                lOFSSubTLV_pst            = &lSubTLV_pst->OFSSubTLV_st[lOFSnum_u8];

                lOFSStbMTimeStamp_st.timeBaseStatus = ( lOFSSubTLV_pst->Status_u8 & ETHTSYN_STATUS_MASK ) << ETHTSYN_TWO;
                lOFSStbMTimeStamp_st.nanoseconds    = lOFSSubTLV_pst->OFS_Time_st.nanoseconds;
                lOFSStbMTimeStamp_st.seconds        = lOFSSubTLV_pst->OFS_Time_st.seconds;
                lOFSStbMTimeStamp_st.secondsHi      = lOFSSubTLV_pst->OFS_Time_st.secondsHi;

                /*Forward the OFS time to OFS time domains*/
                ( void )StbM_BusSetGlobalTime( lOFSSubTLV_pst->OFS_TimeDomain_u8, &lOFSStbMTimeStamp_st, &lOFSSubTLV_pst->UserData_st );
            }
        }
    }
    return lRetVal_en;
}
#endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_ValidateCRC()
 *
 * \Function Description
 * Validates CRC for AUTOSAR sub TLVs
 *
 * Parameters (in) :
 * \param IdxDomain_u8      - Index of the current configured Global Time Domain
 * \param IdxGlbTimSlv_u8   - Index of the Slave Data Structure currently being used
 *
 * Parameters (inout):
 * None
 *
 * Parameters (out):
 * None
 *
 * Return Value: RetVal_en
 * Return Type : Std_ReturnType
 ***************************************************************************************************
 */
static Std_ReturnType EthTSyn_ValidateCRC( uint8 IdxDomain_u8,
                                           uint8 IdxGlbTimSlv_u8 )
{
    /* Local variable declaration */
    EthTSyn_SubTLV_tst              *lSubTLV_pst;
    EthTSyn_TimeSubTLV_tst          *lTimeSubTLV_pst;
    EthTSyn_StatusSubTLV_tst        *lStatusSubTLV_pst;
    EthTSyn_UserDataSubTLV_tst      *lUserDataSubTLV_pst;
    const EthTSyn_GlbTimeSlave_tst  *lGlbTimeSlave_pst;
    Std_ReturnType                   lRetVal_en;
    uint8                            lDataIDVal_u8;
    uint8                            lIdxSlaveCfg_u8;
    EthTSyn_RxCrcValidated_ten       lRxCrcValidated_en;
# if 0
    /*OFS not released*/
    uint8                            lOFSnum_u8;
# endif

    /* Local variable Initialisation */
    lRetVal_en          = E_OK;
    lSubTLV_pst         = &EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st;
    lTimeSubTLV_pst     = &lSubTLV_pst->TimeSubTLV_st;
    lStatusSubTLV_pst   = &lSubTLV_pst->StatusSubTLV_st;
    lUserDataSubTLV_pst = &lSubTLV_pst->UserDataSubTLV_st;
    lIdxSlaveCfg_u8     = EthTSyn_Slave_ast[IdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lGlbTimeSlave_pst   = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst;
    lDataIDVal_u8       = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].FolwUpDataIDList_pcst[EthTSyn_Slave_ast[IdxGlbTimSlv_u8].RxSyncSeqId_u16 % ETHTSYN_SIXTEEN];
    lRxCrcValidated_en  = lGlbTimeSlave_pst->RxCrcValidated_en;

    /* Set all isRead parameters to FALSE before validating sub TLVs */
    lStatusSubTLV_pst->isRead_b   = FALSE;
    lUserDataSubTLV_pst->isRead_b = FALSE;

# if 0
    /*OFS not released*/
    for( lOFSnum_u8 = ETHTSYN_ZERO; lOFSnum_u8< lSubTLV_pst->nrOFSSubTLV_u32; lOFSnum_u8++ )
    {
        lSubTLV_pst->OFSSubTLV_st[lOFSnum_u8].isRead_b = FALSE;
    }
# endif


    if( ETHTSYN_ZERO != lTimeSubTLV_pst->Type_u8 )
    {
        if ( ( ETHTSYN_CRC_VALIDATED == lRxCrcValidated_en ) || ( ETHTSYN_CRC_OPTIONAL == lRxCrcValidated_en ) )
        {
            if( lGlbTimeSlave_pst->CrcFlagsRxValidated_u8 == lTimeSubTLV_pst->CRC_TimeFlags_u8 )
            {
                /* Validation for CRC_Time_0 */
                lRetVal_en  = EthTSyn_ValidateTimeCRCZero( lDataIDVal_u8, IdxDomain_u8, IdxGlbTimSlv_u8 );

                /* Validation for CRC_Time_1 */
                if( E_OK == lRetVal_en )
                {
                    lRetVal_en = EthTSyn_ValidateTimeCRCOne( lDataIDVal_u8, IdxDomain_u8, IdxGlbTimSlv_u8 );
                }
            }
            else
            {
                lRetVal_en = E_NOT_OK;
            }
        }
        else if( ETHTSYN_CRC_NOT_VALIDATED == lRxCrcValidated_en )
        {
            lRetVal_en = E_NOT_OK;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /* Validation for status sub TLV */
    if( ( E_OK == lRetVal_en ) && ( ETHTSYN_ZERO != lStatusSubTLV_pst->Type_u8 ) )
    {
        lRetVal_en = EthTSyn_ValidateStatusCRC( lDataIDVal_u8, IdxDomain_u8, IdxGlbTimSlv_u8 );
    }

    /* Validation for UserData sub TLV */
    if( ( E_OK == lRetVal_en ) && ( ETHTSYN_ZERO != lUserDataSubTLV_pst->Type_u8 ) )
    {
        lRetVal_en = EthTSyn_ValidateUserDataCRC( lDataIDVal_u8, IdxDomain_u8, IdxGlbTimSlv_u8 );
    }

#if 0
    /*OFS not released*/
    /* Validation for OFS sub TLVs */
    if( ( E_OK == lRetVal_en ) && ( ETHTSYN_ZERO != lSubTLV_pst->nrOFSSubTLV_u32 ) )
    {
        lRetVal_en = EthTSyn_ValidateOFSCRC( lDataIDVal_u8, IdxDomain_u8 );
    }
# endif

    /* Set type to zero so sub TLV datstructure is not reused */
    lTimeSubTLV_pst->Type_u8     = ETHTSYN_ZERO;
    lStatusSubTLV_pst->Type_u8   = ETHTSYN_ZERO;
    lUserDataSubTLV_pst->Type_u8 = ETHTSYN_ZERO;

#if 0
    /*OFS not released*/
    for( lOFSnum_u8 = ETHTSYN_ZERO; lOFSnum_u8< lSubTLV_pst->nrOFSSubTLV_u32; lOFSnum_u8++ )
    {
        lSubTLV_pst->OFSSubTLV_st[lOFSnum_u8].Type_u8 = ETHTSYN_ZERO;
    }
# endif

    return lRetVal_en;
}
#  endif

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_RunSyncSlaveSM()
 *
 * \Function Description
 * State transitions of EthTSyn_Sync_Slave_en SM
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
void EthTSyn_RunSyncSlaveSM( uint8 IdxDomain_u8 )
{
    /* Local variable declaration */
    Eth_TimeIntDiffType       lPreciseOriginTimeStamp_st;
    Eth_TimeIntDiffType       lSlaveTime_st;
    Eth_TimeIntDiffType       lSyncTime_st;
    Eth_TimeIntDiffType       lCorrnTime_st;
    Eth_TimeIntDiffType       lAddedTime_st;
    Eth_TimeIntDiffType       lEthTSyn_Offset_st;
    Eth_TimeStampType         lTimeStamp_st;
    uint8                     lMsgType_u8;
    uint8                     lIdxSlaveCfg_u8;
    uint8                     lIdxGlbTimSlv_u8;
# if( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
    Eth_TimeIntDiffType       lLocalTimeStamp_st;
    uint64                    lCurrentLocalTime_u64;
# else
    Eth_TimeIntDiffType       lSyncTimeStamp_st;
# endif
    StbM_TimeStampType        lStbMTimeStamp_st;
    StbM_MeasurementType      lMeasurementData_st;
    StbM_UserDataType         lUserData_st;
    StbM_VirtualLocalTimeType lVirtualLocalTime_st;
    uint32                    lSync_SyncFolwUpTout_u32;
    Std_ReturnType            lRetVal_en;
#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    EthTSyn_SubTLV_tst       *lSubTLV_pst;
#  endif
    Std_ReturnType            lResult_en;
#  if( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF )
    uint64                    lSyncRxTimeStamp_u64;
#  else
    Eth_TimeStampQualType     lTimeQual_en;
#  endif
#  ifdef ETHTSYN_SWITCH
    Eth_TimeIntDiffType       lPriorResidenceTi_st;
    Eth_TimeIntDiffType       lResidenceTi_st;
#   ifndef ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME
    EthSwt_MgmtObjectType    *lSlaveMgmtObj_pst;
#   endif
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
    boolean                   lFupInv_b;
#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
    uint8                     lMstPortNum_u8;
#    endif
#   endif
#  endif

    /* Local variable initialization*/
    if ( NULL_PTR == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst )
    {
        /* Return to the calling context if Time Slave is not configured for the passed Domain Index*/
        return;
    }
    /* Index of the current global time slave */
    lIdxGlbTimSlv_u8 = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst->PortRole_pcst->GlbTimeSlave_pcst->GlbTimSLvIdx_u8;

    lPreciseOriginTimeStamp_st.diff = EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st.PreciseOriginTimeStamp_st;
    lPreciseOriginTimeStamp_st.sign = TRUE;
    lUserData_st.userDataLength     = ETHTSYN_ZERO;
    lUserData_st.userByte0          = ETHTSYN_ZERO;
    lUserData_st.userByte1          = ETHTSYN_ZERO;
    lUserData_st.userByte2          = ETHTSYN_ZERO;
    lIdxSlaveCfg_u8                 = EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].IdxSlaveCfg_u8;
    lSync_SyncFolwUpTout_u32        = EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[lIdxSlaveCfg_u8].PortRole_pcst->GlbTimeSlave_pcst->Sync_SyncFolwUpTout_u32;
    lMeasurementData_st.pathDelay   = EthTSyn_Pdelay_ast[lIdxSlaveCfg_u8].diff.nanoseconds;
    lRetVal_en                      = E_OK;
    lMsgType_u8                     = ETHTSYN_FOLLOW_UP_TYPE;
    lTimeStamp_st.nanoseconds       = ETHTSYN_ZERO;
    lTimeStamp_st.seconds           = ETHTSYN_ZERO;
    lTimeStamp_st.secondsHi         = ETHTSYN_ZERO;
    lResult_en                      = E_NOT_OK;
#  if( ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON )
    lTimeQual_en                    = ETH_UNCERTAIN;
#  endif

#  if ( ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF )
    lSubTLV_pst         = &EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st.SubTLV_st;
#  endif

#  ifdef ETHTSYN_SWITCH
#   ifndef ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME
    lSlaveMgmtObj_pst = EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst;
#   endif
#   if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
    lFupInv_b = FALSE;
#   endif
#  endif

    /* TimeOut Observation is done only if Follow-Up TimeOut value is not Zero */
    if( ( ETHTSYN_ZERO != lSync_SyncFolwUpTout_u32 ) && ( ETHTSYN_ZERO != EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] ) )
    {
        EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX]--;
    }

    /* Check the state machine and if the SeqIds match and retrieve the timestamp received*/
    if( ( ETHTSYN_FOLLOW_UP_E == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en )
#  if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
     && ( ETHSWT_MGMT_OBJ_OWNED_BY_UPPER_LAYER == lSlaveMgmtObj_pst->Ownership )
# endif
       )
    {
#  if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
        if ( ( E_OK == lSlaveMgmtObj_pst->Validation.EgressTimestampValid )
          && ( E_OK == lSlaveMgmtObj_pst->Validation.IngressTimestampValid ) )
# endif
        {
            lStbMTimeStamp_st.timeBaseStatus    = ETHTSYN_ZERO; /* Local time base is synchronous to Global Time Master */

#  if(ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_ON)

            if ( ETHTSYN == EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].StbM_LocalTimeHardware_en )
            {
                /* The access to controller Index has to be updated with Multi-Domain as with TimeBridge same controller Index is used */
                /* When Hardware Timestamping is enabled and when StbM is referring to ETHTSYN, EthIf_GetCurrentTime is called to obtain the current time value ( T2 )  */
                lResult_en = EthIf_GetCurrentTime( EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].PortCfg_pcst[ETHTSYN_ZERO].EthIfCtrlrIdx_u8, &lTimeQual_en, &lTimeStamp_st );

                /* Store Time T2 in a Eth_TimeIntDiffType Structure */
                lLocalTimeStamp_st.diff = lTimeStamp_st;
                lLocalTimeStamp_st.sign = TRUE;

                /* Convert TimeStamp_st which is of Eth_TimeStampType to StbM_VirtualLocalTimeType that will be further used for Pdelay calculations */
                EthTSyn_CnvEthTitoHex( lLocalTimeStamp_st, ETHTSYN_ZERO, &lCurrentLocalTime_u64 );

                /* Convert and assign Current Local Time to nanoseconds low and nanoseconds high part of the StbM_VirtualLocalTimeType structure.
                 * This value of time will be sent to StbM StbM_BusSetGlobalTime() API ( T2vlt ) */
                lVirtualLocalTime_st.nanosecondsHi = ( uint32 )( ( lCurrentLocalTime_u64 & 0xFFFFFFFF00000000UL ) >> ETHTSYN_THIRTYTWO );
                lVirtualLocalTime_st.nanosecondsLo = ( uint32 )( lCurrentLocalTime_u64 & 0x00000000FFFFFFFFUL );

#   if ( ETHTSYN_DEV_ERROR_DETECT == STD_ON )
                if ( ( E_OK != lResult_en ) || ( ETH_VALID != lTimeQual_en ) )
                {
                    /* Report DET: Development Error: EthTSyn module was not initialized */
                    ( void ) Det_ReportError( ETHTSYN_MODULE_ID, ETHTSYN_INSTANCE_ID, ETHTSYN_SID_RUNSYNCSLAVESM, ETHTSYN_E_INV_TIMESTAMP );
                }
#   endif
            } /* If StbM refers to Eth free running clock */
            else
            {
                /* If StbM does not refer to Eth Clock Tme, call StbM_GetCurrentVirtualLocalTime() that gives you the current virtual local time.
                 * This value of time will be sent to StbM StbM_BusSetGlobalTime() API ( T3vlt ) */
                lResult_en = EthTSyn_GetCurrentVirtualLocalTime( &lMsgType_u8, &lVirtualLocalTime_st, &lTimeStamp_st, ETHTSYN_RECEPTION, IdxDomain_u8 );
            } /* If StbM does not refer to Eth free running clock */
#  else
            /* If Hardware timestamp support is disabled, call StbM_GetCurrentVirtualLocalTime() that gives you the current virtual local time
             * This value of time will be sent to StbM StbM_BusSetGlobalTime() API ( T2vlt ) */
            lResult_en = EthTSyn_GetCurrentVirtualLocalTime( &lMsgType_u8, &lVirtualLocalTime_st, &lTimeStamp_st, ETHTSYN_RECEPTION, IdxDomain_u8 );
#  endif

            /*If Autosar TLV is used */
#  if (ETHTSYN_MESSAGE_COMPLIANCE == STD_OFF)
            /* Check that TLV header is not empty*/
            if( FALSE!= EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st.TLVHdr_b )
            {
                /* Calculate CRC */
                lRetVal_en      = EthTSyn_ValidateCRC( IdxDomain_u8, lIdxGlbTimSlv_u8 );

                if( FALSE != lSubTLV_pst->StatusSubTLV_st.isRead_b )
                {
                    /* Read SGW from status sub TLV into SGW bit of timebaseStatus */
                    lStbMTimeStamp_st.timeBaseStatus =  ( ( lSubTLV_pst->StatusSubTLV_st.Status_u8 ) & ( ETHTSYN_STATUS_MASK ) ) << ETHTSYN_TWO;
                }

                if( FALSE != lSubTLV_pst->UserDataSubTLV_st.isRead_b )
                {
                    /* Read user data from sub TLV into local variables */
                    lUserData_st  = lSubTLV_pst->UserDataSubTLV_st.UserData_st;
                }
            }
#  endif

            if ( ( E_OK == lRetVal_en ) && ( E_OK == lResult_en ) )
            {
#  if(ETHTSYN_HARDWARE_TIMESTAMP_SUPPORT == STD_OFF)

                /* Left shift nanoseconds high position by 32 times and perform OR operation with nanoseconds low value  */
                lSyncRxTimeStamp_u64 = ( uint64 )( ( ( ( uint64 ) lVirtualLocalTime_st.nanosecondsHi ) << ETHTSYN_THIRTYTWO ) | lVirtualLocalTime_st.nanosecondsLo );

                /* Convert and store the current time in Eth_TimeIntDiffType structure */
                EthTSyn_CnvToEthTi( lSyncRxTimeStamp_u64, ETHTSYN_ZERO, &lSyncTimeStamp_st );

                lTimeStamp_st = lSyncTimeStamp_st.diff;
#  endif
                /* Obtain time difference between Master and Slave by subtracting Ingress Time from PreciseOriginTimeStamp obtained in FollowUp frame */
                EthTSyn_SubTi( &lTimeStamp_st, &EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].SyncIngressTimeStamp_st, &lEthTSyn_Offset_st );

                /* Add the offset time to Slave to make Slave time sync with Master's */
                EthTSyn_AddTi( &lPreciseOriginTimeStamp_st, &lEthTSyn_Offset_st, &lSyncTime_st );

                /* Add Path delay to the time obtained*/
                EthTSyn_AddTi( &lSyncTime_st, &EthTSyn_Pdelay_ast[EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].IdxSlaveCfg_u8], &lAddedTime_st );

#  ifdef ETHTSYN_SWITCH
                lPriorResidenceTi_st = lAddedTime_st;
#   ifdef ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME
                /* Obtain Residence Time using config parameter */
                EthTSyn_CnvToEthTi( ( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ), ETHTSYN_ZERO, &lResidenceTi_st );
#   else
                /* Obtain Residence time by subtracting egress timestamp at host port (management port) and ingress time stamp at slave port (ingress port) in switch */
                EthTSyn_SubTi( &lSlaveMgmtObj_pst->EgressTimestamp, &lSlaveMgmtObj_pst->IngressTimestamp, &lResidenceTi_st );

#   endif
#   if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
#    ifdef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                /* Assign Slave Port Residence Time to a RAM variable */
                EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveTiStamp_st      = lResidenceTi_st;
#    else
                /* Assign Slave Port Ingress TimeStamp to a RAM variable */
                EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveTiStamp_st.diff = lSlaveMgmtObj_pst->IngressTimestamp;
#    endif
#   endif
                /* Add residence time to sum of Sync Time and Pdelay Time */
                EthTSyn_AddTi( &lPriorResidenceTi_st, &lResidenceTi_st, &lAddedTime_st );
#  endif

                /* Convert correctionField to Time according to EthTimeStamp structure */
                EthTSyn_CnvToEthTi( EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].RxFollowup_st.CorrectionField_u64, ETHTSYN_SIXTEEN, &lCorrnTime_st );

                /* Add CorrectionField to the time difference */
                EthTSyn_AddTi( &lAddedTime_st, &lCorrnTime_st, &lSlaveTime_st );

                /* Assign the obtained time to a variable of type StbMTimeStamp_st */
                lStbMTimeStamp_st.nanoseconds       = lSlaveTime_st.diff.nanoseconds;
                lStbMTimeStamp_st.seconds           = lSlaveTime_st.diff.seconds;
                lStbMTimeStamp_st.secondsHi         = lSlaveTime_st.diff.secondsHi;

# ifdef ETHTSYN_DEBUG_AND_TEST
                if( ETHTSYN_DEBUG_ARRAY_SIZE > index )
                {
                    EthTSyn_StbMTimArry_ast[index] = lSlaveTime_st.diff;
                    index++;
                }
# endif

                /* Time value is calculated and forwarded to StbM by using StbM_BusSetGlobalTime */
                ( void ) StbM_BusSetGlobalTime( EthTSyn_TimeDomainCfg_pcst[IdxDomain_u8].StbM_SyncdTimeBasId_u16, &lStbMTimeStamp_st, &lUserData_st, &lMeasurementData_st, &lVirtualLocalTime_st );
            } /* If return value from is E_OK when Hardware Timestamp is enabled or disabled */
#   if ( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
            else
            {
                /* Set FUP message Received variable to Message Invalid */
                EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;

                /* Set local variable to true */
                lFupInv_b = TRUE;

#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
                {
                    if( NULL_PTR != EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst )
                    {
                        /* Set the owner of EthSwt Management object back to unused */
                        EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
                    }
                }
#    endif

            }
#   endif

            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_INIT_E;

            if( ETHTSYN_ZERO != lSync_SyncFolwUpTout_u32 )
            {
                /* Reinitialise Main Function Sync Timer to Zero after receiving Sync_FollowUp*/
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = ETHTSYN_ZERO;
            }

#  if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )
            if( FALSE == lFupInv_b )
            {
                /* Set FUP message Received variable to Message Received */
                EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_RXD;
            }
#  endif
        } /* If Ingress and Egress Timestamp values are Valid */
#  if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
        else
        {
            /* Reset the states of Slave and Time Bridge state machine */
            EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_INIT_E;

            if( ETHTSYN_ZERO != lSync_SyncFolwUpTout_u32 )
            {
                /* Reinitialise Main Function Sync Timer to Zero after receiving Sync_FollowUp*/
                EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] = ETHTSYN_ZERO;
            }

#   if( ETHTSYN_TIME_BRIDGE_SM == STD_ON )

            if( E_OK != lSlaveMgmtObj_pst->Validation.IngressTimestampValid )
            {
                /* Set FUP message Received variable to Message Invalid */
                EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;

#    ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
                {
                    if( NULL_PTR != EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst )
                    {
                        /* Set the owner of EthSwt Management object back to unused */
                        EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
                    }
                }
#    endif
            }
            else
            {
                /* Assign Slave Port Ingress TimeStamp to a RAM variable */
                EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveTiStamp_st.diff = lSlaveMgmtObj_pst->IngressTimestamp;
            }
#   endif
        }

        /* Set the owner of EthSwt Management object back to unused */
        EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
#  endif
    } /* Check the state machine and if ownership is with upper layer */
    /* Reinitialise Main Function Sync Timer to Zero and Slave state to Init after not receiving Sync_FollowUp within timeout*/
    else if( ( ETHTSYN_ZERO != lSync_SyncFolwUpTout_u32 ) && ( ETHTSYN_SYNC_PREP_E == EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en )
          && ( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[IdxDomain_u8].MainFunTimer_au32[ETHTSYN_SYNC_MAINFUN_IDX] ) )
    {
        EthTSyn_Slave_ast[lIdxGlbTimSlv_u8].Sync_Slave_en = ETHTSYN_SYNC_INIT_E;

#  if(ETHTSYN_TIME_BRIDGE_SM == STD_ON)
        /* Set FUP message Received variable to Message Invalid */
        EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_INV;

#   ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
        for( lMstPortNum_u8 = ETHTSYN_ZERO; lMstPortNum_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lMstPortNum_u8++ )
        {
            if( NULL_PTR != EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst )
            {
                /* Set the owner of EthSwt Management object back to unused */
                EthTSyn_MstMgmtObject_ast[lMstPortNum_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
            }
        }
#   endif
#  endif

#  if ( defined( ETHTSYN_SWITCH ) && !defined( ETHTSYN_RX_TO_UPLINK_SWITCH_RESIDENCE_TIME ) )
        EthTSyn_SlaveMgmtObject_ast[lIdxGlbTimSlv_u8].SlaveEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;
#  endif
    }
    else
    {
        /* Do Nothing */
    }
} /* Function End */

/**
 ***************************************************************************************************
 * \Function Name : EthTSyn_RunTimBrdgSM()
 *
 * \Function Description
 * Function for a Time Bridge to transmit Sync or FollowUp messages
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
#  if (ETHTSYN_TIME_BRIDGE_SM == STD_ON)
void EthTSyn_RunTimBrdgSM( uint8 IdxDomain_u8 )
{
    /* Local Variable declaration */
    uint8 lIdxMstPort_u8;

    if( ETHTSYN_MSG_RXD == EthTSyn_TimBrdg_st.SyncMsgRxd_u8 )
    {
        /* Call EthTSyn_Transmit with ETHTSYN_SYNC_TYPE once if message to be sent is Sync */
        EthTSyn_Transmit( ETHTSYN_SYNC_TYPE, IdxDomain_u8, ETHTSYN_ZERO );

        /* Set the state to ETHTSYN_MSG_TXT after transmitting Sync message */
        EthTSyn_TimBrdg_st.SyncMsgRxd_u8 = ETHTSYN_MSG_TXT;
    }
    else if( ( ETHTSYN_MSG_RXD == EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 ) && ( ETHTSYN_MSG_CLR == EthTSyn_TimBrdg_st.SyncMsgRxd_u8 ) )
    {
        /* Call EthTSyn_Transmit with ETHTSYN_FOLLOW_UP_TYPE in a for loop for Master Ports if message to be sent is FollowUp and TxConfirmation of Sync has been successful*/
        for( lIdxMstPort_u8 = ETHTSYN_ZERO; lIdxMstPort_u8 < ETHTSYN_NUMBER_MASTER_PORTS; lIdxMstPort_u8++ )
        {
#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
            /* Transmit the FollowUp message with proper correction field only if the ownership of the management object is given to EthTSyn */
            if( ( NULL_PTR                              != EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst )
            &&  ( ETHSWT_MGMT_OBJ_OWNED_BY_UPPER_LAYER  == EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Ownership ) )
#  endif
            {

#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
                /* Transmit FollowUp only if Egress time stamp is valid */
                if( E_OK == EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Validation.EgressTimestampValid )
#  endif
                {
                    /* Increment for every successful FollowUp transmission */
                    EthTSyn_RamDataTyp_ast[IdxDomain_u8].TrsmIdx_u8 = lIdxMstPort_u8;

                    /* Call API EthTSyn_Transmit to send FollowUp*/
                    EthTSyn_Transmit( ETHTSYN_FOLLOW_UP_TYPE, IdxDomain_u8, ETHTSYN_ZERO );
                }

#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME

                    /* Decrement the counter for number of successful transmissions */
                    EthTSyn_RamDataTyp_ast[IdxDomain_u8].nrTrsm_u8--;

                    /* Set the ownership to unused so switch can use this datastructure when EthIf_GetTxMgmtObject is called */
                    EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst->Ownership = ETHSWT_MGMT_OBJ_UNUSED;

                    /* Set the Management object pointer to null pointer after transmitting the message for that port */
                    EthTSyn_MstMgmtObject_ast[lIdxMstPort_u8].MstEthSwt_MgmtObject_pst = NULL_PTR;
#  endif
            }
        }
#  ifndef ETHTSYN_UPLINK_TO_TX_SWITCH_RESIDENCE_TIME
            if( ETHTSYN_ZERO == EthTSyn_RamDataTyp_ast[IdxDomain_u8].nrTrsm_u8 )
#  endif
        {
            /* Set State of Sync FollowUp message to received */
            EthTSyn_TimBrdg_st.SyncFupMsgRxd_u8 = ETHTSYN_MSG_TXT;
        }
    }
    else
    {
        /* Do Nothing */
    }
}
#  endif

#define ETHTSYN_STOP_SEC_CODE
#include "EthTSyn_MemMap.h"

# endif
#endif /* ETHTSYN_CONFIGURED */

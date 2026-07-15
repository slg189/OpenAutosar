

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_SoAdIfTxConfirmation_ParameterCheck(PduIdType TxPduId_uo)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_IF_TxConfirmation, DOIP_E_UNINIT);
    }
    else if (!DoIP_SoCon_IsUdpIndex(TxPduId_uo))
    {
        DOIP_DET(DOIP_DET_APIID_IF_TxConfirmation, DOIP_E_INVALID_PDU_SDU_ID);
    }
    else
    {
        retVal_b = TRUE;
    }
    return retVal_b;
}

void DoIP_SoAdIfTxConfirmation(PduIdType TxPduId)
{
    DoIP_ConnectionType_tst* soCon_pst;
    DoIP_Type_UDPConnection_tst* udpCon_pst;
    uint8 queueIdx_u8;
    boolean emptyQueue_b = TRUE;
    /* Perform paramter check */
    if (DoIP_SoAdIfTxConfirmation_ParameterCheck(TxPduId))
    {
        soCon_pst = DoIP_SoCon_GetSocketConnection(TxPduId);
        udpCon_pst = DoIP_SoCon_GetUDPCon(soCon_pst);
        /* Set remote address for UDP socket back to wildcard, if it was configured as wildcard. */
        /* todo:think about usecase that they can change the tester if we reset remote address */
        for (queueIdx_u8 = 0; queueIdx_u8 < DOIP_CFG_MaxUDPQueueSize; queueIdx_u8++)
        {
            if(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 != DOIP_UDP_DATA_EMPTY)
            {
                emptyQueue_b = FALSE;
                break;
            }
        }

        if(emptyQueue_b == TRUE)
        {
            DoIP_Connection_SetUdpRemoteAddressToWildcard(soCon_pst);
            udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b = FALSE; //reset the confirmation reminder bit
        }
        else
        {
            udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b = TRUE;
        }
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"


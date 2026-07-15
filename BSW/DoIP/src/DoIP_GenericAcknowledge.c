

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

Std_ReturnType DoIP_GenericAcknowledge_CheckVersionInfo(const DoIP_HeaderFields_tst* HeaderFields_pst)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    /*
     * REQ 7. [DoIP-156] AL DoIP entity supports protocol version
     * Each DoIP entity shall support the protocol version default value for vehicle identification request messages as
     * specified in Table 16. This means that a DoIP entity shall always ignore the protocol version default value in
     * vehicle identification request messages.
     */
    /* [SWS_DoIP_00015]
     * On a vehicle identification request the Protocol Type 0xFF and the inverse Protocol
     * Type 0x00 shall be supported as default values, additionally to the ProtocolType
     * described in SWS_DoIP_00005 and SWS_DoIP_00006.
     *(SRS_Eth_00026)
     */
    /* Check: Protocol information is correct */
    if ( ( ( HeaderFields_pst->ProtoVer_u8 == DOIP_PROTOCOL_VERSION ) &&
           ( HeaderFields_pst->InvProtoVer_u8 == DOIP_PROTOCOL_VERSION_INV ) ) ||
         ( ( ( HeaderFields_pst->ProtoVer_u8 == DOIP_PROTOCOL_VERSION_DEFAULT ) &&
             ( HeaderFields_pst->InvProtoVer_u8 == DOIP_PROTOCOL_VERSION_DEFAULT_INV ) &&
             ( ( HeaderFields_pst->payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ) ||
               ( HeaderFields_pst->payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ )||
               ( HeaderFields_pst->payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ ) ) ) ) )
    {
        retVal_u8 = E_OK;
    }

    return retVal_u8;
}

Std_ReturnType DoIP_GenericAcknowledge_CheckPayloadType(uint16 payloadType_u16, boolean isTP_b)
{
    Std_ReturnType retVal_u8 = E_OK;
    if (isTP_b)
    {
        if (!((payloadType_u16 == DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_REQ)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_ALIVE_CHECK_RESP)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_DIAG_MSG)))
        {
            retVal_u8 = E_NOT_OK;
        }
    }
    else
    {
        /* Allowed messages for UDP */
        if (!((payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_ENTITY_STATUS_REQ)
                || (payloadType_u16 == DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_REQ)))
        {
            /* Should invalid UDP messages be ignored */
#if (DOIP_CFG_GENERIC_HEADER_IGNORE_INVALID_UDP == TRUE)
            if (!((payloadType_u16 == DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK)
                    || (payloadType_u16 == DOIP_PAYLOAD_TYPE_VEHICLE_ID_RESP)
                    || (payloadType_u16 == DOIP_PAYLOAD_TYPE_ENTITY_STATUS_RESP)
                    || (payloadType_u16 == DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_RESP)))
            {
                retVal_u8 = E_NOT_OK;
            }
#else
            retVal_u8 = E_NOT_OK;

#endif /* TRUE == DOIP_CFG_GENERIC_HEADER_IGNORE_INVALID_UDP */

        }
    }

    return retVal_u8;
}

Std_ReturnType DoIP_GenericAcknowledge_CheckPayloadLength(const DoIP_HeaderFields_tst* HeaderFields_pst)
{
    Std_ReturnType retVal_u8 = E_OK;

    /* check: payload length is not valid for payloadType: nack 0x04, close socket */
    if ( ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_VEHICLE_ID_REQ)) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_REQ ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_REQ_7 ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_REQ_11 ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_ALIVE_CHECK_RESP ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_ALIVE_CHECK_RESP ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_DIAG_MSG ) &&
           ( HeaderFields_pst->payloadLength_u32 <= DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_VEHICLE_ID_EID_REQ ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_VEHICLE_ID_VIN_REQ ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_ENTITY_STATUS_REQ ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_ENTITY_STATUS_REQ ) ) ||
         ( ( HeaderFields_pst->payloadType_u16   == DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_REQ ) &&
           ( HeaderFields_pst->payloadLength_u32 != DOIP_PAYLOAD_LENGTH_DIAG_POWERMODE_REQ ) ) )
    {
        retVal_u8 = E_NOT_OK;
    }

    return retVal_u8;
}

Std_ReturnType DoIP_GenericAcknowledge_ConnectionNotRegisteredCheckPayloadLength(const DoIP_Type_TCPConnection* tcpCon_pst, const DoIP_HeaderFields_tst* HeaderFields_st, boolean* isNACKset, uint8* respCode_pu8)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;

    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-3028 and BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-3029]*/
    switch (HeaderFields_st->payloadType_u16)
    {
        case DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_REQ:
            /*Its a Routine Activation request then perform payload length check */
            if(E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadLength(HeaderFields_st))
            {
                *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE;
                *isNACKset = TRUE;
            }
            else
            {
                retVal_u8 = E_OK;
            }

            break;

        case DOIP_PAYLOAD_TYPE_ALIVE_CHECK_RESP:
            /*Payload lenght check forAlic check request should only be done when connection is in Registered[Authentication Pending] or Registered[Confirmation Pending]*/
            if((DOIP_RA_AUTH_E == tcpCon_pst->ra_st.raState_en) || (DOIP_RA_CONF_E == tcpCon_pst->ra_st.raState_en ))
            {
                if (E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadLength(HeaderFields_st))
                {
                    *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE;
                    *isNACKset = TRUE;
                }
                else
                {
                    retVal_u8 = E_OK;
                }

            }
            break;

        default:
            /*Invalid Payload*/
            retVal_u8 = E_NOT_OK;
            break;
    }
    return retVal_u8;

}
Std_ReturnType DoIP_GenericAcknowledge_TCPHeaderHandler(const DoIP_Type_TCPConnection*   tcpCon_pst, const uint8* header_pu8, boolean* isNACKset_b, uint8* respCode_pu8)
{

    Std_ReturnType retVal_u8 = E_NOT_OK;
    DoIP_HeaderFields_tst HeaderFields_st;

    DoIP_Header_Read(header_pu8, &HeaderFields_st);

    //check: invalid protocol pattern format -> close socket
    //Let Tcp connection be in any state this is mandatory check
    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2973]*/
    if (E_NOT_OK == DoIP_GenericAcknowledge_CheckVersionInfo(&HeaderFields_st))
    {
        *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PROTOCOL;
        *isNACKset_b = TRUE;
    }
    /*Check if Connection is registered*/
    else if ( tcpCon_pst->ra_st.isActive_b)
    {
        /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2926]*/
        //check: if connection is in registered state, payload type is not supported: nack 0x01, discard
        if(E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadType(HeaderFields_st.payloadType_u16, TRUE))
        {
            *respCode_pu8 = DOIP_GA_RESPCODE_PAYLOAD_TYPE_UNSUPPORTED;
            *isNACKset_b = TRUE;
        }
        /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2927]*/
        //check: if Tcp connection is in registered state, payload exceeds DoIPMaxRequestBytes: nack 0x02, discard
        else if (HeaderFields_st.payloadLength_u32 > DOIP_CFG_MAX_REQUEST_BYTES)
        {
            *respCode_pu8 = DOIP_GA_RESPCODE_EXCEEDS_MAX_REQ_BYTES;
            *isNACKset_b = TRUE;
        }
        /*
         * this makes no sense, because we dont have internal memory, and the upperlayer buffer will be checked via StartOfReception
         * For UDP cases this check is not required
         */
        //check: payload exceeds DoIP internal memory: nack 0x03, discard
        //else if (E_NOT_OK == DoIP_GenericAcknowledge_CheckInternalMemory(&HeaderFields_st))
        //{
        //    *respCode_pu8 = DOIP_GA_RESPCODE_EXCEEDS_DOIP_BUFFER;
        //}
        //check: if Tcp connection is in registered state, payloadLength doesnt match to payloadtype: nack 0x04, discard
        /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-3028]*/
        else if (E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadLength(&HeaderFields_st))
        {
            *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE;
            *isNACKset_b = TRUE;
        }
        else
        {
            retVal_u8 = E_OK;
        }

    }
    /*When Tcp connection is not in registered state and the payload is either Routine activation or Alive check*/
    else
    {
        /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-3023 , BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-3025]*/
        retVal_u8 =  DoIP_GenericAcknowledge_ConnectionNotRegisteredCheckPayloadLength(tcpCon_pst, &HeaderFields_st, isNACKset_b, respCode_pu8);

    }

    return retVal_u8;
}

Std_ReturnType DoIP_GenericAcknowledge_UDPHeaderHandler(const uint8* header_pu8, uint8* respCode_pu8)
{

    Std_ReturnType retVal_u8 = E_NOT_OK;
    DoIP_HeaderFields_tst HeaderFields_st;

    DoIP_Header_Read(header_pu8, &HeaderFields_st);

    //check: invalid protocol pattern format -> close socket
    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2973]*/
    if (E_NOT_OK == DoIP_GenericAcknowledge_CheckVersionInfo(&HeaderFields_st))
    {
        *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PROTOCOL;
    }
    //check: payload type is not supported: nack 0x01, discard
    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2926]*/
    else if (E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadType(HeaderFields_st.payloadType_u16, FALSE))
    {
        *respCode_pu8 = DOIP_GA_RESPCODE_PAYLOAD_TYPE_UNSUPPORTED;
    }
    //check: payload exceeds DoIPMaxRequestBytes: nack 0x02, discard
    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2927]*/
    else if (HeaderFields_st.payloadLength_u32 > DOIP_CFG_MAX_REQUEST_BYTES)
    {
        *respCode_pu8 = DOIP_GA_RESPCODE_EXCEEDS_MAX_REQ_BYTES;
    }
    /*TRACE[BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2928]*/
    //check: payloadLength doesnt match to payloadtype: nack 0x04, discard
    else if (E_NOT_OK == DoIP_GenericAcknowledge_CheckPayloadLength(&HeaderFields_st))
    {
        *respCode_pu8 = DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE;
    }
    else
    {
        retVal_u8 = E_OK;
    }

    return retVal_u8;
}

void DoIP_GenericAcknowledge_PrepareResponse(uint8* buffer_pu8, uint8 responseCode_u8)
{
    /* Header */
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_GENERIC_HEADER_NACK, DOIP_PAYLOAD_LENGTH_GENERIC_HEADER_NACK, buffer_pu8);

    /* Payload */
    /* responde code */
    buffer_pu8[DOIP_MSG_POS_CONTENT] = responseCode_u8;
}

void DoIP_GenericAcknowledge_TpTxConfirmation(DoIP_Type_TCPConnection* tcpCon_pst)
{
    uint8 responseCode_u8 = tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackBuffer_au8[DOIP_MSG_POS_CONTENT];

	tcpCon_pst->TX_st.genericNACKBuffer_st.genericNackAvailable_b = FALSE;
    switch (responseCode_u8)
    {
        case DOIP_GA_RESPCODE_INVALID_PROTOCOL:
        case DOIP_GA_RESPCODE_INVALID_PAYLOAD_LENGTH_TYPE:
            /* Set flag to close connection */
            DoIP_TCPConnection_SetCloseNeeded(tcpCon_pst);
            break;
        default:
            tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
            break;
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"


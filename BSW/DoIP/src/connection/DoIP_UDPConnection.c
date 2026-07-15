

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

static DoIP_Type_UDPConnection_tst DoIP_UDPConnection_ast[DOIP_CFG_MAX_NUM_OF_UDP_CONN];
static uint8 DoIP_UDPConnectionIdx_u8;

/* This read only variable contains constant user configuration and is used in order to avoid extra instructions each time the function "DoIP_UDPConnection_HandleTimer"
 * is called.
 * This read only variable contains the sum of the Vehicle Announcement Interval Times between all the Vehicle Announcement messages
 * Since the first Vehicle Announcement Message is sent directly after Initial Vehicle Announcement Time is Elapsed, one time shall be deducted from the number of
 * repetitions */
static const uint32 VehicleAnnouncementIntervalAllCycles_cu32 = (DOIP_CFG_VEHICLE_ANNOUNCEMENT_REPETITION - 1u) * DOIP_CFG_VEHICLE_ANNOUNCEMENT_INTERVAL_CYCLES;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_UDPConnection_Init(void)
{
    DoIP_UDPConnectionIdx_u8 = 0;
}

void DoIP_UDPConnection_Create(const DoIP_ConnectionType_tst* soCon_pcst)
{
    DoIP_Type_UDPConnection_tst* udpCon_pst;

    if (DoIP_UDPConnectionIdx_u8 < DOIP_CFG_MAX_NUM_OF_UDP_CONN)
    {
        udpCon_pst = &DoIP_UDPConnection_ast[DoIP_UDPConnectionIdx_u8];
        DoIP_UDPConnectionIdx_u8++;
        udpCon_pst->DoIP_IsResetVehicleAnnTimer_b = FALSE;
        udpCon_pst->soCon_pcst = soCon_pcst;
    }
}

void DoIP_UDPConnection_MainFunction(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    /* Timer */
    DoIP_UDPConnection_HandleTimer(udpCon_pst);

    /* Messages */
    DoIP_UDPConnection_HandleMessages(udpCon_pst);
}


void DoIP_UDPConnection_HandleMessages(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    uint8 queueIdx_u8;
    uint16 payloadType_u16;

    /* Loop over all queue elements */
    for (queueIdx_u8 = 0; queueIdx_u8 < DOIP_CFG_MaxUDPQueueSize; queueIdx_u8++)
    {
        /* Check if new message was received */
        if (udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 == DOIP_UDP_DATA_RECEIVED)
        {
            /* Handle data in queue according to payload type. */
            payloadType_u16 = DoIP_Header_GetPayloadType(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);
            switch (payloadType_u16)
            {
                case DOIP_PAYLOAD_TYPE_VEHICLE_ID_REQ:
                    DoIP_VehicleIdentification_HandleRequest(udpCon_pst, queueIdx_u8);
                    break;
                case DOIP_PAYLOAD_TYPE_VEHICLE_ID_EID_REQ:
                    DoIP_VehicleIdentification_HandleEIDRequest(udpCon_pst, queueIdx_u8);
                    break;
                case DOIP_PAYLOAD_TYPE_VEHICLE_ID_VIN_REQ:
                    DoIP_VehicleIdentification_HandleVINRequest(udpCon_pst, queueIdx_u8);
                    break;
                case DOIP_PAYLOAD_TYPE_ENTITY_STATUS_REQ:
                    DoIP_EntityStatus_HandleRequest(udpCon_pst, queueIdx_u8);
                    break;
                case DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_REQ:
                    DoIP_Powermode_HandleRequest(udpCon_pst, queueIdx_u8);
                    break;
                default:
                    /* Nothing to do */
                    break;
            }
        }

        /* Announcement and VehicleID response are send with delay */
        if (udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 == DOIP_UDP_DATA_PROCESSED)
        {
            /* If timer elapsed, set data in queue to ready */
            if (udpCon_pst->isAnnouncementMessageReady_b == TRUE)
            {
                DoIP_VehicleIdentification_PrepareMessage(udpCon_pst->soCon_pcst->soConId_uo,
                        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);
                udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
            }
        }

        /* Send message if ready */
        if (udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 == DOIP_UDP_DATA_READY)
        {
            DoIP_UDPConnection_SendMessage(udpCon_pst, queueIdx_u8);
        }
    }

    if (udpCon_pst->isAnnouncementMessageReady_b == TRUE)
    {
        udpCon_pst->isAnnouncementMessageReady_b = FALSE;
    }

    if(udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b == TRUE)
    {
        DoIP_SetRemoteAddressToWildCard(udpCon_pst);
    }
}

/***********************************************************************************************************************
 Function name    : DoIP_UDPConnection_HandleTimer
 Syntax           : DoIP_UDPConnection_HandleTimer(DoIP_Type_UDPConnection_tst* udpCon_pst)
 Description      : This Function shall manage the Timer linked to Udp Connections in order to decide the moments of
                    sending Vehicle Announcement Message and how much times shall be resent and it manages also when
                    Vehicle Identification Message shall be sent.
 Parameter        : udpCon_pst : A pointer to structure of type "DoIP_Type_UDPConnection_tst" supposed to store information
                    related to one Socket Connection.
 Return value     : None
 Requirement Id   : BSW_SWS_AR4_2_R2_DiagnosticOverIP_Ext-2995, BSW_SWS_AR4_2_R2_DiagnosticOverIP-3599
***********************************************************************************************************************/
void DoIP_UDPConnection_HandleTimer(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    /* Handle UDP timers */
    if (udpCon_pst->timerVehicleAnnouncement_u32 > 0u)
    {
        udpCon_pst->timerVehicleAnnouncement_u32--;

        /* Handle Vehicle Announcement : Sending Vehicle Announcement first message shall done once Initial Vehicle Announcement Time is Elapsed */
        if((udpCon_pst->isCurrentPhaseIdentification_b == FALSE) && (udpCon_pst->timerVehicleAnnouncement_u32 <= VehicleAnnouncementIntervalAllCycles_cu32))
        {
            if((udpCon_pst->timerVehicleAnnouncement_u32 % DOIP_CFG_VEHICLE_ANNOUNCEMENT_INTERVAL_CYCLES) == 0u)
            {
                udpCon_pst->isAnnouncementMessageReady_b = TRUE;
            }
        }

        if(udpCon_pst->timerVehicleAnnouncement_u32 == 0u)
        {
            /* Handle Vehile Identification : Vehicle Identification Response Message shall be sent once Initial Vehicle Announcement Time Elapsed */
            if(udpCon_pst->isCurrentPhaseIdentification_b == TRUE)
            {
                udpCon_pst->isAnnouncementMessageReady_b = TRUE;
            }

            /* Last Vehicle Announcement during Vehicle Discovery phase has been trasmitted, switch UDP Connection to ONLINE now */
            udpCon_pst->udpState_en = DOIP_UDP_IDLE;
        }
    }
}

void DoIP_UDPConnection_Open(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    // no VAs on UDP connections with a wildcard remote address
    // they do not go online immediately after open
    uint8 queueIdx_u8;

    udpCon_pst->udpState_en = DOIP_UDP_IDLE;
    udpCon_pst->isAnnouncementMessageReady_b = FALSE;
    udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b = FALSE;

    for (queueIdx_u8 = 0; queueIdx_u8 < DOIP_CFG_MaxUDPQueueSize; queueIdx_u8++)
    {
        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_EMPTY;
    }

    if (udpCon_pst->soCon_pcst->cfg_pcst->hasUdpWildcardRemoteAddress_b == FALSE)
    {
        udpCon_pst->udpState_en = DOIP_UDP_DISCOVERY;
        /* Reserve the second location of the queue for the vehicle announcement message*/
        udpCon_pst->DoIP_UDPQueue_au8[1].udpQueueState_u8 = DOIP_UDP_DATA_PROCESSED;
        // initialize the vehicle announcement timer only in case that the Ip adress was changed before
        if(TRUE == udpCon_pst->DoIP_IsResetVehicleAnnTimer_b)
        {
            DoIP_UDPConnection_InitVehicleAnnouncementTime(udpCon_pst);
            udpCon_pst->DoIP_IsResetVehicleAnnTimer_b = FALSE;
        }
    }
}

void DoIP_UDPConnection_Close(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    udpCon_pst->udpState_en = DOIP_UDP_IDLE;
    udpCon_pst->timerVehicleAnnouncement_u32 = 0u;
}

void DoIP_UDPConnection_SendMessage(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    PduInfoType pduInfo_pst;
    uint32 payLoadLength;

    pduInfo_pst.SduDataPtr = udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8;
    payLoadLength = DoIP_Header_GetPayloadLength(pduInfo_pst.SduDataPtr);
    pduInfo_pst.SduLength = (PduLengthType) (payLoadLength + DOIP_HEADER_SIZE);

    // DoIP clear the queue if SoAd_IfTransmit returns E_NOT_OK (e.g tester it not more member of the network)
    // If the tester wants to get the message again, he have to ask in a new request
    if (SoAd_IfTransmit(DoIP_SoCon_GetSoAdPdu(udpCon_pst->soCon_pcst), &pduInfo_pst) == E_OK)
    {
        if ((udpCon_pst->udpState_en == DOIP_UDP_DISCOVERY) && (queueIdx_u8 == 1))
        {
            /* Reserve the second location of the queue for the vehicle announcement message during discovery phase*/
            udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_PROCESSED;
        }
        else
        {
            /* Clear the queue*/
            udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_EMPTY;
        }
    }
    else
    {
        /* Clear the queue and reset remote address to wildcard address */
        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_EMPTY;
        DoIP_SetRemoteAddressToWildCard(udpCon_pst);
    }
}

DoIP_Type_UDPConnection_tst* DoIP_UDPConnection_Get(uint16 udpIdx_u16)
{
    return &DoIP_UDPConnection_ast[udpIdx_u16];
}

void DoIP_UDPConnection_InitVehicleAnnouncementTime(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    /* Set vehicle-announcement-timer, equals initial time + number of repetitions */
    udpCon_pst->timerVehicleAnnouncement_u32 = DOIP_CFG_InitialVehicleAnnouncementTime_Cycles +
	((DOIP_CFG_VEHICLE_ANNOUNCEMENT_REPETITION - 1u) * DOIP_CFG_VEHICLE_ANNOUNCEMENT_INTERVAL_CYCLES);

    /* Make the DoIP aware of the Current Communication Phase : Current Phase is Announcement */
    udpCon_pst->isCurrentPhaseIdentification_b = FALSE;
}

void DoIP_UDPConnection_InitVehicleIdentificationTime(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    if (udpCon_pst->timerVehicleAnnouncement_u32 == 0u)
    {
        /* Set vehicle-identification-timer, at least 1 cycle */
        udpCon_pst->timerVehicleAnnouncement_u32 = DOIP_CFG_InitialVehicleAnnouncementTime_Cycles;

        /* Make the DoIP aware of the Current Communication Phase : Current Phase is Identification */
        udpCon_pst->isCurrentPhaseIdentification_b = TRUE;
    }
}

void DoIP_EntityStatus_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    uint32 maxReqBytes_u32;
    uint8* msg_pu8 = udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8;

    /* header */
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_ENTITY_STATUS_RESP, DOIP_PAYLOAD_LENGTH_ENTITY_STATUS_RESP, msg_pu8);
    msg_pu8 += DOIP_HEADER_SIZE;

    /* payload */
    *msg_pu8 = DOIP_CFG_NodeType;
    msg_pu8++;
    *msg_pu8 = DOIP_CFG_MaxTesterConnections;
    msg_pu8++;
    *msg_pu8 = DoIP_NumOfActiveTCPConnections_u8;
    msg_pu8++;

#if ( DOIP_CFG_EntityStatusMaxByteFieldUse != FALSE )
    maxReqBytes_u32 = rba_DiagLib_ByteOrderUtils_Htonl(DOIP_CFG_MAX_REQUEST_BYTES);
    DOIP_MEMCPY(msg_pu8, &maxReqBytes_u32, 4);
#endif
    udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
}

void DoIP_Powermode_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    DoIP_PowerStateType powerState_en;
    uint8* msg_pu8 = udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8;

    /* header */
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_DIAG_POWERMODE_RESP, DOIP_PAYLOAD_LENGTH_DIAG_POWERMODE_RESP, msg_pu8);
    msg_pu8 += DOIP_HEADER_SIZE;

    /* payload */
    if (DoIP_Callback_PowerModeCallback(&powerState_en) != E_OK)
    {
        powerState_en = DOIP_NOT_READY;
    }
    *msg_pu8 = powerState_en;
    udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"




#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static void DoIP_VehicleIdentification_IgnoreRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_EMPTY;

    /* Set remote address for UDP socket back to wildcard, if it was configured as wildcard. */
     DoIP_SetRemoteAddressToWildCard(udpCon_pst);
}

void DoIP_VehicleIdentification_HandleRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
   /*
    * [SWS_DoIP_00060]
    * If a DoIP message with payload Type 0x0001 is received on the configured
    * DoIPUDPConnection, the DoIP module shall respond with a vehicle identification
    * response/vehicle announcement message after the configured
    * DoIPInitialVehicleAnnouncementTime with payload type 0x0004 as described
    * inTable 6.
    */

   /* NOTE 3 The additional delay before responding to a vehicle identification request is necessary in order to avoid UDP
    * packet bursts on the network if many DoIP entities are connected to the same network. In such a case, the random delay
    * of the vehicle identification announcement response allows for UDP packets that were dropped due to high network
    * utilization to reach the external test equipment on subsequent vehicle identification request broadcasts
    */

    /* Only one timer per udp connection, therefore if counter already set because of other request, nothing to do */
    DoIP_UDPConnection_InitVehicleIdentificationTime(udpCon_pst);
    udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_PROCESSED;
}

void DoIP_VehicleIdentification_HandleEIDRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    uint8 EID[6];

    /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
     * But actually the value of DOIP_CFG_UseMacAddressForIdentification is not constant but depends on configuration.*/
    if (DOIP_CFG_UseMacAddressForIdentification != FALSE)
    {
        retVal_u8 = SoAd_GetPhysAddr((udpCon_pst->soCon_pcst->soConId_uo), EID);
    }

    if (retVal_u8 != E_OK)
    {
        DOIP_MEMCPY(EID, &(DoIP_activeConfig_pcst->eid_au8), 6);
        retVal_u8 = E_OK;
    }

    /*If the elements compared are equal, memory comparision function returns false, else a non zero value */
    if (DOIP_MEMCMP(EID, &udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8[DOIP_HEADER_SIZE], 6) == 0x0)
    {
        /* Frame Vehicle Announcement Message */
        DoIP_VehicleIdentification_PrepareMessage(udpCon_pst->soCon_pcst->soConId_uo, udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);
        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
    }
    else
    {
        /* Ignore this request as it is not meant for this node to respond */
        DoIP_VehicleIdentification_IgnoreRequest(udpCon_pst, queueIdx_u8);
    }
}

void DoIP_VehicleIdentification_HandleVINRequest(DoIP_Type_UDPConnection_tst* udpCon_pst, uint8 queueIdx_u8)
{
    /*If the elements compared are equal, memory comparision function returns false, else a non zero value */
    if (DOIP_MEMCMP(DoIP_IntGetVin(), &udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8[DOIP_HEADER_SIZE], 17) == 0x0)
    {
        /* Frame Vehicle Announcement Message */
        DoIP_VehicleIdentification_PrepareMessage(udpCon_pst->soCon_pcst->soConId_uo, udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].buffer_au8);
        udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 = DOIP_UDP_DATA_READY;
    }
    else
    {
        /* Ignore this request as it is not meant for this node to respond */
        DoIP_VehicleIdentification_IgnoreRequest(udpCon_pst, queueIdx_u8);
    }
}

void DoIP_VehicleIdentification_PrepareMessage(SoAd_SoConIdType soConId_uo, uint8* msg_pu8)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    uint16 logicalAddr;
    uint8 EID[6];
    uint8 GID[6];

    /* Header */
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_VEHICLE_ID_RESP, DOIP_PAYLOAD_LENGTH_VEHICLE_ID_RESP, msg_pu8);
    msg_pu8 += DOIP_HEADER_SIZE;

    /* Payload */
    /* Vin */
    DOIP_MEMCPY(msg_pu8, DoIP_IntGetVin(), 17);
    msg_pu8 += 17;

    logicalAddr = rba_DiagLib_ByteOrderUtils_Htons(DoIP_activeConfig_pcst->logicalAddress_u16);
    DOIP_MEMCPY(msg_pu8, &logicalAddr, 2);
    msg_pu8 += 2;

    /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
     * But actually the value of DOIP_CFG_UseMacAddressForIdentification is not constant but depends on configuration.*/
    if (DOIP_CFG_UseMacAddressForIdentification != FALSE)
    {
        retVal_u8 = SoAd_GetPhysAddr(soConId_uo, EID);
    }
    if (retVal_u8 != E_OK)
    {
        DOIP_MEMCPY(EID, &(DoIP_activeConfig_pcst->eid_au8), 6);
    }
    DOIP_MEMCPY(msg_pu8, EID, 6);
    msg_pu8 += 6;

    /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
     * But actually the value of DOIP_CFG_UseEIDasGID is not constant but depends on configuration.*/
    if (DOIP_CFG_UseEIDasGID != FALSE)
    {
        /* EID should be used as GID */
        DOIP_MEMCPY(msg_pu8, EID, 6);
    }
    else
    {
        /* Get Gid via callback */
        retVal_u8 = DoIP_Callback_GetGidCallback(GID);

        /* If callback wasn't successful or isn't configured */
        if (retVal_u8 != E_OK)
        {
            DOIP_MEMCPY(GID, &(DoIP_activeConfig_pcst->gid_au8), 6);
        }
        DOIP_MEMCPY(msg_pu8, GID, 6);
    }
    msg_pu8 += 6;

    /* Further Action byte */
    *msg_pu8 = DoIP_VehicleIdentification_GetFurtherActionByte();
    msg_pu8++;

    /* VIN / GID Synchronisation Status Code */
#if (DOIP_CFG_UseVehicleIdentificationSyncStatus != FALSE)
    *msg_pu8 = DoIP_VehicleIdentification_GetGIDVINStatusByte();
#endif
}

uint8 DoIP_VehicleIdentification_GetGIDVINStatusByte(void)
{
    uint8 statusVINSync = DOIP_VIN_GID_NOT_SYNCHRONIZED;

    /* Check if Dcm_GetVIN was successful */
    if (DoIP_IsVinReady_b)
    {
        statusVINSync = DOIP_VIN_GID_SYNCHRONIZED;
    }
    else
    {
        /* Check if this node it GID-master */

#if (TRUE == DOIP_CFG_VinGidMaster)
        /* Check if synchronisation is currenlty pending */
        if (DoIP_GIDSyncState_u8 == DOIP_GIDSYNC_SUCCESS)
        {
            statusVINSync = DOIP_VIN_GID_SYNCHRONIZED;
        }
#endif /* TRUE == DOIP_CFG_VinGidMaster */


    }
    return statusVINSync;
}

/* Function returns the further action byte value in vehicle announcement message */
uint8 DoIP_VehicleIdentification_GetFurtherActionByte(void)
{
    /* If central security is not configured, further action byte value is set to 0x00 */
    uint8 furtheraction_u8 = DOIP_ROUTEACTIVE_NO_FURTHER_ACTION;

    if (DoIP_activeConfig_pcst->oemCentralsecurity_b != FALSE)
    {
        uint8 idx_u8;
        furtheraction_u8 = DOIP_ROUTENOTACTIVE_INIT_CENTRAL_SECURITY;

        /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
         * But actually the value of DOIP_CFG_FURTHER_ACTION_BYTE_INDEPENDENT is not constant but depends on
         * configuration.*/
        if (DOIP_CFG_FURTHER_ACTION_BYTE_INDEPENDENT == FALSE)
        {
            for (idx_u8 = 0; idx_u8 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; idx_u8++)
            {
                /* Check whether at least one routing activation with central security is activated */
                if (DoIP_SoCon_IsActive(DoIP_TCPConnection_ast[idx_u8].soCon_pst))
                {
                    if(DoIP_RoutingActivation_IsActivationTypeActive(&DoIP_TCPConnection_ast[idx_u8].ra_st, DOIP_CENTRAL_SECURITY_ID))
                    {
                        furtheraction_u8 = DOIP_ROUTEACTIVE_NO_FURTHER_ACTION;
                        break;
                    }
                }
            }
        }
    }

    return furtheraction_u8;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"


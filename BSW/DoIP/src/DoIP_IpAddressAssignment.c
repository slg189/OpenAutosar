

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

boolean DoIP_IsIPAddressAssigned_b;
boolean DoIP_IsIPAddressAssignmentRequested_b;
DoIP_Type_GIDSyncState_tu8 DoIP_GIDSyncState_u8;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static boolean DoIP_LocalIpAddrAssignmentChg_ParameterCheck(SoAd_SoConIdType SoConId_uo)
{
    boolean retVal_b = FALSE;

    if (DoIP_Initialized_b == FALSE)
    {
        DOIP_DET(DOIP_DET_APIID_LocalIpAddrAssignmentChg, DOIP_E_UNINIT);
    }
    else if (!DoIP_SoCon_IsSoAdSoConIdValid(SoConId_uo))
    {
        DOIP_DET(DOIP_DET_APIID_LocalIpAddrAssignmentChg, DOIP_E_INVALID_PARAMETER);
    }
    else
    {
        retVal_b = TRUE;
    }

    return retVal_b;
}

void DoIP_IpAddressAssignmentDHCPHostname(SoAd_SoConIdType SoConId_uo)
{
    uint8 length;
    //TODO postbuild: differe between current cfg and max size
    /* Hostname size is either the configured maximum size, or the size of VIN, depending what is bigger */
    /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
     * But actually the value of DOIP_CFG_HostNameSizeMax is not constant but depends on configuration. */
    uint8 hostname[DOIP_MAX(DOIP_CFG_HostNameSizeMax, 25u)];
    boolean writeHostname = FALSE;

    length = DOIP_CFG_HostNameSizeMax;

    /* Clear buffer */
    DOIP_MEMSET(hostname, 0, DOIP_CFG_HostNameSizeMax);

    /* Read DHCP hostname */
    if(SoAd_ReadDhcpHostNameOption(SoConId_uo, &length, &hostname[0]) != E_OK)
    {
        writeHostname = TRUE;
    }
    else
    {
        if(DOIP_MEMCMP(hostname, "DoIP-", 5) != 0)
        {
            writeHostname = TRUE;
        }
    }

    /* Read failed or hostname not correct --> hostname has to be written */
    if(writeHostname)
    {
        DOIP_MEMCPY(hostname, "DoIP-VIN", 8u);

        /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove it.
         * But actually the value of DOIP_CFG_DhcpOptionVinUse is not constant but depends on configuration. */
        if (DOIP_CFG_DhcpOptionVinUse != FALSE)
        {
            length = 25;
            DOIP_MEMCPY(&hostname[8], DoIP_IntGetVin(), 17);
        }
        else
        {
            length = 5;
        }

        (void) SoAd_WriteDhcpHostNameOption(SoConId_uo, length, &hostname[0]);
    }
}

void DoIP_LocalIpAddrAssignmentChg(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State)
{
    uint8 idx_u8;

    if (DoIP_LocalIpAddrAssignmentChg_ParameterCheck(SoConId))
    {
        if (State == TCPIP_IPADDR_STATE_ASSIGNED)
        {
            DoIP_IpAddressAssignmentDHCPHostname(SoConId);
            DoIP_IsIPAddressAssigned_b = TRUE;
            DoIP_IsIPAddressAssignmentRequested_b = TRUE;
            DoIP_AL_StatusChg_u8 = DOIP_AL_HIGH_IDLE;
            /*Set this flag so that Vehicle Announcement starts once connections are online.*/
            for (idx_u8 = 0; idx_u8 < DoIP_activeConfig_pcst->udpConnSize_u8; idx_u8++)
            {
                DoIP_Type_UDPConnection_tst* udpCon_pst = DoIP_SoCon_GetUDPCon(DoIP_SoCon_GetSocketConnection(idx_u8));
                udpCon_pst->DoIP_IsResetVehicleAnnTimer_b = TRUE;
            }
            /* MR12 RULE 14.3 VIOLATION: MISRA identifies the controlling expression as constant and suggests to remove
             * it. But actually the value of DOIP_CFG_VinGidMaster is not constant but depends on configuration. */
#if DOIP_CFG_VinGidMaster == TRUE
                DoIP_GIDSyncState_u8 = DOIP_GIDSYNC_PENDING;
#endif
        }
        else
        {
            DoIP_IsIPAddressAssigned_b = FALSE;
            DoIP_GIDSyncState_u8 = DOIP_GIDSYNC_IDLE;
            DoIP_IsIPAddressAssignmentRequested_b = FALSE;
            DoIP_OpenConnections_b = FALSE;
            DoIP_AL_StatusChg_u8 = DOIP_AL_LOW_IDLE;
        }
    }
}
static void DoIP_IpAddressAssignment_RequestIpAddressAssignment(void)
{
    Std_ReturnType retVal_u8 = E_NOT_OK;
    uint8 netmask_u8 = 0;
    uint16 idx_u16;

    /* First cycle with activation line on ? */

    // todo: assure that for every connection min. one IpAddressAssignmentRequest called
    if ( (!DoIP_IsIPAddressAssignmentRequested_b) && (!DoIP_IsIPAddressAssigned_b))
    {
        for (idx_u16 = 0; idx_u16 < DoIP_activeConfig_pcst->connSize_u16; idx_u16++)
        {
            DoIP_ConnectionType_tst* DoIP_Connection_ast = DoIP_SoCon_GetSocketConnection(idx_u16);
            if (DoIP_Connection_ast->cfg_pcst->requestAddressAssignment_b)
            {
                retVal_u8 = SoAd_RequestIpAddrAssignment(DoIP_Connection_ast->soConId_uo, TCPIP_IPADDR_ASSIGNMENT_ALL,
                        (TcpIp_SockAddrType*) NULL_PTR, netmask_u8, (TcpIp_SockAddrType*) NULL_PTR);
            }
        }
        if (retVal_u8 != E_OK)
        {
            /* Det Trigger ? */
        }
        DoIP_IsIPAddressAssignmentRequested_b = TRUE;
    }

    if (DoIP_IsIPAddressAssigned_b && DoIP_IsIPAddressAssignmentRequested_b)
    {
        /* Set offline sockets in listen_mode */
        DoIP_OpenConnections_b = TRUE;
    }
    else
    {
        /* Wait until IpAddress is assigned */
    }
    return;
}

static void DoIP_IpAddressAssignment_ReleaseIpAddressAssignment(void)
{
    uint16 idx_u16;
    DoIP_OpenConnections_b = FALSE;

    if (DoIP_IsIPAddressAssignmentRequested_b)
    {
        if (DoIP_SoCon_AllRelevantConnectionsOffline())
        {
            for (idx_u16 = 0; idx_u16 < DoIP_activeConfig_pcst->connSize_u16; idx_u16++)
            {
                DoIP_ConnectionType_tst* DoIP_Connection_ast = DoIP_SoCon_GetSocketConnection(idx_u16);
                if (DoIP_Connection_ast->cfg_pcst->requestAddressAssignment_b)
                {
                    (void) SoAd_ReleaseIpAddrAssignment(DoIP_Connection_ast->soConId_uo);
                }
            }
            DoIP_IsIPAddressAssignmentRequested_b = FALSE;
        }
    }
    else
    {
        /* All connections offline, IPAddress released - nothing to do */
    }
    return;
}


void DoIP_IpAddressAssignmentMainFunction(void)
{
    /**
     * In case of ActivationLine is ACTIVE, there is possible, that between this and previous Mainfunction
     * the ActivationLineStatus changed to INACTIVE. This status change is indicated by DoIP_AL_StatusChg == DOIP_AL_SWITCH_ONLINE2OFFLINE.
     * In that case we have to trigger first the release if the IP address assignment.
     * Only if the ActivationLine is still high in the next Mainfunction the IP address assignment will be requested again.
     */

    if (DoIP_ActivationLineStatus_u8 == DOIP_ACTIVATION_LINE_ACTIVE)
    {
        if (DoIP_AL_StatusChg_u8 == DOIP_AL_SWITCH_ONLINE2OFFLINE)
        {
            DoIP_IpAddressAssignment_ReleaseIpAddressAssignment();
        }
        else
        {
            DoIP_IpAddressAssignment_RequestIpAddressAssignment();
        }
    }

    /**
     * In case of INACTIVE and only if the ActivationLine switched from INACTIV to ACTIVE since the last Mainfunction
     * we call the release IP address assignment. Otherwise we ignore the status change, because DoIP is already INACTIVE
     */

    if (DoIP_ActivationLineStatus_u8 == DOIP_ACTIVATION_LINE_INACTIVE)
    {
        if (DoIP_AL_StatusChg_u8 == DOIP_AL_SWITCH_ONLINE2OFFLINE)
        {
            DoIP_IpAddressAssignment_ReleaseIpAddressAssignment();
        }

    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"


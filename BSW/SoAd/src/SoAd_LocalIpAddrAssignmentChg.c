

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "SoAd.h"
#include "SoAd_Cbk.h"

#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

/**
 ***********************************************************************************************************************
 * \Function Name : SoAd_LocalIpAddrAssignmentChg()
 *
 * \Function description
 *  SoAd_LocalIpAssignmentChg() gets called by the TCP/IP stack if an IP address assignment changes.
 *
 *  Parameters (in):
 * \param   TcpIp_LocalAddrIdType
 * \arg     IpAddrId - IP address Identifier, representing an IP address specified in the TcpIp module configuration
 * \param   TcpIp_IpAddrStateType
 * \arg     State - state of IP address assignment
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 *
 ***********************************************************************************************************************
 */
void SoAd_LocalIpAddrAssignmentChg( TcpIp_LocalAddrIdType IpAddrId ,\
                                    TcpIp_IpAddrStateType State )
{
    /* Index to store the local address Id */
    SoAd_LocalAddrIdInfoType_tst *       lLocalAddressIdInfo_past;

    /* local variable for for loop*/
    uint8                                lLoop_u8;

    /* Index for looping across static sockets*/
    SoAd_SoConIdType                     lIdxSoCon_uo ;

    /* Index to store the localaddressId */
    TcpIp_LocalAddrIdType                lLocalAddressId_u8;

    /* Local pointer to store the static SoCon array one by one */
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;


    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_VOID( ( FALSE == SoAd_InitFlag_b ), SOAD_LOCAL_IPADDR_ASSIGNMENT_CHG_API_ID, SOAD_E_NOTINIT )

    /* Check whether IpAddrId is valid or not */
    SOAD_DET_CHECK_ERROR_VOID( ( SOAD_MAX_TCPIP_LOCALADDRIDX <= IpAddrId ), SOAD_LOCAL_IPADDR_ASSIGNMENT_CHG_API_ID, SOAD_E_INV_ARG)

    /********************************* Start: DET Checking *************************************/

    lLocalAddressIdInfo_past = &(SoAd_CurrConfig_cpst->SoAd_LocalAddrIdInfo_past[IpAddrId]);

    /* If the requested local address id is present in SoAd config */
    if(TRUE == lLocalAddressIdInfo_past->isLocalAddrIdCfgd_b )
    {
        /* store the status of the local address Id in the table*/
        lLocalAddressIdInfo_past->localAddrIdx_State_en = State ;

        /* loop through all the sockets for the processing */
        for (lIdxSoCon_uo=0; lIdxSoCon_uo<SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo; lIdxSoCon_uo++)
        {
            /* copy the static socket config array to local pointer for further processing */
            lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxSoCon_uo]);

            /* Get the local address Id of this socket connection */
            lLocalAddressId_u8 = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[lSoAdStaticSocConfig_cpst->soConGrpId_uo].soConGrpLocalAddressIdx_u8 ;

            if((lLocalAddressId_u8 != IpAddrId) || (NULL_PTR == lSoAdStaticSocConfig_cpst->LocalIpAddrChgCbkULInfo_pst))
            {
                continue;
            }

            /* Condition check: mode change notification is enabled and call back should be  configured in upper layer */
            for(lLoop_u8 = 0; lLoop_u8 < lSoAdStaticSocConfig_cpst->LocalIpAddrChgCbkULInfo_pst->NumberOfUlCfgd_u8; lLoop_u8++)
            {
                /* call the upper layer to notify about Local Address assignment change */
                SoAd_CurrConfig_cpst->SoAd_LocalIpAddrAsgnChgCbk_cpst[lSoAdStaticSocConfig_cpst->LocalIpAddrChgCbkULInfo_pst->CbkULsIdxList_pu8[lLoop_u8]].soAdLocalIpAddrAsgnChgCbk_pfn(\
                                                                                lSoAdStaticSocConfig_cpst->soConId_uo, State);
            }
        }/* for (lIdxSoCon_uo=0; lIdxSoCon_uo<SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo; lIdxSoCon_uo++) */
    }/*End of If condition: Check SoAd Module should be Initialized */
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */



/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv_Domain.h"


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
 ***************************************************************************************************
 * \Function Name : SoAd_SetUniqueRemoteAddr()
 *
 * \Function description
 * The TCP/IP stack calls this function after a socket was set into the listen state with TcpIp_TcpListen() and a
 * TCP connection is requested by the peer.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId : Index of any socket connection that is part of the SoAdSocketConnectionGroup.
 * \param   TcpIp_SockAddrType
 * \arg     RemoteAddrPtr : Pointer to the structure containing the requested remote IP address and port..
 * \param   SoAd_SoConIdType
 * \arg     AssignedSoConIdPtr: Pointer to the SoAd_SoConIdType where the index of the socket connection
 *                              configured with the remote address (IpAddrPtr) shall be stored..
 *
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK :      The request was accepted.
 *          E_NOT_OK:   The request was rejected and AssignedSoConIdPtr remains unchanged.
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_SetUniqueRemoteAddr( SoAd_SoConIdType           SoConId, \
                                         const TcpIp_SockAddrType * RemoteAddrPtr, \
                                         SoAd_SoConIdType *         AssignedSoConIdPtr)
{
    /* Local pointer to store the Dynamic array one by one */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold the return value */
    Std_ReturnType               lFunctionRetVal_u8;

    /* variable to store the return value of internal function */
    Std_ReturnType               lRetVal_u8;

    /* variable to hold index of static soc table */
    SoAd_SoConIdType             lIdxStaticSocket_uo;

    /* variable to store the dynamic socket connection table index */
    SoAd_SoConIdType             lIdxDynSocket_uo;

    /* Variable to get the socket connection groupe Id */
    SoAd_SoConIdType             lSoConGrpId_uo;

    /* Variable to store the port for remote address */
    uint16                       lRemoteAddrPort_u16;

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_SET_UNIQUE_REMOTE_ADDRESS_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ), SOAD_SET_UNIQUE_REMOTE_ADDRESS_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /* Check whether AssignedSoConIdPtr & RemoteAddrPtr pointers are valid or not */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( NULL_PTR == AssignedSoConIdPtr ) || ( NULL_PTR == RemoteAddrPtr ) ), SOAD_SET_UNIQUE_REMOTE_ADDRESS_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

    /********************************* End: DET Checking *************************************/

    /* get the port from socket AFTER checking socket pointer for NULL value */
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lRemoteAddrPort_u16 = SOAD_GET_PORT_FROM_SOCK_ADDR(RemoteAddrPtr);

    /* Check whether Domain and port are valid */
    SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo].domain_en != RemoteAddrPtr->domain ) || \
                             ( lRemoteAddrPort_u16 == SOAD_DHCP_RESERVED_CLIENT_PORT) || \
                             ( lRemoteAddrPort_u16 == SOAD_DHCP_RESERVED_SERVER_PORT ) ), \
                                 SOAD_SET_UNIQUE_REMOTE_ADDRESS_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

    /* Initialize the local variables which are declared above */
    lIdxStaticSocket_uo     = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

    /* Get the socket connection group Id */
    lSoConGrpId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo;
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lRemoteAddrPort_u16 = SOAD_GET_PORT_FROM_SOCK_ADDR(RemoteAddrPtr);


    /* As per SoAd old design 0xFFFFU was treated as wildcard remote address
     * As per Diamler Importer scripts, 0 has to be treated as wild card.
     * So, SoAd will treat both 0xFFFFU and 0 as wildcard by considering the backward compatibility and TCPIP_PORT_ANY will be used
     * as wildcard inside SoAd component. So It will be overwritten with TCPIP_PORT_ANY if port is 0xFFFFU */
    if( 0xFFFFU == lRemoteAddrPort_u16)
    {
        lRemoteAddrPort_u16 = TCPIP_PORT_ANY;
    }

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00675] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** The function SoAd_SetUniqueRemoteAddr() shall check if one of the socket connections of the socket        ***/
    /** connection group, identified by SoConId, is already configured with the address specified by              ***/
    /** RemoteAddrPtr. In this case, it shall return the socket connection index via AssignedSoConIdPtr           ***/
    /**                             and return E_OK                                                               ***/
    /****************************************************************************************************************/

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00676] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** If no socket connection is already configured with the address specified by RemoteAddrPtr,                ***/
    /** SoAd_SetUniqueRemoteAddr() shall                                                                          ***/
    /** (1) choose an unused socket connection using the best match algorithm described in [SWS_SoAd_00680]       ***/
    /** (2) set it to the remote address specified by RemoteAddrPtr                                               ***/
    /** (3) set AssignedSoConIdPtr to the index of the chosen socket connection and                               ***/
    /** (4) return E_OK.                                                                                          ***/
    /** A socket connection is 'unused' if its actual remote address has an IP address wildcard and/or port       ***/
    /** wildcard                                                                                                  ***/
    /****************************************************************************************************************/

    /****************************************************************************************************************/
    /****************************************** [SWS_SoAd_00678] ****************************************************/
    /****************************************************************************************************************/
    /****************************************************************************************************************/
    /** SoAd_SetUniqueRemoteAddr() shall reject the request and return E_NOT_OK if there are no unused socket     ***/
    /** connections within the socket connection group identified by SoConId                                      ***/
    /****************************************************************************************************************/

    /* Above listed all 3 requirements are covered at below function */
    /* get the Best Macht Algorithm */

    lRetVal_u8 = SoAd_BestMatchAlgorithm( SoConId, lSoConGrpId_uo, RemoteAddrPtr, &lIdxStaticSocket_uo );

    if ( E_OK == lRetVal_u8 )
    {
        /* Get the dynamic socketId of the static socketId */
        lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxStaticSocket_uo].SoAd_Prv_idxDynSocTable_puo);

        lSoAdDyncSocConfig_pst = &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        /* Load the remote address value in dynamic socket table */
        /* If remote port is 0xFFFFU then remote port address is stored as TCPIP_PORT_ANY in dynamic socket table else remote port is stored with the  port value in RemoteAddrPtr */
        /* Since RemoteAddrPtr is a constant pointer , the remote port passed in RemoteAddrPtr is not overwritten and the remote port to be stored is passed as an argument. */

        /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
        SchM_Enter_SoAd_RemoteAddrUpdate();

        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr(RemoteAddrPtr, lRemoteAddrPort_u16, lSoAdDyncSocConfig_pst);

        SchM_Exit_SoAd_RemoteAddrUpdate();

        /* return the socket connection index via AssignedSoConIdPtr */
        *AssignedSoConIdPtr = lIdxStaticSocket_uo;

        /* The request was accepted */
        lFunctionRetVal_u8 = E_OK;
    }

    return(lFunctionRetVal_u8);
}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */

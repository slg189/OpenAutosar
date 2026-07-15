

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "EthIf.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/* TcpIp_GetPhysAddr()  - Obtains the physical source address used by the EthIf controller implicitly specified via */
/*                      LocalAddrId                                                                                 */
/*                                                                                                                  */
/* Service ID           - 0x11                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant                                                                             */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId     - Local address identifier implicitly specifying the EthIf controller for which the physical    */
/*                      address shall be obtained.                                                                  */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  PhysAddrPtr     - Pointer to the memory where the physical source address (MAC address) in network byte         */
/*                      order is stored.                                                                            */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                      */
/*                        E_NOT_OK: The request was not successful,                                                 */
/*                                  e.g. no unique Ctrl specified via IpAddrId.                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_GetPhysAddr( TcpIp_LocalAddrIdType LocalAddrId,
                                  uint8 * PhysAddrPtr )
{
    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_GET_PHYSADDR_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                      TCPIP_E_GET_PHYSADDR_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Report DET if PhysAddrPtr is a Null Pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == PhysAddrPtr ), TCPIP_E_GET_PHYSADDR_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    EthIf_GetPhysAddr( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalAddrId].EthIfCtrlRef_u8,
                       PhysAddrPtr );

    return E_OK;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

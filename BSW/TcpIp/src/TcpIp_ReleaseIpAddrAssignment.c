

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/* TcpIp_ReleaseIpAddrAssignment() - This API service the local IP address assignment for the IP address specified  */
/*                          by LocalAddrId shall be released.                                                       */
/*                                                                                                                  */
/* Sync/Async            - Asynchronous                                                                             */
/* Reentrancy            - Reentrant                                                                                */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      LocalAddrId     - IP address index specifying the IP address for which an assignment shall  be initiated.   */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/*      Std_ReturnType    E_OK: The request has been accepted.                                                      */
/*                        E_NOT_OK: The request has not been accepted.                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_ReleaseIpAddrAssignment( const TcpIp_LocalAddrIdType LocalAddrId )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *    lLocalAddrConfig_pcst;
    uint8                               lAddrAssgntMethIdx_u8;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_RELEASE_IP_ADDRASSIGNMENT_API_ID,
                                      TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),
                                      TCPIP_E_RELEASE_IP_ADDRASSIGNMENT_API_ID, TCPIP_E_INV_ARG, E_NOT_OK )

    /* Update the pointer to LocalAddrId structure */
    lLocalAddrConfig_pcst = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId );

    /* Loop through all address assignment methods configured for the LocalAddrId and stop the assignment methods */
    for(lAddrAssgntMethIdx_u8 = 0; lAddrAssgntMethIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8; lAddrAssgntMethIdx_u8++ )
    {
        /* Trigger release sequence if assignement is MANUAL. */
        /* (release need to be executed independent of the currently assigned method and independent of assignment pending request bit because ongoing assignement in probing sequence need also to be stopped) */
        /* (in probing sequence assignement method is still INVALID and assignment pending request bit is cleared) */
        if( TCPIP_MANUAL == lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssgntMethIdx_u8].AddrAsgnmentTrig_en )
        {
           /* Enter critical section: The release address assignment method variable can be updated in different execution context and should be protected in a critical section. */
            SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign();

           /* Set IP assignment release request bit */
           TCPIP_UINT8_SET_BIT_WITH_SHIFT((lLocalAddrConfig_pcst->LocalIpReqRel_pst->ReleaseAddrAssgnMeth_u8), (lAddrAssgntMethIdx_u8));

           /* Clear IP assignment pending request bit */
           TCPIP_UINT8_CLEAR_BIT_WITH_SHIFT((lLocalAddrConfig_pcst->LocalIpReqRel_pst->RequestAddrAssgnMeth_u8), (lAddrAssgntMethIdx_u8));

           /* Exit critical section. */
           SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign();
        }
    }

    return E_OK;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */


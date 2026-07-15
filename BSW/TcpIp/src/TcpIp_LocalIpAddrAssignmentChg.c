

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
/* TcpIp_LocalIpAddrAssignmentChg() - This API is callback function called from modules such as AutoIp or DHCP after*/
/*                       a new IP address has been created. The new IP address is assigned to the corresponding     */
/*                       local address id                                                                           */
/*                                                                                                                  */
/* Sync/Async            - Asynchronous                                                                             */
/* Reentrancy            - Reentrant                                                                                */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8        - IP address index specifying the IP address for which an assignment shall be initiated.  */
/*  State_en              - IP address is assigned or unassigned.                                                   */
/*  IPAddrParam_pcun      - Pointer to a structure containing the assigned IP address, net mask and default router  */
/*                          IP address                                                                              */
/*  IPAddrAssignMethod_en - IP address assignment method - DHCP / AutoIp                                            */
/*  IpDomainType_cu32     - IP address domain type - TCPIP_AF_INET/ TCPIP_AF_INET6                                  */
/*                                                                                                                  */
/* Return :               void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
void TcpIp_LocalIpAddrAssignmentChg( TcpIp_LocalAddrIdType LocalAddrId_u8,
                                     TcpIp_IpAddrStateType State_en,
                                     const TcpIp_IPAddrParamType_tun * IPAddrParam_pcun,
                                     TcpIp_IpAddrAssignmentType IPAddrAssignMethod_en,
                                     const TcpIp_DomainType IpDomainType_cu32 )
{
    /* Local variables declaration */
    const TcpIp_LocalAddrConfig_to *   lLocalAddrConfig_pcst;
    TcpIp_AssignmentPrio_tu8           lIPAsgnPrioNew_u8;
    uint16                             lIdx_u16;
    uint8                              lAddrAssignmentlIdx_u8;

    /* Initialize local variables. */
    lIPAsgnPrioNew_u8 = 0xFFU;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialized */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_LOCAL_IP_ADDRASSIGNMENTCHG_API_ID, TCPIP_E_NOTINIT )

    /* Report DET if local address ID is invalid */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),  TCPIP_E_LOCAL_IP_ADDRASSIGNMENTCHG_API_ID , TCPIP_E_INV_ARG )

    /* Retrieve the local address configuration pointer*/
    lLocalAddrConfig_pcst   = ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco + LocalAddrId_u8 );

    /* Check if the domain of the localAddrId and that of the notified IP address match */
    TCPIP_DET_REPORT_ERROR_RET_VOID((lLocalAddrConfig_pcst->IpDomainType_u32 != IpDomainType_cu32),  TCPIP_E_LOCAL_IP_ADDRASSIGNMENTCHG_API_ID , TCPIP_E_INV_ARG )

    /* Check if the indicated state is ASSIGNED. */
    if( TCPIP_IPADDR_STATE_ASSIGNED == State_en )
    {
        /* Report DET if IPAddrParam_pcun is a Null Pointer */
        TCPIP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == IPAddrParam_pcun ), TCPIP_E_LOCAL_IP_ADDRASSIGNMENTCHG_API_ID, TCPIP_E_NULL_PTR )

        /* Initialize lIPAsgnPrioNew_u8. */
        lIPAsgnPrioNew_u8 = 0xFFU;

        /* Get the assignment priority of IP to be assigned. */
        for( lIdx_u16 = 0U; lIdx_u16 < ( lLocalAddrConfig_pcst->NumAddrAsgnment_u8 ); lIdx_u16++ )
        {
            if ( IPAddrAssignMethod_en == ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lIdx_u16].AddrAsgnmentMeth_en ))
            {
                lIPAsgnPrioNew_u8 = ( lLocalAddrConfig_pcst->AddrAsgnment_pcst[lIdx_u16].AsgnmentPrio_u8 );
                break;
            }
        }

        /* Enter critical section: The requested assignment priority can be changed in a different execution context via API TcpIp_LocalIPAddrAssignmentChg(). */
        /* ExclusiveAreaRequestedAssignPrioAccess is used to protect in case of reentrancy. If TcpIp_LocalIPAddrAssignmentChg() is reentered,                  */
        /* then the highest priority assignment might get overwritten by a lower priority assignment.                                                          */
        /* Hence the access to it shall be protected under a critical section.                                                                                 */
        SchM_Enter_TcpIp_ExclusiveAreaRequestedAssignPrioAccess();

        /* Check if the newly assigned IP address priority is equal to or greater than the already stored assignment change notification. */
        if( lIPAsgnPrioNew_u8 <= ( lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignPrio_u8 ))
        {
            /* Enter critical section: TcpIp_MainFunction() will read the same Local IP change global variable and reset its content, in a different execution context. */
            /* Therefore locking mechanism needs to be used. */
            SchM_Enter_TcpIp_ExclusiveAreaLocalIPChgParamAccess();

            /* Store the new address assignment parameters into the address assignment change data structure. */
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignAddressParam_un = *(IPAddrParam_pcun);
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignType_en = IPAddrAssignMethod_en;
            lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignPrio_u8 = lIPAsgnPrioNew_u8;

            /* Exit critical section. */
            SchM_Exit_TcpIp_ExclusiveAreaLocalIPChgParamAccess();
        }

        /* Exit critical section. */
        SchM_Exit_TcpIp_ExclusiveAreaRequestedAssignPrioAccess();

    }
    /* check if the indicated state is UNASSIGNED. */
    else if( TCPIP_IPADDR_STATE_UNASSIGNED == State_en )
    {
        /* Loop through all AddressAssignments configured for the requested LocalAddress. */
        for( lAddrAssignmentlIdx_u8 = 0U; (lAddrAssignmentlIdx_u8 < lLocalAddrConfig_pcst->NumAddrAsgnment_u8); lAddrAssignmentlIdx_u8++ )
        {
            /* If the assignment method match */
            if( IPAddrAssignMethod_en == lLocalAddrConfig_pcst->AddrAsgnment_pcst[lAddrAssignmentlIdx_u8].AddrAsgnmentMeth_en )
            {
                /* Enter critical section: TcpIp_MainFunction() will read the same Local IP change global variable and reset its content, in a different execution context. */
                /* Therefore locking mechanism needs to be used. */
                SchM_Enter_TcpIp_ExclusiveAreaLocalIPChgParamAccess();

                /* Store the unassigned notified address assignment type. */
                TCPIP_UINT8_SET_BIT_WITH_SHIFT((lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqUnassignType_u8), (lAddrAssignmentlIdx_u8));

                /* If there was already ASSIGNED state notification for the requested LocalAddress clear the address assignment data structure to initial values */
                if( IPAddrAssignMethod_en == lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignType_en )
                {
                    /* Set the assignment change notification to initial values. */
                    lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignType_en = TCPIP_IPADDR_ASSIGNMENT_INVALID;
                    lLocalAddrConfig_pcst->LocalIpChgAssigned_pst->ReqAssignPrio_u8 = 0xFFU;
                }
                /* Exit critical section. */
                SchM_Exit_TcpIp_ExclusiveAreaLocalIPChgParamAccess();

                break;
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    return;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

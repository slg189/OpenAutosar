

#include "TcpIp.h"
#include "TcpIp_Prv.h"

#if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) )
#include "rba_EthAutoIp.h"

#include "EthIf.h"
#if (!defined(ETHIF_AR_RELEASE_MAJOR_VERSION) || (ETHIF_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(ETHIF_AR_RELEASE_MINOR_VERSION) || (ETHIF_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif

#if (TCPIP_DEV_ERROR_DETECT != STD_OFF)
#include "Det.h"
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif
#endif

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp.h"
#endif

#include "rba_EthAutoIp_Prv.h"

#define RBA_ETHAUTOIP_START_SEC_VAR_CLEARED_8
#include "rba_EthAutoIp_MemMap.h"

/* Flag to indicate whether rba_EthAutoIp_Init has been executed or not */
boolean rba_EthAutoIp_InitFlag_b = FALSE;

#define RBA_ETHAUTOIP_STOP_SEC_VAR_CLEARED_8
#include "rba_EthAutoIp_MemMap.h"

#define RBA_ETHAUTOIP_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthAutoIp_MemMap.h"

/* Pointer - will be initialized with the DHCP Client structure in the Init Function */
static const rba_EthAutoIp_Config_to * rba_EthAutoIp_CurrConfig_pco;

#define RBA_ETHAUTOIP_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthAutoIp_MemMap.h"

/*
****************************************************************************************************
*                                   Prototype static Functions
****************************************************************************************************
*/
#define RBA_ETHAUTOIP_START_SEC_CODE
#include "rba_EthAutoIp_MemMap.h"

static void rba_EthAutoIp_CreateIpAddr( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst);

static uint32 rba_EthAutoIp_RandomTime( const uint8 * lHwAddr_au8 );

static void rba_EthAutoIp_HandleIpConflict( uint8 lIpStatus_u8,
                                            uint8 lTableIdx_u8 );

static void rba_EthAutoIp_Restart( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst  );

LOCAL_INLINE void  rba_EthAutoIp_ProcInitState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst);

LOCAL_INLINE void   rba_EthAutoIp_ProcProbeState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst);

LOCAL_INLINE void   rba_EthAutoIp_ProcAnnounceState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst);

LOCAL_INLINE void   rba_EthAutoIp_ProcStopState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst);

/*************************************************************************************************/
/* EthAutoIp_Init() - Initialize Auto Ip Structure                                               */
/*                                                                                               */
/* Input Parameters :                                                                            */
/* rba_EthAutoIp_Config_cst     - Pointer to the configuration data of the AutoIp module.        */
/*                                                                                               */
/* return   - void                                                                               */
/*************************************************************************************************/
void rba_EthAutoIp_Init( const rba_EthAutoIp_Config_to * rba_EthAutoIp_Config_cst )
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst * lAutoIp_SubCfg_pco;
    uint8                               lLoop_u8;
    uint8                               lIdxTable_u8;

    /* Report DET, If NULL_PTR is passed as Input*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR( (NULL_PTR == rba_EthAutoIp_Config_cst), RBA_ETHAUTOIP_E_APIINIT_ID, RBA_ETHAUTOIP_E_PARAM_POINTER )


        /* Copy General AutoIp structure into the local structure */
        rba_EthAutoIp_CurrConfig_pco = rba_EthAutoIp_Config_cst;
        /***************************************************************/

        for ( lIdxTable_u8 = 0; lIdxTable_u8 < rba_EthAutoIp_Config_cst->procConfigured_u8; lIdxTable_u8++ )
        {
            lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxTable_u8];

            /* Initialize state of the AutoIp function */
            lAutoIp_SubCfg_pco->autoIpTable_pst->entryState_en          = RBA_ETHAUTOIP_STATE_OFF;
            lAutoIp_SubCfg_pco->autoIpTable_pst->sentArpFrameCnt_u8         = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->lastConflict_u32           = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->triedLocalLinkIpAddr_u8    = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->autoIpState_en         = TCPIP_IPADDR_STATE_INIT;
            lAutoIp_SubCfg_pco->autoIpTable_pst->timeToWait_u32             = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->initTimeOut_u32            = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32        = 0;
            lAutoIp_SubCfg_pco->autoIpTable_pst->baseIpStatus_b         = FALSE;

            for ( lLoop_u8 = 0; lLoop_u8 < ETHIF_HWADDR_LEN; lLoop_u8++ )
            {
                lAutoIp_SubCfg_pco->autoIpTable_pst->srcHwAddr_au8[lLoop_u8] = 0;
            }

            if( TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL == lAutoIp_SubCfg_pco->assignmentMethod_en )
            {
                /* Initialize Time Parameters with values defined by RFC 3927 */
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeWait_u32      = (uint32)RBA_ETHAUTOIP_PROBE_WAIT_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeMin_u32           = (uint32)RBA_ETHAUTOIP_PROBE_MIN_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeMax_u32           = (uint32)RBA_ETHAUTOIP_PROBE_MAX_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeNum_u8            = (uint8) RBA_ETHAUTOIP_PROBE_NUM;

                lAutoIp_SubCfg_pco->autoIpTable_pst->announceNum_u8     = (uint8) RBA_ETHAUTOIP_ANNOUNCE_NUM;
                lAutoIp_SubCfg_pco->autoIpTable_pst->announceInterval_u32   = (uint32)RBA_ETHAUTOIP_ANNOUNCE_INTERVAL_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->announceWait_u32       = (uint32)RBA_ETHAUTOIP_ANNOUNCE_WAIT_TICK;
            }
            else if( TCPIP_IPADDR_ASSIGNMENT_LINKLOCAL_DOIP == lAutoIp_SubCfg_pco->assignmentMethod_en )
            {
                /* Initialize Time Parameters with values recommended by ISO 13400-2 */
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeWait_u32      = (uint32)RBA_ETHAUTOIP_DOIP_PROBE_WAIT_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeMin_u32           = (uint32)RBA_ETHAUTOIP_DOIP_PROBE_MIN_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeMax_u32           = (uint32)RBA_ETHAUTOIP_DOIP_PROBE_MAX_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->probeNum_u8            = (uint8) RBA_ETHAUTOIP_DOIP_PROBE_NUM;

                lAutoIp_SubCfg_pco->autoIpTable_pst->announceNum_u8     = (uint8)RBA_ETHAUTOIP_DOIP_ANNOUNCE_NUM;
                lAutoIp_SubCfg_pco->autoIpTable_pst->announceInterval_u32   = (uint32)RBA_ETHAUTOIP_DOIP_ANNOUNCE_INTERVAL_TICK;
                lAutoIp_SubCfg_pco->autoIpTable_pst->announceWait_u32       = (uint32)RBA_ETHAUTOIP_DOIP_ANNOUNCE_WAIT_TICK;
            }
            else
            {
                /* This case must never appear => Wrong configuration */
            }
        }

        /* Flag to indicate that AutoIp init function have been called */
        rba_EthAutoIp_InitFlag_b = TRUE;

}

/************************************************************************************************************/
/* EthAutoIp_Start() - Start the sequence to create a Local Link Ip Address and check if there              */
/* is no conflict with the IP generated.                                                                    */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* llocalAddrId_tu8 - IP address index specifying the IP address for which an assigment shall be done.      */
/*                                                                                                          */
/* Std_ReturnType   - Result of operation :                                                                 */
/*                      E_OK : AutoIP process have been started.                                            */
/*                      E_NOT_OK : AutoIP process have not been launched.                                   */
/*                                                                                                          */
/************************************************************************************************************/

Std_ReturnType rba_EthAutoIp_Start( TcpIp_LocalAddrIdType lLocalAddrId_tu8  )
{

    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst * lAutoIp_SubCfg_pco;
    uint8                               lIdxCounter_u8;
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    boolean                             lValidLocalAddr_b;
#endif

    /* Initialize Variables */
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    lValidLocalAddr_b  = FALSE;
#endif
    lAutoIp_SubCfg_pco = NULL_PTR;

    /* Report DET, If module is unintialized and this API is called*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APISTART_ID, RBA_ETHAUTOIP_E_NOTINIT )

    for(lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++)
    {
        /* Local Copy of each Specific Clients */
        lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

        if(lLocalAddrId_tu8 == lAutoIp_SubCfg_pco->localAddrId_tu8)
        {
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
            lValidLocalAddr_b = TRUE;
#endif
            break;
        }
    }
    /* Report DET, If Invalid argument is passed */
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( lValidLocalAddr_b == FALSE ), RBA_ETHAUTOIP_E_APISTART_ID, RBA_ETHAUTOIP_E_INV_ARG )

    /* Check the state of the AutoIp                                                                    */
    /* If  RBA_ETHAUTOIP_STATE_OFF, then AutoIp process is not Started, Trigger the assignment process  */
    /* If  !RBA_ETHAUTOIP_STATE_OFF, then process already started, Return E_OK                          */
    if( lAutoIp_SubCfg_pco->autoIpTable_pst->entryState_en == RBA_ETHAUTOIP_STATE_OFF )
    {
        rba_EthAutoIp_Restart(lAutoIp_SubCfg_pco);
    }

    return E_OK;
}

/************************************************************************************************************/
/* EthAutoIp_Stop() - Stop the Auto Ip sequence.                                                            */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* llocalAddrId_tu8 - IP address index specifying the IP address for which an assignment shall be initiated.*/
/*                                                                                                          */
/* return   - void                                                                                          */
/*                                                                                                          */
/************************************************************************************************************/

Std_ReturnType rba_EthAutoIp_Stop( TcpIp_LocalAddrIdType lLocalAddrId_tu8 )
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst * lAutoIp_SubCfg_pco;
    uint8                               lIdxCounter_u8;
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    boolean                             lValidLocalAddr_b;
#endif

    /* Variables Initialization */
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    lValidLocalAddr_b = FALSE;
#endif
    lAutoIp_SubCfg_pco = NULL_PTR;

    /* Report DET, If module is unintialized and this API is called*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APISTOP_ID, RBA_ETHAUTOIP_E_NOTINIT )

    for(lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++)
    {
        /* Local Copy of each Specific Clients */
        lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

        if(lLocalAddrId_tu8 == lAutoIp_SubCfg_pco->localAddrId_tu8)
        {
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
            lValidLocalAddr_b = TRUE;
#endif
            break;
        }
    }
    /* Report DET, If Invalid argument is passed */
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( lValidLocalAddr_b == FALSE ), RBA_ETHAUTOIP_E_APISTOP_ID, RBA_ETHAUTOIP_E_INV_ARG )


    lAutoIp_SubCfg_pco->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_STOP;

    return E_OK;
}

/*************************************************************************************************/
/* EthAutoIp_MainFunction() - Function called by an OS_Schedul_Task.                             */
/* This function is called to handle the AutoIP Process and send ARP announce and probe frame.   */
/*                                                                                               */
/* Input Parameters :                                                                            */
/* param    - void                                                                               */
/* return   - void                                                                               */
/*                                                                                               */
/*************************************************************************************************/
void rba_EthAutoIp_MainFunction ( void )
{
    /* Local variable declaration */

    const rba_EthAutoIp_SubConfig_tst *       lAutoIp_SubCfg_pco;
    rba_EthAutoIp_AutoIpState_ten             lEntryState_en;
    uint8                                     lIdxCounter_u8;

    /* Report DET, If module is unintialized and this API is called*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APIMAINFUNCTION_ID, RBA_ETHAUTOIP_E_NOTINIT )

        for ( lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++ )
        {
            /* Local Copy of each Specific Clients */
            lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

            /* Local Copy of the Entry State */
            lEntryState_en = lAutoIp_SubCfg_pco->autoIpTable_pst->entryState_en ;

            /* State of the Auto Ip function */
            switch( lEntryState_en )
            {

                /* AutoIP State Off */
                case RBA_ETHAUTOIP_STATE_OFF :
                {
                    /* If AutoIp Process is not running ( RBA_ETHAUTOIP_STATE_OFF ) */
                    /* Nothing to do */
                }
                break;


                /* AutoIP State Init */
                case RBA_ETHAUTOIP_STATE_INIT :
                {
                    rba_EthAutoIp_ProcInitState( lAutoIp_SubCfg_pco );
                }
                break;

                /* Probing State */
                case RBA_ETHAUTOIP_STATE_PROBING :
                {
                    rba_EthAutoIp_ProcProbeState( lAutoIp_SubCfg_pco );
                }
                break;

                /* Announcing State */
                case RBA_ETHAUTOIP_STATE_ANNOUNCING:
                {
                    rba_EthAutoIp_ProcAnnounceState( lAutoIp_SubCfg_pco );
                }
                break;

                /* Ip Bound State */
                case RBA_ETHAUTOIP_STATE_BOUND :
                {
                    /* In the bound state only the IP Conflict detection has to be handled */
                    if(lAutoIp_SubCfg_pco->autoIpTable_pst->lastConflict_u32 > 0U)
                    {
                        lAutoIp_SubCfg_pco->autoIpTable_pst->lastConflict_u32--;
                    }
                }
                break;

                /* Ip Stop State */
                case RBA_ETHAUTOIP_STATE_STOP :
                {
                    rba_EthAutoIp_ProcStopState( lAutoIp_SubCfg_pco );
                }
                break;

                default :
                    /*Nothing to do*/
                break ;
            }
        } /* End For Loop */

}


/*
****************************************************************************************************
*                       Functions used to the IP Conflict Detection functionality
****************************************************************************************************
*/

/*************************************************************************************************/
/* rba_EthAutoIp_GetIpAddress() - Function called by the ARP Component                           */
/* This function is called to provide the current IP Address in process to ARP to do the IP      */
/* Conflict detection internally.                                                                */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  lCtrlIdx_u8             - index of the Ethernet controller                                   */
/*                                                                                               */
/* Output Parameters :                                                                           */
/*  lLocalIpAddress_pu32    - AutoIp address currently generated and checked by the process.     */
/*  lCurrentIpState_pen     - AutoIp assignment state of the IP Address.                         */
/*                                                                                               */
/* return lRetVal_u8.                                                                            */
/*      E_OK : AutoIp table has been found and the AutoIp Address is well returned.              */
/*      E_NOT_OK : AutoIp table has not been found and the AutoIp Address is not returned        */
/*************************************************************************************************/

Std_ReturnType rba_EthAutoIp_GetIpAddress ( uint8 lCtrlIdx_u8,
                                            uint32 * lLocalIpAddress_pu32,
                                            TcpIp_IpAddrStateType * lCurrentIpState_pen )
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst *  lAutoIp_SubCfg_pco;
    uint8                                lIdxCounter_u8;
    Std_ReturnType                       lRetVal_tu8;

    /* Local variable initialization */
     lRetVal_tu8        = E_NOT_OK;
     lAutoIp_SubCfg_pco = NULL_PTR;

     /* Report DET, If module is unintialized and this API is called*/
     RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APIGETIPADDRESS_ID, RBA_ETHAUTOIP_E_NOTINIT )
     /* Report DET, If NULL_PTR is passed as Input*/
     RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( (NULL_PTR == lLocalIpAddress_pu32),    RBA_ETHAUTOIP_E_APIGETIPADDRESS_ID, RBA_ETHAUTOIP_E_PARAM_POINTER )
     /* Report DET, If NULL_PTR is passed as Input*/
     RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( (NULL_PTR == lCurrentIpState_pen),     RBA_ETHAUTOIP_E_APIGETIPADDRESS_ID, RBA_ETHAUTOIP_E_PARAM_POINTER )

        for ( lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++ )
        {
            /* Local Copy of each Specific Clients */
            lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

            if( lCtrlIdx_u8 == lAutoIp_SubCfg_pco->ctrlIdx_u8 )
            {
                /* Return the Current LinkLocalIpAddress generated */
                 *lLocalIpAddress_pu32  = (uint32)                  lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32;
                 *lCurrentIpState_pen   = (TcpIp_IpAddrStateType)   lAutoIp_SubCfg_pco->autoIpTable_pst->autoIpState_en;
                 lRetVal_tu8 = E_OK;
                 break;
            }
        }

     /* The process is ongoing (Not STOP and Not OFF)*/
     return lRetVal_tu8;
}

/*************************************************************************************************/
/* rba_EthAutoIp_IpConflictDetected() - Function called by rba_EthArp_RxIndication()             */
/* This API is called by rba_EthArp to inform the AutoIp component that an IP Conflict occurs    */
/* on the specific Controller given as parameters.                                               */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*          CtrlIdx_u8 - index of the Ethernet controller for which the Conflict occurs.         */
/*                                                                                               */
/* Return - E_OK/E_NOT_OK depending on the handling of the IP Conflict                           */
/*                                                                                               */
/*************************************************************************************************/

Std_ReturnType rba_EthAutoIp_IpConflictDetected( uint8 lCtrlIdx_u8)
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst * lAutoIp_SubCfg_pco;
    uint8                               lIdxCounter_u8;
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    boolean                             lValidCtrlId_u8;
#endif

     /* Local variable initialization */
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
     lValidCtrlId_u8 = FALSE;
#endif
     lAutoIp_SubCfg_pco = NULL_PTR;

     /* Report DET, If module is unintialized and this API is called*/
     RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APICONFLICTDETECTED_ID, RBA_ETHAUTOIP_E_NOTINIT )

        for ( lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++ )
        {
            /* Local Copy of each Specific Clients */
            lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

            /* Check if the Ctrl Index is stored in the AutoIp Table */
            if( lCtrlIdx_u8 == lAutoIp_SubCfg_pco->ctrlIdx_u8 )
            {
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
                 lValidCtrlId_u8 = TRUE;
#endif
                 break;
            }
        }
     /* Report DET, If Invalid argument is passed */
     RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( lValidCtrlId_u8 == FALSE ), RBA_ETHAUTOIP_E_APICONFLICTDETECTED_ID, RBA_ETHAUTOIP_E_INV_ARG )


        /* The Controller Index has been found into one AutoIp process */
        if(TCPIP_IPADDR_STATE_ASSIGNED == lAutoIp_SubCfg_pco->autoIpTable_pst->autoIpState_en)
        {
            rba_EthAutoIp_HandleIpConflict( RBA_ETHAUTOIP_DEFENDING, lIdxCounter_u8 );
        }
        else if (TCPIP_IPADDR_STATE_UNASSIGNED == lAutoIp_SubCfg_pco->autoIpTable_pst->autoIpState_en)
        {
            /* The process is ongoing (Not STOP and Not OFF)*/
            /* => The process must be restarted to generate a new IP Address */

            /* Increment the number of tries */
            lAutoIp_SubCfg_pco->autoIpTable_pst->triedLocalLinkIpAddr_u8++;

            /* Start a new sequence */
            rba_EthAutoIp_Restart( lAutoIp_SubCfg_pco );
        }
        else
        {
            /* Nothing to do. */
            /* The Process is stopped : State == RBA_ETHAUTOIP_STATE_OFF || RBA_ETHAUTOIP_STATE_STOP */
            /* The Conflict is not considered because the process will be restarted with the next */
            /* calling of the StartFunction */
        }

        /* The conflict has been handled */
    return E_OK;
}


/*************************************************************************************************/
/* rba_EthAutoIp_AllocBaseIpAddr() - Function called by the TCPIP Component                      */
/* This function is called to provide the Base IP Address to be used for the assignment          */
/*                                                                                               */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  llocalAddrId_tu8 - IP address index specifying the IP address for which an                   */
/*                     assigment shall be done.                                                  */
/*  LocalIpAddr_pst   - Pointer to the Ip address to be used for the assignment.                 */
/*                                                                                               */
/* Output Parameters :                                                                           */
/*  NIL                                                                                          */
/*                                                                                               */
/* return lRetVal_u8.                                                                            */
/*      E_OK : The Base Ip address is set for the IP assignment                                  */
/*      E_NOT_OK : The Base Ip address is NOT set for the IP assignment                          */
/*************************************************************************************************/

Std_ReturnType rba_EthAutoIp_AllocBaseIpAddr( TcpIp_LocalAddrIdType lLocalAddrId_tu8,
                                              const TcpIp_SockAddrInetType * LocalIpAddr_pst )
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst * lAutoIp_SubCfg_pco;
    Std_ReturnType                      lRetVal_tu8;
    uint8 lIdxCounter_u8;
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    boolean                             lValidLocalAddr_b;
#endif

    /* Initialize Variables */
    lRetVal_tu8 = E_NOT_OK;
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
    lValidLocalAddr_b = FALSE;
#endif
    lAutoIp_SubCfg_pco = NULL_PTR;

    /* Report DET, If module is unintialized and this API is called*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( rba_EthAutoIp_InitFlag_b == FALSE ), RBA_ETHAUTOIP_E_APIALLOCBASEIPADDR_ID, RBA_ETHAUTOIP_E_NOTINIT )
    /* Report DET, If NULL_PTR is passed as Input*/
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( (NULL_PTR == LocalIpAddr_pst), RBA_ETHAUTOIP_E_APIALLOCBASEIPADDR_ID, RBA_ETHAUTOIP_E_PARAM_POINTER )

        for(lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthAutoIp_CurrConfig_pco->procConfigured_u8); lIdxCounter_u8++)
        {
            /* Local Copy of each Specific Clients */
            lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lIdxCounter_u8] ;

            if(lLocalAddrId_tu8 == lAutoIp_SubCfg_pco->localAddrId_tu8)
            {
#if(TCPIP_DEV_ERROR_DETECT != STD_OFF)
                lValidLocalAddr_b = TRUE;
#endif
                break;
            }
        }
    /* Report DET, If Invalid argument is passed */
    RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( ( lValidLocalAddr_b == FALSE ), RBA_ETHAUTOIP_E_APIALLOCBASEIPADDR_ID, RBA_ETHAUTOIP_E_INV_ARG )

    /* If DET is OFF, Process request only when Local address is matched */
    /* Check if the Base Ip address is already set*/
    if (lAutoIp_SubCfg_pco->autoIpTable_pst->baseIpStatus_b == FALSE)
    {
        /* Set the Base Ip Address state to True to indicate IP address is available for the assignment */
        lAutoIp_SubCfg_pco->autoIpTable_pst->baseIpStatus_b = TRUE;

        /* Store the IP address to be used for the Assignment */
        lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32 = LocalIpAddr_pst->addr[0];

        lRetVal_tu8 = E_OK;
    }

    return lRetVal_tu8;
}

/*
****************************************************************************************************
*                           Internal Functions of the AutoIP Component
****************************************************************************************************
*/

/************************************************************************************************************/
/* rba_EthAutoIp_ProcInitState() - Process the Init entry state of AutoIp process                           */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure                                                                     */
/*                                                                                                          */
/* Std_ReturnType   -   - void                                                                              */
/*                                                                                                          */
/************************************************************************************************************/

LOCAL_INLINE void   rba_EthAutoIp_ProcInitState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst)
{
    uint32 lRandomTime_u32;

    /* Configured TimeOut before the starting of the AutoIp Process */
    if(rba_EthAutoIp_tst->autoIpTable_pst->initTimeOut_u32 >= rba_EthAutoIp_CurrConfig_pco->initTimeout_u32)
    {
        /* Create random Ip Address */
        rba_EthAutoIp_CreateIpAddr( rba_EthAutoIp_tst );

        /* Erase Sent Probes counter */
        rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8 = 0;

        /* RFC 3927 Section 2.2.1 */
        /* if we tried more then MAX_CONFLICTS we must limit our rate for acquiring and probing address. */
        if(rba_EthAutoIp_tst->autoIpTable_pst-> triedLocalLinkIpAddr_u8 > RBA_ETHAUTOIP_MAX_CONFLICTS)
        {
            rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 = RBA_ETHAUTOIP_RATE_LIMIT_INTERVAL_TICK;
        }
        else
        {
            /* RFC 3927 Section 2.2.1 :*/
            /* Time to wait to first probe, this is randomly chosen out of 0 to PROBE_WAIT seconds.*/
            /* The using of modulo must be verified : Modulo can be used ???  */
            lRandomTime_u32 = (uint32) rba_EthAutoIp_RandomTime (& rba_EthAutoIp_tst->autoIpTable_pst->srcHwAddr_au8[0]);

            rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 =
                                (uint32) (lRandomTime_u32 % rba_EthAutoIp_tst->autoIpTable_pst->probeWait_u32);
        }

        /* Change the state of the AutoIP sequence => Next Step : Probing Sequence */
        rba_EthAutoIp_tst->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_PROBING;

        /* we can set the IP State as unassigned to activate the conflict */
        /* detection into the ARP Component */
        rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en = TCPIP_IPADDR_STATE_UNASSIGNED;
    }
    else
    {
        /* Increment the InitTimeOut Variable */
        rba_EthAutoIp_tst->autoIpTable_pst->initTimeOut_u32++;
    }

}

/************************************************************************************************************/
/* rba_EthAutoIp_ProcProbeState() - Process the Probing entry state of AutoIp process                       */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure                                                                     */
/*                                                                                                          */
/* Std_ReturnType   -   - void                                                                              */
/*                                                                                                          */
/************************************************************************************************************/

LOCAL_INLINE void   rba_EthAutoIp_ProcProbeState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst)
{
    Std_ReturnType lRetVal_tu8;
    uint32         lDiffTime_u32;
    uint32         lRandomTime_u32;

    /* Time To Wait before the sending of the next Probe Frame ??? */
    if( rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 > 0U)
    {
        /* Decrease Counter */
        rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32--;
    }
    else
    {


            #if (TCPIP_ARP_ENABLED == STD_ON)
            /* Send an ARP Probe frame */
            /* Probe Frame = ARP Request with Destination IP address = 0 */
            lRetVal_tu8 = rba_EthArp_SendArpFrame(
                                rba_EthAutoIp_tst->ctrlIdx_u8,
                                0,                      // idxIpInTable_u16 : Not used for ARP Probe Frame
                                rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32,
                                NULL_PTR,                               // Destination MAC address : Not used for ARP Probe Frame
                                RBA_ETHARP_ARP_PROBE_FRAME);
            #else
            /* E_NOT_OK and the Process is stopped by setting the state as STATE_STOP */
            lRetVal_tu8 = E_NOT_OK;
            rba_EthAutoIp_tst->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_STOP;
            #endif

            if( E_OK == lRetVal_tu8 )
            {


                /* Increase the counter of Probe Frames sent */
                rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8++;

                lDiffTime_u32 = (uint32)(rba_EthAutoIp_tst->autoIpTable_pst->probeMax_u32 -
                                         rba_EthAutoIp_tst->autoIpTable_pst->probeMin_u32);

                /* Avoid n%0 in case of AUTO_IP_DOIP Assignment Method */
                if( lDiffTime_u32 > 0U )
                {
                    /* calculate random time around fixed value to wait to next probe / MACRO must be verified */
                    lRandomTime_u32 = (uint32)(rba_EthAutoIp_RandomTime(&rba_EthAutoIp_tst->autoIpTable_pst->srcHwAddr_au8[0]));

                    rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 =
                            (uint32)((lRandomTime_u32 % lDiffTime_u32) + (rba_EthAutoIp_tst->autoIpTable_pst->probeMin_u32));
                }
                else
                {
                    rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 =
                            rba_EthAutoIp_tst->autoIpTable_pst->probeMin_u32;
                }
                /* If all Probe frames have been sent */
                if( rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8 >=
                            rba_EthAutoIp_tst->autoIpTable_pst->probeNum_u8)
                {
                    /* Entry State = Next Step : Gratuitous Sequence */
                    rba_EthAutoIp_tst->autoIpTable_pst->entryState_en      = RBA_ETHAUTOIP_STATE_ANNOUNCING;
                    rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8     = 0;
                    rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32     = rba_EthAutoIp_tst->autoIpTable_pst->announceWait_u32;
                }
                /* If still Probe frames to send */
                else
                {
                    /*Do nothing*/
                }
            }

    }

}

/************************************************************************************************************/
/* rba_EthAutoIp_ProcAnnounceState() - Process the Announce entry state of AutoIp process                   */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure                                                                     */
/*                                                                                                          */
/* Std_ReturnType   -   - void                                                                              */
/*                                                                                                          */
/************************************************************************************************************/

LOCAL_INLINE void   rba_EthAutoIp_ProcAnnounceState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst)
{
    TcpIp_IPAddrParamType_tun lIPv4AddrParam_un;
    Std_ReturnType            lRetVal_tu8;

    if(rba_EthAutoIp_tst->autoIpTable_pst->lastConflict_u32 > 0U)
    {
        /* Decrease the time counter updated when a conflict is detected */
        rba_EthAutoIp_tst->autoIpTable_pst->lastConflict_u32--;
    }
    else
    {
        /* Nothing to do */
    }

    /* Time To Wait before the sending of the next Gratuitous Frame ??? */
    if( rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 > 0U )
    {
        /* Decrease Counter */
        rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32--;
    }
    else
    {
        /* Store the new IP Address and send an ARP announce Frame. */
        /* We are here the first time, so we waited ANNOUNCE_WAIT seconds */

        #if (TCPIP_ARP_ENABLED == STD_ON)
        /* Send an announce ARP Frame */
        lRetVal_tu8 = rba_EthArp_SendArpGratuitousFrame(
                                    rba_EthAutoIp_tst->ctrlIdx_u8,
                                    rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32);
        #else
        /* E_NOT_OK and the Process is stopped by setting the state as STATE_STOP */
        lRetVal_tu8 = E_NOT_OK;
        rba_EthAutoIp_tst->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_STOP;
        #endif

        /* If Frame have been sent */
        if( E_OK == lRetVal_tu8 )
        {
            /* If current IP Address is not assigned */
            if(TCPIP_IPADDR_STATE_ASSIGNED != rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en)
            {
                /* RFC 5227 : page 12*/
                /* The host may begin legitimately using the IP address immediately after sending the first*/
                /* of the two ARP Announcements; the sending of the second ARP Announcement may be completed*/
                /* asynchronously, concurrent with other networking operations the host may wish to perform */

                lIPv4AddrParam_un.IPv4Par_st.IpAddr_u32  = rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32;
                lIPv4AddrParam_un.IPv4Par_st.DftRtr_u32  = 0; /* Parameter not used for AutoIp */
                lIPv4AddrParam_un.IPv4Par_st.NetMask_u32 = 0xFFFF0000U; /* Parameter not used for AutoIp */

                TcpIp_LocalIpAddrAssignmentChg(
                                    rba_EthAutoIp_tst->localAddrId_tu8,
                                    TCPIP_IPADDR_STATE_ASSIGNED,
                                    &lIPv4AddrParam_un,
                                    rba_EthAutoIp_tst->assignmentMethod_en,
                                    TCPIP_AF_INET );

                rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en = TCPIP_IPADDR_STATE_ASSIGNED;
            }

            /* Increment counter frame sent */
            rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8++;

            /* Reload Time Counter  */
            rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 =
                                                rba_EthAutoIp_tst->autoIpTable_pst->announceInterval_u32;

            /* All the ARP Announce frames have been sent ? */
            if(rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8 >=
                                                rba_EthAutoIp_tst->autoIpTable_pst->announceNum_u8)
            {
                rba_EthAutoIp_tst->autoIpTable_pst->entryState_en      = RBA_ETHAUTOIP_STATE_BOUND;
                rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8     = 0;
                rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32     = 0;
            }
            else
            {
                /* Nothing to do */
            }
        }
        else
        {

            /* If ARP announce frame not sent*/
            /* Nothing to do ( we keep the same state )*/

        }
    }


}

/************************************************************************************************************/
/* rba_EthAutoIp_ProcStopState() - Process the Stop entry state of AutoIp process                           */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure                                                                     */
/*                                                                                                          */
/* Std_ReturnType   -   - void                                                                              */
/*                                                                                                          */
/************************************************************************************************************/

LOCAL_INLINE void   rba_EthAutoIp_ProcStopState( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst)
{
    TcpIp_IPAddrParamType_tun lIPv4AddrParam_un;

    /* If the IP Address has been assigned, the same IP must be unassigned */
    if(TCPIP_IPADDR_STATE_ASSIGNED == rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en)
    {
        lIPv4AddrParam_un.IPv4Par_st.IpAddr_u32  = rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32;
        lIPv4AddrParam_un.IPv4Par_st.DftRtr_u32 = 0;    /* Parameter not used for AutoIp */
        lIPv4AddrParam_un.IPv4Par_st.NetMask_u32 = 0;   /* Parameter not used for AutoIp */

        /* The Process can be directly stop and the IP Address Unassigned */
        TcpIp_LocalIpAddrAssignmentChg(
                        rba_EthAutoIp_tst->localAddrId_tu8,
                        TCPIP_IPADDR_STATE_UNASSIGNED,
                        &lIPv4AddrParam_un,
                        rba_EthAutoIp_tst->assignmentMethod_en,
                        TCPIP_AF_INET );
    }
    else
    {
        /* Nothing To Do */
    }

    /* AutoIp Table variables to Re-Initialize */
    rba_EthAutoIp_tst->autoIpTable_pst->sentArpFrameCnt_u8         = 0;
    rba_EthAutoIp_tst->autoIpTable_pst->lastConflict_u32           = 0;
    rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32             = 0;
    rba_EthAutoIp_tst->autoIpTable_pst->initTimeOut_u32            = 0;
    rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32        = 0;

    /* Set the BaseIp status flag to False*/
    rba_EthAutoIp_tst->autoIpTable_pst->baseIpStatus_b = FALSE;

    /* State of the IP address as INIT to disable the Conflict Detection in ARP */
    rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en = TCPIP_IPADDR_STATE_INIT;

    /* Go to the OFF State */
    rba_EthAutoIp_tst->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_OFF;

}


/************************************************************************************************************/
/* rba_EthAutoIp_Restart() - ReStart the sequence to create a Local Link Ip Address and check if there      */
/* is no conflict with the IP generated.                                                                    */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure to fill with the IP Address generated.                              */
/*                                                                                                          */
/* Std_ReturnType   -   - void                                                                              */
/*                                                                                                          */
/************************************************************************************************************/

static void rba_EthAutoIp_Restart( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst  )
{
    /* Local variable declaration */
    uint8     lSrcEthAddr_au8[ETHIF_HWADDR_LEN];
    uint8     lLoop_u8;

        /* Get the source Ethernet address based on the controller index */
        EthIf_GetPhysAddr(  rba_EthAutoIp_tst->ctrlIdx_u8,
                                &lSrcEthAddr_au8[0] );

        /* Store the source MAC address in the AutoIp Table */
        /* MAC address is used in the process to generate random values */
        for ( lLoop_u8 = 0; lLoop_u8 < ETHIF_HWADDR_LEN; lLoop_u8++ )
        {
             /* MAC address is used in the process to generate random values */
            rba_EthAutoIp_tst->autoIpTable_pst->srcHwAddr_au8[lLoop_u8] = (uint8) lSrcEthAddr_au8[lLoop_u8];
        }

        /* Init AutoIp Table variables to start or restart the process */
        rba_EthAutoIp_tst->autoIpTable_pst->timeToWait_u32 = 0;

        /* Change the state of the AutoIP entry to modify the state in the next MainFunction */
        rba_EthAutoIp_tst->autoIpTable_pst->entryState_en = RBA_ETHAUTOIP_STATE_INIT;

        /* State of the IP address as INIT to disable the Conflict Detection in ARP */
        rba_EthAutoIp_tst->autoIpTable_pst->autoIpState_en = TCPIP_IPADDR_STATE_INIT;

}



/*************************************************************************************************/
/* rba_EthAutoIp_Handle_IpConflict()                                                             */
/* Function called when an IP Conflict occurs. Depending of the number of conflicts and the      */
/* state of the process, the IP Address is regenerated or defended.                              */
/*                                                                                               */
/* Input Parameters :                                                                            */
/* lIpStatus_u8 - Value that defines if the Ip address generated must be defended or immediately */
/*               modified.                                                                       */
/* lTableIdx_u8 - AutoIp Table index on which the conflict occurs.                               */
/*                                                                                               */
/* return   - void                                                                               */
/*                                                                                               */
/*************************************************************************************************/
static void rba_EthAutoIp_HandleIpConflict( uint8 lIpStatus_u8,
                                            uint8 lTableIdx_u8 )
{
    /* Local variable declaration */
    const rba_EthAutoIp_SubConfig_tst *     lAutoIp_SubCfg_pco;
    TcpIp_IPAddrParamType_tun               lIpParam_un;

    /* Local variable initialization */
    lAutoIp_SubCfg_pco = &rba_EthAutoIp_CurrConfig_pco->subConfig_cst[lTableIdx_u8];

      /* Somehow detect if we are defending or retreating */
    if ( lIpStatus_u8 == RBA_ETHAUTOIP_DEFENDING )
    {
        /* Retreat, there was a conflicting ARP in the last DEFEND_INTERVAL seconds */
        if( lAutoIp_SubCfg_pco->autoIpTable_pst->lastConflict_u32 > 0U)
        {
            lIpParam_un.IPv4Par_st.IpAddr_u32   = lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32;
            lIpParam_un.IPv4Par_st.DftRtr_u32 = 0; /* Parameter not used for AutoIp */
            lIpParam_un.IPv4Par_st.NetMask_u32  = 0; /* Parameter not used for AutoIp */

            /* Callback to inform TcpIp layer of a conflict */
            TcpIp_LocalIpAddrAssignmentChg (
                            lAutoIp_SubCfg_pco->localAddrId_tu8,
                            TCPIP_IPADDR_STATE_UNASSIGNED,
                            &lIpParam_un,
                            lAutoIp_SubCfg_pco->assignmentMethod_en,
                            TCPIP_AF_INET );

            /* State of the IP address modified as INIT to disable the Conflict Detection in ARP */
            /* in the Start Function  */

            /******************************/
            /* Restart the AutoIP Process */

            /* Increment the number of tries */
            lAutoIp_SubCfg_pco->autoIpTable_pst->triedLocalLinkIpAddr_u8++;
            /* Start a new sequence */
            rba_EthAutoIp_Restart( lAutoIp_SubCfg_pco );
            /******************************/
        }
        else
        {

            #if (TCPIP_ARP_ENABLED == STD_ON)
            /* Send an announce ARP Frame */
            (void)rba_EthArp_SendArpGratuitousFrame(
                            lAutoIp_SubCfg_pco->ctrlIdx_u8,
                            lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32);
            #endif

            lAutoIp_SubCfg_pco->autoIpTable_pst->lastConflict_u32 = RBA_ETHAUTOIP_DEFEND_INTERVAL_TICK;
        }
    }
    else
    {
        lIpParam_un.IPv4Par_st.IpAddr_u32   = lAutoIp_SubCfg_pco->autoIpTable_pst->localLinkIpAddr_u32;
        lIpParam_un.IPv4Par_st.DftRtr_u32 = 0; /* Parameter not used for AutoIp */
        lIpParam_un.IPv4Par_st.NetMask_u32  = 0; /* Parameter not used for AutoIp */

        /* Callback to inform TcpIp layer of a conflict */
        TcpIp_LocalIpAddrAssignmentChg (
                            lAutoIp_SubCfg_pco->localAddrId_tu8,
                            TCPIP_IPADDR_STATE_UNASSIGNED,
                            &lIpParam_un,
                            lAutoIp_SubCfg_pco->assignmentMethod_en,
                            TCPIP_AF_INET);

        /* State of the IP address modified as INIT to disable the Conflict Detection in ARP */
        /* in the Start Function  */

        /******************************/
        /* Restart the AutoIP Process */

        /* Increment the number of tries */
        lAutoIp_SubCfg_pco->autoIpTable_pst->triedLocalLinkIpAddr_u8++;
        /* Start a new sequence */
        rba_EthAutoIp_Restart( lAutoIp_SubCfg_pco );
        /******************************/
    }
}

/************************************************************************************************************/
/* EthAutoIp_Create_IpAddr() - Create a random Ip Address from source MAC Address and store this Ip in the  */
/* AutoIp Table.                                                                                            */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* rba_EthAutoIp_tst - AutoIp Structure to fill with the IP Address generated.                              */
/*                                                                                                          */
/* return   - void                                                                                          */
/*                                                                                                          */
/************************************************************************************************************/
static void rba_EthAutoIp_CreateIpAddr( const rba_EthAutoIp_SubConfig_tst * rba_EthAutoIp_tst )
{
    /* Local variable declaration */
    uint32     lAutoIpAddr_u32;

    /* Check if the Base IP address is set for the IP address assignment */
    if(rba_EthAutoIp_tst->autoIpTable_pst->baseIpStatus_b == FALSE)
    {
        /* Here we create an IP-Address out of range 169.254.1.0 to 169.254.254.255 */
        /* compliant to RFC 3927 Section 2.1. We have 254 * 256 possibilities */
        lAutoIpAddr_u32 =   (uint32)TCPIP_AUTOIP_RANGE_START +
                            (((uint32)rba_EthAutoIp_tst->autoIpTable_pst->srcHwAddr_au8[4]) |
                            ((uint32)rba_EthAutoIp_tst->autoIpTable_pst->srcHwAddr_au8[5] << 8U)) ;

        /* If previous conflict : Add the number of attempts to generate a good IP Address */
        lAutoIpAddr_u32 += rba_EthAutoIp_tst->autoIpTable_pst->triedLocalLinkIpAddr_u8;

        /* Take the lower 2 bytes generated and add it to the RBA_ETHAUTOIP_NET value */
        lAutoIpAddr_u32 =  TCPIP_AUTOIP_NET | (lAutoIpAddr_u32 & 0xFFFFU);
    }
    else /* If baseIpStatus_en == TRUE */
    {
        lAutoIpAddr_u32 = rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32;

        /* If previous conflict : Add the number of attempts to generate a good IP Address */
        lAutoIpAddr_u32 += (uint32)rba_EthAutoIp_tst->autoIpTable_pst->triedLocalLinkIpAddr_u8;
    }


    /* 169.254.0.0 <= addr <= 169.254.255.255 */
    if (lAutoIpAddr_u32 < TCPIP_AUTOIP_RANGE_START)
    {
        lAutoIpAddr_u32 += ((TCPIP_AUTOIP_RANGE_END - TCPIP_AUTOIP_RANGE_START) + 1U);
    }
    else if (lAutoIpAddr_u32 > TCPIP_AUTOIP_RANGE_END)
    {
        lAutoIpAddr_u32 -= ((TCPIP_AUTOIP_RANGE_END - TCPIP_AUTOIP_RANGE_START) + 1U);
    }
    else
    {
        /* The IP Address generated is already in the admitted range */
    }

    /* Store the Local Ip generated in the AutoIp struct */
    rba_EthAutoIp_tst->autoIpTable_pst->localLinkIpAddr_u32 = lAutoIpAddr_u32;
}

/************************************************************************************************************/
/* rba_EthAutoIp_RandomTime() - Create a random time from the source MAC Address.                           */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* lHwAddr_au8 - The Ethernet MAC Address from which generates the random time.                             */
/*                                                                                                          */
/* return   -                                                                                               */
/* lRandomTime_u16 - Random Time Generated.                                                                 */
/*                                                                                                          */
/************************************************************************************************************/
static uint32 rba_EthAutoIp_RandomTime( const uint8 * lHwAddr_au8 )
{

    /* Local variable declaration */
    uint32     lRandomTime_u32;

    /* Generate the random time from the MAC Address */
    lRandomTime_u32 =((((uint32)lHwAddr_au8[5] ) << 24U) |
                      (((uint32)lHwAddr_au8[3] ) << 16U) |
                      (((uint32)lHwAddr_au8[2] ) <<  8U) |
                       ((uint32)lHwAddr_au8[4] ));

    return lRandomTime_u32;
}

#define RBA_ETHAUTOIP_STOP_SEC_CODE
#include "rba_EthAutoIp_MemMap.h"

#endif /* #if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) ) */







#include "TcpIp.h"
#include "TcpIp_Prv.h"

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )
#include "rba_EthDHCP.h"

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

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#include "rba_BswSrv.h"

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"
#include "rba_EthDHCP_Prv.h"
#include "rba_EthDHCP_Client_Prv.h"

/*
****************************************************************************************************
* 									Declarations
****************************************************************************************************
*/

#define RBA_ETHDHCP_START_SEC_VAR_CLEARED_8
#include "rba_EthDHCP_MemMap.h"

/* State of DHCP_ClientInit() API call */
boolean rba_EthDHCP_ClientInitFlag_b = FALSE;

#define RBA_ETHDHCP_STOP_SEC_VAR_CLEARED_8
#include "rba_EthDHCP_MemMap.h"

#define RBA_ETHDHCP_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthDHCP_MemMap.h"

/* Structure used by APIs to specify an IP address */
TcpIp_SockAddrInetType rba_EthDHCP_SrvrSockAddr_st;
/* Pointer - will be initialized with the DHCP Client structure in the Init Function */
const rba_EthDHCP_ClntConfig_tst * rba_EthDHCP_ClientCurrConfig_pco;

#define RBA_ETHDHCP_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthDHCP_MemMap.h"

/*
****************************************************************************************************
*                                   Prototype static Functions
****************************************************************************************************
*/

#define RBA_ETHDHCP_START_SEC_CODE
#include "rba_EthDHCP_MemMap.h"


static void rba_EthDHCP_ClientHandleIpConflict( uint8 lClientIdx_u8 );

static void rba_EthDHCP_ClientLocalIpAddrAssignmentChg( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                                        TcpIp_IpAddrStateType lTypeAssignmentChange_en);

static Std_ReturnType rba_EthDHCP_ClientGetSocket( const rba_EthDHCP_ClntSubConfig_tst * lClientSubConfig_st);

static void rba_EthDHCP_ClientCloseSocket( rba_EthDHCPClient_Table_tst * lClientStruct_st );

static Std_ReturnType rba_EthDHCP_ClientSendDhcpFrame( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                       uint8 lTypeOfFrame_u8);

static void rba_EthDHCP_ClientUpdateDhcpFrame( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                               rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                               uint8 lTypeOfFrame_u8,
                                               uint16 * lLengthOfFrame_u16);

static void rba_EthDHCP_ClientUpdateTxBuffer( const rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                              uint8 * lClntFrmBuf_au8,
                                              uint16 lLengthOfFrame_u16);

static void rba_EthDHCP_ClientParseOptionPart( rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_pst,
                                               const uint8 * lDataPtr_pu8,
                                               uint16 lDataLength_u16);

static uint8 rba_EthDHCP_ClientGetOption( const uint8 * lDataPtr_pu8,
                                          uint8 * lOptionPtr_pu8,
                                          uint8 lOptionCode_u8,
                                          uint16 lDataLength_u16);

static void rba_EthDHCP_ClientUpdateOptionsInClientStruct( const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_pst,
                                                           rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst);

static void rba_EthDHCP_ClientCleanToRestart( rba_EthDHCPClient_Table_tst * lEthDHCP_Client_st);


static void rba_EthDHCP_ClientRetransmission( rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst);


static TcpIp_ReturnType rba_EthDHCP_ClientGetMacIDForDestIp( uint8 lCtrlIdx_u8,
                                                             const rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst);

static uint32 rba_EthDHCP_ClientMul_u32u32_u32_Inl( uint32 lTimerVal_u32,
                                                    uint32 lTickVal_u32);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Init ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Select ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Request ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Probe ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Bound ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Inform ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Renew ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Rebind ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Decline ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Release ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Stop ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientRx_Offer ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                                const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_st,
                                                uint32 lRxIpAddressToAssign_u32);

LOCAL_INLINE void  rba_EthDHCP_ClientRx_Ack ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                              const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_st,
                                              uint32 lRxIpAddressToAssign_u32,
                                              uint32 lXIdDhcpRxData_u32);

LOCAL_INLINE void  rba_EthDHCP_ClientRx_Nack ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco);

LOCAL_INLINE void  rba_EthDHCP_ClientUpdate_DiscoverFrm ( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                          rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                                          uint16 * lOffset_u16);

LOCAL_INLINE void  rba_EthDHCP_ClientUpdate_RequestFrm ( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                         rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                                         uint16 * lOffset_u16);


/*
****************************************************************************************************
*                                   DHCP Client Functions (Extern)
****************************************************************************************************
*/

/************************************************************************************************************/
/* rba_EthDHCPClient_Init() - Initialize the DHCP Client.                                                   */
/*                                                                                                          */
/* Input Parameters :       void                                                                            */
/* Output Parameters :      void                                                                            */
/*                                                                                                          */
/************************************************************************************************************/
void rba_EthDHCP_ClientInit( const rba_EthDHCP_ClntConfig_tst * rba_EthDHCP_ClientConfig_cst )
{
    /* Local variable declarations - Client */
    const rba_EthDHCP_ClntSubConfig_tst *      lDHCP_ClientSubCfgPtr_pco;
    uint8                                      lIdxClnt_u8;
    uint8                                      lLoop_u8;

    /* Local variable Initialization - Client */
    lIdxClnt_u8                     = 0;
    lLoop_u8                        = 0;
    lDHCP_ClientSubCfgPtr_pco       = NULL_PTR;
    rba_EthDHCP_ClientInitFlag_b    = FALSE;

    RBA_ETHDHCP_DET_REPORT_ERROR( NULL_PTR == rba_EthDHCP_ClientConfig_cst, RBA_ETHDHCP_E_APICLIENTINIT_ID, RBA_ETHDHCP_E_PARAM_POINTER )


        /* Copy General DHCP Client structure into the local structure */
        rba_EthDHCP_ClientCurrConfig_pco = rba_EthDHCP_ClientConfig_cst;


        /* Initialization of the Client Table for each Client configured */
        for(lIdxClnt_u8 = 0; lIdxClnt_u8 < ((uint8) rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxClnt_u8++)
        {
            /* Local Copy of each Specific Clients */
            lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

            /* Process Part */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten               = RBA_ETHDHCP_CLNTOFF;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procSocketState_ten         = RBA_ETHDHCP_SOCK_NOTBOUND;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procSocketId_u16            = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32                 = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32  = RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntConflict_u8          = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntLastConflict_u32     = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32             = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxProbeFr_u8         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxGratuFr_u8         = 0;
            /* Client Part */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32               = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXIdInform_u32         = 0;

            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en     = TCPIP_IPADDR_STATE_INIT;
            /* HW Address */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[0]         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[1]         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[2]         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[3]         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[4]         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[5]         = 0;
            /* Server Part */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverIpAddress_u32         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten          = RBA_ETHDHCP_SRVROFF;

            /* Option Part Initialization */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionClientIdFlag_b        = FALSE;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstListFlag_b   = TRUE;

            for(lLoop_u8 = 0; lLoop_u8 < RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH; lLoop_u8++)
            {
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[lLoop_u8] = 0;
            }

            /* Values received from the Server */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionMessType_u8           = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionClientId_u32          = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionLeaseTime_u32         = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionSubnetMaskId_u32      = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionServerId_u32          = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionRenewalTime_u32       = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionRebindingTime_u32     = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16              = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16              = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32= 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b = FALSE;

        }

        /* Set init flag to true  */
        rba_EthDHCP_ClientInitFlag_b    = TRUE;

}

/************************************************************************************************************/
/* rba_EthDHCPClient_Start() - Start the DHCP sequence to get an IP address from DHCP server                */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* lLocalAddrId_tu8 - IP address index specifying the IP address for which an assignment shall be done.     */
/*                                                                                                          */
/* Std_ReturnType   - Result of operation :                                                                 */
/*                      E_OK : DHCP process has been started.                                               */
/*                      E_NOT_OK : DHCP process has not been launched.                                      */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientStart( TcpIp_LocalAddrIdType lLocalAddrId_tu8  )
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;

    uint8                      lIdxCounter_u8;
    uint8                      lLoop_u8;
    uint8                      lDestEthAddr_au8[RBA_ETHDHCP_HWADDR_LENGTH];
    Std_ReturnType             lStdRet_en;
    Std_ReturnType             lRetVal_tu8;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    boolean lValidLocalAddr_b;
#endif

    /* Local variable initialization */
    lStdRet_en                  = E_NOT_OK;
    lRetVal_tu8                 = E_NOT_OK;
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    lValidLocalAddr_b           = FALSE;
#endif

    /* Check whether the Client DHCP module has been initialized  */
     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTSTART_ID, RBA_ETHDHCP_E_NOTINIT )

        for(lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxCounter_u8++)
        {
            /* Local Copy of each Specific Clients */
            lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxCounter_u8] ;

            /* Check if the Client has not been stored previously into the Client Table */
            if(lLocalAddrId_tu8 == lDHCP_ClientSubCfgPtr_pco->ClntLocalAddrId_u8)
            {
                #if( TCPIP_DEV_ERROR_DETECT == STD_ON )
                /* If lLocalAddrId_tu8 is found */
                lValidLocalAddr_b           = TRUE;
                #endif

                break;
                /* Break the Loop, the current pointer is kept for the next step */
            }
        }

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lValidLocalAddr_b == FALSE), RBA_ETHDHCP_E_APICLIENTSTART_ID, RBA_ETHDHCP_E_INV_ARG )


        /* Check if the client process is not running */
        if(RBA_ETHDHCP_CLNTOFF == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten)
        {

                /* Get controller MAC address */
                lStdRet_en = TcpIp_GetPhysAddr(lLocalAddrId_tu8, (uint8 *) &(lDestEthAddr_au8[0]));

                if( E_OK == lStdRet_en)
                {
                    /* Initialize variables  */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32 = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32         = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFF;

                    /* Store the Client MAC address into the Table */
                    for(lLoop_u8 = 0; lLoop_u8 < RBA_ETHDHCP_HWADDR_LENGTH; lLoop_u8++)
                    {
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[lLoop_u8] = lDestEthAddr_au8[lLoop_u8];
                    }

                    /****************/
                    /* Options Part */
                    /****************/
                    if(TRUE == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstListFlag_b)
                    {
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[0]= RBA_ETHDHCP_OPTION_SUBNETMASK_ID;
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[1]= RBA_ETHDHCP_OPTION_ROUTER_ID;
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[2]= RBA_ETHDHCP_OPTION_RENEWALTIME_ID;
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[3]= RBA_ETHDHCP_OPTION_REBINDINGTIME_ID;
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionParamRqstList_au8[4]= RBA_ETHDHCP_OPTION_FQDN_ID;
                    }

                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionClientId_u32      = 0;


                    /***********************************/
                    /* Values received from the Server */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionMessType_u8           = 0;        /* Massage Type */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionServerId_u32          = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionLeaseTime_u32         = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionRouter_u32            = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionSubnetMaskId_u32      = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16              = 0;    /*Reset FQDN length for every new DHCP Start sequence*/
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16              = 0;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32= 0;


                    /* Set the Client to the Init state to start the process in the next MainFunction */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;

                    lRetVal_tu8 = E_OK;
                }
                else
                {
                    /* No MAC Address returned from the
                     * lLocalAddrId_tu8 given as parameter */
                    lRetVal_tu8 = E_NOT_OK;
                }

        }
        else
        {
            /* The client process is running no need to call Start
             * E_OK is returned to inform that the process is already started */
            lRetVal_tu8 = E_OK;
        }




    return lRetVal_tu8;
}

/************************************************************************************************************/
/* rba_EthDHCP_Stop() - Stop the DHCP sequence. The process is directly stopped without sending a Release   */
/* Frame. This function is called from TcpIp function in case of HW error.                                  */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/*  localAddrId_tu8 - IP address index specifying the IP address for which an assignment shall be stopped.  */
/*                                                                                                          */
/* Std_ReturnType   - Result of operation :                                                                 */
/*                      E_OK : DHCP process has been stopped.                                               */
/*                      E_NOT_OK : DHCP process has not been stopped.                                       */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientStop( TcpIp_LocalAddrIdType lLocalAddrId_tu8 )
{
    /* Local variable declarations - Client */
    const rba_EthDHCP_ClntSubConfig_tst *    lDHCP_ClientSubCfgPtr_pco;
    rba_EthDHCPClient_State_ten              lCurrentProcState_ten;
    uint8                                    lIdxClnt_u8;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    boolean                                  lValidLocalAddr_b;
#endif

    /* Local variable initialization */
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    lValidLocalAddr_b           = FALSE;
#endif
    lCurrentProcState_ten       = RBA_ETHDHCP_CLNTOFF;

    /* Check whether the Client DHCP module has been initialized  */
     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTSTOP_ID, RBA_ETHDHCP_E_NOTINIT )


        /* Search the entry to Stop the process */
        for(lIdxClnt_u8 = 0; lIdxClnt_u8 < ((uint8) rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxClnt_u8++)
        {
            /* Local Copy of each Specific Clients */
            lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

            lCurrentProcState_ten = lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten;

            /* LocalAddrId found in the table AND Process running */
            if(lLocalAddrId_tu8 == lDHCP_ClientSubCfgPtr_pco->ClntLocalAddrId_u8)
            {
                #if( TCPIP_DEV_ERROR_DETECT == STD_ON )
                lValidLocalAddr_b = TRUE;
                #endif

                break;
            }

        }
     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lValidLocalAddr_b == FALSE), RBA_ETHDHCP_E_APICLIENTSTOP_ID, RBA_ETHDHCP_E_INV_ARG )


        /* If the Process is running */
        if( (RBA_ETHDHCP_CLNTBOUND      == lCurrentProcState_ten) ||
            (RBA_ETHDHCP_CLNTRENEWING   == lCurrentProcState_ten) ||
            (RBA_ETHDHCP_CLNTREBINDING  == lCurrentProcState_ten) )
        {
            /* If BOUND/RENEW/REBIND : a DHCP RELEASE frame shall be sent */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTRELEASE;
        }
        else if ((RBA_ETHDHCP_CLNTOFF  != lCurrentProcState_ten) &&
                 (RBA_ETHDHCP_CLNTSTOP != lCurrentProcState_ten))
        {
            /* For all the others state different of STOP and OFF, go directly in the STOP State */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTSTOP;
        }
        else
        {
            /* Process already in OFF or STOP State
             * Nothing to do */
        }

        /* Entry has been found and the state modified => The Loop is broken */
        return E_OK;
}

/********************************************************************************************************************/
/*                                                                                                                  */
/* rba_EthDHCPClient_RxIndication() -                                                                               */
/*                      This API service is called by the rba_EthUdp module on reception of a DHCP frame.           */
/*                      This API check the type of frame received. If it is a DHCP-DISCOVER frame then it updates   */
/*                      the DHCP table with Client details.                                                         */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lSocketIdOut_u16    - SocketId on which the DHCP frame was received.                                            */
/*  lIPAddrPort_pst     - Pointer to IP address of the DHCPClient and the port on which the remote node transmitted */
/*                        this frame.                                                                               */
/*  lBufPtr_pu8         - uint8 pointer to a buffer containing a DHCP frame.                                        */
/*  lLength_u16         - Length of the received frame.                                                             */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
/* HIS METRIC LEVEL VIOLATION IN rba_EthDHCP_ClientRxIndication:
LEVEL- Design is optimal and cannot be split further.*/
void rba_EthDHCP_ClientRxIndication( TcpIp_SocketIdType lSocketIdOut_u16,
                                     const TcpIp_SockAddrType * lIPAddrPort_pst,
                                     const uint8 * lBufPtr_pu8,
                                     uint16 lLength_u16 )
{
    /* Local variable declarations */
    const rba_EthDHCP_ClntSubConfig_tst *        lDHCP_ClientSubCfgPtr_pco;
    rba_EthDHCPClient_OptionsTable_tst           lDHCP_ClientOptionsTable_st;

    uint32                                       lMagicCookieDhcp_u32;       /* Magic Cookie received */
    uint32                                       lXIdDhcpRxData_u32;         /* xId received */
    uint32                                       lRxIpAddressToAssign_u32;   /* Ip address received from the Server */
    uint8                                        lIdxClnt_u8;
    uint8                                        lIdxHwA_u8;
    uint8                                        lMessageType_u8;
    uint8                                        lHwType_u8;
    uint8                                        lHwAddrLen_u8;
    boolean                                      lClntHwAddrMatched_b;             /* Flag to check if CHADDR address is matched */

    /* Local variable initialization */
    lMagicCookieDhcp_u32        = 0;
    lXIdDhcpRxData_u32          = 0;
    lRxIpAddressToAssign_u32    = 0;
    lHwType_u8                  = 0;
    lHwAddrLen_u8               = 0;
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;
    lClntHwAddrMatched_b        = TRUE;


    /* Check whether the Client DHCP module has been initialized  */
     RBA_ETHDHCP_DET_REPORT_ERROR( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTRXINDICATION_ID, RBA_ETHDHCP_E_NOTINIT )

     RBA_ETHDHCP_DET_REPORT_ERROR( (lBufPtr_pu8 == NULL_PTR), RBA_ETHDHCP_E_APICLIENTRXINDICATION_ID, RBA_ETHDHCP_E_PARAM_POINTER )

     (void)lSocketIdOut_u16;
     (void)lIPAddrPort_pst;

     if( lLength_u16 >= RBA_ETHDHCP_MIN_RXFRAME_LEN )
     {
        lHwType_u8      = lBufPtr_pu8[RBA_ETHDHCP_HWTYPE_POS];
        lHwAddrLen_u8   = lBufPtr_pu8[RBA_ETHDHCP_HWADDR_LENGTH_POS];

        /* Get the Magic Cookie Value */
        TcpIp_ReadU32(  ( & ( lBufPtr_pu8[RBA_ETHDHCP_MAGIC_COOKIE_POS] ) ),
                        ( & lMagicCookieDhcp_u32 ) );

        if( ( RBA_ETHDHCP_HWTYPE_ETH        == lHwType_u8 )     &&
            ( RBA_ETHDHCP_HWADDR_LENGTH     == lHwAddrLen_u8 )  &&
            ( RBA_ETHDHCP_MAGICCOOKIE_VAL   == lMagicCookieDhcp_u32 ))
        {
            /* Get the XId Value */
            TcpIp_ReadU32(  (&(lBufPtr_pu8[RBA_ETHDHCP_XID_POS])), (&lXIdDhcpRxData_u32) );



            /* Check if the xId received is a Client xId contained in the database */
            for(lIdxClnt_u8 = 0; lIdxClnt_u8 < (rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxClnt_u8++)
            {
                lClntHwAddrMatched_b        = TRUE;

                /* Local Copy of the specific client */
                lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

                /* Compare stored hardware address with received hardware address */
                for(lIdxHwA_u8 = 0; lIdxHwA_u8 < (RBA_ETHDHCP_HWADDR_LENGTH); lIdxHwA_u8++)
                {
                    /* Match the CHADDR byte by byte with stored hardware address */
                    if(lBufPtr_pu8[ (RBA_ETHDHCP_CHADDR_POS + lIdxHwA_u8 )]  != lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientHwAddr_au8[lIdxHwA_u8] )
                    {
                        lClntHwAddrMatched_b = FALSE;
                        break;
                        /* Break loop if Received CHADDR does not match stored H/W ADDR  */
                    }

                }

                /* Check if the XiD and CHADDR received matches to a XiD's of a Client */
                if((( lXIdDhcpRxData_u32 == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32)||
                    ( lXIdDhcpRxData_u32 == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXIdInform_u32)) &&
                    ( TRUE == lClntHwAddrMatched_b ))
                {
                    /* Client is in the OFF state ? */
                    if( RBA_ETHDHCP_CLNTOFF != lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten )
                    {
                        /* Parse the Options part of the frame and get the options value */
                        rba_EthDHCP_ClientParseOptionPart ( &lDHCP_ClientOptionsTable_st,
                                                            &lBufPtr_pu8[RBA_ETHDHCP_OPTION_POS],
                                                            (lLength_u16 - RBA_ETHDHCP_OPTION_POS));

                        /* Read the IP address to assign received from the Server */
                        TcpIp_ReadU32(  ( &( lBufPtr_pu8[RBA_ETHDHCP_YIADDR_POS] ) ), ( & lRxIpAddressToAssign_u32 ) );

                        if(TRUE == lDHCP_ClientOptionsTable_st.rxOptionMessTypeRxFlag_b)
                        {
                            /* What is the message type of the frame received => Reading in the "Parse Options" part */
                            lMessageType_u8 = lDHCP_ClientOptionsTable_st.rxOptionMessType_u8;

                            switch(lMessageType_u8)
                            {
                                case RBA_ETHDHCP_OFFER :
                                {
                                    rba_EthDHCP_ClientRx_Offer ( lDHCP_ClientSubCfgPtr_pco,
                                                                 &lDHCP_ClientOptionsTable_st,
                                                                 lRxIpAddressToAssign_u32);
                                }
                                break;  /* End of RBA_ETHDHCP_OFFER case */

                                case RBA_ETHDHCP_ACK :
                                {
                                    rba_EthDHCP_ClientRx_Ack (    lDHCP_ClientSubCfgPtr_pco,
                                                                  &lDHCP_ClientOptionsTable_st,
                                                                  lRxIpAddressToAssign_u32,
                                                                  lXIdDhcpRxData_u32);
                                }
                                break; /* End of case RBA_ETHDHCP_ACK */

                                case RBA_ETHDHCP_NACK :
                                {

                                    rba_EthDHCP_ClientRx_Nack (lDHCP_ClientSubCfgPtr_pco);
                                }
                                break; /* End of RBA_ETHDHCP_NACK case */

                                default :
                                {
                                    /* Type Frame not defined */
                                }
                                break;
                            }
                        }
                    }
                    /* Break the loop, the Client has been found */
                    break;
                }
                else
                {
                    /* XId not found in the Client Structures
                     * => The frame must be discarded */
                }
            }
        }
        else
        {
            /* Magic Cookie OR HW Type OR HW Length
             * doesn't match with the DHCP Magic Cookie */
        }
     }


    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientRx_Offer()                                                                                     */
/* This function is called to process the received Offer message                                                    */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.                  */
/*  lDHCP_ClientOptionsTable_st         - client option table                                                       */
/*  lRxIpAddressToAssign_u32         - received IP address to be assigned                                           */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientRx_Offer ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                                const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_st,
                                                uint32 lRxIpAddressToAssign_u32)

{
    /* Client State == Selecting State ? */
    if(RBA_ETHDHCP_CLNTSELECTING == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten)
    {
        /* The first DHCP Offer Frame is used to continue the process */

        if( (lDHCP_ClientOptionsTable_st->rxOptionServerIdRxFlag_b   == TRUE) &&
            (lDHCP_ClientOptionsTable_st->rxOptionLeaseTimeRxFlag_b  == TRUE))
        {

            /* Store the Offered IP Address into the Table */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32 = lRxIpAddressToAssign_u32;

            /* Update the Client Structure with the Options received */
            rba_EthDHCP_ClientUpdateOptionsInClientStruct ( lDHCP_ClientOptionsTable_st,
                                                            lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

            /* Reset of the TxFrame and Time counter*/
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32             = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32                 = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32  = RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK;

            /* Indicate that Offer frames have been received */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten = RBA_ETHDHCP_SRVROFFER;
        }
        else
        {
            /* Mandatory Options not included : the OFFER frame is discarded */
        }
    }
    else
    {
        /* No Client is waiting for an OFFER Frame => The frame must be discarded.*/
    }


}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientRx_Ack()                                                                                       */
/* This function is called to process the received ACK message                                                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.                  */
/*  lDHCP_ClientOptionsTable_st         - client option table                                                       */
/*  lRxIpAddressToAssign_u32         - received IP address to be assigned                                           */
/*  lXIdDhcpRxData_u32         - Transaction ID recieved                                                            */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientRx_Ack ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                              const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_st,
                                              uint32 lRxIpAddressToAssign_u32,
                                              uint32 lXIdDhcpRxData_u32)
{

    /* Check the Options flags received from the Server (Server Id and Lease Time are mandatory in OFFER) */
    if( (lDHCP_ClientOptionsTable_st->rxOptionServerIdRxFlag_b    == TRUE) &&
        (lDHCP_ClientOptionsTable_st->rxOptionLeaseTimeRxFlag_b   == TRUE))
    {
        /* Client State == Requesting State ? */
        if( (RBA_ETHDHCP_CLNTREQUESTING == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten)||
            (RBA_ETHDHCP_CLNTRENEWING   == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten)||
            (RBA_ETHDHCP_CLNTREBINDING  == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten))
        {
            /* Get the informations contained in the ACK Frame :
             *  - Mandatory DHCP Options RxFrom - Lease Time
             *  - DHCP Options RxFrom - Rebinding Time
             *  - DHCP Options RxFrom - Renewing Time
             *      => Values stored in the parse options function
             */

            if( (RBA_ETHDHCP_CLNTRENEWING   == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten) ||
                (RBA_ETHDHCP_CLNTREBINDING  == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten))
            {

                /* check if the same Ip address is provided by the server*/
                /* If the IP address is not same, Drop the old Ip address and assign the new Ip address*/
                if( lRxIpAddressToAssign_u32 != lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32 )
                {

                    /* Store the Offered IP Address into the Table */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32 = lRxIpAddressToAssign_u32;

                    /* Set the number of probe frame sent to Zero to trigger the probe frames again*/
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxGratuFr_u8 = 0;
                    /* Set the number of Gratuitous frame sent to Zero to trigger the Gratuitous frames again*/
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxProbeFr_u8 = 0;

                    /* Go to the next step to check the validity of the IP provided */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten       = RBA_ETHDHCP_CLNTPROBING;
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFF;

                }
            }

            /* Update the Client Structure with the Options received */
            rba_EthDHCP_ClientUpdateOptionsInClientStruct ( lDHCP_ClientOptionsTable_st,
                                                            lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32             = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32                 = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32  = RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK;

            /* Indicates that an ACK frame has been received */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten = RBA_ETHDHCP_SRVRACK;
        }

        else
        {
            /*  No Client is waiting for an ACK Frame
             * => The frame must be discarded. */
        }
    }
    else
    {
        /* No Server IP has been received : The Server ID and Lease Time Options are mandatory */
    }

    /* ACK frame received for Inform frame */
    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXIdInform_u32 == lXIdDhcpRxData_u32)
    {
        /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
        /* Copy the Requested FQDN to the Known FQDN */
        (void)rba_BswSrv_MemCopy( (void*)&(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNBuf_au8[0U]),
                (const void*)&(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNBuf_au8[0U]),
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16);

        /* Updated the length of the FQDN received */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16=lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16 ;
        /* CLEAR  INFORM Message Re-Transmission flag on receiveing ACK */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b = FALSE;

        /*Clear FQDN Length  */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16 = 0;
    }


}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientRx_Nack()                                                                                      */
/* This function is called to process the received NACK message                                                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientRx_Nack ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    /* Client State == Requesting/Renewing/Rebinding/Rebooting State ? */
    if( (RBA_ETHDHCP_CLNTREQUESTING == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten) ||
        (RBA_ETHDHCP_CLNTRENEWING   == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten) ||
        (RBA_ETHDHCP_CLNTREBINDING  == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten) ||
        (RBA_ETHDHCP_CLNTREBOOTING  == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten) )
    {
        /* Reset of the TxFrame and Time counter*/
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32             = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32                 = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32  = RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK;

        /* Indicates that a NACK frame has been received */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten = RBA_ETHDHCP_SRVRNACK;
    }
    else
    {

    }
}

/********************************************************************************************************************/
/* rba_EthDHCP_MainFunction() - This DHCP MainFunction is called from TcpIp_MainFunction. The MainFunction does the */
/*                          following:                                                                              */
/*                          1. Allocate a UDP socket with pre-configured port number and pre-configured LocalAddrId.*/
/*                          2. The request to calculate the IP address based on Ethernet-switch-port and Client MAC */
/*                              address is done by calling the API - rba_SwitchDriver_GetIpAddr()                   */
/*                          3. Transmit a DHCP-OFFER frame according to IPBrandingOption-1 by calling the API -     */
/*                              TcpIp_UdpTransmit().                                                                */
/* Input Parameters :                                                                                               */
/*                        void                                                                                      */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
void rba_EthDHCP_ClientMainFunction ( void )
{
    /* Local variable declaration - Client */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;

    rba_EthDHCPClient_State_ten     lCurrentStateClnt_ten;
    uint8                           lIdxClnt_u8;

    /* Local variable Initialization */
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;

     RBA_ETHDHCP_DET_REPORT_ERROR( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTMAINFUNCTION_ID, RBA_ETHDHCP_E_NOTINIT )

    /* State Machine */
        for(lIdxClnt_u8 = 0; lIdxClnt_u8 < rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8; lIdxClnt_u8++)
        {
            /* Local Copy of the specific client */
            lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

            if( RBA_ETHDHCP_SOCK_BOUND == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procSocketState_ten )
            {
                /* Get the current Client state */
                lCurrentStateClnt_ten = lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten;

                /* Decrease the conflict time counter updated when a conflict is detected */
                if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntLastConflict_u32 > 0U)
                {
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntLastConflict_u32--;
                }

                /* procSocketState_u8 Bound => DHCP Frame can be sent */
                switch(lCurrentStateClnt_ten)
                {
                    case RBA_ETHDHCP_CLNTOFF :
                    {
                        /* DHCP Process not started => Keep the Client socket closed */
                        rba_EthDHCP_ClientCloseSocket(lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

                    }break;

                    case RBA_ETHDHCP_CLNTINIT :
                    {
                        rba_EthDHCP_ClientMainProc_Init (lDHCP_ClientSubCfgPtr_pco);

                    }break;

                    case RBA_ETHDHCP_CLNTSELECTING :
                    {
                        rba_EthDHCP_ClientMainProc_Select (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTREQUESTING :
                    {
                        rba_EthDHCP_ClientMainProc_Request (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTPROBING :
                    {
                        rba_EthDHCP_ClientMainProc_Probe (lDHCP_ClientSubCfgPtr_pco);
                    }
                    break;

                    case RBA_ETHDHCP_CLNTBOUND :
                    {
                        rba_EthDHCP_ClientMainProc_Bound (lDHCP_ClientSubCfgPtr_pco);
                        rba_EthDHCP_ClientMainProc_Inform(lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTRENEWING :
                    {
                        rba_EthDHCP_ClientMainProc_Renew (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTREBINDING :
                    {
                        rba_EthDHCP_ClientMainProc_Rebind (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTDECLINE :
                    {
                        rba_EthDHCP_ClientMainProc_Decline (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTREBOOTING :
                    {
                        /* A client that has rebooted with an assigned address is
                         * waiting for a confirming reply from a server.*/
                    }break;

                    case RBA_ETHDHCP_CLNTRELEASE :
                    {
                        rba_EthDHCP_ClientMainProc_Release (lDHCP_ClientSubCfgPtr_pco);
                    }break;

                    case RBA_ETHDHCP_CLNTSTOP :
                    {
                        rba_EthDHCP_ClientMainProc_Stop (lDHCP_ClientSubCfgPtr_pco);
                    }
                    break;

                    default :
                    {
                        /* Nothing To Do */
                    }
                    break;
                }   /* End of Switch */
            }
            else
            {
                /* Client SocketState_u8 not Bound */
                    if( RBA_ETHDHCP_CLNTINIT ==lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten )
                    {
                            /* Start to bind the DHCP Client Socket */
                        (void)rba_EthDHCP_ClientGetSocket(lDHCP_ClientSubCfgPtr_pco);
                    }
                    else if(RBA_ETHDHCP_CLNTSTOP ==lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten)
                    {
                        /* Clean the Client Part */
                        rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten               = RBA_ETHDHCP_CLNTOFF;
                    }
                    else
                    {
                       /* DO NOTHING */
                    }
            }
        }/* End of FOR */



    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Init()                                                                                */
/* This function is called to Initialize the DHCP assignment process by triggering the DHCP Discover Frame          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Init ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetValFct_tu8;

    lRetValFct_tu8  = E_NOT_OK;
    /********************************/
    /* DHCP Discover Frame to build */
    /********************************/

    /* Destination IP Address (UDP Part) */
    ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = 0xFFFFFFFFU;
    ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
    ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

    /* Generate a random XId when DHCP DISCOVER frame is transmitted or retransmitted */
    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32 = TcpIp_RandNumXor64();

    /* Send the DHCP Discover Frame */
    lRetValFct_tu8 = rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                        RBA_ETHDHCP_DISCOVER);

    if(E_OK == lRetValFct_tu8)
    {
        /* DHCP Client entry = Next Step if the transmission OK */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten       = RBA_ETHDHCP_CLNTSELECTING;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVRWAITRESP;
    }

}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Select()                                                                              */
/* This function is called to process the selection of client in DHCP assignment process                            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Select ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetValFct_tu8;

    lRetValFct_tu8  = E_NOT_OK;
    /* The client is waiting to receive DHCPOFFER messages
     * from one or more DHCP servers, so it can choose one.*/

    /* DHCPREQUEST generated during SELECTING state
     * Client inserts the address of the selected server in server identifier,
     * ciaddr MUST be zero,
     * requested IP address MUST be filled in with the yiaddr value from the chosen DHCPOFFER.
     *
     * Note that the client may choose to collect several DHCPOFFER messages and select the "best" offer.
     * The client indicates its selection by identifying the offering server in the DHCPREQUEST message.
     * If the client receives no acceptable offers, the client may choose to try another DHCPDISCOVER message.
     * Therefore, the servers may not receive a specific DHCPREQUEST from which they can decide whether or not
     * the client has accepted the offer. Because the servers have not committed any network address assignments
     * on the basis of a DHCPOFFER, servers are free to reuse offered network addresses in response to subsequent
     * requests. As an implementation detail, servers SHOULD NOT reuse offered addresses and may use an
     * implementation-specific timeout mechanism to decide when to reuse an offered address. */

    /* Time to receive server response too long ? */
    if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32 >= lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 )
    {
        /* Check if a response has been received */
        if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten == RBA_ETHDHCP_SRVROFFER)
        {
            /* OFFER frame has been received. The informations about the server have been parsed
             * in the RxIndication function. These informations are available in the client structure. */

            /* Destination IP Address (Broadcast) */
            ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = 0xFFFFFFFFU;
            ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
            ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

            /* Send the DHCP Request Frame with XID from DHCP OFFER and DISCOVER frame */
            lRetValFct_tu8 = rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                                RBA_ETHDHCP_REQUEST);

            if(E_OK == lRetValFct_tu8)
            {
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten                   = RBA_ETHDHCP_CLNTREQUESTING;
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten              = RBA_ETHDHCP_SRVRWAITRESP;

            }
        }
        else
        {
            /* No response received => Update time counter */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;
        }
    }
    else
    {
        /* If no response received before the response time
        * defined the client retransmits DHCP Discover Frame */
        /* Go back to the INIT state / Increment the counter of frame sent */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten   = RBA_ETHDHCP_CLNTINIT;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;

        if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 < RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST )
        {


            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32++;
            /* Infinite Retransmission if no OFFER Frames received */
            rba_EthDHCP_ClientRetransmission( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        }
        else
        {
            /* Offer not received , restart the cleint */
            rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        }


    }


}
/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Inform()                                                                              */
/* This function is handles the Re-Transmissions of Inform Messages.                                                */
/* The INFORM message uses the timings suggested by the Exponential backoff algorithm  as used by REQUEST           */
/* The messages are transmitted only when DHCP state is in BOUND and transmissions may get interrupted while        */
/* entering RENEW and REBIND  STATES. It is assumed in RENEW and REBIND states FQDN would be transmitted as an      */
/* option if its already known to the DHCP                                                                          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*          void                                                                                      */
/********************************************************************************************************************/

LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Inform ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    /* Trigger the Inform Message */
    if( (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16 !=0) && ( FALSE == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b) )
    {
        /********************************/
        /* DHCP Inform Frame to build */
        /********************************/

        /* Destination IP Address (UDP Part) */
        ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = 0xFFFFFFFFU;
        ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
        ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;
        /* generate the Random Transaction ID for the Inform message */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXIdInform_u32 = TcpIp_RandNumXor64();

        /* Re-Transmission of INFORM MSG might be required, hence enable Retransmission flag */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b = TRUE;

        /*  Reset the counters required by Retransmission */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 = 0;

        /* Calculate next ReTransmission Time */
        rba_EthDHCP_ClientRetransmission( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        /* Send the DHCP Inform Frame */
        (void)rba_EthDHCP_ClientSendDhcpFrame ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                                    RBA_ETHDHCP_INFORM);

    }

    if( TRUE == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b  )
    {
        /* ReTransmission is performed only if ReTransmission Flag is set */

        if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32 <= lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 )
        {

            /********************************/
            /* DHCP Inform Frame to build */
            /********************************/
            /* Destination IP Address (UDP Part) */
            ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = 0xFFFFFFFFU;
            ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
            ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

            /* Change XId when DHCP frame is retransmitted */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXIdInform_u32 = TcpIp_RandNumXor64();

            /* Send the DHCP INFORM Frame */
            (void)rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                                RBA_ETHDHCP_INFORM);

            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;

            /* Increment ReTransmission Counter */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32++;

            if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 > RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST )
            {
                /*  Reset the counters required by Retransmission */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 = 0;
            }

            rba_EthDHCP_ClientRetransmission( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        }
        else
        {
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;

        }

   }


}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Request()                                                                             */
/* This function is called to process the TX of Request message in DHCP assignment process                          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Request ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    /* The client is waiting to hear back from
     * the server to which it sent its request. */
    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32 < 0xFFFFFFFFU)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32++;
    }

    /* Time to receive server response too long ? */
    if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 <= lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procReTransmissionTime_u32)
    {
        /* If a server ACK response has been received */
        if( (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten == RBA_ETHDHCP_SRVRACK) && (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32!=0U))
        {
            /* Go to the next step to check the validity of the IP provided */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten       = RBA_ETHDHCP_CLNTPROBING;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFF;

            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 = 0;
        }
        /* If a server NACK response has been received */
        else if( (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten == RBA_ETHDHCP_SRVRNACK ) ||
                 ( ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32==0U) && (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten == RBA_ETHDHCP_SRVRACK))
                )
        {
            /* If the client receives a DHCPNAK message, it cannot reuse its remembered network address.
             * It must instead request a new address by restarting the configuration process, this time
             * using the (non-abbreviated) procedure described in section 3.1. This action also corresponds
             * to the client moving to the INIT state in the DHCP state diagram. */

            /* Clean the Client Part */
            rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

            /* Restart the configuration process => Go back to the init state */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;
        }
        else
        {
            /* No response received => Update time counter  */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;
        }
    }
    else
    {
        /* Timer value exceeded =>  */

        if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 < RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST )
        {
            /* A client retransmitting as described in section 4.1 might retransmit
             * the DHCPREQUEST message four times, for a total delay of 60 seconds,
             * before restarting the initialization procedure. */

            /* Go back to the SELECTING state / Increment the counter of frame sent */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten   = RBA_ETHDHCP_CLNTSELECTING;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32++;
            /* To indicate that an OFFER Frame has already been received */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFFER;

            /* Change XId when DHCP REQUEST frame is retransmitted */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32 = TcpIp_RandNumXor64();

            /* BackOff Algorithm Retransmission if no Server Frame received */
            rba_EthDHCP_ClientRetransmission( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );
        }
        else
        {
            /* If the client receives neither a DHCPACK or a DHCPNAK message after
             * employing the retransmission algorithm, the client reverts to INIT
             * state and restarts the initialization process. */
            rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

            /* The client reverts to INIT */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;
        }
    }


}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Probe()                                                                               */
/* This function is called to process the Probing sequence of DHCP assignment process                               */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Probe ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetVal_tu8;

    lRetVal_tu8 = E_NOT_OK;

    /* Perform a final check with ARP frames. If the address is already in use
     * => send a DHCPDECLINE Message
     * => wait a minimum of ten seconds before restarting the process */

    /* Rebinding/Renewing Timer */
   if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRenewTickTime_u32!=0UL)
   {
       lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRenewTickTime_u32--;
   }

   if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32!=0UL)
   {
       lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32--;
   }

   if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32!=0UL)
   {
       lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32--;
   }
    #if (TCPIP_ARP_ENABLED == STD_ON)
    if( RBA_ETHDHCP_PROBE_NUM > lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxProbeFr_u8 )
    {
        /* Wait Delay between Probe Frames */
        if(RBA_ETHDHCP_PROBE_WAIT_TICK < lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32)
        {
            /* Send an ARP Probe frame
             * Probe Frame = ARP Request with Sender IP address = 0 and Target IP = Assigned Ip*/
            lRetVal_tu8 = rba_EthArp_SendArpFrame(  lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                                    0,  // idxIpInTable_u16 : Not used for ARP Probe Frame
                                                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32,
                                                    NULL_PTR,   // Destination MAC address : Not used for ARP Probe Frame
                                                    RBA_ETHARP_ARP_PROBE_FRAME);

            if( E_OK == lRetVal_tu8 )
            {
                /* Update the Probe Frame counter if the frame has been sent */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxProbeFr_u8++;
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 = 0;
                /* First ARP PROBE Frame has been sent : the IP Conflict process is active */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en = TCPIP_IPADDR_STATE_UNASSIGNED;
            }
        }
        else
        {
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;
        }
    }
    else
    {
        /* Wait Delay before announcing */
        if(RBA_ETHDHCP_ANNOUNCE_WAIT_TICK < lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32)
        {
            /* All the probes frames have been sent => Gratuitous Frames */
            if (RBA_ETHDHCP_ANNOUNCE_NUM > lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxGratuFr_u8 )
            {
                /* Send an announce ARP Frame */
                lRetVal_tu8 = rba_EthArp_SendArpGratuitousFrame(
                                    lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32);

                if( E_OK == lRetVal_tu8 )
                {
                    /* Update the Gratuitous Frame counter if the frame has been sent */
                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxGratuFr_u8++;

                    /* If the IP Address has not been already assigned (after the 1st gratuitous frame) */
                    if( TCPIP_IPADDR_STATE_ASSIGNED != lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en)
                    {
                        rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                                    TCPIP_IPADDR_STATE_ASSIGNED);
                    }
                }
            }
            else
            {
                /* All the gratuitous frames have been sent => We can now go in the BOUND state */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten   = RBA_ETHDHCP_CLNTBOUND;
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;

                /* Call the local function to trigger the ARP request to get the MAC address of the Server or the Router */
                (void)rba_EthDHCP_ClientGetMacIDForDestIp(  lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s);
            }
        }
        else
        {
            /* Increment Time Counter */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;
        }
    }
    #else
        /* If the IP Address has not been already assigned (after the 1st gratuitous frame) */
        if( TCPIP_IPADDR_STATE_ASSIGNED != lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en)
        {
            rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                        TCPIP_IPADDR_STATE_ASSIGNED);
        }

        /* The IP Conflict prevention cannot be done => We go directly to the BOUND State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTBOUND;
    #endif

}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Bound()                                                                               */
/* This function is called to Update the timers in the Bound state of DHCP assignment process                       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Bound ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{

    /* Client has a valid lease and is in its normal operating state.*/

    /* Rebinding/Renewing Timer */
    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRenewTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRenewTickTime_u32--;
    }

    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32--;
    }

    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32--;
    }

    if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRenewTickTime_u32 == 0UL)
    {
        /* If the Renewal Counter is exceeded => Next Step */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten                   = RBA_ETHDHCP_CLNTRENEWING;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten              = RBA_ETHDHCP_SRVROFF;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32                     = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32                 = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32    = 0;
    }
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Renew()                                                                               */
/* This function is called to process the Renew Sequence of DHCP Assignment process                                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Renew ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetValFct_tu8;

    lRetValFct_tu8 = E_NOT_OK;

    /* Client is trying to renew its lease. It regularly sends
     * DHCPREQUEST messages with the server that gave it its
     * current lease specified, and waits for a reply. */

    /* Renewing Timer + Process Timer */
    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;

    /* The client is waiting to hear back from
    * the server to which it sent its request. */
    if((lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32 < 0xFFFFFFFFU) &&
       (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 > 0UL))
    {
      lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32++;
    }

    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32--;
    }

    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32--;
    }

    if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32 == 0UL )
    {
        /* If Rebinding counter is exceeded => Next Step : REBINDING */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten   = RBA_ETHDHCP_CLNTREBINDING;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32     = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32 = 0;
        /* Stop any further INFORM retransmissions */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b = FALSE;
    }
    else if ( RBA_ETHDHCP_SRVRACK == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten )
    {
        /* Call the local function to trigger the ARP request to get the MAC address of the Server or the Router */
        (void)rba_EthDHCP_ClientGetMacIDForDestIp(  lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                                lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

        /* If a server ACK response has been received => Return to BOUND State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten       = RBA_ETHDHCP_CLNTBOUND;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFF;

        /* The updating of the new lease time is done into the RxIndication function */
    }
    else if ( RBA_ETHDHCP_SRVRNACK == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten )
    {
        rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED   );

        /* Close the socket connection */
        rba_EthDHCP_ClientCloseSocket(lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

        /* Reset Client Structure */
        rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        /* If a server NACK response has been received => Return to INIT State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;
    }
    else
    {
        /* DHCPREQUEST generated during RENEWING state
         * server identifier            MUST NOT be filled in
         * requested IP address option  MUST NOT be filled in
         * ciaddr                       MUST be filled in with client s IP address
         * In this situation, the client is completely configured, and is trying to extend its lease.
         * This message will be unicast, so no relay agents will be involved in its transmission.
         * Because giaddr is therefore not filled in, the DHCP server will trust the value in ciaddr,
         * and use it when  replying to the client. */

        /* If first frame not transmitted or if Timer > 60 sec */
            if( (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 == 0U) ||
            ((lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 > RBA_ETHDHCPCLIENT_MINUTETICK) &&
            (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32  >= lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32 ) &&
            (((uint32)(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 - lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientRebindTickTime_u32)) <= 1U )))
        {
            /* Destination IP Address (Unicast - Frame sent to the Server) */
            ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = ( (uint32) ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionServerId_u32 ) );
            ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
            ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

            /* Change XId when DHCP frame is transmitted */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32 = TcpIp_RandNumXor64();

            /* Send the DHCP Request Frame */
            lRetValFct_tu8 = rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                                RBA_ETHDHCP_REQUEST);

            if(E_OK == lRetValFct_tu8)
            {
                /* Frame Counter */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32++;
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 = 0;
                /* Stop any further INFORM retransmissions */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->InformReTransmissionsActive_b = FALSE;
            }
        }
    }


}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Rebind()                                                                              */
/* This function is called to process the Rebind Sequence of DHCP Assignment process                                */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Rebind ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetValFct_tu8;

    lRetValFct_tu8 = E_NOT_OK;

    /* Rebinding Timer + Process Timer */
    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32++;

    /* The client is waiting to hear back from
    * the server to which it sent its request. */
    if((lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32 < 0xFFFFFFFFU) &&
       (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 > 0UL))
    {
      lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseDeltaTickTime_u32++;
    }


    /* The client has failed to renew its lease with the server
     * that originally granted it, and now seeks a lease extension
     * with any server that can hear it. It periodically sends
     * DHCPREQUEST messages with no server specified until it gets
     * a reply or the lease ends. */
    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32!=0UL)
    {
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32--;
    }

    if( ( RBA_ETHDHCP_SRVRNACK == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten ) ||
        ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32 == 0U))
    {
        /* The Lease time is exceeded => The process must be restarted for a new Lease */
        rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED   );

        /* Close the socket connection */
        rba_EthDHCP_ClientCloseSocket(lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

        /* Restart the process to get a new IP Address */
        /* Reset Client Structure */
        rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

        /* If a server NACK response has been received => Return to INIT State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;
    }
    else if (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten == RBA_ETHDHCP_SRVRACK)
    {
        /* If a server ACK response has been received => Return to REQUESTING State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten       = RBA_ETHDHCP_CLNTBOUND;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->serverRespType_ten  = RBA_ETHDHCP_SRVROFF;

        /* If a server NACK response has been received => Return to INIT State */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32         = 0;
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32     = 0;

        /* Call the local function to trigger the ARP request to get the MAC address of the Server or the Router */
        (void)rba_EthDHCP_ClientGetMacIDForDestIp(  lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                                lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

    }
    else
    {
        /* DHCPREQUEST generated during REBINDING state:
         * server identifier            MUST NOT be filled in,
         * requested IP address option  MUST NOT be filled in,
         * ciaddr                       MUST be filled in with client s IP address.
         * In this situation, the client is completely configured, and is trying to extend its lease.
         * This message MUST be broadcast to the 0xffffffff IP broadcast address.
         * The DHCP server SHOULD check ciaddr for correctness before replying to the DHCPREQUEST. */

        /* If first frame not transmitted or if Timer > 60 sec */
        if( (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32 == 0U) ||
            ((lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 > RBA_ETHDHCPCLIENT_MINUTETICK) &&
            (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 >= lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32 ) &&
            (((uint32)(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 - lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientLeaseTickTime_u32)) <= 1U )))
        {
            /* Destination IP Address (Broadcast)*/
            ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = 0xFFFFFFFFU;
            ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
            ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

            /* Change XId when DHCP frame is transmitted */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientXId_u32 = TcpIp_RandNumXor64();

            /* Send the DHCP Request Frame */
            lRetValFct_tu8 = rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                                RBA_ETHDHCP_REQUEST);

            if(E_OK == lRetValFct_tu8)
            {
                /* Frame Counter */
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxFr_u32++;
                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCnt_u32 = 0;
            }
        }
    }

}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Release()                                                                             */
/* This function is called to Transmit the Decline Packet                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Decline ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{

    /* If An IP Conflict Occurs, the Client must send a DHCP DECLINE Frame broadcast */
    ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) =  0xFFFFFFFFU;
    ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
    ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

    /* Send the DHCP Decline Frame */
    (void)rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                             RBA_ETHDHCP_DECLINE );

    /* If the IP is assigned, he must no longer be used => Inform the upper layer */
    if( TCPIP_IPADDR_STATE_ASSIGNED == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en )
    {
        rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED   );
    }

    /* Close the socket connection */
    rba_EthDHCP_ClientCloseSocket(lDHCP_ClientSubCfgPtr_pco->ClntTable_s);

    /* Clean the Client Part */
    rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );

    /* Even if the DECLINE frame has not been transmitted, the
     * Process goes to the next step to not be locked into this Case */
    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTINIT;
}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Release()                                                                             */
/* This function is called to process the release of IP address in DHCP assignment process                          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Release ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    Std_ReturnType lRetValFct_tu8;

    lRetValFct_tu8 = E_NOT_OK;

    /* Stop State : requested by upper layer to stop the DHCP process.
     * Cases :
     * 1) The process is running, any IP has been assigned.
     * 2) Address has already been assigned (BOUND / RENEW State)*/

    if (RBA_ETHDHCP_MAX_TXATTEMPTS_RELEASE_FRM > lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxRlsFrmAttempts_u8)
    {
        /* Destination IP Address => Unicast to the Server */
        ( rba_EthDHCP_SrvrSockAddr_st.addr[0] ) = ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionServerId_u32 );
        ( rba_EthDHCP_SrvrSockAddr_st.domain )  = TCPIP_AF_INET;
        ( rba_EthDHCP_SrvrSockAddr_st.port )    = TCPIP_DHCPSERVER_PORT;

        if (lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxRlsFrmAttempts_u8 == 0)
        {

            /* Call the local function to trigger the ARP request to get the MAC address of the Server or the Router */
            (void)rba_EthDHCP_ClientGetMacIDForDestIp(  lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                                                    lDHCP_ClientSubCfgPtr_pco->ClntTable_s);
        }
        /* Send the DHCP Release Frame */
        lRetValFct_tu8 = rba_EthDHCP_ClientSendDhcpFrame (  lDHCP_ClientSubCfgPtr_pco->ClntTable_s,
                                                            RBA_ETHDHCP_RELEASE);

        /* increment the counter value by 1 */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxRlsFrmAttempts_u8++;

        if( lRetValFct_tu8==E_OK )
        {
            /* Reset the Release message counter to Zero */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxRlsFrmAttempts_u8 = 0;
            /* Even if the RELEASE frame has not been transmitted, the
             * Process goes to the next step to not be locked into this Case */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTSTOP;
        }

    }
    else
    {
        /* Reset the Release message counter to Zero */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntTxRlsFrmAttempts_u8 = 0;

        /* Even if the RELEASE frame has not been transmitted, the
         * Process goes to the next step to not be locked into this Case */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTSTOP;
    }


}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMainProc_Stop()                                                                                */
/* This function is called to stop the DHCP process from Mainfunction                                               */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lDHCP_ClientSubCfgPtr_pco - Client SubConfig Structure for which the ports have to be checked.          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientMainProc_Stop ( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco)
{
    /* If the IP is assigned, he must no longer be used => Inform the upper layer */
    if( TCPIP_IPADDR_STATE_ASSIGNED == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en )
    {
        rba_EthDHCP_ClientLocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco,
                                                    TCPIP_IPADDR_STATE_UNASSIGNED   );
    }

    /* Close the socket connection */
    rba_EthDHCP_ClientCloseSocket(lDHCP_ClientSubCfgPtr_pco->ClntTable_s);


    /* Clean the Client Part */
    rba_EthDHCP_ClientCleanToRestart ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s );
    /* Stop the DHCP Assignment Process */
    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten   = RBA_ETHDHCP_CLNTOFF;

}


/*
****************************************************************************************************
*                       Functions used to the IP Conflict Detection functionality
****************************************************************************************************
*/

/*************************************************************************************************/
/* rba_EthDHCPClient_GetIpAddress() - Function called by the DHCP                                */
/* This function is called to provide the current IP Address in process and the current IP       */
/* Assignment State to ARP to do the IP Conflict detection internally.                           */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  CtrlIdx_u8          - index of the Ethernet controller                                       */
/* Output Parameters :                                                                           */
/*  lLocalIpAddress_u32 - DHCP address currently got from Server and checked by the process.     */
/*  lCurrentIpState_pen - IP Assignment State (INIT / UNASSIGNED / ASSIGNED)                     */
/*                                                                                               */
/* return lRetVal_tu8.                                                                           */
/*      E_OK : DHCP Client table has been found and the IP Address is well returned.             */
/*      E_NOT_OK : DHCP Client table has not been found and the IP Address is not returned       */
/*************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientGetIpAddress( uint8 lCtrlIdx_u8,
                                               uint32 * lLocalIpAddress_pu32,
                                               TcpIp_IpAddrStateType * lCurrentIpState_pen)
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;
    uint8                                 lIdxCounter_u8;

    Std_ReturnType                        lRetVal_tu8;


    /* Local variable initialization */
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;

    lRetVal_tu8                 = E_NOT_OK;


    RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTGETIPADDRESS_ID, RBA_ETHDHCP_E_NOTINIT )

    RBA_ETHDHCP_DET_REPORT_ERROR_NOK( lLocalIpAddress_pu32 == NULL_PTR, RBA_ETHDHCP_E_APICLIENTGETIPADDRESS_ID, RBA_ETHDHCP_E_PARAM_POINTER )

    RBA_ETHDHCP_DET_REPORT_ERROR_NOK( lCurrentIpState_pen  == NULL_PTR, RBA_ETHDHCP_E_APICLIENTGETIPADDRESS_ID, RBA_ETHDHCP_E_PARAM_POINTER )


    for ( lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxCounter_u8++ )
    {
        /* Local Copy of each Specific Clients */
        lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxCounter_u8] ;

        if( lCtrlIdx_u8 == lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8 )
        {

            /* The process is ongoing (Not STOP and Not OFF)*/
            /* Return the Current LinkLocalIpAddress generated */
            *lLocalIpAddress_pu32 = (uint32)                lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32;
            *lCurrentIpState_pen = (TcpIp_IpAddrStateType)  lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en;

            lRetVal_tu8             = E_OK;

            /* Break the Loop */
            break;
        }
    }

    return lRetVal_tu8;

}

/*************************************************************************************************/
/* rba_EthDHCPClient_IpConflictDetected()                                                        */
/* This function is called to check the Ip received in the ARP frame and to detect if there is   */
/* no conflict between Ip address.                                                               */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  CtrlIdx_u8          - index of the Ethernet controller                                       */
/*                                                                                               */
/* return lRetVal_tu8.                                                                           */
/*************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientIpConflictDetected ( uint8 lCtrlIdx_u8 )
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;
    uint8                                 lIdxCounter_u8;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    boolean                               lValidCtrlIdx_b;
#endif

    /* Local variable initialization */
     lDHCP_ClientSubCfgPtr_pco  = NULL_PTR;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
     lValidCtrlIdx_b            = FALSE;
#endif

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTIPCONFLICTDETECTED_ID, RBA_ETHDHCP_E_NOTINIT )

            for ( lIdxCounter_u8 = 0; lIdxCounter_u8 < ((uint8) rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8); lIdxCounter_u8++ )
            {
                /* Local Copy of each Specific Clients */
                lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxCounter_u8] ;

                if( lCtrlIdx_u8 == lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8 )
                {
                    #if( TCPIP_DEV_ERROR_DETECT == STD_ON )
                    lValidCtrlIdx_b         = TRUE;
                    #endif

                    break;
                }
            }

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( ( lValidCtrlIdx_b == FALSE ), RBA_ETHDHCP_E_APICLIENTIPCONFLICTDETECTED_ID, RBA_ETHDHCP_E_NOTINIT )


         if(TCPIP_IPADDR_STATE_ASSIGNED     == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en)
         {
            /* RBA_ETHDHCP_IPADDR_ASSIGNED means that an IP Addr has been
             * received from a Server and has been assigned : BOUND / RENEW / REBIND State */

            /* If the IP Address is Assigned => He must be defended */
             rba_EthDHCP_ClientHandleIpConflict( lIdxCounter_u8 );
         }
         else if((TCPIP_IPADDR_STATE_UNASSIGNED     == lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en))
         {
            /* RBA_ETHDHCP_IPADDR_UNASSIGNED means that an IP Addr has been
             * received from a Server but has been not yet assigned : SELECTING / REQUESTING / PROBING States*/

            /* The process is ongoing (Not STOP and Not OFF)
             * => The process must be restarted to generate a new IP Address */

            /* Modify the State of the Client for the next State Machine */
            lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTDECLINE;
         }
         else
         {
            /* Nothing to do.
             * The Conflict is not considered because the process will be restarted with the next
             * calling of the StartFunction */
         }

         /* The conflict has been handled */
         return E_OK;
}

/************************************************************************************************************/
/* rba_EthDHCP_ClientReadOption() -                                                                         */
/* Function called from rba_EthDHCP_ReadOption()                                                            */
/* By this API service the TCP/IP stack retrieves DHCP option data identified by parameter option for       */
/* already received DHCP options.                                                                           */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/*  LocalIpAddrId   - IP address identifier representing the local IP address and EthIf controller for      */
/*                  which the DHCP option shall be read.                                                    */
/*  Option          - DHCP option                                                                           */
/*  DataPtr         - Pointer to memory containing DHCP option data                                         */
/*  DataLength      - As input parameter, contains the length of the provided data buffer.                  */
/*                  Will be overwritten with the length of the actual data.                                 */
/*                                                                                                          */
/* return :                                                                                                 */
/*  Std_ReturnType  - Result of operation :                                                                 */
/*       - E_OK requested data retrieved successfully.                                                      */
/*       - E_NOT_OK requested data could not be retrieved.                                                  */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientReadOption( TcpIp_LocalAddrIdType lLocalIpAddrId_u8,
                                             uint8 lOption_u8,
                                             uint8 * lDataLength_pu8,
                                             uint8 * lDataPtr_pu8)
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;

    uint8                                 lIdxClnt_u8;
    Std_ReturnType                        lRetVal_tu8;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    boolean                               lValidLocalAddr_b;
#endif

    /* Local variable initialization */
    lRetVal_tu8                 = E_NOT_OK;
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    lValidLocalAddr_b           = FALSE;
#endif

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTREADOPTION_ID, RBA_ETHDHCP_E_NOTINIT )

    /* 1- Search the Client for which we want to read the option value. */
    for(lIdxClnt_u8 = 0; lIdxClnt_u8 < rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8; lIdxClnt_u8++)
    {
        /* Local Copy of each Specific Clients */
        lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

        if(lLocalIpAddrId_u8 == lDHCP_ClientSubCfgPtr_pco->ClntLocalAddrId_u8)
        {
            #if( TCPIP_DEV_ERROR_DETECT == STD_ON )
            lValidLocalAddr_b           = TRUE;
            #endif

            break;
        }
    }

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lValidLocalAddr_b == FALSE), RBA_ETHDHCP_E_APICLIENTREADOPTION_ID, RBA_ETHDHCP_E_INV_ARG )

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lDataLength_pu8 == NULL_PTR), RBA_ETHDHCP_E_APICLIENTREADOPTION_ID, RBA_ETHDHCP_E_PARAM_POINTER )

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lDataPtr_pu8 == NULL_PTR), RBA_ETHDHCP_E_APICLIENTREADOPTION_ID, RBA_ETHDHCP_E_PARAM_POINTER )

        /* 2- Switch Case with the Option ID */
        switch (lOption_u8)
        {
            case RBA_ETHDHCP_OPTION_SUBNETMASK_ID:
            {
                /* Update the Value */
                TcpIp_WriteU32( (&lDataPtr_pu8[0]), (uint32) lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionSubnetMaskId_u32);
                /* Update the Length */
                (*lDataLength_pu8) = RBA_ETHDHCP_OPTION_SUBNETMASK_LENGTH;
                /* Update the Return Value */
                lRetVal_tu8 = E_OK;
            }break;

            case RBA_ETHDHCP_OPTION_ROUTER_ID :
            {
                /* Update the Value */
                TcpIp_WriteU32( (&lDataPtr_pu8[0]), (uint32) lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionRouter_u32);
                /* Update the Length */
                (*lDataLength_pu8) = RBA_ETHDHCP_OPTION_ROUTER_LENGTH;
                /* Update the Return Value */
                lRetVal_tu8 = E_OK;
            }
            break;

            case RBA_ETHDHCP_OPTION_SERVERID_ID :
            {
                /* Update the Value */
                TcpIp_WriteU32( (&lDataPtr_pu8[0]), (uint32) lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionServerId_u32);
                /* Update the Length */
                (*lDataLength_pu8) = RBA_ETHDHCP_OPTION_SERVERID_LENGTH;
                /* Update the Return Value */
                lRetVal_tu8 = E_OK;
            }
            break;

            case RBA_ETHDHCP_OPTION_REQUESTEDIP_ID :
            {

            }break;

            case RBA_ETHDHCP_OPTION_LEASETIME_ID :
            {
                /* Update the Value */
                TcpIp_WriteU32((&lDataPtr_pu8[0]), (uint32) lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionLeaseTime_u32);
                /* Update the Length */
                (*lDataLength_pu8) = RBA_ETHDHCP_OPTION_LEASETIME_LENGTH;
                /* Update the Return Value */
                lRetVal_tu8 = E_OK;
            }break;

            case RBA_ETHDHCP_OPTION_FQDN_ID :
            {
                if ( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16 != 0U )
                {


                    /* Updated the length of the FQDN */
                    if(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16 > RBA_ETHDHCP_UINT8_MAX_VALUE)
                    {
                        (*lDataLength_pu8) = RBA_ETHDHCP_UINT8_MAX_VALUE;
                    }
                    else
                    {
                        (*lDataLength_pu8) = (uint8)lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNLen_u16 ;
                    }

                    /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
                                       /* Update the FQDN */
                    (void)rba_BswSrv_MemCopy((void*)lDataPtr_pu8, (const void*)&(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->KwnFQDNBuf_au8[0U]),
                            (*lDataLength_pu8));

                    /* Update the Return Value */
                    lRetVal_tu8 = E_OK;
                }
                else
                {
                    /* Return E_NOT_OK if the FQDN was requested before the IP address is assigned */
                    lRetVal_tu8 = E_NOT_OK;
                 }

            }break;

            default :
            {
                lRetVal_tu8 = E_NOT_OK;
            }break;
        }

    return lRetVal_tu8;
}

/************************************************************************************************************/
/* rba_EthDHCP_ClientWriteOption() -                                                                        */
/* Function called from rba_EthDHCP_WriteOption()                                                           */
/* By this API service the TCP/IP stack writes the DHCP option data identified by parameter option.         */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/*  LocalIpAddrId   - IP address identifier representing the local IP address and EthIf controller for      */
/*                  which the DHCP option shall be read.                                                    */
/*  Option          - DHCP option                                                                           */
/*  DataLength      - length of DHCP option data                                                            */
/*  DataPtr         - Pointer to memory containing DHCP option data.                                        */
/*                                                                                                          */
/* return :                                                                                                 */
/*  Std_ReturnType  - Result of operation :                                                                 */
/*       - E_OK no error occured.                                                                           */
/*       - E_NOT_OK DHCP option data could not be written.                                                  */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_ClientWriteOption( TcpIp_LocalAddrIdType lLocalIpAddrId_u8,
                                              uint8 lOption_u8,
                                              uint8 lDataLength_u8,
                                              const uint8 * lDataPtr_pu8)
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;

    uint8                                 lIdxClnt_u8;
    Std_ReturnType                        lRetVal_tu8;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    boolean                               lValidLocalAddr_b;
#endif

    /* Local variable initialization */
    lRetVal_tu8                 = E_NOT_OK;
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;
#if( TCPIP_DEV_ERROR_DETECT == STD_ON )
    lValidLocalAddr_b           = FALSE;
#endif

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( TRUE != rba_EthDHCP_ClientInitFlag_b, RBA_ETHDHCP_E_APICLIENTWRITEOPTION_ID, RBA_ETHDHCP_E_NOTINIT )

    /* 1- Search the Client for which we want to read the option value. */
    for(lIdxClnt_u8 = 0; lIdxClnt_u8 < rba_EthDHCP_ClientCurrConfig_pco->ClntConfigured_u8; lIdxClnt_u8++)
    {
        /* Local Copy of each Specific Clients */
        lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lIdxClnt_u8] ;

        if(lLocalIpAddrId_u8 == lDHCP_ClientSubCfgPtr_pco->ClntLocalAddrId_u8)
        {

            #if( TCPIP_DEV_ERROR_DETECT == STD_ON )
            lValidLocalAddr_b           = TRUE;
            #endif

            break;
        }
    }

     RBA_ETHDHCP_DET_REPORT_ERROR_NOK( (lValidLocalAddr_b == FALSE), RBA_ETHDHCP_E_APICLIENTWRITEOPTION_ID, RBA_ETHDHCP_E_INV_ARG )


        /* 2- Switch Case with the Option ID */
        switch (lOption_u8)
        {

            case RBA_ETHDHCP_OPTION_FQDN_ID :
            {
                if( (lDataLength_u8 <= RBA_ETHDHCP_OPTION_FQDN_MAX_LENGTH) && (lDataLength_u8 != 0U) )
                {
                    /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
                    /* DHCP Options - FQDN */
                    (void)rba_BswSrv_MemCopy( (void*) &(lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNBuf_au8[0U]),
                            ((const void*)lDataPtr_pu8),
                            lDataLength_u8 );

                     /* copy the length of the requested FQDN buffer */
                     lDHCP_ClientSubCfgPtr_pco->ClntTable_s->ReqFQDNLen_u16 = lDataLength_u8 ;

                     lRetVal_tu8 = E_OK;

                }

            }break;


            case RBA_ETHDHCP_OPTION_REQUESTEDIP_ID :
            {
                lRetVal_tu8 = E_NOT_OK;
            }break;

            case RBA_ETHDHCP_OPTION_LEASETIME_ID :
            {
                /* Requesting a configurable DHCP lease time as client shall be supported.
                 * => Currently Rejected */
                lRetVal_tu8 = E_NOT_OK;
            }break;

            default :
            {
                lRetVal_tu8 = E_NOT_OK;
            }break;
        }


    return lRetVal_tu8;
}

/*
****************************************************************************************************
*                                   DHCP Client Functions (Static)
****************************************************************************************************
*/

/*************************************************************************************************/
/* rba_EthAutoIp_Handle_IpConflict()                                                             */
/* Function called when an IP Conflict occurs. Depending on the number of conflicts and the      */
/* state of the process, the IP Address is regenerated or defended.                              */
/*                                                                                               */
/* Input Parameters :                                                                            */
/* lClientIdx_u8 - Client index on which the conflict occurs.                                    */
/*                                                                                               */
/* return   - void                                                                               */
/*                                                                                               */
/*************************************************************************************************/
static void rba_EthDHCP_ClientHandleIpConflict( uint8 lClientIdx_u8 )
{
    /* Local variable declaration */
    const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco;
    /* Initialization */
    lDHCP_ClientSubCfgPtr_pco   = NULL_PTR;

    /* Local variable initialization */
    lDHCP_ClientSubCfgPtr_pco = &rba_EthDHCP_ClientCurrConfig_pco->rba_EthDHCP_ClientSubConfig_cst[lClientIdx_u8];

    /* Retreat, there was a conflicting ARP in the last DEFEND_INTERVAL seconds */
    if( lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntLastConflict_u32 > 0U)
    {
        /******************************/
        /* Restart the DHCP Process */

        /* Increment the number of conflict */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntConflict_u8++;
        /* Modify the State of the Client for the next State Machine */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTDECLINE;
        /******************************/
    }
    else
    {
        #if (TCPIP_ARP_ENABLED == STD_ON)
        /* Send an announce ARP Frame */
        (void)rba_EthArp_SendArpGratuitousFrame(
                        lDHCP_ClientSubCfgPtr_pco->ClntCtrlIdx_u8,
                        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32);

        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntLastConflict_u32 = RBA_ETHDHCP_DEFEND_INTERVAL_TICK;
        #else
        /* The IP Address cannot be defended : it is released */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procState_ten = RBA_ETHDHCP_CLNTDECLINE;
        /* Increment the number of conflict */
        lDHCP_ClientSubCfgPtr_pco->ClntTable_s->procCntConflict_u8++;
        #endif
    }
}

/************************************************************************************************************/
/* rba_EthDHCP_ClientLocalIpAddrAssignmentChg()-Function call to provide the information for the IP Address */
/* assignment : IP Address / Subnet Mask / Router IP                                                        */
/* Input Parameters :                                                                                       */
/*      lDHCP_ClientSubCfgPtr_pco - Client Structure on which the changing is done.                         */
/*      lTypeAssignmentChange_en - Type of Assignment Action (UNASSIGNED / ASSIGNED)                */
/* return :                                                                                                 */
/*                                                                                                          */
/************************************************************************************************************/
static void rba_EthDHCP_ClientLocalIpAddrAssignmentChg( const rba_EthDHCP_ClntSubConfig_tst * lDHCP_ClientSubCfgPtr_pco,
                                                        TcpIp_IpAddrStateType lTypeAssignmentChange_en)
{
    /* Local variable declaration */
    TcpIp_IPAddrParamType_tun lIPAddrParam_un;

    /* Update the IP address to assign */
    lIPAddrParam_un.IPv4Par_st.IpAddr_u32 = lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddress_u32;

    /* Update the Default Gateway parameter */
    lIPAddrParam_un.IPv4Par_st.DftRtr_u32 = lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionRouter_u32;

    /* Update the SubnetMask parameter */
    lIPAddrParam_un.IPv4Par_st.NetMask_u32 = lDHCP_ClientSubCfgPtr_pco->ClntTable_s->optionSubnetMaskId_u32;

    /* Now we can bind to an IP address and use it */
    TcpIp_LocalIpAddrAssignmentChg( lDHCP_ClientSubCfgPtr_pco->ClntLocalAddrId_u8,
                                    lTypeAssignmentChange_en,
                                    &lIPAddrParam_un,
                                    TCPIP_IPADDR_ASSIGNMENT_DHCP,
                                    TCPIP_AF_INET);

    lDHCP_ClientSubCfgPtr_pco->ClntTable_s->clientIpAddressState_en = lTypeAssignmentChange_en;

    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientGetSocket()                                                                                    */
/* By this API service a UDP socket is allocated with a pre-configured port number and configured LocalAddrId. the  */
/* state of the socket allocation is updated in the variable rba_EthDHCP_SocketState.                               */
/* This API calls socket API's: TcpIp_GetSocket() and TcpIp_Bind()                                                  */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*          lClientSubConfig_st - Client SubConfig Structure for which the ports have to be checked.                */
/*          lLocalAddrId_tu8    - LocalAddrId of the Client.                                                        */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
static Std_ReturnType rba_EthDHCP_ClientGetSocket( const rba_EthDHCP_ClntSubConfig_tst * lClientSubConfig_st)
{
    /* Local variable declaration */
    uint16                 lClientUdpPort_u16;
    uint16                 lUDPClientSocket_u16;
    TcpIp_LocalAddrIdType  lDHCPLocalAddrId_u8;
    Std_ReturnType         lDHCPStdRetType_tu8;
    Std_ReturnType         lStdRetVal_tu8;

    /* Initialize local variables */
    lStdRetVal_tu8          = E_NOT_OK;
    lDHCPStdRetType_tu8     = E_NOT_OK;
    lDHCPLocalAddrId_u8     = 0xFF;
    lClientUdpPort_u16      = 0;

    /* Call the socket allocation API to allocate a UDP socket in the TcpIp table */
    lDHCPStdRetType_tu8 = TcpIp_DhcpGetSocket( TCPIP_AF_INET,
                                               TCPIP_IPPROTO_UDP,
                                               &lUDPClientSocket_u16 );

    if( E_OK == lDHCPStdRetType_tu8 )
    {
        /* The socket allocation is successful call the TcpIp_Bind() API to bind
         * this socket to pre-configured port number and configured LocalAddrId. */

        lDHCPLocalAddrId_u8 = (uint8) lClientSubConfig_st->ClntLocalAddrId_u8;
        lClientUdpPort_u16  = TCPIP_DHCPCLIENT_PORT;

        lDHCPStdRetType_tu8 = TcpIp_Bind( lUDPClientSocket_u16,
                                          lDHCPLocalAddrId_u8,
                                          &lClientUdpPort_u16 );

        if( E_OK == lDHCPStdRetType_tu8 )
        {
            /* If TcpIp_Bind() calling is successful change the state of DHCP Client SocketState
            * to BOUND, so that rba_EthDHCP_GetSocket() is not called again. */
            lClientSubConfig_st->ClntTable_s->procSocketState_ten   = RBA_ETHDHCP_SOCK_BOUND;
            lClientSubConfig_st->ClntTable_s->procSocketId_u16      = lUDPClientSocket_u16;

            lStdRetVal_tu8 = E_OK;
        }
    }

    return lStdRetVal_tu8;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientCloseSocket                                                                                    */
/*                      - By this API service the TCP/IP stack is requested to close the socket and release all     */
/*                      related resources. The service TcpIp_Close() shall perform the following actions for the    */
/*                      socket specified by SocketId in case it is a TCP socket:                                    */
/*                      (a) If the connection is active and                                                         */
/*                          (a1)abort = FALSE: the connection shall be terminated after performing a regular        */
/*                          (a2) abort = TRUE: connection shall immediately be terminated by sending a RST-Segment  */
/*                          and releasing all related resources.                                                    */
/*                      (b) If the socket is in the Listen state, the Listen state shall be left immediately and    */
/*                      related resources shall be released.                                                        */
/*                      The service TcpIp_Close() shall release all related resources immediately for the socket    */
/*                      specified by SocketId in case it is a UDP socket.                                           */
/* Input Parameters :                                                                                               */
/* SocketId             - Socket identifier of the related local socket resource.                                   */
/* Abort                - TRUE: Connection will immediately be terminated by sending a RST-Segment and releasing    */
/*                      all related resources.                                                                      */
/*                       FALSE: Connection will be terminated after performing a regular connection termination     */
/*                      handshake and releasing all related resources.                                              */
/*                      socket to.                                                                                  */
/* Output Parameters :                                                                                              */
/* Std_ReturnType       - E_OK / E_NOT_OK - Result of operation                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientCloseSocket( rba_EthDHCPClient_Table_tst * lClientStruct_st )
{

    /* Call the socket API to close the UDP socket in the TcpIp table */
    (void)TcpIp_Close( lClientStruct_st->procSocketId_u16, FALSE );

    lClientStruct_st->procSocketState_ten = RBA_ETHDHCP_SOCK_NOTBOUND;

    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientSendDhcpFrame()                                                                                */
/* This API is used to execute the mechanism (Provide and Transmit) to transmit a frame on the network              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lClientStruct_t - DHCP Client structure used to fill the frame to send.                                     */
/*      lTypeOfFrame_u8 - DHCP Type frame that is sent.                                                             */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* Std_ReturnType       - E_OK / E_NOT_OK - Result of the sending operation                                         */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType rba_EthDHCP_ClientSendDhcpFrame( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                       uint8 lTypeOfFrame_u8 )
{
    /* Local variable declaration */
    TcpIp_ReturnType    lUdpTxRetVal_en;
    Std_ReturnType      lDHCPStdRetType_tu8;
    uint16              lLengthOfFrame_u16;
    rba_EthDHCP_Hdr_tst lDhcpFrame_st;
    uint8               lData_au8[RBA_ETHDHCP_CLIENT_MSG_MAXLENGTH];

    /* Initialize local variables */
    lDHCPStdRetType_tu8 = E_NOT_OK;
    lLengthOfFrame_u16  = 0;

    /* Start to build the DHCP Frame to get the length. */
    rba_EthDHCP_ClientUpdateDhcpFrame(  lClientStruct_st,
                                        &lDhcpFrame_st,
                                        lTypeOfFrame_u8,
                                        &lLengthOfFrame_u16);

    /* The prepared DHCP frame is stored in lData_au8. */
    rba_EthDHCP_ClientUpdateTxBuffer (  &lDhcpFrame_st,
                                        &lData_au8[0],
                                        lLengthOfFrame_u16);

    /* Trigger transmission of DHCP frame by calling TcpIp_UdpTransmit() with input parameter DataPtr as lData_au8. */
    /* MR12 RULE 11.3 VIOLATION : typecasted to TcpIp_SockAddrType to match the AUTOSAR API Definition. */
    lUdpTxRetVal_en = TcpIp_UdpTransmit( lClientStruct_st->procSocketId_u16,
                                         &lData_au8[0],
                                         ( ( TcpIp_SockAddrType *)( &rba_EthDHCP_SrvrSockAddr_st ) ),
                                         lLengthOfFrame_u16 );

    /* Frame Transmission done ? */
    if( TCPIP_OK == lUdpTxRetVal_en )
    {/* The frame has been transmitted */
        lDHCPStdRetType_tu8 = E_OK;
    }
    else
    { /* Frame not transmitted */
        lDHCPStdRetType_tu8 = E_NOT_OK;
    }

    return lDHCPStdRetType_tu8;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientUpdateDhcpFrame()                                                                              */
/* By this API the DHCP Frame Structure is filled with specific Clients parameters depending on                     */
/* the type of frame sent.                                                                                          */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lClientStruct_t - Structure of the Client for which the Structure must be filled.                           */
/*      lDhcpFrame_st - Structure that must be updated with the Clients parameters.                                 */
/*      lTypeOfFrame_u8 - Type of frame to prepare (DISCOVER / REQUEST /...)                                        */
/*                                                                                                                  */
/* In/Output Parameters :                                                                                           */
/*      lLengthOfFrame_u8 - Length of frame returned at the end of the function.                                    */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientUpdateDhcpFrame( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                               rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                               uint8 lTypeOfFrame_u8,
                                               uint16 * lLengthOfFrame_u16)
{
    /* Local Variables */
    uint16 lLoop_u16;
    sint16 lLoop_s16;
    uint16 lOffset_u16;

    /* Local Variable Initialization */
    lLoop_u16   = 0;
    lLoop_s16   = 0;
    lOffset_u16 = 0;

    /* 'op' / 'htype' / 'hlen' / 'hops' the values are the same during all the process */
    lDhcpFrame_st->packetOpcode_u8   = RBA_ETHDHCP_BOOTREQUEST;     /* Code Operation       */
    lDhcpFrame_st->hwType_u8         = RBA_ETHDHCP_HWTYPE_ETH;      /* Hardware Type        */
    lDhcpFrame_st->hwAddrLen_u8      = RBA_ETHDHCP_HWADDR_LENGTH;   /* HW address length    */
    lDhcpFrame_st->hops_u8           = 0;                           /*                      */

    /* DHCP Frame - Xid */
    lDhcpFrame_st->xId_u32 = lClientStruct_st->clientXId_u32;
    /* DHCP Frame - Flags */
    lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_UNICAST;
    /* Client IP Address */
    lDhcpFrame_st->ciAddr_u32 = (uint32) 0;

    switch( lTypeOfFrame_u8 )
    {
        case RBA_ETHDHCP_DISCOVER:
        {
            rba_EthDHCP_ClientUpdate_DiscoverFrm (  lClientStruct_st,
                                                    lDhcpFrame_st,
                                                    &lOffset_u16);
        }
        break;

        case RBA_ETHDHCP_REQUEST:
        {

            rba_EthDHCP_ClientUpdate_RequestFrm (  lClientStruct_st,
                                                   lDhcpFrame_st,
                                                   &lOffset_u16);
        }
        break;

        case RBA_ETHDHCP_DECLINE:
        {
            /* DHCP Frame - Flags */
            lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_UNICAST;
            /* Client IP Address */
            lDhcpFrame_st->ciAddr_u32 = (uint32) 0;

            /*********************/
            /* Mandatory Options */

            /* Message Type */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + lOffset_u16] = RBA_ETHDHCP_DECLINE;

            lOffset_u16 += (uint16) (RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH);

            /* Requested IP address */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_REQUESTEDIP_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_REQUESTEDIP_LENGTH;

            TcpIp_WriteU32(
                    (&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0 + lOffset_u16]),
                    (uint32) lClientStruct_st->clientIpAddress_u32);

            lOffset_u16 += (uint16) (RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_REQUESTEDIP_LENGTH);

            /* Server ID */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_SERVERID_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_SERVERID_LENGTH;

            TcpIp_WriteU32(
                    (&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0 + lOffset_u16]),
                    (uint32) lClientStruct_st->optionServerId_u32);

            lOffset_u16 += (uint16) (RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_SERVERID_LENGTH);

        }
        break;

        case RBA_ETHDHCP_RELEASE:
        {
            /* DHCP Frame - Flags */
            lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_UNICAST;

            /* Client IP Address */
            lDhcpFrame_st->ciAddr_u32 = lClientStruct_st->clientIpAddress_u32;

            /*********************/
            /* Mandatory Options */

            /* Message Type */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + lOffset_u16] = RBA_ETHDHCP_RELEASE;

            /* Header + Length of data */
            lOffset_u16 += (uint16) (RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH);

            /* Server ID */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_SERVERID_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_SERVERID_LENGTH;

            TcpIp_WriteU32(
                    (&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0 + lOffset_u16]),
                    (uint32) lClientStruct_st->optionServerId_u32);

            /* Header + Length of data */
            lOffset_u16 += (uint16) (RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_SERVERID_LENGTH);

        }
        break;

        case RBA_ETHDHCP_INFORM:
        {
            /* DHCP Frame - Flags */
            lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_BROADCAST;
            /* Client IP Address */
            lDhcpFrame_st->ciAddr_u32 = lClientStruct_st->clientIpAddress_u32;

            /* DHCP Frame - Xid */
            lDhcpFrame_st->xId_u32 = lClientStruct_st->clientXIdInform_u32;

            /*********************/
            /* Mandatory Options */

            /* Message Type */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + lOffset_u16] = RBA_ETHDHCP_INFORM;

            lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH);

            /********************/
            /* Optional Options */

            /* Parameter request */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_INFRM_LENGTH;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + lOffset_u16] = RBA_ETHDHCP_OPTION_FQDN_ID;

            lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_PARAMRQSTLIST_INFRM_LENGTH);

            /*  FQDN option */

            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS     + lOffset_u16] = RBA_ETHDHCP_OPTION_FQDN_ID;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + lOffset_u16] = ((uint8)lClientStruct_st->ReqFQDNLen_u16) + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN;

            /* Write 3 bytes of FQDN flags here */
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_FLAGPOS + lOffset_u16]   = RBA_ETHDHCP_OPTION_FQDN_FLAG_VALUE;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE1POS + lOffset_u16] = RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE2POS + lOffset_u16] = RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;



            /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
            (void)rba_BswSrv_MemCopy(((void*)&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_DATAPOS0 + lOffset_u16]),
                                ((const void*)&lClientStruct_st->ReqFQDNBuf_au8[0U]),
                                lClientStruct_st->ReqFQDNLen_u16);

            lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + lClientStruct_st->ReqFQDNLen_u16 + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN);



        }
        break;

        default :
        {
            /* Nothing to Do */
        }
        break;
    }

    /**************************/
    /* Mandatory End Of Frame */
    /* Add the End Of Frame byte (0xFF) to the end of the option part */
    lDhcpFrame_st->options_au8[lOffset_u16] = RBA_ETHDHCP_OPTION_ENDOPT;
    lOffset_u16 += (uint16) RBA_ETHDHCP_OPTION_ENDOPT_LENGTH;

    /* DHCP Frame - YIADDR / SIADDR / GIADDR => Always set to 0 */
    lDhcpFrame_st->yiAddr_u32 =(uint32) 0;
    lDhcpFrame_st->siAddr_u32 =(uint32) 0;
    lDhcpFrame_st->giAddr_u32 =(uint32) 0;

    /* DHCP Frame - CHADDR */
    for(lLoop_u16 = 0;lLoop_u16 < RBA_ETHDHCP_CHADDR_LENGTH; lLoop_u16++)
    {
        if(RBA_ETHDHCP_HWADDR_LENGTH > lLoop_u16)
        {
            lDhcpFrame_st->clientHwAddr_au8[lLoop_u16] = lClientStruct_st->clientHwAddr_au8[lLoop_u16];
        }
        else
        {
            /* Set the end of CHADDR to 0 */
            lDhcpFrame_st->clientHwAddr_au8[lLoop_u16] = 0;
        }
    }

    /* Set the ServerHost Name part of the Buffer to 0  */
    for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_SRVRNAME_LENGTH; lLoop_u16++)
    {
        lDhcpFrame_st->serverName_au8[lLoop_u16] = 0;
    }

    /* Set the BootFile Name part of the Buffer to 0  */
    for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_BOOTFILENAME_LENGTH; lLoop_u16++)
    {
        lDhcpFrame_st->bootFile_au8[lLoop_u16] = 0;
    }

    /* DHCP Frame - Magic Cookie */
    /* Update the magic cookie field with the configured value. Size - 4 bytes */
    lDhcpFrame_st->magicCookie_u32 = RBA_ETHDHCP_MAGICCOOKIE_VAL;


    /* If the Length of the current DHCP Frame is < 300 Bytes => Padding Bytes */
    if( RBA_ETHDHCP_MIN_FRAME_SIZE > (uint32)(lOffset_u16 + RBA_ETHDHCP_OPTION_POS))
    {
        /* Add padding bytes */
        for(lLoop_s16 = 0; lLoop_s16 < ((sint16)RBA_ETHDHCP_MIN_FRAME_SIZE-((sint16)lOffset_u16 + (sint16)RBA_ETHDHCP_OPTION_POS)); lLoop_s16++)
        {
            lDhcpFrame_st->options_au8[lOffset_u16 + lLoop_s16] = 0;
        }

        /* Return the minimal Length of the DHCP Frame */
        *lLengthOfFrame_u16 = RBA_ETHDHCP_MIN_FRAME_SIZE;
    }
    else
    {
        /* Return the Length of the DHCP Frame */
        *lLengthOfFrame_u16 = (lOffset_u16 + RBA_ETHDHCP_OPTION_POS);
    }

    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientUpdate_DiscoverFrm()                                                                           */
/* This function is called to Update the Discover packet for transmission                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lClientStruct_st         - client option table                                                                  */
/*  lDhcpFrame_st         -  DHCP Header frame structure                                                            */
/*  lOffset_u16         - Offset of DHCP frame                                                                      */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
LOCAL_INLINE void  rba_EthDHCP_ClientUpdate_DiscoverFrm ( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                          rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                                          uint16 * lOffset_u16)
{
    /* Local Variables */
    uint16 lLoop_u16;


    lLoop_u16 = 0;


    /* DHCP Frame - Flags */
    lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_BROADCAST;

    /* Client IP Address */
    lDhcpFrame_st->ciAddr_u32 = (uint32) 0;

    /* DHCP Frame - Xid */

    /*********************/
    /* Mandatory Options */

    /* Message Type */
    lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_MSGTYPE_ID;
    lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH;
    lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + (*lOffset_u16)] = RBA_ETHDHCP_DISCOVER;

    *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH);

    /********************/
    /* Optional Options */

    /* Parameter Request List */
    if( TRUE == lClientStruct_st->optionParamRqstListFlag_b )
    {
        lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_ID;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH;

        for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH; lLoop_u16++)
        {
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + (*lOffset_u16) + lLoop_u16]
                                       = lClientStruct_st->optionParamRqstList_au8[lLoop_u16];
        }

        *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH);
    }
    else
    {   /* Option not used by the customer */
    }

    /*  FQDN option */
    if(lClientStruct_st->ReqFQDNLen_u16 !=0 )
    {
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS + (*lOffset_u16)] =  RBA_ETHDHCP_OPTION_FQDN_ID;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS + (*lOffset_u16)] = ((uint8)lClientStruct_st->ReqFQDNLen_u16) + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_FLAGPOS+ (*lOffset_u16)]   =RBA_ETHDHCP_OPTION_FQDN_FLAG_VALUE;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE1POS+ (*lOffset_u16)] =RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE2POS+ (*lOffset_u16)] =RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;

        /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
        (void)rba_BswSrv_MemCopy(((void*)&lDhcpFrame_st->options_au8[(RBA_ETHDHCP_OPTFRM_FQDN_DATAPOS0  + *lOffset_u16)]),
                        ((const void*)&lClientStruct_st->ReqFQDNBuf_au8[0U]),
                        lClientStruct_st->ReqFQDNLen_u16);

        *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + lClientStruct_st->ReqFQDNLen_u16 + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN);
    }

}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientUpdate_RequestFrm()                                                                            */
/* This function is called to Update the Request packet for transmission                                            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lClientStruct_st         - client option table                                                                  */
/*  lDhcpFrame_st         -  DHCP Header frame structure                                                            */
/*  lOffset_u16         - Offset of DHCP frame                                                                      */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/********************************************************************************************************************/
 LOCAL_INLINE void  rba_EthDHCP_ClientUpdate_RequestFrm ( const rba_EthDHCPClient_Table_tst * lClientStruct_st,
                                                         rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                                         uint16 * lOffset_u16)
{
    /* Local Variables */
    uint16 lLoop_u16;



    lLoop_u16 = 0;


    /*********************/
    /* Mandatory Options */

    /* Message Type */
    lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_MSGTYPE_ID;
    lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH;
    lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + (*lOffset_u16)] = RBA_ETHDHCP_REQUEST;

    *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH);

    if( RBA_ETHDHCP_CLNTSELECTING == lClientStruct_st->procState_ten )
    {
        /* In this state, the client broadcast
         * a request to all the server on the network */

        /* DHCP Frame - Flags */
        lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_BROADCAST;
        /* Client IP Address */
        lDhcpFrame_st->ciAddr_u32 = (uint32) 0;


        /* Requested IP Addr Option and Server ID Option MUST NOT appear
         * for a DHCP REQUEST Frame send in the RENEW and REBIND State. */
        lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_REQUESTEDIP_ID;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS     + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_REQUESTEDIP_LENGTH;

        /* Requested IP Address */
        TcpIp_WriteU32(
                (&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0 + (*lOffset_u16)]),
                (uint32) lClientStruct_st->clientIpAddress_u32);

        *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_REQUESTEDIP_LENGTH);

        /* Server ID To respond to an OFFER Frame */
        lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_SERVERID_ID;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS     + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_SERVERID_LENGTH;

        TcpIp_WriteU32(
                (&lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0 + (*lOffset_u16)]),
                (uint32) lClientStruct_st->optionServerId_u32);

        *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_SERVERID_LENGTH);
    }
    else if (RBA_ETHDHCP_CLNTRENEWING == lClientStruct_st->procState_ten)
    {
        /* In this case, the client wants to renew
         * the lease time with the current lease Server  */

        /* DHCP Frame - Flags */
        lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_UNICAST;
        /* Client IP Address */
        lDhcpFrame_st->ciAddr_u32 = lClientStruct_st->clientIpAddress_u32;
    }
    else if (RBA_ETHDHCP_CLNTREBINDING == lClientStruct_st->procState_ten)
    {
        /* In this case, the client wants to broadcast a DHCP request */

        /* DHCP Frame - Flags */
        lDhcpFrame_st->flags_u16 = RBA_ETHDHCP_BROADCAST;
        /* Client IP Address */
        lDhcpFrame_st->ciAddr_u32 = lClientStruct_st->clientIpAddress_u32;
    }
    else
    {
        /* Nothing */
    }

    /* Parameter Request List */
    if( TRUE == lClientStruct_st->optionParamRqstListFlag_b )
    {
        lDhcpFrame_st->options_au8[(*lOffset_u16)] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_ID;
        lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS    + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH;

        for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH; lLoop_u16++)
        {
            lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_DATAPOS0  + (*lOffset_u16) + lLoop_u16]
                                       = lClientStruct_st->optionParamRqstList_au8[lLoop_u16];
        }

        *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH);
    }

    /*  FQDN option */
    if(lClientStruct_st->ReqFQDNLen_u16 !=0)
    {
           lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_IDPOS + (*lOffset_u16)] = RBA_ETHDHCP_OPTION_FQDN_ID;
           lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_LENPOS + (*lOffset_u16)] = ((uint8)lClientStruct_st->ReqFQDNLen_u16) + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN;
           lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_FLAGPOS+ (*lOffset_u16)]   =RBA_ETHDHCP_OPTION_FQDN_FLAG_VALUE;
           lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE1POS+ (*lOffset_u16)] =RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;
           lDhcpFrame_st->options_au8[RBA_ETHDHCP_OPTFRM_FQDN_RCODE2POS+ (*lOffset_u16)] =RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE;

           /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
           (void)rba_BswSrv_MemCopy(((void*)&lDhcpFrame_st->options_au8[(RBA_ETHDHCP_OPTFRM_FQDN_DATAPOS0 + *lOffset_u16)]),
                                      ((const void*)&lClientStruct_st->ReqFQDNBuf_au8[0U]),
                                      lClientStruct_st->ReqFQDNLen_u16);

           *lOffset_u16 += (uint16)(RBA_ETHDHCP_OPTFRM_DATAPOS0 + lClientStruct_st->ReqFQDNLen_u16 + RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN + RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN);
    }

}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientUpdateOptionsInClientStruct()                                                                  */
/* This API is used to update the Client Structure give as parameter with the Options received from the Server and  */
/* stored into the lDHCP_ClientOptionsTable_st structure.                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lDHCP_ClientOptionsTable_pst - Pointer to the Option Struct in which the Options reveived have been stored. */
/*      lEthDHCP_Client_pst          - Pointer to the Client Struct to update with the options values received.     */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
 /* HIS METRIC PATH  VIOLATION IN rba_EthDHCP_ClientUpdateOptionsInClientStruct: PATH - Design is optimal and cannot be split further. */
static void rba_EthDHCP_ClientUpdateOptionsInClientStruct ( const rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_pst,
                                                            rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst)
{
    uint32 lTempMulValue_u32;

    lTempMulValue_u32 =0;

    /*********************/
    /* Renew Time Option */
    /*********************/

    /* If Renew Time Option has been received */
    if( lDHCP_ClientOptionsTable_pst->rxOptionRenewalTimeRxFlag_b == TRUE )
    {

        /* If Option received => The Renew Time is calculated from the value received */
        lTempMulValue_u32 = rba_EthDHCP_ClientMul_u32u32_u32_Inl( lDHCP_ClientOptionsTable_pst->rxOptionRenewalTime_u32, (uint32)RBA_ETHDHCP_TICKTIME );

        if(lTempMulValue_u32 > lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32)
        {
            lTempMulValue_u32 = lTempMulValue_u32 - lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32;
        }
        else
        {
            lTempMulValue_u32 = 0;
        }

        lEthDHCP_Client_pst->clientRenewTickTime_u32 =  lTempMulValue_u32;

    }
    else
    {
        /* If Option not received => The Renew Time is calculated from the Lease Time received */
        lTempMulValue_u32 = rba_EthDHCP_ClientMul_u32u32_u32_Inl( lDHCP_ClientOptionsTable_pst->rxOptionLeaseTime_u32, (uint32)RBA_ETHDHCPCLIENT_T1COEFF_TICK );

        if(lTempMulValue_u32 > lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32)
          {
              lTempMulValue_u32 = lTempMulValue_u32 - lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32;
          }
          else
          {
              lTempMulValue_u32 = 0;
          }

        lEthDHCP_Client_pst->clientRenewTickTime_u32 =  lTempMulValue_u32;
    }

    /**********************/
    /* Rebind Time Option */
    /**********************/

    /* If Rebind Time Option has been received */
    if( lDHCP_ClientOptionsTable_pst->rxOptionRebindingTimeRxFlag_b == TRUE )
    {
        /* If Option received => The Renew Time is calculated from the value received */

        lTempMulValue_u32 = rba_EthDHCP_ClientMul_u32u32_u32_Inl( lDHCP_ClientOptionsTable_pst->rxOptionRebindingTime_u32, (uint32)RBA_ETHDHCP_TICKTIME );

        if(lTempMulValue_u32 > lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32)
          {
              lTempMulValue_u32 = lTempMulValue_u32 - lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32;
          }
          else
          {
              lTempMulValue_u32 = 0;
          }

        lEthDHCP_Client_pst->clientRebindTickTime_u32 = lTempMulValue_u32;

    }
    else
    {
        /* If Option not received => The Rebind Time is calculated from the Lease Time received */
        lTempMulValue_u32 = rba_EthDHCP_ClientMul_u32u32_u32_Inl( lDHCP_ClientOptionsTable_pst->rxOptionLeaseTime_u32, (uint32)RBA_ETHDHCP_TICKTIME * 7U);

        if(lTempMulValue_u32<RBA_ETHDHCP_MAXUINT32)
        {
            lTempMulValue_u32 = lTempMulValue_u32 /8U;
        }

        if(lTempMulValue_u32 > lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32)
          {
              lTempMulValue_u32 = lTempMulValue_u32 - lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32;
          }
          else
          {
              lTempMulValue_u32 = 0;
          }

        lEthDHCP_Client_pst->clientRebindTickTime_u32 = lTempMulValue_u32;

    }


    /*********************/
    /* Lease Time Option */
    /*********************/
    if( lDHCP_ClientOptionsTable_pst->rxOptionLeaseTimeRxFlag_b == TRUE )
    {
        /* No conditions for the Lease Time => It's a mandatory function */
        /* Convert the times received as Tick Time to use in the state machine */
        lTempMulValue_u32 = rba_EthDHCP_ClientMul_u32u32_u32_Inl( lDHCP_ClientOptionsTable_pst->rxOptionLeaseTime_u32, (uint32)RBA_ETHDHCP_TICKTIME );

        if(lTempMulValue_u32 > lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32)
          {
              lTempMulValue_u32 = lTempMulValue_u32 - lEthDHCP_Client_pst->clientLeaseDeltaTickTime_u32;
          }
          else
          {
              lTempMulValue_u32 = 0;
          }
        lEthDHCP_Client_pst->clientLeaseTickTime_u32 =  lTempMulValue_u32;

    }

    /********************/
    /* Server Id Option */
    /********************/
    if( lDHCP_ClientOptionsTable_pst->rxOptionServerIdRxFlag_b == TRUE )
    {
        /* No conditions for the Server Id => It's a mandatory function */
        lEthDHCP_Client_pst->optionServerId_u32  = lDHCP_ClientOptionsTable_pst->rxOptionServerId_u32;
    }

    /**************************/
    /* Default Gateway Option */
    /**************************/

    if(TRUE == lDHCP_ClientOptionsTable_pst->rxOptionRouterRxFlag_b)
    {
        /* From the Server Options received */
        lEthDHCP_Client_pst->optionRouter_u32 = lDHCP_ClientOptionsTable_pst->rxOptionRouter_u32;
    }
    else
    {
        lEthDHCP_Client_pst->optionRouter_u32 = 0;
    }

    /**********************/
    /* Subnet Mask Option */
    /**********************/

    if(TRUE == lDHCP_ClientOptionsTable_pst->rxOptionSubnetMaskIdRxFlag_b)
    {
        lEthDHCP_Client_pst->optionSubnetMaskId_u32 = lDHCP_ClientOptionsTable_pst->rxOptionSubnetMaskId_u32;
    }
    else
    {
        lEthDHCP_Client_pst->optionSubnetMaskId_u32 = 0;
    }


    /**********************/
    /* FQDN Option   */
    /**********************/

    if(lDHCP_ClientOptionsTable_pst->rxOptionFQDNLen_u16 !=0 )
    {
        /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
        /* DHCP Options RxFrom - FQDN */
        (void)rba_BswSrv_MemCopy((void*)&(lEthDHCP_Client_pst->KwnFQDNBuf_au8[0U]),
                (const void*)&(lDHCP_ClientOptionsTable_pst->rxFQDNTempBufr_au8[0U]),
                                lDHCP_ClientOptionsTable_pst->rxOptionFQDNLen_u16 );

        /* Updated the length of the FQDN received */
        lEthDHCP_Client_pst->KwnFQDNLen_u16 = lDHCP_ClientOptionsTable_pst->rxOptionFQDNLen_u16 ;
    }
    else
    {
        lEthDHCP_Client_pst->KwnFQDNLen_u16 = 0;
    }



    return;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientUpdateTxBuffer()                                                                               */
/* This API is used to update the Tx Buffer with the DHCP Frame previously built into                               */
/* rba_EthDHCP_ClientUpdateDhcpFrame ()                                                                             */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lDhcpFrame_st       - DHCP Frame to transmit on the network.                                                */
/*      lClntFrmBuf_au8     - Buffer provided by lower layer and to fill with the data.                             */
/*      lLengthOfFrame_u16  - Length of the frame to send.                                                          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientUpdateTxBuffer( const rba_EthDHCP_Hdr_tst * lDhcpFrame_st,
                                              uint8 * lClntFrmBuf_au8,
                                              uint16 lLengthOfFrame_u16)
{
    /* Local Variables */
    uint16 lLoop_u16;
    sint16 lLoop_s16;

    /* Local Variable Initialization */
    lLoop_u16 = 0;
    lLoop_s16 = 0;

    /* 'op' / 'htype' / 'hlen' / 'hops' the values are the same during all the process */
    lClntFrmBuf_au8[ RBA_ETHDHCP_OP_POS ]            = lDhcpFrame_st->packetOpcode_u8;  /* Code Operation       */
    lClntFrmBuf_au8[ RBA_ETHDHCP_HWTYPE_POS ]        = lDhcpFrame_st->hwType_u8;        /* Hardware Type        */
    lClntFrmBuf_au8[ RBA_ETHDHCP_HWADDR_LENGTH_POS ] = lDhcpFrame_st->hwAddrLen_u8;     /* HW address length    */
    lClntFrmBuf_au8[ RBA_ETHDHCP_HOPS_POS ]          = 0;                               /*                      */

    /* DHCP Frame - Xid */
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_XID_POS]), (uint32) lDhcpFrame_st->xId_u32);

    /* DHCP Frame - Secs */
    TcpIp_WriteU16((&lClntFrmBuf_au8[RBA_ETHDHCP_SEC_ELAPSED_POS]), 0);

    /* DHCP Frame - Flags */
    TcpIp_WriteU16((&lClntFrmBuf_au8[RBA_ETHDHCP_BOOTPFLAGS_POS]), (uint16) lDhcpFrame_st->flags_u16);
    /* Client IP Address */
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_CIADDR_POS]),(uint32) lDhcpFrame_st->ciAddr_u32);

    /* DHCP Frame - YIADDR / SIADDR / GIADDR => Always set to 0 */
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_YIADDR_POS]),(uint32) lDhcpFrame_st->yiAddr_u32);
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_SIADDR_POS]),(uint32) lDhcpFrame_st->siAddr_u32);
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_GIADDR_POS]),(uint32) lDhcpFrame_st->giAddr_u32);

    /* DHCP Frame - CHADDR*/
    for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_CHADDR_LENGTH; lLoop_u16++)
    {
        lClntFrmBuf_au8[RBA_ETHDHCP_CHADDR_POS + lLoop_u16] = lDhcpFrame_st->clientHwAddr_au8[lLoop_u16];
    }

    /* Set the ServerHost Name part of the Buffer to 0  */
    for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_SRVRNAME_LENGTH; lLoop_u16++)
    {
        lClntFrmBuf_au8[RBA_ETHDHCP_SERVER_HOSTNAME_POS + lLoop_u16] = lDhcpFrame_st->serverName_au8[lLoop_u16];
    }

    /* Set the BootFile Name part of the Buffer to 0  */
    for(lLoop_u16 = 0; lLoop_u16 < RBA_ETHDHCP_BOOTFILENAME_LENGTH; lLoop_u16++)
    {
        lClntFrmBuf_au8[RBA_ETHDHCP_BOOT_FILENAME_POS + lLoop_u16] = lDhcpFrame_st->bootFile_au8[lLoop_u16];
    }

    /* DHCP Frame - Magic Cookie */
    /* Update the magic cookie field with the configured value. Size - 4 bytes */
    TcpIp_WriteU32((&lClntFrmBuf_au8[RBA_ETHDHCP_MAGIC_COOKIE_POS]), (uint32) lDhcpFrame_st->magicCookie_u32);

    /* DHCP Frame -  Option Part */
    for(lLoop_s16 = 0; lLoop_s16 < ((sint16)lLengthOfFrame_u16 - (sint16)RBA_ETHDHCP_OPTION_POS); lLoop_s16++)
    {
        lClntFrmBuf_au8[(sint16)RBA_ETHDHCP_OPTION_POS + lLoop_s16] = lDhcpFrame_st->options_au8[lLoop_s16];
    }
}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientGetOption()                                                                                    */
/* This API is used to parse the 'options' field and concatenate the fragments of option in the DHCP frame received.*/
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lDataPtr_pu8    - Buffer containing the DHCP Frame received.                                                */
/*      lOptionPtr_pu8  - Destination buffer that will contain the concatenated option.                             */
/*      lOptionCode_u8  - Option code that should be extracted from the DHCP Frame received.                        */
/*      lDataLength_u16 - Length of the DHCP Frame Received.                                                        */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/*      lOptionPtr_pu8  - Destination buffer that will contain the concatenated option.                             */
/*                                                                                                                  */
/* Return                                                                                                           */
/*      returns the length of concatenated option                                                                   */
/********************************************************************************************************************/
static uint8 rba_EthDHCP_ClientGetOption( const uint8 * lDataPtr_pu8,
                                          uint8 * lOptionPtr_pu8,
                                          uint8 lOptionCode_u8,
                                          uint16 lDataLength_u16)
{

    /* Local variable declarations */
    uint16 lOptionIndex_u16;
    uint8 lOptionLength_u8;
    uint8 lOptionFragByte_u8;
    uint8 lNxtOptionFragByte_u8;
    boolean lSkipFQDNOptionHeader_b;


    /* Initialize local variables */
    lOptionIndex_u16        = 0;
    lOptionLength_u8        = 0;
    lOptionFragByte_u8      = 0;
    lNxtOptionFragByte_u8   = 0;
    lSkipFQDNOptionHeader_b = TRUE;

    /* Checks if the length is less than 255 or if the END option is  reached*/
    while(((lOptionIndex_u16 < lDataLength_u16) && (lOptionLength_u8 < RBA_ETHDHCP_OPTION_MAX_LENGTH )) && (RBA_ETHDHCP_OPTION_ENDOPT != lDataPtr_pu8[lOptionIndex_u16]))
    {
        /* Check for PAD option*/
        if(lDataPtr_pu8[lOptionIndex_u16] == RBA_ETHDHCP_OPTION_PADOPT)
        {
            /*If PAD option is received,  increment the index*/
            lOptionIndex_u16 = lOptionIndex_u16 + RBA_ETHDHCP_OPTION_PADOPT_SIZE;
            continue;
        }

        /*Update the Offset value*/
        lNxtOptionFragByte_u8 = RBA_ETHDHCP_OPTION_NONFQDN_OFFSET_VALUE + lDataPtr_pu8[lOptionIndex_u16 + RBA_ETHDHCP_OPTION_OFFSET_CODE];

        /*Checks if Option Code is found*/
        if(lDataPtr_pu8[lOptionIndex_u16] == lOptionCode_u8)
        {
            /*Checks if FQDN Option is received*/
            if((lDataPtr_pu8[lOptionIndex_u16] == RBA_ETHDHCP_OPTION_FQDN_ID) && (lSkipFQDNOptionHeader_b == TRUE))
            {
                /*If 1st fragment of FQDN  is received add FLAG + RCODE0 + RCODE1  Offset */
                lOptionFragByte_u8 = RBA_ETHDHCP_OPTION_FQDN_OFFSET_VALUE;
                lSkipFQDNOptionHeader_b  = FALSE;
            }
            else
            {
                /* If any other option or remaining fragment of FQDN is received */
                lOptionFragByte_u8 = RBA_ETHDHCP_OPTION_NONFQDN_OFFSET_VALUE;
            }

            /*If Fragment length is less than 255 copy Option into buffer*/
            while((lOptionFragByte_u8 < lNxtOptionFragByte_u8) && (lOptionLength_u8 < RBA_ETHDHCP_OPTION_MAX_LENGTH))
            {
                lOptionPtr_pu8[lOptionLength_u8] = (lDataPtr_pu8[lOptionIndex_u16 + lOptionFragByte_u8]);
                lOptionLength_u8++;
                lOptionFragByte_u8++;
            }
        }

        /*Update the position of index to point to the next option*/
        lOptionIndex_u16 = lOptionIndex_u16 + lNxtOptionFragByte_u8;

    }

    /* Return the sum of length of all fragments of a particular Option*/
    return lOptionLength_u8;
}



/********************************************************************************************************************/
/* rba_EthDHCP_ClientParseOptionPart()                                                                              */
/* This API is used to parse the Option part of the DHCP frame received.                                            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lDHCP_ClientOptionsTable_pst - Pointer to the Option struct to update with the Options received from Server */
/*      lDataPtr_pu8    - Buffer Containing the Option Part of the DHCP Frame received.                             */
/*      lDataLength_u16 - Length of the Option part of the DHCP Frame Received.                                     */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientParseOptionPart( rba_EthDHCPClient_OptionsTable_tst * lDHCP_ClientOptionsTable_pst,
                                               const uint8 * lDataPtr_pu8,
                                               uint16 lDataLength_u16)
{
    /* Local variable declarations */
    uint8 lrxOptionBuf_au8[RBA_ETHDHCP_OPTION_MAX_LENGTH];
    uint8 lOptionLength_u8;

    /* Initialize local variables */
    lOptionLength_u8    = 0;

    /* Init the Rx Flags */
    lDHCP_ClientOptionsTable_pst->rxOptionLeaseTimeRxFlag_b         = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionRebindingTimeRxFlag_b     = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionRenewalTimeRxFlag_b       = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionMessTypeRxFlag_b          = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionRouterRxFlag_b            = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionServerIdRxFlag_b          = FALSE;
    lDHCP_ClientOptionsTable_pst->rxOptionSubnetMaskIdRxFlag_b      = FALSE;

    lDHCP_ClientOptionsTable_pst->rxOptionFQDNLen_u16               = 0;

    /* SubnetMask */
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_SUBNETMASK_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if(RBA_ETHDHCP_OPTION_SUBNETMASK_LENGTH ==  lOptionLength_u8)
    {
            /*  Read DHCP Option - SubnetMask*/
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                            &(lDHCP_ClientOptionsTable_pst->rxOptionSubnetMaskId_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionSubnetMaskIdRxFlag_b = TRUE;


    }

    /*Message type*/
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_MSGTYPE_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if(RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH ==  lOptionLength_u8)
    {

        /* Read DHCP Option - Message Type */
        lDHCP_ClientOptionsTable_pst->rxOptionMessType_u8 =(uint8)(lrxOptionBuf_au8[0]);

        /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
        lDHCP_ClientOptionsTable_pst->rxOptionMessTypeRxFlag_b = TRUE;


    }

    /*Router ID*/
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_ROUTER_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if((0U == (lOptionLength_u8 % 4U)) && (lOptionLength_u8 >= 4U) )
    {
            /* Read DHCP Option - Router Option, read only 1st 4 bytes */
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                             &(lDHCP_ClientOptionsTable_pst->rxOptionRouter_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionRouterRxFlag_b = TRUE;


    }

    /*Lease Time*/
   lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_LEASETIME_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if(RBA_ETHDHCP_OPTION_LEASETIME_LENGTH == lOptionLength_u8 )
    {
            /* Read DHCP Option - Lease Time */
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                             &(lDHCP_ClientOptionsTable_pst->rxOptionLeaseTime_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionLeaseTimeRxFlag_b = TRUE;


    }

    /*Server ID*/
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_SERVERID_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if(RBA_ETHDHCP_OPTION_SERVERID_LENGTH == lOptionLength_u8 )
    {
            /*  Read DHCP Option  - Lease Time */
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                            &(lDHCP_ClientOptionsTable_pst->rxOptionServerId_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionServerIdRxFlag_b = TRUE;

    }

    /*Renewal Time*/
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_RENEWALTIME_ID,lDataLength_u16 );

    /* Check if the length is correct */
    if(RBA_ETHDHCP_OPTION_RENEWALTIME_LENGTH == lOptionLength_u8 )
    {
            /*  Read DHCP Option  - Renewal Time */
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                             &(lDHCP_ClientOptionsTable_pst->rxOptionRenewalTime_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionRenewalTimeRxFlag_b = TRUE;


    }

    /*Rebind Time*/
    lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_REBINDINGTIME_ID,lDataLength_u16 );

    /* Check if the length is correct */
   if(RBA_ETHDHCP_OPTION_REBINDINGTIME_LENGTH == lOptionLength_u8 )
   {
            /*  Read DHCP Option  - Rebind Time */
            TcpIp_ReadU32(  (&lrxOptionBuf_au8[0U]),
                             &(lDHCP_ClientOptionsTable_pst->rxOptionRebindingTime_u32));

            /* Set the Flag to TRUE to indicate that a value has been received and can be processed */
            lDHCP_ClientOptionsTable_pst->rxOptionRebindingTimeRxFlag_b = TRUE;


   }

   /*FQDN*/
   lOptionLength_u8 = rba_EthDHCP_ClientGetOption(lDataPtr_pu8, &lrxOptionBuf_au8[0],RBA_ETHDHCP_OPTION_FQDN_ID,lDataLength_u16 );

   if(lOptionLength_u8 > 0)
   {
       /* MR12 DIR 1.1 VIOLATION: cast needed by design of rba_BswSrv_MemCopy. No risk. */
        /* DHCP Options RxFrom - FQDN */
        (void)rba_BswSrv_MemCopy( (void*)&(lDHCP_ClientOptionsTable_pst->rxFQDNTempBufr_au8[0U]),
                                 ((const void*)&lrxOptionBuf_au8[0]),
                                 lOptionLength_u8);

        /*Set the length of the option obtaind from GetOption() */
        lDHCP_ClientOptionsTable_pst->rxOptionFQDNLen_u16 = lOptionLength_u8;
   }

}
/********************************************************************************************************************/
/* rba_EthDHCP_ClientCleanToRestart()                                                                               */
/* This API is used to initialize the Client structure provided as parameter in case of restarting.                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lEthDHCP_Client - DHCP Client Structure to reinit.                                                          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientCleanToRestart( rba_EthDHCPClient_Table_tst * lEthDHCP_Client_st)
{
        /* Clean the Client Part */
    lEthDHCP_Client_st->procCnt_u32                 = 0;
    lEthDHCP_Client_st->procReTransmissionTime_u32  = RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK;
    lEthDHCP_Client_st->procCntLastConflict_u32     = 0;
    lEthDHCP_Client_st->procCntTxFr_u32             = 0;
    lEthDHCP_Client_st->procCntTxProbeFr_u8         = 0;
    lEthDHCP_Client_st->procCntTxGratuFr_u8         = 0;


    lEthDHCP_Client_st->clientIpAddress_u32         = 0;
    /* IP Address is now unknown => Value 0.0.0.0 */
    lEthDHCP_Client_st->clientIpAddressState_en     = TCPIP_IPADDR_STATE_INIT;

    lEthDHCP_Client_st->serverIpAddress_u32         = 0;
    lEthDHCP_Client_st->serverRespType_ten          = RBA_ETHDHCP_SRVROFF;

    /* Clean the Servers Options  - These variables must not be longer read by upper layer */
    lEthDHCP_Client_st->optionServerId_u32          = 0;
    lEthDHCP_Client_st->optionLeaseTime_u32         = 0;
    lEthDHCP_Client_st->clientLeaseTickTime_u32     = 0;
    lEthDHCP_Client_st->optionRouter_u32            = 0;
    lEthDHCP_Client_st->optionSubnetMaskId_u32      = 0;
    lEthDHCP_Client_st->optionRenewalTime_u32       = 0;
    lEthDHCP_Client_st->clientRenewTickTime_u32     = 0;
    lEthDHCP_Client_st->optionRebindingTime_u32     = 0;
    lEthDHCP_Client_st->clientRebindTickTime_u32    = 0;
    lEthDHCP_Client_st->KwnFQDNLen_u16              = 0;
    lEthDHCP_Client_st->clientLeaseDeltaTickTime_u32= 0;
}


/********************************************************************************************************************/
/* rba_EthDHCP_ClientRetransmission()                                                                               */
/* This API is used to calculate the next Retransmission parameters of the Assignment Process.                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lEthDHCP_Client_pst - Pointer to the DHCP Client Structure to update with the retransmission algorithm.     */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
static void rba_EthDHCP_ClientRetransmission( rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst)
{
    /* BackOff algorithm as describe into the RFC 2131 */


    if(lEthDHCP_Client_pst->procCntTxFr_u32 >= RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST)
    {
        /* Limit the counter of retransmission at Max Tx value */
        lEthDHCP_Client_pst->procCntTxFr_u32 = RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST;

    }
    else
    {
        /* Nothing to do */
    }
    /*TcpIp_RandNumXor64() gets random number of size uint32 in range 0 to 199 */

    /* To the retransmission time adjust with a random delay */
    lEthDHCP_Client_pst->procReTransmissionTime_u32
            =  (((uint32)RBA_ETHDHCP_INIT_RETRANSMIT_TIME_TICK << lEthDHCP_Client_pst->procCntTxFr_u32) + (TcpIp_RandNumXor64() % (RBA_ETHDHCP_TICKTIME * 2U)) - RBA_ETHDHCP_TICKTIME);



    return;
}



/********************************************************************************************************************/
/* rba_EthDHCP_ClientGetMacIDForDestIp()                                                                            */
/* This API is used to trigger the ARP frames to get the MAC Address of the server or the Router                    */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lCtrlIdx_u8         - Ctrler Index                                                                          */
/*      lEthDHCP_Client_pst - Pointer to the Client Table                                                           */
/* Output Parameters :                                                                                              */
/*              TcpIp_ReturnType                                                                                    */
/*                                                                                                                  */
/********************************************************************************************************************/
static TcpIp_ReturnType rba_EthDHCP_ClientGetMacIDForDestIp( uint8 lCtrlIdx_u8,
                                                             const rba_EthDHCPClient_Table_tst * lEthDHCP_Client_pst)
{
    TcpIp_ReturnType lRetVal_en;
    uint8            lDestEthAddr_au8[6];

    /* Local variable Initialization */
    lRetVal_en  = TCPIP_E_NOT_OK;

    /* Check if the Server is in the same Sub net, If Yes send ARP request to the Server Ip address else sent the ARP Request to the Default router */
    if ( ((lEthDHCP_Client_pst->clientIpAddress_u32)&(lEthDHCP_Client_pst->optionSubnetMaskId_u32)) == ((lEthDHCP_Client_pst->optionServerId_u32)&(lEthDHCP_Client_pst->optionSubnetMaskId_u32)) )
    {

        lRetVal_en = rba_EthArp_GetDestEthAddr(     lCtrlIdx_u8,
                                                    lEthDHCP_Client_pst->optionServerId_u32,
                                                    ( &( lDestEthAddr_au8[0] ) ) );
    }
    else
    {
        lRetVal_en = rba_EthArp_GetDestEthAddr(     lCtrlIdx_u8,
                                                    lEthDHCP_Client_pst->optionRouter_u32,
                                                    ( &( lDestEthAddr_au8[0] ) ) );
    }

    return lRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthDHCP_ClientMul_u32u32_u32_Inl()                                                                           */
/* This API is used to calculate the Timer value of the Renew,Rebind,Lease and also to detect the Overflow of timers*/
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      lTimerVal_u32 -  Timer value provided by the DHCP server                                                    */
/*      lTickVal_u32  -  Tick value                                                                                 */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*      lRes_u32    -   Timer value in uint32                                                                       */
/*                                                                                                                  */
/********************************************************************************************************************/

static uint32 rba_EthDHCP_ClientMul_u32u32_u32_Inl( uint32 lTimerVal_u32,
                                                    uint32 lTickVal_u32)
{
    uint32 lRes_u32;

    lRes_u32 =0;


    if (lTickVal_u32 == (uint32)0)
    {
        lRes_u32 = 0;
    }
    else
    {
        lRes_u32 = RBA_ETHDHCP_MAXUINT32 / lTickVal_u32;

        if (lRes_u32 >= lTimerVal_u32)
        {
            lRes_u32 = lTimerVal_u32 * lTickVal_u32;
        }
        else
        {
            lRes_u32 = RBA_ETHDHCP_MAXUINT32;
        }
    }

    return (lRes_u32);
}

#define RBA_ETHDHCP_STOP_SEC_CODE
#include "rba_EthDHCP_MemMap.h"


#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) ) */




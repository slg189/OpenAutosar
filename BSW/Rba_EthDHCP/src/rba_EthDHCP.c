

#include "TcpIp.h"

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )
#include "rba_EthDHCP.h"
#include "rba_EthDHCP_Prv.h"

/*
****************************************************************************************************
*                                   Declarations
****************************************************************************************************
*/

/*
****************************************************************************************************
*                                   Common Functions
****************************************************************************************************
*/
#define RBA_ETHDHCP_START_SEC_CODE
#include "rba_EthDHCP_MemMap.h"
/********************************************************************************************************************/
/* rba_EthDHCP_Init() - Initialize the DHCP table (Client and Server) and the DHCP global variables.                */
/* Called from TcpIp Init function.                                                                                 */
/*                                                                                                                  */
/* Input Parameters :       void                                                                                    */
/*      rba_EthDHCP_Config_cst  - Pointer to the configuration data of the TcpIp->DHCP module.                      */
/* Output Parameters :      void                                                                                    */
/*                                                                                                                  */
/********************************************************************************************************************/
void rba_EthDHCP_Init( const rba_EthDHCP_Config_to * rba_EthDHCP_Config_cst )
{
    if( NULL_PTR != rba_EthDHCP_Config_cst )
    {
        /** CLIENT INITIALIZATION **/
        #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
        rba_EthDHCP_ClientInit(rba_EthDHCP_Config_cst->rba_EthDHCP_ClientConfig_cst);
        #endif

    }
}

/************************************************************************************************************/
/* rba_EthDHCP_Start() - Start the DHCP sequence to get an IP address from DHCP server                      */
/* Called from TcpIp Address Assignment function.                                                           */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* lLocalAddrId_tu8 - IP address index specifying the IP address for which an assignment shall be done.     */
/*                                                                                                          */
/* Std_ReturnType   - Result of operation :                                                                 */
/*                      E_OK : DHCP process has been started.                                               */
/*                      E_NOT_OK : DHCP process has not been launched.                                      */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_Start( TcpIp_LocalAddrIdType lLocalAddrId_tu8  )
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en      = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    /* Call the function defined in the client C file */
    lRetVal_en = rba_EthDHCP_ClientStart(lLocalAddrId_tu8);
    #endif

    return lRetVal_en;
}

/************************************************************************************************************/
/* rba_EthDHCP_Start() - Stop the DHCP sequence to get or to maintain an IP address from DHCP server        */
/* Called from TcpIp function.                                                                              */
/*                                                                                                          */
/* Input Parameters :                                                                                       */
/* lLocalAddrId_tu8 - IP address index specifying the IP address for which an assignment shall be stopped.  */
/*                                                                                                          */
/* Std_ReturnType   - Result of operation :                                                                 */
/*                      E_OK : DHCP process has been stopped.                                               */
/*                      E_NOT_OK : DHCP process has not been stopped.                                       */
/*                                                                                                          */
/************************************************************************************************************/
Std_ReturnType rba_EthDHCP_Stop( TcpIp_LocalAddrIdType lLocalAddrId_tu8  )
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    /* Call the function defined in the client C file */
    lRetVal_en = rba_EthDHCP_ClientStop(lLocalAddrId_tu8);
    #endif

    return lRetVal_en;
}

/********************************************************************************************************************/
/*                                                                                                                  */
/* rba_EthDHCP_RxIndication() - This API service is called by the rba_EthUdp module on reception of a DHCP frame.   */
/*                      This API check the type of frame received. If it is a DHCP-DISCOVER frame then it updates   */
/*                      the DHCP table with Client details.                                                         */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  lSocketIdOut_u16    - SocketId on which the DHCP frame was received.                                            */
/*  lIPAddrPort_ps      - Pointer to IP address of the DHCPClient and the port on which the remote node transmitted */
/*                        this frame.                                                                               */
/*  lBufPtr_pu8         - uint8 pointer to a buffer containing a DHCP frame.                                        */
/*  lLength_u16         - Length of the received frame.                                                             */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*                        void                                                                                      */
/*                                                                                                                  */
/********************************************************************************************************************/
/* MR12 RULE 8.13 VIOLATION: P2VAR is defined in AUTOSAR for lBufPtr_pu8 for Up_RxIndication() API. DHCP is considered as an internal upper layer to TcpIp. */
void rba_EthDHCP_RxIndication( TcpIp_SocketIdType lSocketIdOut_u16,
                               const TcpIp_SockAddrType * lIPAddrPort_ps,
                               uint8 * lBufPtr_pu8,
                               uint16 lLength_u16 )
{
    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
        if( (RBA_ETHDHCP_OP_POS<lLength_u16) && (RBA_ETHDHCP_BOOTREPLY == lBufPtr_pu8[RBA_ETHDHCP_OP_POS]) )
        {
            rba_EthDHCP_ClientRxIndication ( lSocketIdOut_u16,
                                             lIPAddrPort_ps,
                                             lBufPtr_pu8,
                                             lLength_u16 );
        }
    #endif


    return;
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
void rba_EthDHCP_MainFunction ( void )
{
    /* Main Function - Client Part */
    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    rba_EthDHCP_ClientMainFunction ();
    #endif
    /*******************************/

}



/*
****************************************************************************************************
*                       Functions used to the IP Conflict Detection functionality
****************************************************************************************************
*/

/*************************************************************************************************/
/* rba_EthDHCP_GetIpAddress() - Function called by the ARP Component                             */
/* This function is called to provide the current IP Address in process to ARP to realize the IP */
/* Conflict detection internally.                                                                */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  CtrlIdx_u8          - index of the Ethernet controller                                       */
/* Output Parameters :                                                                           */
/*  lLocalIpAddress_u32 - DHCP address currently got from Server and checked by the process.     */
/*                                                                                               */
/* return lRetVal_u8.                                                                            */
/*      E_OK : DHCP Client table has been found and the IP Address is well returned.             */
/*      E_NOT_OK : DHCP Client table has not been found and the IP Address is not returned       */
/*************************************************************************************************/
Std_ReturnType rba_EthDHCP_GetIpAddress(   uint8 lCtrlIdx_u8,
                                           uint32 * lLocalIpAddress_pu32,
                                           TcpIp_IpAddrStateType * lCurrentIpState_pen)
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en      = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    /* Call the function defined in the client C file */
    lRetVal_en = rba_EthDHCP_ClientGetIpAddress( lCtrlIdx_u8,
                                                 lLocalIpAddress_pu32,
                                                 lCurrentIpState_pen );
    #endif

    return lRetVal_en;
}

/*************************************************************************************************/
/* rba_EthDHCP_IpConflictDetected() - Function called by the ARP Component                       */
/* This function is called to check the Ip received in the ARP frame and to detect if there is   */
/* no conflict between Ip address.                                                               */
/*                                                                                               */
/* Input Parameters :                                                                            */
/*  CtrlIdx_u8          - index of the Ethernet controller                                       */
/*                                                                                               */
/* return lRetVal_en.                                                                            */
/*************************************************************************************************/
Std_ReturnType rba_EthDHCP_IpConflictDetected ( uint8 lCtrlIdx_u8 )
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en      = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    /* Call the function defined in the client C file */
    lRetVal_en = rba_EthDHCP_ClientIpConflictDetected( lCtrlIdx_u8 );
    #endif
    return lRetVal_en;
}

/*
****************************************************************************************************
*                                   Common Functions to handle the option part
****************************************************************************************************
*/

/************************************************************************************************************/
/* rba_EthDHCP_ReadOption() -                                                                               */
/* Function called from TcpIp_DhcpReadOption()                                                              */
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
Std_ReturnType rba_EthDHCP_ReadOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                       uint8 Option,
                                       uint8 * DataLength,
                                       uint8 * DataPtr)
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    lRetVal_en = rba_EthDHCP_ClientReadOption(  LocalIpAddrId,
                                                Option,
                                                DataLength,
                                                DataPtr);
    #endif

    return lRetVal_en;
}

/************************************************************************************************************/
/* rba_EthDHCP_WriteOption() -                                                                              */
/* Function called from TcpIp_DhcpWriteOption()                                                             */
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
Std_ReturnType rba_EthDHCP_WriteOption( TcpIp_LocalAddrIdType LocalIpAddrId,
                                        uint8 Option,
                                        uint8 DataLength,
                                        const uint8 * DataPtr)
{
    /* Local variable declaration */
    Std_ReturnType lRetVal_en;

    /* Local variable initialization */
    lRetVal_en = E_NOT_OK;

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    lRetVal_en = rba_EthDHCP_ClientWriteOption( LocalIpAddrId,
                                                Option,
                                                DataLength,
                                                DataPtr);
    #endif

    return lRetVal_en;
}

#define RBA_ETHDHCP_STOP_SEC_CODE
#include "rba_EthDHCP_MemMap.h"

#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )  */




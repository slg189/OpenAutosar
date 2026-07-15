/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */


#include "rba_EthIcmp_Stubs.h"

#define RBA_ETHICMP_START_SEC_VAR_INIT_8
#include "rba_EthIcmp_MemMap.h"
uint8   TestIcmp_UpperlayerRxIndication = 0 ;
#define RBA_ETHICMP_STOP_SEC_VAR_INIT_8
#include "rba_EthIcmp_MemMap.h"

/**
 *********************************************************************
 * CDD_ULIcmpRxIndication(): ICMP message handler function
 *
 * Template function for Icmp upper layer Rx Indication.
 * The function name is configurable in TcpIpIcmpConfig/TcpIpIcmpMsgHandler/TcpIpIcmpMsgHandlerName.
 * The function prototype has to be the same as below.
 * It is invoked in rba_EthIcmp_RxIndication.
 *
 * \param    LocalAddrId_u8:    EthIfCtrl and local IP address derived in IPv4 module from IP header
 * \param    RemoteAddr_pst:    Remote node IP address
 * \param    Ttl_u8:            TTL = 0 indicates default value will be used
 * \param    Type_u8:           ICMP Control Message Type
 * \param    Code_u8:           ICMP Control Message Code
 * \param    DataLength_u16:    DataLength
 * \param    Data_pu8:          Pointer to ICMP data field
 *
 * \return
 * \retval   E_OK:              Rx message is handled successfully.
 * \retval   E_NOT_OK:          Rx message is not handled successfully.
 * \seealso
 * \usedresources
 *********************************************************************
 */

#define RBA_ETHICMP_START_SEC_CODE
#include "rba_EthIcmp_MemMap.h"
Std_ReturnType CDD_ULIcmpRxIndication  ( VAR(TcpIp_LocalAddrIdType, AUTOMATIC )                 LocalAddrId_u8,
                                         P2VAR( TcpIp_SockAddrType, AUTOMATIC, AUTOMATIC )      RemoteAddr_pst,
                                         VAR( uint8,         AUTOMATIC )                        Ttl_u8,
                                         VAR( uint8,         AUTOMATIC )                        Type_u8,
                                         VAR( uint8,         AUTOMATIC )                        Code_u8,
                                         VAR( uint16,        AUTOMATIC )                        DataLength_u16,
                                         P2VAR( uint8,       AUTOMATIC, AUTOMATIC )             Data_pu8 )
{
    COMPILER_RB_PARAMUNUSED(LocalAddrId_u8);
    COMPILER_RB_PARAMUNUSED(RemoteAddr_pst);
    COMPILER_RB_PARAMUNUSED(Ttl_u8);
    COMPILER_RB_PARAMUNUSED(Type_u8);
    COMPILER_RB_PARAMUNUSED(Code_u8);
    COMPILER_RB_PARAMUNUSED(DataLength_u16);
    COMPILER_RB_PARAMUNUSED(Data_pu8);
#ifdef BSWSIM
    TestIcmp_UpperlayerRxIndication = 1;
    return E_OK ;
#else
    return E_OK ;
#endif
}
#define RBA_ETHICMP_STOP_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

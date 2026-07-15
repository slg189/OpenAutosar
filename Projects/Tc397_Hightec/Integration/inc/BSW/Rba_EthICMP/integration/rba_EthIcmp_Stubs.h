/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */


#ifndef RBA_ETHICMP_STUBS_H_
#define RBA_ETHICMP_STUBS_H_

#include "TcpIp.h"

extern uint8 TestIcmp_UpperlayerRxIndication;

extern  Std_ReturnType  CDD_ULIcmpRxIndication   (  VAR(TcpIp_LocalAddrIdType, AUTOMATIC )                 LocalAddrId_u8,
                                                    P2VAR( TcpIp_SockAddrType, AUTOMATIC, AUTOMATIC )      RemoteAddr_pst,
                                                    VAR( uint8,         AUTOMATIC )                        Ttl_u8,
                                                    VAR( uint8,         AUTOMATIC )                        Type_u8,
                                                    VAR( uint8,         AUTOMATIC )                        Code_u8,
                                                    VAR( uint16,        AUTOMATIC )                        DataLength_u16,
                                                    P2VAR( uint8,       AUTOMATIC, AUTOMATIC )             Data_pu8 );

#endif /* RBA_ETHICMP_STUBS_H_ */

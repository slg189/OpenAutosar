

#ifndef TCPIP_LIB_H
#define TCPIP_LIB_H

#include "TcpIp_Cfg.h"

#ifdef TCPIP_CONFIGURED
#include "TcpIp_Types.h"



#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

extern uint32 TcpIp_RandNumXor64( void );

#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
extern Std_ReturnType TcpIp_OnesComplChkSum ( const void * Data_pcv,
                                              const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                              uint16 Length_u16,
                                              uint16 * OutputChkSum_pu16 );
#endif


/* Local inline declarations */
LOCAL_INLINE void TcpIp_ReadU16( const uint8 * Data_pcu8,
                                 uint16 * Data_pu16 );

LOCAL_INLINE void TcpIp_ReadU32( const uint8 * Data_pcu8,
                                 uint32 * Data_pu32 );

LOCAL_INLINE void TcpIp_WriteU16( uint8 * Data_pu8,
                                  uint16 Data_u16 );

LOCAL_INLINE void TcpIp_WriteU32( uint8 * Data_pu8,
                                  uint32 Data_u32 );


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_ReadU16    - This API packs or read two consecutive uint8 memory locations into a uint16 variable          */
/*                                                                                                                  */
/* Service ID       - 0x32                                                                                          */
/* Sync/Async       - Synchronous                                                                                   */
/* Reentrancy       - Reentrant                                                                                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  Data_pu8        - Pointer to uint8 memory location                                                              */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  Data_pu16        - Pointer to uint16 memory location updated with two uint8 variables                           */
/*                                                                                                                  */
/********************************************************************************************************************/
LOCAL_INLINE void TcpIp_ReadU16( const uint8 * Data_pcu8,
                                 uint16 * Data_pu16 )
{
    *Data_pu16 = (uint16)((uint16)((uint16)Data_pcu8[0] << 8u ) |
                                          (Data_pcu8[1] ));

    return;
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_ReadU32    - This API packs or read four consecutive uint8 memory locations into a uint32 variable         */
/*                                                                                                                  */
/* Service ID       - 0x33                                                                                          */
/* Sync/Async       - Synchronous                                                                                   */
/* Reentrancy       - Reentrant                                                                                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  Data_pu8        - Pointer to uint8 memory location                                                              */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  Data_pu32        - Pointer to uint32 memory location updated with four uint8 variables                          */
/*                                                                                                                  */
/********************************************************************************************************************/
LOCAL_INLINE void TcpIp_ReadU32( const uint8 * Data_pcu8,
                                 uint32 * Data_pu32 )
{
    *Data_pu32 = (uint32)( ( (uint32)Data_pcu8[0] << 24u ) |
                           ( (uint32)Data_pcu8[1] << 16u ) |
                           ( (uint32)Data_pcu8[2] <<  8u ) |
                           (uint32)Data_pcu8[3] );

    return;
}


/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_WriteU16    - This API unpacks or writes into two consecutive uint8 memory locations from a uint16 variable*/
/*                     in Network Byte Order                                                                        */
/*                                                                                                                  */
/* Service ID       - 0x34                                                                                          */
/* Sync/Async       - Synchronous                                                                                   */
/* Reentrancy       - Reentrant                                                                                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  Data_u16        - Input uint16 variable which is to be unpacked                                                 */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  Data_pu8        - The unpacked uint16 variable is updated into an address pointed by this variable              */
/*                                                                                                                  */
/********************************************************************************************************************/
LOCAL_INLINE void TcpIp_WriteU16( uint8 * Data_pu8,
                                  uint16 Data_u16 )
{
    Data_pu8[0] = (uint8)( ( Data_u16 & 0xFF00uL ) >> 8u );
    Data_pu8[1] = (uint8)(   Data_u16 & 0x00FFuL );

    return;
}


/*********************************************************************************************************************/
/*                                                                                                                   */
/* TcpIp_WriteU32    - This API unpacks or writes into four consecutive uint8 memory locations from a uint32 variable*/
/*                    in Network Byte Order                                                                          */
/*                                                                                                                   */
/* Service ID       - 0x35                                                                                           */
/* Sync/Async       - Synchronous                                                                                    */
/* Reentrancy       - Reentrant                                                                                      */
/*                                                                                                                   */
/* Input Parameters :                                                                                                */
/*  Data_u32        - Input uint32 variable which is to be unpacked                                                  */
/*                                                                                                                   */
/* Output Parameters :                                                                                               */
/*  Data_pu8        - The unpacked uint32 variable is updated into an address pointed by this variable               */
/*                                                                                                                   */
/*********************************************************************************************************************/
LOCAL_INLINE void TcpIp_WriteU32( uint8 * Data_pu8,
                                  uint32 Data_u32 )
{
    Data_pu8[0] = (uint8)( ( Data_u32 & 0xFF000000uL ) >> 24u );
    Data_pu8[1] = (uint8)( ( Data_u32 & 0x00FF0000uL ) >> 16u );
    Data_pu8[2] = (uint8)( ( Data_u32 & 0x0000FF00uL ) >>  8u );
    Data_pu8[3] = (uint8)(   Data_u32 & 0x000000FFuL );

    return;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
#endif /* TCPIP_LIB_H */

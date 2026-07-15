

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cfg_SchM.h"

#include "TcpIp_Lib.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_RandNumXor64()  - This API Generates the pseudo random number based on XorShift with 2 seed                */
/*                         Basic formula is to generate random number based on a ^ (a shift[Left and Right] count)  */
/*                                                                                                                  */
/* Service ID           - 0x43                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant                                                                                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* NIL                                                                                                              */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  NIL                                                                                                             */
/*                                                                                                                  */
/* Return  :                                                                                                        */
/* uint32 - Generated random Number                                                                                 */
/*                                                                                                                  */
/********************************************************************************************************************/
uint32 TcpIp_RandNumXor64( void )
{
    uint32 lTemp_u32;

    /* Enter critical section ( to avoid race condition if TcpIp_RandNumXor64 is reentered ) */
    SchM_Enter_TcpIp_ExclusiveAreaRandNumGenerating();

    lTemp_u32 = ( TcpIp_RandNumSeed1_u32 ^ ( TcpIp_RandNumSeed1_u32 << 23u ) );

    TcpIp_RandNumSeed1_u32 = TcpIp_RandNumSeed2_u32;
    TcpIp_RandNumSeed2_u32 = ( ( TcpIp_RandNumSeed2_u32 ^ (TcpIp_RandNumSeed2_u32 >> 17u ) ) ^ ( lTemp_u32 ^ ( lTemp_u32 >> 5u ) ) );

    lTemp_u32 =  TcpIp_RandNumSeed2_u32;

    /* Exit critical section ( to avoid race condition if TcpIp_RandNumXor64 is reentered ) */
    SchM_Exit_TcpIp_ExclusiveAreaRandNumGenerating();

    return lTemp_u32;
}


/*********************************************************************************************************************/
/* TcpIp_OnesComplChkSum() - Function to calculate ones compliment checksum                                          */
/*                                                                                                                   */
/* Service ID                    - 0x1A                                                                              */
/* Sync/Async                    - Synchronous                                                                       */
/* Reentrancy                    - Reentrant                                                                         */
/*                                                                                                                   */
/* Input Parameters :                                                                                                */
/* Parameter- Data_pcv           - Buffer pointer to void that contains data on which the checksum must be calculated*/
/*          - PseudoHdr_pcst     - Pointer to pseudo header structure. Contains information about protocol type,     */
/*                                 local IP and Remote IP in Tx/Rx buffer.                                         */
/*                                 If it's NULL_PTR, indicates Pseudo header is not required. Eg: Ipv4, Icmp          */
/*          - Length_u16         - Length of data in the buffer                                                      */
/*                                                                                                                   */
/* Output Parameters :                                                                                               */
/*          - OutputChkSum_pu16  - The calculated ones compliment checksum                                         */
/*                                                                                                                   */
/* Return   - Result status of operation :                                                                           */
/*          - Std_ReturnType     - E_OK if checksum calculation is successful                                         */
/*                               - E_NOT_OK if checksum calculation failed                                            */
/*                                                                                                                   */
/*********************************************************************************************************************/
/* API TcpIp_OnesComplChkSum() is required only if checksum is configured for either IPv4, Udp, Tcp or Icmp and the corresponding protocols are enabled */
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
Std_ReturnType TcpIp_OnesComplChkSum ( const void * Data_pcv,
                                       const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                       uint16 Length_u16,
                                       uint16 * OutputChkSum_pu16 )
{
    /* Local variable declaration */
    const uint16 *                        lBuf_pcu16;
    uint32                                lShiftLeft_u32;
    uint32                                lPseudoHdrSum_u32;
    uint32                                lSumAlign_u32;
    Std_ReturnType                        lRetType_en;

    /* Initialisation */
    lShiftLeft_u32          = Length_u16;
    lSumAlign_u32           = 0;
    /* MR12 RULE 11.5 VIOLATION: Cast uint16 pointer is required for faster checksum calculation. It is done on the Tx/Rx buffer that should be 2 byte aligned */
    lBuf_pcu16               = ( ( const uint16 * )Data_pcv );
    lPseudoHdrSum_u32       = 0;
    lRetType_en             = E_OK;

    /* Report DET if Data_pcv is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR ==  Data_pcv ), TCPIP_E_ONES_COMPL_CHKSUM_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if OutputChkSum_pu16 is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR ==  OutputChkSum_pu16 ), TCPIP_E_ONES_COMPL_CHKSUM_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    /* Algorithm : Using a 32 bit accumulator (lSumAlign_u32), we add sequential 16 bit words to it */
    /* it, and at the end, fold back all the carry bits from the top 16 bits into the lower 16 bits */
    while( lShiftLeft_u32 > 1u )
    {
        lSumAlign_u32   = ( lSumAlign_u32 + ( *lBuf_pcu16 ) );
        lBuf_pcu16++;
        lShiftLeft_u32  = ( lShiftLeft_u32 - 2u );
    }

    /* Mop up an odd byte, if necessary */
    if( 1u == lShiftLeft_u32 )
    {
#if ( TCPIP_ENDIANESS == TCPIP_LITTLE_ENDIAN )
        lSumAlign_u32   = lSumAlign_u32 + ( ( *lBuf_pcu16 ) & 0x00FFu );
#else
        /* In case of big endian, odd byte from buffer shall be appended with zero */
        lSumAlign_u32   = lSumAlign_u32 + ( ( *lBuf_pcu16 ) & 0xFF00u );
#endif
    }

    /* Include pseudo header for checksum calculation only for Tcp and Udp */
    /* Tcp and Udp shall pass valid pointer for PseudoHdr_pcst, and other protocols (IPv4 and Icmp) shall pass NULL_PTR */
    if( PseudoHdr_pcst != NULL_PTR )
    {

        switch (PseudoHdr_pcst->IpDomainType_t)
        {
#if (TCPIP_IPV4_ENABLED == STD_ON)
            case TCPIP_AF_INET:
            {
                uint32    lLocalAddr_u32     = PseudoHdr_pcst->LocalIpAddr_un.IPv4Addr_u32;
                uint32    lRemoteAddr_u32    = PseudoHdr_pcst->RemoteIpAddr_un.IPv4Addr_u32;

                lPseudoHdrSum_u32 = ( lLocalAddr_u32 & 0x0000FFFFuL )  + (( lLocalAddr_u32 & 0xFFFF0000uL ) >> 16u );
                lPseudoHdrSum_u32 += ( lRemoteAddr_u32 & 0x0000FFFFuL ) + (( lRemoteAddr_u32 & 0xFFFF0000uL ) >> 16u );
            }
            break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
            case TCPIP_AF_INET6:
            {
                uint32    lIdx_u32;
                uint32    lAddr_u32;

                for (lIdx_u32 = 0; lIdx_u32 < 4u; lIdx_u32++)
                {
                    lAddr_u32 = PseudoHdr_pcst->LocalIpAddr_un.IPv6Addr_pcu32[lIdx_u32];
                    lPseudoHdrSum_u32 += (lAddr_u32 & 0x0000FFFFuL ) + (( lAddr_u32 & 0xFFFF0000uL ) >> 16u );

                    lAddr_u32 = PseudoHdr_pcst->RemoteIpAddr_un.IPv6Addr_pcu32[lIdx_u32];
                    lPseudoHdrSum_u32 += (lAddr_u32 & 0x0000FFFFuL ) + (( lAddr_u32 & 0xFFFF0000uL ) >> 16u );
                }
            }
            break;
#endif

            default:
            {
                TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_ONES_COMPL_CHKSUM_API_ID, TCPIP_E_NOPROTOOPT )
                lRetType_en = E_NOT_OK;
            }
            break;
        }

        lPseudoHdrSum_u32 += (uint32)PseudoHdr_pcst->ProtoType_en + Length_u16;

        /* If the controller is little endian, swap the pseudo header sum to network byte order */
#if ( TCPIP_ENDIANESS == TCPIP_LITTLE_ENDIAN )
        /* BSW-15128 */
        while( ( lPseudoHdrSum_u32 >> 16u ) != 0u )
        {   /* Add higher 16 bits to lower 16 bits */
            lPseudoHdrSum_u32 = ( lPseudoHdrSum_u32 & (uint32)0x0000FFFFuL ) + ( lPseudoHdrSum_u32 >> 16u ) ;

        }
        /* END BSW-15128 */

        lPseudoHdrSum_u32 = ( ( lPseudoHdrSum_u32 & 0x000000FFuL ) << 8u )|
                            ( ( lPseudoHdrSum_u32 & 0x0000FF00uL ) >> 8u );
#endif

        lSumAlign_u32 += lPseudoHdrSum_u32;
    }

    /* Add back carry outs from top 16 bits to low 16 bits */
    while ( ( lSumAlign_u32 >> 16u ) != 0u )
    {   /* Add higher 16 bits to lower 16 bits */
        lSumAlign_u32 = ( lSumAlign_u32 & 0x0000FFFFuL ) + ( lSumAlign_u32 >> 16u ) ;
    }

    /* Truncate to 16 bits */
    (* OutputChkSum_pu16 )   = ( ~(uint16)lSumAlign_u32 );

    /* If the controller is little endian, swap the checksum to network byte order */
#if ( TCPIP_ENDIANESS == TCPIP_LITTLE_ENDIAN )
    (* OutputChkSum_pu16 )   = ( (uint16)( (* OutputChkSum_pu16 ) >> 8u ) | (uint16)( (* OutputChkSum_pu16 ) << 8u ) );
#endif

    return lRetType_en;
}
#endif



#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */

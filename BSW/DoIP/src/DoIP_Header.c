

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_Header_Write(uint16 payloadType_u16, uint32 payloadLength_u32, uint8* headerData_pu8)
{
    DoIP_PRV_WRITE_8BITS_AT_POS( headerData_pu8, DoIP_PRV_PROTOCOL_VERSION_POS, DOIP_PROTOCOL_VERSION )
    DoIP_PRV_WRITE_8BITS_AT_POS( headerData_pu8, DoIP_PRV_INV_PROTOCOL_VERSION_POS, DOIP_PROTOCOL_VERSION_INV )
    DoIP_PRV_WRITE_16BITS_AT_POS( headerData_pu8, DoIP_PRV_PAYLOAD_TYPE_POS, payloadType_u16 )
    DoIP_PRV_WRITE_32BITS_AT_POS( headerData_pu8, DoIP_PRV_PAYLOAD_LENGTH_POS, payloadLength_u32 )
}

/* MR12 RULE 8.13 VIOLATION: The tool does not recognize that the pointer is used in a memcopy macro and data is written, this is a tool issue */
void DoIP_Header_WriteOnlyPayloadType(uint16 payloadType_u16, uint8* headerData_pu8)
{
    DoIP_PRV_WRITE_16BITS_AT_POS( headerData_pu8, DoIP_PRV_PAYLOAD_TYPE_POS, payloadType_u16 )
}

void DoIP_Header_Read(const uint8* headerData_pcu8, DoIP_HeaderFields_tst* HeaderFields_pst)
{
    HeaderFields_pst->ProtoVer_u8       = DoIP_PRV_READ_8BITS_AT_POS( headerData_pcu8, DoIP_PRV_PROTOCOL_VERSION_POS );
    HeaderFields_pst->InvProtoVer_u8    = DoIP_PRV_READ_8BITS_AT_POS( headerData_pcu8, DoIP_PRV_INV_PROTOCOL_VERSION_POS );
    HeaderFields_pst->payloadType_u16     = DoIP_PRV_READ_16BITS_AT_POS( headerData_pcu8, DoIP_PRV_PAYLOAD_TYPE_POS );
    HeaderFields_pst->payloadLength_u32   = DoIP_PRV_READ_32BITS_AT_POS( headerData_pcu8, DoIP_PRV_PAYLOAD_LENGTH_POS );
}

uint16 DoIP_Header_GetPayloadType(const uint8* headerData_pcu8)
{
    return DoIP_PRV_READ_16BITS_AT_POS( headerData_pcu8, DoIP_PRV_PAYLOAD_TYPE_POS );
}

uint32 DoIP_Header_GetPayloadLength(const uint8* headerData_pcu8)
{
    return DoIP_PRV_READ_32BITS_AT_POS( headerData_pcu8, DoIP_PRV_PAYLOAD_LENGTH_POS );
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

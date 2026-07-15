

#ifndef ETHIF_PRV_TYPES_H
#define ETHIF_PRV_TYPES_H

/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */

/* -------------------------------------------- */
/* Enumerations                                 */
/* -------------------------------------------- */

/* [SWS_EthIf_00150] Status supervision used for Development Error Detection. The state shall be available for debugging */
typedef enum {
    ETHCTRL_STATE_UNINIT                            = 0x00U,    /* Ethernet Interface is not yet configured */
    ETHCTRL_STATE_INIT                              = 0x01U     /* Ethernet Interface is configured */
}EthIf_StateType;


/* -------------------------------------------- */
/* Structures                                   */
/* -------------------------------------------- */

/* This data type defines the virtual address entry structure which is used by the virtual filter */
typedef struct
{
    uint32 ReferenceCounter_u32;               /* Reference counter for the virtual address filter`s entries. */
    uint8 HardwareControllerIndex_u8;          /* Index of the Ethernet controller within the context of the Ethernet Interface */
    uint8 PhysicalAddress_au8[6];              /* Physical source address (MAC address) in network byte order */
} EthIf_PhysAddrFilterRefCounterType_tst;


#endif /* ETHIF_PRV_TYPES_H */

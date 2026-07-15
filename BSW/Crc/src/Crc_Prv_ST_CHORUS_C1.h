

#ifndef CRC_PRV_ST_CHORUS_C1_H
#define CRC_PRV_ST_CHORUS_C1_H

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/

#if (defined(MCU_RB_ST_CHORUS_4M_C1) && (MCU_RB_MACHINE_TYPE == MCU_RB_ST_CHORUS_4M_C1)) ||\
    (defined(MCU_RB_ST_CHORUS_2M_C1) && (MCU_RB_MACHINE_TYPE == MCU_RB_ST_CHORUS_2M_C1)) ||\
    (defined(MCU_RB_ST_CHORUS_1M_C1) && (MCU_RB_MACHINE_TYPE == MCU_RB_ST_CHORUS_1M_C1))

/*
**********************************************************************************************************************
* Typedefines
**********************************************************************************************************************
*/

typedef struct
{
    volatile uint32 CFG;                                             //[0xFBF64000] : Configuration Register
    volatile uint32 INP;                                             //[0xFBF64004] : Input Register
    volatile uint32 CSTAT;                                           //[0xFBF64008] : Current Status Register
    volatile uint32 OUTP;                                            //[0xFBF6400C] : Output Register
}Crc_ChorusRegType;

/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/

#define CRC_HW_CCITT16_CONFIG   (0x00000010U)

#define CRC_0                   (*(volatile Crc_ChorusRegType*)(0xF7F64000UL))
#define CRC_1                   (*(volatile Crc_ChorusRegType*)(0xFBF64000UL))

/**
 **********************************************************************************************************************
 * \brief Calculates a 16 bit CCITT CRC using the Data CRC Function of the Renesas microcontroller
 *
 * \param    DataPtr         holds a pointer to the start address of the data block to be calculated
 * \param    Length      length of data block to be calculated in bytes
 * \param    StartValue  start value when the algorithm starts
 *
 * \return   16 bit result of CRC calculation
 **********************************************************************************************************************
*/
CRC_INLINE uint16 Crc_Prv_CalcCRC16_Hw(const uint8* DataPtr, uint32 Length, uint16 StartValue)
{
    Crc_ChorusRegType Backup;
    volatile Crc_ChorusRegType *Context = &CRC_0;
    volatile uint8 *INP = ((volatile uint8 *)&CRC_0) + 0x004U;   // 0xF7F64000UL + 0x004 + n*0x10 + 0x00
    uint32 i;
    uint32 result;

    /* save register context for Reentrancy */
    Backup.CFG = Context->CFG;
    Backup.INP = Context->INP;
    Backup.CSTAT = Context->CSTAT;

    /* configure CRC generation process and execute it */
    Context->CFG = CRC_HW_CCITT16_CONFIG;
    Context->CSTAT = StartValue;

    for (i=0U; i<Length; i++)
    {
        *INP = DataPtr[i];
    }

    result = Context->OUTP;

    Context->CFG = Backup.CFG;
    Context->INP = Backup.INP;
    Context->CSTAT = Backup.CSTAT;

    return (uint16)result;
}

#endif
#endif /* CRC_PRV_ST_CHORUS_C1_H */

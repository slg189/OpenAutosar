



#ifndef CRC_PRV_HW_H
#define CRC_PRV_HW_H

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/
/*
 **************************************************************************************************************
 * Include of the addresses for the register of the JDP CRC-HW. The constant RBA_REG_CRC_H
 * already is defined with the hardware-specific file-name to include (e.g. rba_Reg_crc_jdp_dev4.h).
 **************************************************************************************************************
 */

#include "Crc_Cfg.h"
#include "rba_BswSrv.h"
#if(CRC_ST == CRC_HARDWARE_FAM)
#include "Crc_Prv_ST_CHORUS_C1.h"
#elif(CRC_JDP == CRC_HARDWARE_FAM)
#include "Crc_Prv_Jdp_Hw.h"
#elif(CRC_IFX == CRC_HARDWARE_FAM)
#include "Crc_Prv_Ifx_Hw.h"
#else
#include "Crc_Prv_RH850_Hw.h"
#endif
/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/

/*
**********************************************************************************************************************
* The optimization by HW-based byte-reflection can be used with JDP dev 4 Cut 2. The upcoming hardware
* devices are supposed to still offer the manipulation in future. Still it is possible they won't.
* This is why currently just exactly JDP dev 4 is chosen for the optimization, avoiding problems.
**********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/

/* Also ST devices belong to machine family MCU_RB_JDP_UC1_DEV4_ALL */
#if((CRC_JDP == CRC_HARDWARE_FAM) || (CRC_IFX == CRC_HARDWARE_FAM))
/**
**********************************************************************************************************************
* Crc_Prv_ReverseBytes_Hw
*
* \brief reverses 4 bytes
*
* \param    uint32*      Crc_data_u32,           input data for the crc calculation
**********************************************************************************************************************
*/
CRC_INLINE void Crc_Prv_ReverseBytes_Hw(uint32* Crc_data_u32)
{
    uint32 output_u32;


    /* output_u32 gets *Crc_data_u32 with reversed bytes */

    /* First, the byte on position one is shifted to position 4 */
    output_u32  = ((* Crc_data_u32) >> 24U);

    /* Second, the position two is shifted to position three */
    output_u32 ^= (((* Crc_data_u32) & 0x00FF0000UL) >> 8U);

    /* Third, the position three is shifted to position two */
    output_u32 ^= (((* Crc_data_u32) & 0x0000FF00UL) << 8U);

    /* Fourth, the position four is shifted to position one */
    output_u32 ^= (((* Crc_data_u32) & 0x000000FFUL) << 24U);

    /* Finally, *Crc_data_u32 gets the data of output_u32. This step completes the byte reversal. */
    *Crc_data_u32 = output_u32;
}

/**
**********************************************************************************************************************
* Crc_Prv_CastAU8ToU32
*
* \brief Casting an 8 byte array with four elements to uint32. This is done independently of the byte endianness.
*
* \param    uint8*      Crc_data_au8,           input for the casting.
* \param    uint8 arrayLength_u8          the amount of 8-byte fields in the array, to be casted
**********************************************************************************************************************
*/
CRC_INLINE uint32 Crc_Prv_Convert_AU8ToU32_Hw(const uint8 * Crc_data_au8, uint8_least arrayLength_u8)
{
    uint32 output_u32;
    const uint8* data_u8_ptr = Crc_data_au8;
    uint8 i_u8; /* Used for counting in loops */


    output_u32 =  ((uint32)(*data_u8_ptr));

    /* In a loop it goes through the 8 byte array and concatenates the array-elements to a single uint32 */
    for(i_u8 = 1U; i_u8 < arrayLength_u8; i_u8++)
    {
        /* Shifting 8 bits = 1 byte */
        output_u32  <<= 8U;
        data_u8_ptr ++;
        output_u32  ^= ((uint32)(*data_u8_ptr));
    }

    return (output_u32);
}

/*
 **********************************************************************************************************************
 * Crc_Prv_UseSoftware_Hw
 *
 * \brief checks if the current core can use hardware or software crc solution
 *
 * A-periodical: General function can be called by the Start-Function.
 *
 * \return   boolean     TRUE: software solution must be used
 *                       FALSE: hardware solution can be used
 **********************************************************************************************************************
 */
CRC_INLINE boolean Crc_Prv_UseSoftware_Hw(void)
{
    boolean useSoftware_b = FALSE;
    uint8_least physCore_u8 = rba_BswSrv_GetPhysCore();


    /* If the physcial core number is greater than 2, an additional core has been added.*/
    /* For this core there is no CRC engine left.                                       */
    if(physCore_u8 > 2U)
    {
        useSoftware_b = TRUE;
    }
    else
    {
       if (Crc_Core_2_CrcHWTable[physCore_u8].Crc32_HW == CRC_HW_UNDEFINED)
       {
        useSoftware_b = TRUE;
       }
    }

    return (useSoftware_b);
}

/**
 **********************************************************************************************************************
 * Crc_Prv_BytesForSwCount_Hw
 *
 * \brief checks how many of the bytes must be calculated by software
 *
 * A-periodical: General function can be called by the Start-Function.
 *
 * \return   uint32  The amount of bytes, the software has to calculate
 **********************************************************************************************************************
 */
CRC_INLINE uint32 Crc_Prv_BytesForSwCount_Hw(const uint32 Crc_Length_u32, uint32 Crc_StartValue_u32)
{
    uint32 bytesForSwCount_u32 = (0U);


    if(Crc_Prv_UseSoftware_Hw() != FALSE)
    {
        bytesForSwCount_u32 = Crc_Length_u32;
    }
    else
    {
        if ((Crc_Length_u32 < (4U)) && (Crc_StartValue_u32 != (0x00000000UL)))
        {
            bytesForSwCount_u32 = Crc_Length_u32;
        }
    }

    return (bytesForSwCount_u32);
}

/**
**********************************************************************************************************************
* Crc_Prv_CalculateCrc32_Hw
*
* \brief This function is to proceed the whole crc_32-calculation for the use of the AUTOSAR-function: Crc_CalculateCRC32
*
* \param    Crc_DataPtr         Pointer to the start address of the data block
* \param    Crc_Length_u32      Length of data block to be calculated in bytes
* \param    Crc_StartValue_u32  Start value when the algorithm starts
* \return                       Rest after polynomial division *const Crc_DataPtr / CRCH16H1021
**********************************************************************************************************************
*/
CRC_INLINE uint32 Crc_Prv_CalculateCrc32_Hw(const uint8* Crc_DataPtr, uint32 Crc_Length_u32, uint32 Crc_StartValue_u32)
{
    uint32 save_registerData_au32 [3];
    uint32 i_u32;
    uint32 result_u32;
    uint32 start_value_u32 = Crc_StartValue_u32;
    uint32 Crc_StartValueForFirstCalc_u32;
    uint32 Crc_StartValueRemaining_u32;
    uint32 data_tmp_u32;
    const uint8* data_u8_ptr = Crc_DataPtr;
    Crc_HwReg_tst* Crc_Address_s_ptr;
    uint32 leftBytes_u32 = Crc_Length_u32 % 4U;


    /* Reversion of the start-value */
    Crc_Prv_ReverseBytes_Hw(&start_value_u32);

    Crc_StartValueForFirstCalc_u32 = start_value_u32;
    Crc_StartValueRemaining_u32 = start_value_u32;

    /* get the address of the crc engine */
    Crc_Address_s_ptr = Crc_Prv_DetectCrcEngineToUse_Hw();

    /* Get the data out of the register of the Crc engine, that will be manipulated */
    Crc_Prv_GetRegisterValues_Hw(save_registerData_au32, Crc_Address_s_ptr);

    /* call Crc_Prv_Config_Hw to set the configuration register */
    /* Also ST devices belong to machine family MCU_RB_JDP_UC1_DEV4_ALL */
    Crc_Prv_Config_Hw (Crc_Address_s_ptr);

    /*
     *********************************************************************************************************
     * CRC32 calculation
     * Note: If the input has a length of less than 4 bytes and the start value is not zero,
     * the software solution is called. For all other inputs there is a hardware solution offered.
     * If the input length is not a multiple of 4, the smaller part is calculated as first step.
     * Therefore the startvalue must be moved so it can be xored exactly with the bits that are used
     * for the first calculation and not with the empty bits. By doing this, a part of the start-value
     * can remain for the second calculation.
     *********************************************************************************************************
     */
    if(leftBytes_u32 > 0U)
    {
        /* See explanation in the comment above. */
        Crc_StartValueForFirstCalc_u32 >>= (32U - (leftBytes_u32 << 3U));

        /* To get the complement of leftBytes_u8, (4 [Amount of bytes in uint32] - leftBytes_u8) is calculated */
        Crc_StartValueRemaining_u32 <<= (leftBytes_u32 << 3U);

        /* leftBytes_u8 Stands for the amount of bytes */
        data_tmp_u32 = Crc_Prv_Convert_AU8ToU32_Hw(data_u8_ptr, leftBytes_u32);

        /* Going on in the memory because leftBytes_u8 bytes have been calculated */
        data_u8_ptr += leftBytes_u32;

        /* Including the startvalue into the current calculation */
        data_tmp_u32 ^= Crc_StartValueForFirstCalc_u32;

        /* Also ST devices belong to machine family MCU_RB_JDP_UC1_DEV4_ALL */
        /* reverse the byte order. This is not necessary with JDP Dev 4 Cut 2 because it can be done by HW */
        CRC_HARDWARE_FAM_CHECK(&data_tmp_u32);

        /* call Crc_Prv_CalculateCrc_Hw to start the calculation */
        Crc_Prv_CalculateCrc_Hw(&data_tmp_u32, Crc_Address_s_ptr);
    }

    /* For the first time calculating (of whole 4 bytes) Crc_StartValueRemaining_u32 must be considered. *
     * To avoid checking this everytime in loop, the first calculation (of whole 4 bytes) is separated.  */
    if(Crc_Length_u32 >= 4U)
    {
        data_tmp_u32 = Crc_Prv_Convert_AU8ToU32_Hw(data_u8_ptr, 4U);
        data_u8_ptr += 4U;

        /* Including the startvalue into the current calculation */
        data_tmp_u32 ^= Crc_StartValueRemaining_u32;

        /* Also ST devices belong to machine family MCU_RB_JDP_UC1_DEV4_ALL */
        /* reverse the byte order. This is not necessary with JDP Dev 4 Cut 2 because it can be done by HW */
        CRC_HARDWARE_FAM_CHECK(&data_tmp_u32);


        /* call Crc_Prv_CalculateCrc_Hw to start the calculation */
        Crc_Prv_CalculateCrc_Hw(&data_tmp_u32, Crc_Address_s_ptr);
    }

    /*
     ******************************************************************************************************************************
     This is the main calculation in a loop. In every round 4 bytes are calculated by once. This is why in the condition
     "(i_u32 + 3) < Crc_Length_u32" 3 is added to the counter i_u32. Because just if there are at least 4 bytes left
     a next round of the loop is possible. It is also the reason that i_u32 increments always by 4 ("i_u32 += 4").
     The reason that i_u32 starts with 4 ("i_u32 = 4") is that already one calculation with 4 bytes has been executed in advance.
     ******************************************************************************************************************************
     */
    for(i_u32 = 4U; (i_u32 + 3U) < Crc_Length_u32; i_u32 += 4U)
    {
        data_tmp_u32 = Crc_Prv_Convert_AU8ToU32_Hw(data_u8_ptr, 4U);
        data_u8_ptr += 4U;

        /* Also ST devices belong to machine family MCU_RB_JDP_UC1_DEV4_ALL */
        /* reverse the byte order. This is not necessary with JDP Dev 4 Cut 2 because it can be done by HW */
        CRC_HARDWARE_FAM_CHECK(&data_tmp_u32);

        /* call Crc_Prv_CalculateCrc_Hw to start the calculation */
        Crc_Prv_CalculateCrc_Hw(&data_tmp_u32, Crc_Address_s_ptr);
    }

    /* call Crc_Prv_Result_Hw to get the result of the crc calculation */
    result_u32 = Crc_Prv_Result_Hw(Crc_Address_s_ptr);

    /* Restore the registers for the interrupted task by setting back the data of the changed CRC-engine registers*/
    Crc_Prv_SetRegisterValues_Hw(save_registerData_au32, Crc_Address_s_ptr);

    return (result_u32);
}

/**
 **********************************************************************************************************************
 * Crc_Prv_CalcCRC32REVH04C11DB7_Hw
 *
 * \brief calculates the checksum on the hardware
 *
 * \param    const uint8  *const Crc_DataPtr,      Pointer to start address of data block
 * \param    const uint32        Crc_Length,       Length of data block to be calculated in bytes
 * \param          uint32        Crc_StartValue32  Start value when the algorithm starts
 * \return         uint32                          Rest after polynomial division *const Crc_DataPtr / CRC8H1D
 **********************************************************************************************************************
 */
CRC_INLINE uint32 Crc_Prv_CalcCRC32_Hw (const uint8* Crc_DataPtr,
                             const uint32 Crc_Length_u32, uint32 Crc_StartValue_u32)
{
    /*
     ****************************************************************************************
     * result_u32 gets as value the negated start-value (xored with 0xFFFFFFFF).
     * Commonly, it will be overwritten by the Crc hardware calculation. But sometimes
     * all is calculated by software. This is done by continuing with the software calculation
     * by the use of the hardware calculation result. The hardware Crc result is used as
     * start value. The hardware calculation is directly negating the result. This is why
     * the software calculation (just in special cases SW is necessary) needs to back calculate
     * the negation. If now result_u32 still holds the start-value, it would be back-calculated
     * although it was never negated. To solve this, a negation is done here. So the back
     * calculation of the negated start-value leads again to the original start-value.
     ****************************************************************************************
     */
    uint32 result_u32          = (Crc_StartValue_u32  ^ (CRC_XOR_VALUE32));
    uint32 leftForSw_u32       = (0U);
    uint32 crcTemp;
    uint32  index;
    const uint8* data_u8_ptr   = Crc_DataPtr;


    leftForSw_u32  = Crc_Prv_BytesForSwCount_Hw(Crc_Length_u32, Crc_StartValue_u32);

    if(leftForSw_u32 < Crc_Length_u32)
    {
        result_u32 = Crc_Prv_CalculateCrc32_Hw(Crc_DataPtr, (Crc_Length_u32 - leftForSw_u32), Crc_StartValue_u32);
    }

    data_u8_ptr += (Crc_Length_u32 - leftForSw_u32);

    /*
     *******************************************************************************************************************
     * The reason for "result_u32 ^ CRC_XOR_VALUE32" is that the xor must be back calculated.
     * The hardware calculation includes directly the negation of the result (xor with 0xFFFFFFFF). This is correct if
     * it is the final result. If it is an interim result, this step must be revoked, so it can be used as startvalue
     * for a following calculation.
     *
     * This result is finally again negated because it is not directly included in the software calculation.
     * But the final result requires this step, according to AR 4.0 Rev. 2.
     *******************************************************************************************************************
     */

    /* Initialization of temporary crc result */
    crcTemp = result_u32 ^ (CRC_XOR_VALUE32);

    for (index = 0U; index < leftForSw_u32; ++index)
    {
        /* Impact of temporary rest on next crc rest */
        crcTemp ^= data_u8_ptr[index];

        crcTemp = (crcTemp >> (CRC_JUNK)) ^
                CRC_32_REV_Tbl[crcTemp & (CRC_TABLEMASK)];
    }
    result_u32 = crcTemp ^ (CRC_XOR_VALUE32);

    return (result_u32);
}

#endif

#endif /* CRC_PRV_HW_H */

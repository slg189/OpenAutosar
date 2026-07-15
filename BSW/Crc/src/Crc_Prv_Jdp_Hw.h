

#ifndef CRC_PRV_JDP_HW_H
#define CRC_PRV_JDP_HW_H

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/

#include "Mcu.h"
#include "rba_BswSrv.h"
#include RBA_REG_CRC_H

/*
**********************************************************************************************************************
* Defines/Macros
**********************************************************************************************************************
*/

typedef rba_Reg_CRC_tst Crc_HwReg_tst;

/*
 **********************************************************************************************************************
 * Macros are used for register values for specific device. If the names of devices once would change, a compiler error
 * would occurs. In this case an update of COMP: Crc is required.
 **********************************************************************************************************************
 */
#if(CRC_MCU_RB_MACHINE_DEV == MCU_RB_ST_UC1_DEV3_40NM_ALL)
/* Macros for ST3 Device 3 40NM CUT 3*/
#define CRC_PRV_JDP_CFG(X)          ((*(X)).CFG1)   /* Macro for the configuration register of the CRC engine */
#define CRC_PRV_JDP_INP(X)          ((*(X)).INP1)   /* Macro for the input register of the CRC engine */
#define CRC_PRV_JDP_INTRES(X)       ((*(X)).CSTAT1) /* Macro for the interium result register of the CRC engine */
#define CRC_PRV_JDP_RES(X)          ((*(X)).OUTP1)  /* Macro for the result register of the CRC engine */
#else
/* Macros for ST device */
#define CRC_PRV_JDP_CFG(X)          ((*(X)).CFG)    /* Macro for the configuration register of the CRC engine */
#define CRC_PRV_JDP_INP(X)          ((*(X)).INP)    /* Macro for the input register of the CRC engine */
#define CRC_PRV_JDP_INTRES(X)       ((*(X)).CSTAT)  /* Macro for the interium result register of the CRC engine */
#define CRC_PRV_JDP_RES(X)          ((*(X)).OUTP)   /* Macro for the result register of the CRC engine */
#endif

/*
 **********************************************************************************************************************
 * Crc_Prv_DetectCrcEngineToUse_Hw
 *
 * \brief checks which of the two Jdp crc engines shall be used
 *
 * A-periodical: General function can be called by the Start-Function.
 *
 * \return   rba_Reg_CRC_tst*     address of the crc engine
 **********************************************************************************************************************
 */
CRC_INLINE rba_Reg_CRC_tst* Crc_Prv_DetectCrcEngineToUse_Hw(void)
{
    rba_Reg_CRC_tst *Crc_Prv_Engine_u32;


    /* read number of crc engine in CrcHWTable */
    if (Crc_Core_2_CrcHWTable[rba_BswSrv_GetPhysCore()].Crc32_HW == 0U)
    {
        Crc_Prv_Engine_u32 = (&CRC_0);
    }
    else
    {
        Crc_Prv_Engine_u32 = (&CRC_1);
    }

    return (Crc_Prv_Engine_u32);
}

/*
 **********************************************************************************************************************
 * Crc_Prv_GetRegisterValues_Hw
 *
 * \brief stores the the data of the registers of the flexible Crc Engine into registerValues_au32:
 *
 * [0]: data of the configuration register.
 * [1]: data of the input register.
 * [2]: data of the interim result register.
 **********************************************************************************************************************
 */
CRC_INLINE void Crc_Prv_GetRegisterValues_Hw(uint32* registerValues_au32, const rba_Reg_CRC_tst* const Crc_Address_s_ptr)
{
    /*
     ******************************************************************************************
     * Saving the registers of the crc engine. This is important if a task, currently
     * using the crc calculation is interrupted by another task. If the interrupting task
     * also executes the crc calculation, the data on the registers in manipulated.
     * This is why the registers must be set back to the original state (with original data).
     * Here the original data is loaded from the register.
     ******************************************************************************************
     */
     registerValues_au32[0] = CRC_PRV_JDP_CFG(Crc_Address_s_ptr);
     registerValues_au32[1] = CRC_PRV_JDP_INP(Crc_Address_s_ptr);
     registerValues_au32[2] = CRC_PRV_JDP_INTRES(Crc_Address_s_ptr);
}

/*
 **********************************************************************************************************************
 * Crc_Prv_SetRegisterValues_Hw
 *
 * \brief restores the the data of the registers of the flexible Crc Engine with the data stored in registerValues_au32:
 *
 * [0]: data of the configuration register.
 * [1]: data of the input register.
 * [2]: data of the interim result register.
 **********************************************************************************************************************
 */
CRC_INLINE void Crc_Prv_SetRegisterValues_Hw(const uint32* const registerValues_au32, rba_Reg_CRC_tst* Crc_Address_s_ptr )
{
    /*
     ******************************************************************************************
     * Saving the registers of the crc engine. This is important if a task, currently
     * using the crc calculation is interrupted by another task. If the interrupting task
     * also executes the crc calculation, the data on the registers in manipulated.
     * This is why the registers must be set back to the original state (with original data).
     * Here the registers are set back to the original state.
     ******************************************************************************************
     */
     CRC_PRV_JDP_CFG(Crc_Address_s_ptr)    = registerValues_au32[0];
     CRC_PRV_JDP_INP(Crc_Address_s_ptr)    = registerValues_au32[1];
     CRC_PRV_JDP_INTRES(Crc_Address_s_ptr) = registerValues_au32[2];
}


/*
 **********************************************************************************************************************
 * Crc_Prv_Config_Hw
 *
 * \brief sets the configuration and the status register of the crc engine for the crc32 calculation
 *
 * A-periodical: General function can be called by the Start-Function.
 *
 * \param    uint32 Crc_Conf_u32,                  configuration to set
 * \param    rba_Reg_CRC_tst* Crc_Address_s_ptr,    address of the crc engine
 **********************************************************************************************************************
 */
CRC_INLINE void Crc_Prv_Config_Hw(rba_Reg_CRC_tst *Crc_Address_s_ptr)
{
    /* Writes in Crc_Conf_u32 to the configuration register of JDP */
    CRC_PRV_JDP_CFG(Crc_Address_s_ptr) = CONF_CRC32_FOR_SWAP_AND_INVERSION;
    CRC_PRV_JDP_INTRES(Crc_Address_s_ptr) = (0U);
}

/*
 **********************************************************************************************************************
 * Crc_Prv_CalculateCrc_Hw
 *
 * \brief sets the input register of the crc engine to start the crc calculation
 *
 * Periodical: General function can be called by the Update-Function.
 *
 * \param    uint32 const*    Crc_data_u32,        input data for the crc calculation
 * \param    rba_Reg_CRC_tst* Crc_Address_s_ptr,   address of the crc engine
 **********************************************************************************************************************
 */
CRC_INLINE void Crc_Prv_CalculateCrc_Hw(uint32 const* Crc_data_u32, rba_Reg_CRC_tst *Crc_Address_s_ptr)
{
    CRC_PRV_JDP_INP(Crc_Address_s_ptr) = ((*Crc_data_u32));
}

/*
 **********************************************************************************************************************
 * Crc_Prv_Result_Hw
 *
 * \brief returns the result of the crc calculation (OUTP register)
 *
 * A-periodical: General function  Result_Prv_Crc_Hw can be called by the finish-function
 *
 * \param    rba_Reg_CRC_tst const* Crc_Address_s_ptr,   address of the crc engine
 * \return   uint32                                      result of crc calculation
 **********************************************************************************************************************
 */
CRC_INLINE uint32 Crc_Prv_Result_Hw(rba_Reg_CRC_tst const* Crc_Address_s_ptr)
{
    return CRC_PRV_JDP_RES(Crc_Address_s_ptr);
}

#endif /* CRC_PRV_JDP_HW_H */

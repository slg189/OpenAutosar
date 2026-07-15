/*********************************************************************************************************************
*
* Description:     DataM module private configuration implementation
* FileName:        FBL_DataMPrvCfg.c
* Company:         ETAS GmbH (www.etas.com)
*
**********************************************************************************************************************
*
* COPYRIGHT RESERVED, Copyright ETAS GmbH 2019. All rights reserved, also regarding any disposal,
* exploitation, reproduction, editing, distribution, as well as in the event of applications
* for industrial property rights.
*
**********************************************************************************************************************
* History:
**********************************************************************************************************************
* Author        Date            Comment
**********************************************************************************************************************/
/*********************************************************************************************************************
* Includes
*********************************************************************************************************************/
#include "NvM.h"
/*********************************************************************************************************************
* Local Defines
*********************************************************************************************************************/

/*********************************************************************************************************************
* Local Macros
*********************************************************************************************************************/

/*********************************************************************************************************************
* Local Types
*********************************************************************************************************************/

/*********************************************************************************************************************
* Local Variables
*********************************************************************************************************************/

/*********************************************************************************************************************
* Local Functions Prototypes
*********************************************************************************************************************/

/*********************************************************************************************************************
* Exported Variables Implementation
*********************************************************************************************************************/
uint8 Fbl_DataM_RamMirrorNV_ReprogrammingRequestFlagBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ExternalReprogrammingRequestFlagBlock];
uint8 Fbl_DataM_RamMirrorNV_ValidityFlagsBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ValidityFlagsBlock];
uint8 Fbl_DataM_RamMirrorNV_ResetResponseBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ResetResponseFlagBlock];
uint8 Fbl_DataM_RamMirrorNV_PartNumberBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_PartNumberBlock];
uint8 Fbl_DataM_RamMirrorNV_CrcBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_CrcBlock];
uint8 Fbl_DataM_RamMirrorNV_ProgrammingAttemptCounterBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ProgrammingAttemptCounterBlock];
uint8 Fbl_DataM_RamMirrorNV_ProgrammingCounterBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ProgrammingCounterBlock];

const uint8 Fbl_DataM_RomDataNV_ReprogrammingRequestFlagBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ExternalReprogrammingRequestFlagBlock]
    = {0};
const uint8 Fbl_DataM_RomDataNV_ValidityFlagsBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ValidityFlagsBlock]
    = {0x00};
const uint8 Fbl_DataM_RomDataNV_ResetResponseBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ResetResponseFlagBlock]
    = {0};
const uint8 Fbl_DataM_RomDataNV_PartNumberBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_PartNumberBlock]
    = {0, 0, 0, 0, 0, 0, 0, 0, 0};
const uint8 Fbl_DataM_RomDataNV_CrcBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_CrcBlock]
    = {0, 0, 0, 0};
const uint8 Fbl_DataM_RomDataNV_ProgrammingAttemptCounterBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ProgrammingAttemptCounterBlock]
    = {0};

const uint8 Fbl_DataM_RomDataNV_ProgrammingCounterBlock[NVM_CFG_NV_BLOCK_LENGTH_NV_ProgrammingCounterBlock]
    = {0};

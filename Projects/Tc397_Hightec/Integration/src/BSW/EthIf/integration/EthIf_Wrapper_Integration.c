/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

#ifndef ETHIF_WRAPPER_INTERGRATION_C
#define ETHIF_WRAPPER_INTERGRATION_C


/**
 *********************************************************************
 * Description: This integration file supports EthIf backward compatible with Eth v4.0.3
 * 	- If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * 	Ex: For infineon Eth Driver on TC26x Board, it is required to update
 * 		+ Name of header file included: Eth.h --> Eth_17_EthMac.h
 *		+ Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *			+ Eth_ControllerInit_Internal --> Eth_17_EthMac_ControllerInit_Internal
 *********************************************************************
 */

/**
 *********************************************************************
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 *   Ex: For infineon Eth Driver on TC26x Board, it is required to update
 * 		+ Name of header file included: Eth.h --> Eth_17_EthMac.h
 *********************************************************************
 */
#include "Eth_17_GEthMac.h"
#include "EthIf.h"

/**
 *********************************************************************
 * AR standard header files
 *********************************************************************
 */
#include "EthIf_Cbk.h"
#include "EthIf_Prv.h"
#include "rba_BswSrv.h"
#include "EthIf_Integration.h"

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"


/**
 *********************************************************************
 * EthIf_Cbk_RxIndication(): Handles a received frame received by the indexed controller
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * Below is a template implementation for Ifx MCAL TC26x. Integrators shall
 * implement this function in order to match with the target MCAL.
 *
 * \param    CtrlIdx:     Index of the Ethernet controller within the context of the Ethernet Interface.
 * \param    DataPtr:     Ethernet frame comprising the following elements in the listed order:
 *                    Target MAC, Source MAC, VLAN tag (optional), Type, Payload.
 * \param    LenByte:     Length of the received frame bytes.
 * \return   void
 * \seealso
 * \usedresources
 *********************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
void EthIf_Cbk_RxIndication(uint8 CtrlIdx, Eth_DataType* DataPtr, uint16 LenByte) {
    /* Local variable declaration */
    uint8 PhysAddr[6];
    Eth_FrameType FrameType;
    boolean IsBroadcast;
    uint16 lLenByte_u16;

    /* Local variable initialization */
    FrameType = ((DataPtr[ETHIF_ETHTYPE_BYTE1] << 8) & 0xFF00) | DataPtr[ETHIF_ETHTYPE_BYTE2];
    IsBroadcast = FALSE;
    /* Copy 6 bytes Source MAC address */
    rba_BswSrv_MemCopy(PhysAddr, DataPtr + ETHIF_OFFSET_SOURCEMACADDR, 6U);

    /* Check if Ethernet header length enough */
    if (LenByte >= ETHIF_ETHTHDR_OFFSET) {
        lLenByte_u16 = LenByte - ETHIF_ETHTHDR_OFFSET;
    }
    else
    {
        lLenByte_u16 = 0;
    }

    if ((0xFF == DataPtr[0])  // Check if the destination address is a broadcast address.
        && (0xFF == DataPtr[1]) && (0xFF == DataPtr[2])
        && (0xFF == DataPtr[3]) && (0xFF == DataPtr[4])
        && (0xFF == DataPtr[5])) {
        IsBroadcast = TRUE;
    }

    EthIf_RxIndication(CtrlIdx, FrameType, IsBroadcast, PhysAddr,
            (Eth_DataType*) &DataPtr[ETHIF_ETHTHDR_OFFSET], lLenByte_u16);
}
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
// Current Ethernet version is 4.2.2 - This API is not required
#endif


/**
 *********************************************************************
 * Eth_17_ControllerInit_Internal(): Initializes the indexed controller
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * For v4.0.3: Below sample based on TC26x Board, which required to set controller ACTIVE
 * For v4.2.2: Eth_ControllerInit() is removed. This one is kept as dummy function for compiling
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_ControllerInit_Internal --> Eth_17_EthMac_ControllerInit_Internal
 *    + Eth_ControllerInit --> Eth_17_EthMac_ControllerInit
 *    + Eth_SetControllerMode --> Eth_17_EthMac_SetControllerMode 
 *
 * \param    CtrlIdx:     Index of the controller within the context of the Ethernet Driver.
 * \param    CfgIdx:      Index of the used configuration.
 * \return   void
 * \seealso
 * \usedresources
 *********************************************************************
 */
Std_ReturnType Eth_17_GEthMac_ControllerInit_Internal(uint8 CtrlIdx, uint8 CfgIdx ){
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
    Std_ReturnType retValCtrlInit = E_NOT_OK;
    Std_ReturnType retValSetCtrlMode = E_NOT_OK;
    retValCtrlInit = Eth_ControllerInit(CtrlIdx, CfgIdx);

    /* Eth_WriteMii and Eth_ReadMii can only work properly after EthController mode is changed to ETH_MODE_ACTIVE.
     * In EthIf_Trcv_Transfrm_State_Down, EthIf_Prv_TransceiverInit is called right after EthIf_Prv_ControllerInit.
     * After EthIf_Prv_TransceiverInit function invoked, EthTrcv calls Eth_WriteMii to set the Ethernet Transceiver's registers.
     *
     * This function call here is the workaround solution for the problem.
     */
    retValSetCtrlMode= Eth_SetControllerMode(CtrlIdx, ETH_MODE_ACTIVE);
    return retValCtrlInit | retValSetCtrlMode;    
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
 /*
 * Description      : From AUTOSAR 4.2.2, the Eth_ControllerInit functionality merged into         *
 *		Eth_Init API. This function is dummy function for EthIf's call in EthIf_Prv_ControllerInit.*
 *		In MCAL following AUTOSAR version older than AR4.2.2, because MCAL already implement the   *
 *      Eth_ControllerInit() function, integrators should remove the implementation of             *
 *  	Eth_ControllerInit() in this file and the declaration of it in EthIf_Integration.h         * 
 */
    return E_OK;
#endif

}

/**
 *********************************************************************
 * Eth_UpdatePhysAddrFilter(): Update the physical source address to/from the indexed controller filter.
 *             If the Ethernet Controller is not capable to do the filtering, the software has to do this.
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * This function is not available at v4.0.3
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_UpdatePhysAddrFilter --> Eth_17_EthMac_UpdatePhysAddrFilter
 *
 * \param    CtrlIdx:     Index of the controller within the context of the Ethernet Driver.
 * \param    PhysAddrPtr: Pointer to memory containing the physical destination address (MAC address)
 *                 in network byte order. This is the multicast destination address of the
 *                 layer 2 Ethernet packet.
 * \param    Action:      Add or remove the address from the Ethernet controllers filter.
 *
 * \return   Std_ReturnType
 *              E_OK: filter was successfully changed
 *              E_NOT_OK: filter could not be changed
 * \seealso
 * \usedresources
 *********************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
Std_ReturnType Eth_UpdatePhysAddrFilter( uint8 CtrlIdx, const uint8* PhysAddrPtr, Eth_FilterActionType Action ){
	/* This function is not supported*/	
    return E_OK;
}
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
// Current Ethernet version is 4.2.2 - this API shall be implemented by Eth MCAL
#endif



/**
 *********************************************************************
 * Eth_SetPhysAddr(): Sets the physical source address used by the indexed controller
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * This function is not available at v4.0.3
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_SetPhysAddr --> Eth_17_EthMac_SetPhysAddr
 *
 * \param    CtrlIdx:     Index of the controller within the context of the Ethernet Driver.
 * \param    PhysAddrPtr: Pointer to memory containing the physical source address (MAC address)
 *                 in network byte order.
 *
 * \return   None
 * \seealso
 * \usedresources
 *********************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
void Eth_SetPhysAddr( uint8 CtrlIdx_u8,const uint8 * PhysAddrPtr_pu8 )
{ 
	/* This function is not supported*/
}
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
// Current Ethernet version is 4.2.2 - this API shall be implemented by Eth MCAL
#endif


/**
 *********************************************************************
 * Eth_ProvideTxBuffer(): Provides access to a transmit buffer of the specified controller 
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_ProvideTxBuffer_Internal --> Eth_17_EthMac_ProvideTxBuffer_Internal
 *    + Eth_ProvideTxBuffer --> Eth_17_EthMac_ProvideTxBuffer
 *
 * \param    CtrlIdx:     Index of the controller within the context of the Ethernet Driver.
 * \param    BufIdxPtr:   Index to the granted buffer resource. To be used for subsequent requests.
 * \param    BufPtr:      Pointer to the granted buffer 
 * \param    LenBytePtr:  desired length in bytes, out: granted length in bytes
 *
 * \return   BufReq_ReturnType
 *              BUFREQ_OK: success
 *              BUFREQ_E_NOT_OK: development error detected
 *              BUFREQ_E_BUSY: all buffers in use 
 * \seealso
 * \usedresources
 *********************************************************************
 */

BufReq_ReturnType Eth_17_GEthMac_ProvideTxBuffer_Internal(uint8 CtrlIdx,
                                                         uint8* BufIdxPtr,
                                                         Eth_DataType** BufPtr,
                                                         uint16* LenBytePtr)
{
  BufReq_ReturnType   lBufReqRetVal_en;
  lBufReqRetVal_en = BUFREQ_E_NOT_OK;

#if(ETHIF_WRAPPER_ETH_VERSION == 0)
    lBufReqRetVal_en = Eth_ProvideTxBuffer(CtrlIdx, BufIdxPtr, BufPtr, LenBytePtr);
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
  Eth_BufIdxType  BufIdx;
  BufIdx = 0;
  lBufReqRetVal_en = Eth_17_GEthMac_ProvideTxBuffer(CtrlIdx, &BufIdx, BufPtr, LenBytePtr);
  if ( BufIdx > 255 )
  {
      /* Report DET : Development Error: Frame priority parameter (Priority) is invalid */
      ETHIF_REPORT_ERROR_TRUE_NO_RET(ETHIF_SID_ETHIF_PROVIDETXBUFFER, ETHIF_E_INV_PARAM)
      lBufReqRetVal_en = BUFREQ_E_NOT_OK;
  }
  else
  {
      /* Cast uint32 to uint8 */
      *BufIdxPtr =  ( (uint8)( BufIdx & 0x000000FFU ) );
  } 
#endif
  return lBufReqRetVal_en;
} 



/**
 *********************************************************************
 * Eth_Transmit_Internal(): Triggers transmission of a previously filled transmit buffer 
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_Transmit_Internal --> Eth_17_EthMac_Transmit_Internal
 *    + Eth_Transmit --> Eth_17_EthMac_Transmit
 *
 * \param    CtrlIdx:       Index of the controller within the context of the Ethernet Driver.
 * \param    BufIdxPtr:     Index of the buffer resource.
 * \param    FrameType:     Ethernet frame type.
 * \param    TxConfirmation:Activates transmission confirmation.
 * \param    LenByte:       Data length in byte. 
 * \param    PhysAddrPtr:   Physical target address (MAC address) in network byte order
 *
 * \return   Std_ReturnType
 *              E_OK: success
 *              E_NOT_OK: transmission failed
 * \seealso
 * \usedresources
 *********************************************************************
 */

Std_ReturnType Eth_17_GEthMac_Transmit_Internal( uint8 CtrlIdx,
                                                uint8 BufIdx,
                                                Eth_FrameType FrameType,
                                                boolean TxConfirmation,
                                                uint16 LenByte,
                                                const uint8* PhysAddrPtr)
{

  Std_ReturnType lStdRetVal_en;
  lStdRetVal_en = E_NOT_OK;
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
  uint8                   BufIdx_Internal;
  BufIdx_Internal         = (uint8) BufIdx;
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
  Eth_BufIdxType          BufIdx_Internal;
  BufIdx_Internal         = (Eth_BufIdxType) BufIdx;
#endif
  lStdRetVal_en = Eth_17_GEthMac_Transmit(CtrlIdx,
                                        BufIdx_Internal,
                                        FrameType,
                                        TxConfirmation,
                                        LenByte,
                                        PhysAddrPtr);

   return lStdRetVal_en;

} 

/**
 *********************************************************************
 * Eth_Receive_Internal(): 
 *  The function shall read the next frame from the receive buffers. The function passes
 * 	the received frame to the Ethernet interface using the callback function
 *  EthIf_RxIndication and indicates if there are more frames in the receive buffers
 *  The function prototype can be changed using ETHIF_WRAPPER_ETH_VERSION.
 *
 * This function is for resolving the mismatch between AUTOSAR Eth Driver v4.0.3 and v4.2.2
 * Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_Receive_Internal --> Eth_17_EthMac_Receive_Internal
 *    + Eth_Receive --> Eth_17_EthMac_Receive
 *
 *   For MCAL AR v4.0.3 this API is used:
 *   void Eth_Receive(uint8 CtrlIdx)
 * \param       CtrlIdx: Index of the controller within the context of the Ethernet Driver
 * \return      void
 *   For MCAL AR v4.2.2, this API is used:
 *   void Eth_Receive(uint8 CtrlIdx, Eth_RxStatusType* RxStatusPtr);
 * \param       CtrlIdx: Index of the controller within the context of the Ethernet Driver
 * \param       RxStatusPtr: Indicates whether a frame has been received and if so, whether more
 *                           frames are available or frames got lost.
 * \return      void
 *
 * \seealso
 * \usedresources
 *********************************************************************
 */

void Eth_17_GEthMac_Receive_Internal(uint8 CtrlIdx, uint8 FifoIdx, Eth_RxStatusType* RxStatusPtr) {

#if(ETHIF_WRAPPER_ETH_VERSION == 0)
    Eth_Receive(CtrlIdx);
    *RxStatusPtr = ETH_RECEIVED;
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
    Eth_17_GEthMac_Receive(CtrlIdx, RxStatusPtr);
#endif
}


/**
 *********************************************************************
 * EthIf_TxConfirmation(): Confirms frame transmission by the indexed controller
 * 
 * This function is for remapping the EthIf_TxConfirmation (2 parameters) 
 * to the EthIf_TxConfirmation_Internal (4 parameters in EthIf src).
 * 
 * - When using MCAL 4.0.3, please change this function name to EthIf_Cbk_TxConfirmation.
 * 
 * - Below is a template implementation. Integrators shall
 * implement this function in order to match with the target MCAL.
 * 
 * - According to [SWS_TCPIP_00131], TcpIp shall always call EthIf_Transmit() with parameter
 * TxConfirmation set to FALSE. Therefore, EthIf_TxConfirmation will not be called until EthTSyn module
 * is integrated (according to the current source code version).
 * 
 * \param    CtrlIdx:     Index of the Ethernet controller within the context of the Ethernet Interface.
 * \param    BufIdx:      Index of the transmitted buffer.
 * \return   void
 * \seealso
 * \usedresources
 *********************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
void EthIf_Cbk_TxConfirmation(uint8 CtrlIdx, uint8 BufIdx)
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
extern uint8 *EthIf_EthTxBufferPtr;
void EthIf_TxConfirmation(uint8 CtrlIdx, Eth_BufIdxType BufIdx)
#endif
{
    Eth_DataType* DataPtr = NULL_PTR;
    Std_ReturnType retVal;
    /* LenByte is not used */
    uint16 LenByte = 0;
    uint8 BufIdx_Internal;

#if (ETHIF_TXBUFADDR_PROC_USED != STD_ON)
    /* By default, ETHIF_TXBUFADDR_PROC_USED is set to STD_OFF, and a stub TxBuffer is provided. Users do not need
    * to get the actual TxBuffer address because this function will no be called anyway.
    */
    Eth_DataType EthIf_TxBuffer_Stub[ETHIF_ETHTHDR_OFFSET + ETHIF_VLANTAG_LENGTH] = {0};

    *DataPtr = &EthIf_TxBuffer_Stub[0];
    retVal = E_OK;
#else
    /* If EthTSyn module is used, the correct TxBuffer address is needed.
     *
     * For ISOLAR-EVE MCAL, please implement a method to get relevant TxBuffer address from m_Controllers->m_TxBuffer
     * Integrators should change the method to get TxBuffer address when using other MCAL.
     */

    /* For TC26x MCAL case, the below commented code is sample implementation for getting the TxBuffer Address.
     * Please remove the EVE's part above and un-comment the below lines when EthTSyn is integrated.
     */
//    Eth_DmaDescType TxBuffPtr;
//    uint32 TxDescListAddr;
//
//    /* Calculate BufIdx address */
//    TxDescListAddr = ETH_TRANSMIT_DESCRIPTOR_LIST_ADDRESS.U & 0xFFFFFFFCu;
//    TxBuffPtr = ((Eth_DmaDescType*)TxDescListAddr)[BufIdx];
//    *DataPtr = (Eth_DataType*)TxBuffPtr.Buffer1;
//    retVal = E_OK;
    DataPtr = EthIf_EthTxBufferPtr;
    retVal = E_OK;

#endif  /* #if (ETHIF_TXBUFADDR_PROC_USED != STD_ON) */

#if(ETHIF_WRAPPER_ETH_VERSION == 2)
    /* Report DET : Development Error: The BufIdx value out of bound */
    ETHIF_REPORT_ERROR_RET_VOID( ( BufIdx > 255 ), ETHIF_SID_ETHIF_CBK_TXCONFIRMATION, ETHIF_E_INV_PARAM)
    /* Cast uint32 to uint8 */
    BufIdx_Internal = ( (uint8)( BufIdx & 0x000000FFU ) );

#elif (ETHIF_WRAPPER_ETH_VERSION == 0)
    BufIdx_Internal =  BufIdx;
#endif

    if(retVal == E_OK){
        EthIf_TxConfirmation_Internal(CtrlIdx, BufIdx_Internal, DataPtr, LenByte);
    }else{
        retVal = E_NOT_OK;
    }
}


#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_WRAPPER_INTERGRATION_C */
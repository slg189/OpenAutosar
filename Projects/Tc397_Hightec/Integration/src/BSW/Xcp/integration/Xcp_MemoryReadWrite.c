/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */




/***************************************************************************************************
* Includes
***************************************************************************************************/
#include "Xcp_Cbk.h"
#include "Xcp_Priv.h"

/***************************************************************************************************
* Variables
***************************************************************************************************/


/***************************************************************************************************
* Functions
***************************************************************************************************/
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define XCP_START_SEC_CODE
#include "Xcp_MemMap.h"

#if (XCP_UPLOAD_DOWNLOAD_MEMORY_ACCESS_BY_APPL == STD_ON)
/**
****************************************************************************************************
Copy data from ECU memory to the internal buffer
\param[in]  AddrPtrDst        destination address (Xcp-internal buffer)
\param[in]  XcpAddrSrc        source address (ECU memory)
\param[in]  Length            length of the data to process
\param[in]  ProtocolLayerId   Protocol Layer Id
\retval     XCP_NO_ERROR              All Ok, process finished successfully
\retval     XCP_NO_RESPONSE           Process started, but not finished
\retval     XCP_ERR_MEMORY_OVERFLOW   Memory overflow
\retval     XCP_ERR_ACCESS_DENIED     Access denied
***************************************************************************************************/
Xcp_ErrorCode XcpAppl_MemRead(uint8* AddrPtrDst, const Xcp_AddrType_t XcpAddrSrc, uint8 Length, uint8 ProtocolLayerId)
{
  /*---------------------c--------------------------------------------*/
  /* This function is project specific and shall be totally reworked */
  /*-----------------------------------------------------------------*/

  /* Remove when parameter used */
  //XCP_PARAM_UNUSED(AddrPtrDst);
  //XCP_PARAM_UNUSED(XcpAddrSrc);
  //XCP_PARAM_UNUSED(Length);
  //XCP_PARAM_UNUSED(ProtocolLayerId);

  Xcp_MemCopy(AddrPtrDst, XcpAddrSrc.AddrValue, Length);

  /* Return error code */
  return(XCP_NO_ERROR);
}


# if (XCP_CALIBRATION_CAL == STD_ON)
/**
****************************************************************************************************
Copy data from internal buffer to ECU memory
\param[in]  XcpAddrDst        destination address (ECU memory)
\param[in]  AddrPtrSrc        source address (Xcp-internal buffer)
\param[in]  Length            length of the data to process
\param[in]  ProtocolLayerId   Protocol Layer Id
\retval     XCP_NO_ERROR              All Ok, process finished successfully
\retval     XCP_NO_RESPONSE           Process started, but not finished
\retval     XCP_ERR_MEMORY_OVERFLOW   Memory overflow
\retval     XCP_ERR_ACCESS_DENIED     Access denied
***************************************************************************************************/
/* MR12 RULE 8.13 VIOLATION: AddrPtrDst cannot be const as this is only a template and thus not doing anything with the parameters. */
Xcp_ErrorCode XcpAppl_MemWrite(Xcp_AddrType_t XcpAddrDst, const uint8* AddrPtrSrc, uint8 Length, uint8 ProtocolLayerId)
{
  /*-----------------------------------------------------------------*/
  /* This function is project specific and shall be totally reworked */
  /*-----------------------------------------------------------------*/

  /* Remove when parameter used */
  //XCP_PARAM_UNUSED(AddrPtrDst);
  //XCP_PARAM_UNUSED(AddrPtrSrc);
  //XCP_PARAM_UNUSED(Length);
  //XCP_PARAM_UNUSED(ProtocolLayerId);

  Xcp_MemCopy(XcpAddrDst.AddrValue, AddrPtrSrc, Length);

  /* Return error code */
  return(XCP_NO_ERROR);
}


/**
****************************************************************************************************
Copy data from source to destination - background process
\param[in]  ProtocolLayerId   Protocol Layer Id
\retval     XCP_NO_ERROR              All Ok, process finished successfully
\retval     XCP_NO_RESPONSE           Process started, but not finished
\retval     XCP_ERR_MEMORY_OVERFLOW   Memory overflow
\retval     XCP_ERR_ACCESS_DENIED     Access denied
***************************************************************************************************/
Xcp_ErrorCode XcpAppl_MemWriteMainFunction(uint8 ProtocolLayerId)
{
  /*-----------------------------------------------------------------*/
  /* This function is project specific and shall be totally reworked */
  /*-----------------------------------------------------------------*/

  /* Remove when parameter used */
  XCP_PARAM_UNUSED(ProtocolLayerId);

  /* Return error code */
  return(XCP_NO_ERROR);
}
# endif /* (XCP_CALIBRATION_CAL == STD_ON) */
#endif /* (XCP_UPLOAD_DOWNLOAD_MEMORY_ACCESS_BY_APPL == STD_ON) */

/* ------------------------------------------------------------------------ */
/* End section for code */

#define XCP_STOP_SEC_CODE
#include "Xcp_MemMap.h"



// ----------------------------------------------------------------------------
//! \file
//! \brief Common Exclusive Area Handling
//!
//! $Source: Config/SchM/SchM_Default.h $
//! $Author: XIE2SGH $
//! $Revision: 33135 $
//! $Date: 2021-09-01 18:01:39 +0800 (Wed, 01 Sep 2021) $
//!
//! To ease Configuring Exclusive Area Handling the component specific handling
//! is mapped to a common handling. Only in case if a component specific
//! handling is required the component specific file "SchM_<MSN>.h" has to be
//! adapted.
//! Design specification: AUTOSAR_SWS_BSW_Scheduler.pdf Version 3.1 Rev0001
// ----------------------------------------------------------------------------



#ifndef SCHM_DEFAULT_H
#define SCHM_DEFAULT_H

#include "Os.h"
//#include "SchM.h"
#include "Compiler.h"

// ---- Includes --------------------------------------------------------------

#define SCHM_ENTER_DEFAULT()    SuspendAllInterrupts()

#define SCHM_EXIT_DEFAULT()     ResumeAllInterrupts()


#endif /* SCHM_DEFAULT_H */

// -------- END OF FILE -------------------------------------------------------

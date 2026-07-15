/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_NM
 * Description: Testcode for ASW_NM
 * Version:  1.0   Author: Allen XIE      Date:  Tue Mar 12 11:10:11 2019     Update information:  Standardized the Banner
 * Version:  1.1   Author: Allen XIE      Date:  Tue Mar 12 11:10:11 2020     Update information:  Update the runnable with one feature.
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#ifndef ASW_NM_H_
#define ASW_NM_H_

typedef struct {
    char flag;
    Nm_StateType nm_PreviousState;
    Nm_StateType nm_CurrentState;
    boolean IsNmReceived;
    boolean release_b;
    boolean request_b;
    char nm_rx_data[8];
    char nm_tx_data[8];
    char nmState;
    char nmMode;
    char NotReceiveNMCounter;
}Nm_Test_t;

extern uint8 shutdown_b;

#endif /*ASW_NM_H_*/

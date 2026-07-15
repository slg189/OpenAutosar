

#ifndef ETHSM_TYPES_H_
#define ETHSM_TYPES_H_

#if(STD_ON == ETHSM_DEM_CONFIGURED)
#include "Dem.h"
# if (!defined(DEM_AR_RELEASE_MAJOR_VERSION) || (DEM_AR_RELEASE_MAJOR_VERSION != ETHSM_AR_RELEASE_MAJOR_VERSION))
#  error "DEM AUTOSAR major version undefined or mismatched."
# endif
# if (!defined(DEM_AR_RELEASE_MINOR_VERSION) || (DEM_AR_RELEASE_MINOR_VERSION != ETHSM_AR_RELEASE_MINOR_VERSION))
#  error "DEM AUTOSAR major version undefined or mismatched."
# endif
#endif
#include "EthIf.h"

typedef enum {
    ETHSM_STATE_OFFLINE                            = 0x00U, /* EthSM network state after initialization                                                                       */
    ETHSM_STATE_WAIT_TRCVLINK                      = 0x01U, /* Ctrl and Trcv/Switch/SwitchPortGroup are initialized and set to ACTIVE. EthSM is waiting for EthSM_TrcvLinkStateChg(ACTIVE) call back */
    ETHSM_STATE_WAIT_ONLINE                        = 0x02U, /* Trcv/Switch/SwitchPortGroup link state is ACTIVE and TcpIp requested for IP address assignment. EthSM is waiting for EthSM_TcpIpModeIndication(ACTIVE) call back */
    ETHSM_STATE_ONLINE                             = 0x03U, /* IP assignment process is complete and ComM state COMM_FULL_COMMUNICATION is reached                            */
    ETHSM_STATE_ONHOLD                             = 0x04U, /* Link has gone DOWM after reaching ETHSM_STATE_ONLINE                                                           */
    ETHSM_STATE_WAIT_OFFLINE                       = 0x05U, /* EthSM is requested for COMM_NO_COMMUNICATION and EthSM has requested TcpIp to release IP                       */
    ETHSM_SUBSTATE_OFFLINE_WAIT_CTRLMODE_ACTIVE    = 0x06U  /* Substate of ETHSM_STATE_OFFLINE. EthIfCtrl is requested for mode ACTIVE and waiting for the call back          */
} EthSM_NetworkModeStateType;

typedef enum {
    ETHSM_USER_TCPIP                        = 0x00U,     /* Network user is TcpIp                  */
    ETHSM_USER_CDD                          = 0x01U      /* Network user is Complex Device Driver   */
}EthSM_NetworkUserType_ten;

typedef struct {
    void (*UserTrcvLinkStateChgFunc) ( uint8 CtrlIdx,    /* Function pointer to the User_TrcvLinkStateChg function   */
                                       EthTrcv_LinkStateType TransceiverLinkState);
    EthSM_NetworkUserType_ten NetworkUser_en;            /* Specifies the network user type */
} EthSM_NetworkUserConfig_tst;

typedef struct {
    const EthSM_NetworkUserConfig_tst * NetworkUserConfig_pcst;      /* Reference to the Network user data structure   */
#if(STD_ON == ETHSM_DEM_CONFIGURED)
    Dem_EventIdType DemEventId_u16;                                  /* Holds Id for ETHSM_E_LINK_DOWN if ETHSM_E_LINK_DOWN is configured. Holds 0 if ETHSM_E_LINK_DOWN is not configured */
#endif
    NetworkHandleType comMNetHandleId_u8;                            /* Reference to ComM Network Handle */
    uint8 NumOfNetworkUsers_u8;                                      /* Number of EthSM Network users for the network */
} EthSM_ConfigType;


#endif /* ETHSM_TYPES_H_ */

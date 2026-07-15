

#include "DoIP_Prv.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

static void DoIP_RoutingActivation_PrepareResponse(DoIP_Type_TCPConnection *tcpCon_pst, uint8 responseCode_u8)
{
    uint32 payloadLength_u32 = DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_RESP_9;
    uint8* msg_pu8 = tcpCon_pst->ra_st.buffer_au8;
    uint16 address_u16;

    /* Header */
    if (tcpCon_pst->ra_st.oemBytesResponse_b)
    {
        payloadLength_u32 = DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_RESP_13;
    }
    DoIP_Header_Write(DOIP_PAYLOAD_TYPE_ROUTING_ACTIVATION_RESP, payloadLength_u32, msg_pu8);
    msg_pu8 += DOIP_HEADER_SIZE;

    /* Payload */
    /* tcpCon_pst->ra_st.buffer_au8[8], tcpCon_pst->ra_st.buffer_au8[9] */
    /* Source Address - already there */
    msg_pu8 += 2;

    /* LogicalAddressTester */
    address_u16 = rba_DiagLib_ByteOrderUtils_Htons(DoIP_activeConfig_pcst->logicalAddress_u16);
    DOIP_MEMCPY(msg_pu8, &address_u16, 2);
    msg_pu8 += 2;

    *msg_pu8 = responseCode_u8;
    msg_pu8++;

    *msg_pu8 = 0x00; //ISO
    msg_pu8++;
    *msg_pu8 = 0x00; //ISO
    msg_pu8++;
    *msg_pu8 = 0x00; //ISO
    msg_pu8++;
    *msg_pu8 = 0x00; //ISO
    msg_pu8++;

    if (tcpCon_pst->ra_st.oemBytesResponse_b)
    {
        DOIP_MEMCPY(msg_pu8, tcpCon_pst->ra_st.oemResBuffer_au8, 4u);
    }

    tcpCon_pst->ra_st.comState_en = DOIP_COM_READY2SEND_E;
}

static void DoIP_RoutingActivation_RegisterTester(DoIP_RoutingActivationType_tst *ra_pst, uint16 sourceAddress_u16,
        uint8 routingActivationNumber_u8)
{
    uint8 testerIdx_u8;
    uint8 Idx_u8;
    uint8 raIdx_u8;
    boolean activationTypeChanged = FALSE;

    for (testerIdx_u8 = 0; testerIdx_u8 < DoIP_activeConfig_pcst->testerSize_u8; testerIdx_u8++)
    {
        if (DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].testerSA_u16 == sourceAddress_u16)
        {
            ra_pst->testerCfg_pcst = &DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8];
            break;
        }
    }

    for (Idx_u8 = 0; (Idx_u8 < DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].numOfRoutingActivations_u8); Idx_u8++)
    {
        raIdx_u8 = DoIP_activeConfig_pcst->raJT_pcst[(DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].routingActivationJumpTableIndex_u16
                + Idx_u8)].routingActivationIdx_u8;

        if (DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].routingActivationNumber_u8 == routingActivationNumber_u8)
        {
            if (ra_pst->raCfg_pcst != &DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8])
            {
                ra_pst->raCfg_pcst = &DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8];
                activationTypeChanged = TRUE;
                break;
            }
        }
    }

    /* Did activation type change ?*/
    if (activationTypeChanged)
    {
        DoIP_Channel_ReleaseAllChannelsForTester(ra_pst->testerCfg_pcst, E_NOT_OK);
        ra_pst->isActive_b = FALSE;
    }

    if (ra_pst->oemBytesReceived_b)
    {
        DOIP_MEMCPY(ra_pst->oemReqBuffer_au8, ra_pst->buffer_au8 + (DOIP_MSG_POS_CONTENT + 7u), 4u);
    }
}

static void DoIP_RoutingActivation_SetActive(DoIP_RoutingActivationType_tst *ra_pst)
{
    ra_pst->isActive_b = TRUE;
}

boolean DoIP_RoutingActivation_IsSourceAddressKnown(uint16 sourceAddress_u16, uint8* testerIdx_pu8)
{
    boolean retVal_b = FALSE;
    uint8 Idx_u8;

    for (Idx_u8 = 0; Idx_u8 < DoIP_activeConfig_pcst->testerSize_u8; Idx_u8++)
    {
        if (DoIP_activeConfig_pcst->testerCfg_pcst[Idx_u8].testerSA_u16 == sourceAddress_u16)
        {
            *testerIdx_pu8 = Idx_u8;
            retVal_b = TRUE;
            break;
        }
    }

    return retVal_b;
}
boolean DoIP_RoutingActivation_IsSourceAddressActive(const DoIP_RoutingActivationType_tst *ra_pcst, uint16 sourceAddress_u16)
{
    boolean retVal_b = FALSE;

    if (DoIP_RoutingActivation_IsSourceAddressRegistered(ra_pcst, sourceAddress_u16))
    {
        if (ra_pcst->isActive_b)
        {
            retVal_b = TRUE;
        }
    }

    return retVal_b;
}

boolean DoIP_RoutingActivation_IsSourceAddressRegistered(const DoIP_RoutingActivationType_tst *ra_pcst, uint16 sourceAddress_u16)
{
    boolean retVal_b = FALSE;
    const DoIP_Cfg_TesterType_tst* testerCfg_pcst = ra_pcst->testerCfg_pcst;

    if (testerCfg_pcst != NULL_PTR)
    {
        if (testerCfg_pcst->testerSA_u16 == sourceAddress_u16)
        {
            retVal_b = TRUE;
        }
    }

    return retVal_b;
}
/**
 * Thsis function checks, whether a secured connection is for this route required
 * @param sourceAddress_u16
 * @param testerIdx_u8
 * @return does this route need a secured connection?
 */
static boolean DoIP_RoutingActivation_IsSecuredConnectionRequired(uint16 sourceAddress_u16, const DoIP_Type_TCPConnection* tcpCon_pst, uint8 routingActivationNumber_u8)
{
  boolean retVal_b = TRUE;
  uint8 Idx_u8;
  uint8 testerIdx_u8;
  boolean testerSA_found = FALSE;
  uint8 raIdx_u8;

 /*
  * When receiving a routing activation request on a TCP connection where DoIPTcpConnection/DoIPTcpConnectionSecurityRequired
  * is not set or set to FALSE, the DoIP module shall search for a DoIPTester with an assigned container that matches DoIPTesterSA.
  */

  /* When receiving a routing activation request on a TCP connection where DoIPTcpConnection/DoIPTcpConnectionSecurityRequired set to TRUE,
   * the DoIP module shall search for a DoIPTester with an assigned container that matches DoIPTesterSA.
   * If such a DoIPTester container was found, the connection will be established.
   *
   * Rationale: A secure TCP connection can be established with a DoIPTester that requests a secure or unsecured connection..
   */
  for (Idx_u8 = 0; Idx_u8 < DoIP_activeConfig_pcst->testerSize_u8; Idx_u8++)
  {
      if (DoIP_activeConfig_pcst->testerCfg_pcst[Idx_u8].testerSA_u16 == sourceAddress_u16)
      {
          if(tcpCon_pst->soCon_pst->cfg_pcst->tcpConnectionSecurityRequired_b == FALSE)
          {
              testerIdx_u8 = Idx_u8;
              testerSA_found = TRUE;
              break;
          }
          else
          {
              retVal_b = FALSE;
          }

      }
  }

 /* If such a DoIPTester container was found and the matching DoIPRoutingActivation container (refer to SWS_DoIP_00108)
  * has the attribute DoIPRoutingActivationSecurityRequired not set or set to FALSE, the connection will be established.
  * If such a DoIPTester container was found and the matching DoIPRoutingActivation container (refer to SWS_DoIP_00108)
  * has the attribute DoIPRoutingActivationSecurityRequired is set to TRUE, the connection shall be rejected with the response code "0x07".
  */

  if (testerSA_found)
  {
      for (Idx_u8 = 0; (Idx_u8 < DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].numOfRoutingActivations_u8); Idx_u8++)
      {
          raIdx_u8 = DoIP_activeConfig_pcst->raJT_pcst[(DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].routingActivationJumpTableIndex_u16 + Idx_u8)].routingActivationIdx_u8;

          if ((DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].routingActivationSecurityRequired_b == FALSE) &&
                  (DoIP_activeConfig_pcst->raCfg_pcst[raIdx_u8].routingActivationNumber_u8 == routingActivationNumber_u8))
          {
              retVal_b = FALSE;
              break;
          }
      }
  }

  return retVal_b;

}
static boolean DoIP_RoutingActivation_IsActivationTypeSupported(uint8 testerIdx_u8, uint8 routingActivationNumber_u8,
        uint8* raIdx_u8)
{
    boolean retVal_b = FALSE;
    uint8 Idx_u8;

    for (Idx_u8 = 0; (Idx_u8 < DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].numOfRoutingActivations_u8); Idx_u8++)
    {
        *raIdx_u8 =
                DoIP_activeConfig_pcst->raJT_pcst[(DoIP_activeConfig_pcst->testerCfg_pcst[testerIdx_u8].routingActivationJumpTableIndex_u16
                        + Idx_u8)].routingActivationIdx_u8;

        if (DoIP_activeConfig_pcst->raCfg_pcst[*raIdx_u8].routingActivationNumber_u8 == routingActivationNumber_u8)
        {
            retVal_b = TRUE;
            break;
        }
    }

    return retVal_b;
}
/**
 * DoIP_RoutingActivation_IsConnectionFree
 *
 * This functions checks whether the source address_u16 is already in use
 * @param ra_pcst
 * @param sourceAddress_u16
 * @return
 */
static boolean DoIP_RoutingActivation_IsConnectionFree(const DoIP_RoutingActivationType_tst *ra_pcst, uint16 sourceAddress_u16)
{
    boolean retVal_b = TRUE;
    const DoIP_Cfg_TesterType_tst* testerCfg_pcst = ra_pcst->testerCfg_pcst;

    /* Connection has to be either not used, or is already active with this SA */
    if (testerCfg_pcst != NULL_PTR)
    {
        if (testerCfg_pcst->testerSA_u16 != sourceAddress_u16)
        {
            retVal_b = FALSE;
        }
    }

    return retVal_b;
}
/**
 * DoIP_RoutingActivation_IsActivationTypeActive
 *
 * This functions checks if the Activiation numberes match, is this connection already registered or not
 * @param ra_pcst
 * @param routingActivationNumber_u8
 * @return
 */
boolean DoIP_RoutingActivation_IsActivationTypeActive(const DoIP_RoutingActivationType_tst *ra_pcst,
        uint8 routingActivationNumber_u8)
{
    boolean retVal_b = FALSE;
    const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst = ra_pcst->raCfg_pcst;

    /* Connection has to be either not used, or is already active with this SA */
    if (raCfg_pcst != NULL_PTR)
    {
        if (raCfg_pcst->routingActivationNumber_u8 == routingActivationNumber_u8)
        {
            if (ra_pcst->isActive_b)
            {
                retVal_b = TRUE;
            }
        }
    }

    return retVal_b;
}

/**
 * DoIP_RoutingActivation_IsSourceAddressAlreadyRegisteredOnAnotherConnection
 *
 * This function checks, if a source address_u16 is already in use by a TCP connection other than *tcpCon_pcst.
 * If that is the case, it updates tcpConOther_ppst to point to the other entry that uses sourceAddress_u16 and returns TRUE.
 * @param tcpCon_pcst
 * @param sourceAddress_u16
 * @param tcpConOther_ppst
 * @return
 */
static boolean DoIP_RoutingActivation_IsSourceAddressAlreadyRegisteredOnAnotherConnection(const DoIP_Type_TCPConnection *tcpCon_pcst,
        uint16 sourceAddress_u16, DoIP_Type_TCPConnection** tcpConOther_ppst)
{
    boolean retVal_b = FALSE;
    uint8 idx_u8;
    DoIP_Type_TCPConnection* tcpConTemp_pst;

    for (idx_u8 = 0; idx_u8 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; idx_u8++)
    {
        tcpConTemp_pst = DoIP_TCPConnection_ast+idx_u8;

        if (tcpConTemp_pst != tcpCon_pcst)
        {
            if (DoIP_RoutingActivation_IsSourceAddressRegistered(&(tcpConTemp_pst->ra_st), sourceAddress_u16))
            {
                *tcpConOther_ppst = tcpConTemp_pst;
                retVal_b = TRUE;
                break;
            }
        }
    }

    return retVal_b;
}
/**
 * DoIP_RoutingActivation_WouldExceedConnectionLimit
 *
 * This function checks, whether the requested connection is already active and would the MaxTesterConnection border exceeded
 * @param requestedTcpCon_pcst
 * @return
 */
static boolean DoIP_RoutingActivation_WouldExceedConnectionLimit(const DoIP_Type_TCPConnection *requestedTcpCon_pcst)
{
    uint8 tcpConCnt_u8 = 0;
    uint8 tcpIndex_u8;
    DoIP_Type_TCPConnection* tcpConn_pst = NULL_PTR;

    //Search for all other active tcp conections
    for (tcpIndex_u8 = 0; tcpIndex_u8 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; tcpIndex_u8++)
    {
        tcpConn_pst = DoIP_TCPConnection_Get(tcpIndex_u8);

        // Do not include the requested TCP connection.
        // If it is already active, it gets replaced, and the overall number does not increase.
        if (requestedTcpCon_pcst != tcpConn_pst)
        {
            if (tcpConn_pst->ra_st.isActive_b == TRUE)
            {
                tcpConCnt_u8++;
            }
        }
    }

    //Increment counter by one for this connection we want to open
    tcpConCnt_u8++;
    // we check this condition only if received tcp data socket is available.
    // Because of this, we checking only the greater case and not the equal case.
    return tcpConCnt_u8 > DOIP_CFG_MaxTesterConnections;
}

static boolean DoIP_RoutingActivation_HandleNewRequest(DoIP_Type_TCPConnection *tcpCon_pst)
{
    boolean retVal_b = FALSE;
    uint8 testerIdx_u8;
    uint8 raIdx_u8;

    uint16 sourceAddress_u16;
    uint8 routingActivationNumber_u8;

    DOIP_MEMCPY(&sourceAddress_u16, tcpCon_pst->ra_st.buffer_au8 + DOIP_MSG_POS_CONTENT, 2);
    sourceAddress_u16 = rba_DiagLib_ByteOrderUtils_Ntohs(sourceAddress_u16);

    routingActivationNumber_u8 = tcpCon_pst->ra_st.buffer_au8[DOIP_MSG_POS_CONTENT + 2u];

    /* Set inactivity timer once routing activation request is received */

    /* [DoIP-080]
     * The general inactivity timer of a TCP_DATA socket shall be reset to its initial value T_TCP_
     * General_Inactivity (Table 38) when the socket is initially put into the established state (open)
     * or whenever data is received or sent over this socket
     *
     * [DoIP-085] The initial inactivity timer of a TCP_DATA socket shall be stopped immediately after receipt of a
     *   valid routing activation request (see Table 22) on this TCP_DATA socket.
     */
    DoIP_TCPConnection_SetGeneralInactivity(tcpCon_pst);

    if (!DoIP_RoutingActivation_IsSourceAddressKnown(sourceAddress_u16, &testerIdx_u8))
    {
        DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_UNKNOWN_SA);
    }
    else if (!DoIP_RoutingActivation_IsActivationTypeSupported(testerIdx_u8, routingActivationNumber_u8, &raIdx_u8))
    {
        DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_UNKNWON_TYPE);
    }
    /* SocketHandler */
    /* Check if the TCP_DATA socket on which the current message was received is already registered */
    else if (!DoIP_RoutingActivation_IsConnectionFree(&tcpCon_pst->ra_st, sourceAddress_u16))
    {
        DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_CONNECTION_IN_USE);
    }
    else
    {
        // go here to socket handler in the AliveCheck_HandleNewMessage
        retVal_b = TRUE;
    }

    return retVal_b;
}

static void DoIP_RoutingActivation_HandleAliveCheckForConnectionLimit(DoIP_Type_TCPConnection *tcpCon_pst)
{
    boolean aliveCheckNack_b = TRUE;
    uint8 tcpIndex_u8;
    DoIP_Type_TCPConnection* tcpCon2Check_pst = NULL_PTR;
    DoIP_Type_TCPConnection* tcpCon2CheckActive_pst = NULL_PTR;

    if (tcpCon_pst->ra_st.raState_en != DOIP_RA_WAITALIVE_E)
    {
        /* Request Alive check on all registered connections */
        for (tcpIndex_u8 = 0; tcpIndex_u8 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; tcpIndex_u8++)
        {
            tcpCon2Check_pst = DoIP_TCPConnection_Get(tcpIndex_u8);

            //Search for all other active tcp connections
            if (tcpCon2Check_pst->ra_st.isActive_b == TRUE)
            {
                DoIP_AliveCheck_Request(tcpCon2Check_pst);

                /* Change state to check in next mainfunctions */
                tcpCon_pst->ra_st.raState_en = DOIP_RA_WAITALIVE_E;
                tcpCon2Check_pst->ra_st.raState_en = DOIP_RA_WAITALIVE_E;
            }
        }
     }
    else
    {
        for (tcpIndex_u8 = 0; tcpIndex_u8 < DOIP_CFG_MAX_NUM_OF_TCP_CONN; tcpIndex_u8++)
        {
            tcpCon2CheckActive_pst = DoIP_TCPConnection_Get(tcpIndex_u8);

            /* Send NACK only after alive check is completed */
            if (!DoIP_AliveCheck_IsIdle(tcpCon2CheckActive_pst))
            {
                aliveCheckNack_b = FALSE;
                break;
            }
        }

        if(aliveCheckNack_b)
        {
            DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_NO_FREE_SOCKET);
            tcpCon_pst->ra_st.raState_en = DOIP_RA_IDLE_E;
        }
    }
}
static boolean DoIP_RoutingActivation_HandleAliveCheck(DoIP_Type_TCPConnection *tcpCon_pst)
{
    uint16 sourceAddress_u16;
    uint8 routingActivationNumber_u8;
    DoIP_Type_TCPConnection* tcpOtherCon_pst = NULL_PTR;
    boolean retVal_b = FALSE;

    routingActivationNumber_u8 = tcpCon_pst->ra_st.buffer_au8[DOIP_MSG_POS_CONTENT + 2u];
    DOIP_MEMCPY(&sourceAddress_u16, tcpCon_pst->ra_st.buffer_au8 + DOIP_MSG_POS_CONTENT, 2);
    sourceAddress_u16 = rba_DiagLib_ByteOrderUtils_Ntohs(sourceAddress_u16);

     /* Check if SA is already registered to another TCP_DATA socket */
    if (DoIP_RoutingActivation_IsSourceAddressAlreadyRegisteredOnAnotherConnection(tcpCon_pst, sourceAddress_u16, &tcpOtherCon_pst))
    {
        if (tcpCon_pst->ra_st.raState_en == DOIP_RA_WAITALIVE_E)
        {
            /* check here whether a Alive check response is received */
            if (DoIP_AliveCheck_IsIdle(tcpOtherCon_pst))
            {
                DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_SA_IN_USE);
                tcpCon_pst->ra_st.raState_en = DOIP_RA_IDLE_E;
            }
        }
        else
        {
            /* First time -> Perform alive check (single SA)*/
            DoIP_AliveCheck_Request(tcpOtherCon_pst);

            /* Change state to check in next mainfunctions */
            tcpCon_pst->ra_st.raState_en = DOIP_RA_WAITALIVE_E;
        }
    }
    /* Handling for max tester connections */
    else if(DoIP_RoutingActivation_WouldExceedConnectionLimit(tcpCon_pst))
    {
        DoIP_RoutingActivation_HandleAliveCheckForConnectionLimit(tcpCon_pst);
    }
    /* [DoIP-174]
     * DoIP entity routing activation response code 0x07
     * After reception of a routing activation request message with a routing activation type, which requires the secure
     * TLS connection to the DoIP entity then each DoIP entity supporting the secure TCP communication shall send
     * the routing activation response message with the response code set to 0x07.
     */

    else if(DoIP_RoutingActivation_IsSecuredConnectionRequired(sourceAddress_u16, tcpCon_pst, routingActivationNumber_u8))
    {
        /* If such a DoIPTester container was found and the matching DoIPRoutingActivation container (refer to SWS_DoIP_00108)
         * has the attribute DoIPRoutingActivationSecurityRequired is set to TRUE, the connection shall be rejected with the response code "0x07".
         */
        DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_SECURED_CONNECTION_REQUIRED);
    }
    else
    {
        retVal_b = TRUE;
        //Auth follows -> register tester and raType .(What happens if type changes and auth ongoing?)
        DoIP_RoutingActivation_RegisterTester(&tcpCon_pst->ra_st, sourceAddress_u16, routingActivationNumber_u8);
    }

    return retVal_b;
}

static boolean DoIP_RoutingActivation_HandleAuthentication(DoIP_Type_TCPConnection *tcpCon_pst)
{
    boolean retVal_b = TRUE;
    boolean allowed_b = FALSE;
    Std_ReturnType retVal_u8;

    DoIP_RoutingActivationType_tst* ra_pst = &tcpCon_pst->ra_st;
    const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst = ra_pst->raCfg_pcst;

    DoIP_Cfg_RoutingActivationAuthenticationFunctionType_tpfct authFunc = NULL_PTR;
    uint8 reqLength;
    uint8 resLength;

    if(raCfg_pcst != NULL_PTR)
    {
        authFunc = raCfg_pcst->authenticationFunction_pfct;
        reqLength = raCfg_pcst->authenticationReqLength_u8;
        resLength = raCfg_pcst->authenticationResLength_u8;
    }
    else
    {
        retVal_b = FALSE;
    }

    if (authFunc != NULL_PTR)
    {
        if (reqLength > 0)
        {
            if (!ra_pst->oemBytesReceived_b)
            {
                DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_AUTH_FAILED);
                ra_pst->raState_en = DOIP_RA_IDLE_E;
                retVal_b = FALSE;
            }
        }

        if (retVal_b)
        {
            if (resLength > 0)
            {
                ra_pst->oemBytesResponse_b = TRUE;
            }

            retVal_u8 = authFunc(&allowed_b, ra_pst->oemReqBuffer_au8, ra_pst->oemResBuffer_au8);

            if (retVal_u8 == E_OK)
            {
                /* Go to conf */
            }
            else if (retVal_u8 == DOIP_E_PENDING)
            {
                /* Nothing */
                retVal_b = FALSE;
            }
            else
            {
                /* negative response */
                DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_AUTH_FAILED);

                ra_pst->oemBytesResponse_b = FALSE;
                ra_pst->raState_en = DOIP_RA_IDLE_E;
                retVal_b = FALSE;
            }
        }
    }
    else
    {
        /* No auth needed, go to conf */
    }

    return retVal_b;
}

static boolean DoIP_RoutingActivation_HandleConfirmation(DoIP_Type_TCPConnection *tcpCon_pst)
{
    boolean retVal_b = TRUE;
    boolean confirmed_b = FALSE;
    Std_ReturnType retVal_u8;

    DoIP_RoutingActivationType_tst* ra_pst = &tcpCon_pst->ra_st;
    const DoIP_Cfg_RoutingActivationType_tst* raCfg_pcst = ra_pst->raCfg_pcst;

    DoIP_Cfg_RoutingActivationConfirmationFunctionType_tpfct confFunc = NULL_PTR;
    uint8 reqLength;
    uint8 resLength;

    if(raCfg_pcst != NULL_PTR)
    {
        confFunc = raCfg_pcst->confirmationFunction_pfct;
        reqLength = raCfg_pcst->confirmationReqLength_u8;
        resLength = raCfg_pcst->confirmationResLength_u8;
    }
    else
    {
        retVal_b = FALSE;
    }

    if (confFunc != NULL_PTR)
    {
        if (reqLength > 0)
        {
            if (!ra_pst->oemBytesReceived_b)
            {
                DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_CONF_FAILED);
                ra_pst->raState_en = DOIP_RA_IDLE_E;
                retVal_b = FALSE;
            }
        }

        if (retVal_b)
        {
            if (resLength > 0)
            {
                ra_pst->oemBytesResponse_b = TRUE;
            }

            retVal_u8 = confFunc(&confirmed_b, ra_pst->oemReqBuffer_au8 + (4 - reqLength), ra_pst->oemResBuffer_au8 + (4 - resLength));

            if (retVal_u8 == E_OK)
            {
            }
            else if (retVal_u8 == DOIP_E_PENDING)
            {
                /* Pending response */
                if (tcpCon_pst->ra_st.comState_en == DOIP_COM_PROCESSING_E)
                {
                    DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_PENDING);
                }
                retVal_b = FALSE;
            }
            else
            {
                /* Negative response */
                if (tcpCon_pst->ra_st.comState_en == DOIP_COM_PROCESSING_E)
                {
                    DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_CONF_FAILED);
                }
                retVal_b = FALSE;
                ra_pst->raState_en = DOIP_RA_IDLE_E;
            }
        }
    }
    else
    {
        /* No conf needed */
    }

    return retVal_b;
}

void DoIP_RoutingActivation_Reset(DoIP_RoutingActivationType_tst *ra_pst)
{
    ra_pst->comState_en = DOIP_COM_IDLE_E;
    ra_pst->raState_en = DOIP_RA_IDLE_E;
    ra_pst->testerCfg_pcst = NULL_PTR;
    ra_pst->raCfg_pcst = NULL_PTR;
    ra_pst->isActive_b = FALSE;
    ra_pst->oemBytesReceived_b = FALSE;
    ra_pst->oemBytesResponse_b = FALSE;
}

static void DoIP_RoutingActivation_MainFunction2(DoIP_Type_TCPConnection *tcpCon_pst)
{
    if ((tcpCon_pst->ra_st.raState_en == DOIP_RA_ALIVE_E) || (tcpCon_pst->ra_st.raState_en == DOIP_RA_WAITALIVE_E))
    {
        if (DoIP_RoutingActivation_HandleAliveCheck(tcpCon_pst))
        {
            tcpCon_pst->ra_st.raState_en = DOIP_RA_AUTH_E;
        }
    }

    if (tcpCon_pst->ra_st.raState_en == DOIP_RA_AUTH_E)
    {
        if (DoIP_RoutingActivation_HandleAuthentication(tcpCon_pst))
        {
            tcpCon_pst->ra_st.raState_en = DOIP_RA_CONF_E;
        }
    }

    if (tcpCon_pst->ra_st.raState_en == DOIP_RA_CONF_E)
    {
        if (DoIP_RoutingActivation_HandleConfirmation(tcpCon_pst))
        {
            if (tcpCon_pst->ra_st.comState_en == DOIP_COM_PROCESSING_E)
            {
                DoIP_RoutingActivation_PrepareResponse(tcpCon_pst, DOIP_RA_RESPCODE_SUCCESS);
            }
            DoIP_RoutingActivation_SetActive(&tcpCon_pst->ra_st);
            tcpCon_pst->ra_st.raState_en = DOIP_RA_IDLE_E;
        }
    }
}

void DoIP_RoutingActivation_MainFunction(DoIP_Type_TCPConnection *tcpCon_pst)
{
    /* Mainfunction for communication states */
    /* Was a new message received ? */
    if (tcpCon_pst->ra_st.comState_en == DOIP_COM_RECEIVED_E)
    {
        if (DoIP_RoutingActivation_HandleNewRequest(tcpCon_pst))
        {
            /* Change state next state */
            tcpCon_pst->ra_st.comState_en = DOIP_COM_PROCESSING_E;

            /* Change substate*/
            tcpCon_pst->ra_st.raState_en = DOIP_RA_ALIVE_E;
        }
        else
        {
            /* Reset substate */
            tcpCon_pst->ra_st.raState_en = DOIP_RA_IDLE_E;
        }
    }

    /* Handle routing activation state machine */
    DoIP_RoutingActivation_MainFunction2(tcpCon_pst);

    /* Ready to send the RA status */
    if (tcpCon_pst->ra_st.comState_en == DOIP_COM_READY2SEND_E)
    {
        tcpCon_pst->ra_st.comState_en = DOIP_COM_WAIT4CONFIRMATION_E;

        if (FALSE == DoIP_TCPConnection_SendMessage(tcpCon_pst, DOIP_INVALID_PDUID, tcpCon_pst->ra_st.buffer_au8, FALSE))
        {
            tcpCon_pst->ra_st.comState_en = DOIP_COM_READY2SEND_E;
        }
    }
}

void DoIP_RoutingActivation_TxConfirmation(DoIP_Type_TCPConnection *tcpCon_pst)
{
    uint8 responseCode_u8 = tcpCon_pst->ra_st.buffer_au8[DOIP_HEADER_SIZE + 4u];

    switch (responseCode_u8)
    {
        case DOIP_RA_RESPCODE_AUTH_FAILED:
        case DOIP_RA_RESPCODE_PENDING:
        case DOIP_RA_RESPCODE_SUCCESS:
            tcpCon_pst->ra_st.comState_en = DOIP_COM_IDLE_E;
            tcpCon_pst->TX_st.tcpState_en = DOIP_TCP_TX_READY_E;
            break;
        case DOIP_RA_RESPCODE_UNKNOWN_SA:
        case DOIP_RA_RESPCODE_NO_FREE_SOCKET:
        case DOIP_RA_RESPCODE_CONNECTION_IN_USE:
        case DOIP_RA_RESPCODE_SA_IN_USE:
        case DOIP_RA_RESPCODE_UNKNWON_TYPE:
        case DOIP_RA_RESPCODE_CONF_FAILED:
        default:
            /* Routing Activation failed, connection needs to be closed */
            /* Change state to FAILED to "LOCK" statemachine*/
            tcpCon_pst->ra_st.comState_en = DOIP_COM_FAILED_E;
            /* Set flag to close connection */
            DoIP_TCPConnection_SetCloseNeeded(tcpCon_pst);
            break;

    }
}

uint16 DoIP_RoutingActivation_GetAckNackResponseDataLength(const DoIP_Cfg_TesterType_tst* tester_pcst)
{
    uint16 ackLength_u16 = 0;
    if (tester_pcst != NULL_PTR)
    {
        // the configuration of the tester (numByteDiagAckNack_u16) is relevant
        ackLength_u16 = tester_pcst->numByteDiagAckNack_u16;
        ackLength_u16 = DOIP_MIN(ackLength_u16,
                DOIP_CFG_TCP_BUFFER_SIZE - (DOIP_HEADER_SIZE + DOIP_PAYLOAD_LENGTH_DIAG_MSG_MIN + 1u));
    }
    return ackLength_u16;
}

BufReq_ReturnType DoIP_RoutingActivation_RoutingActivationHandler(DoIP_Type_TCPConnection* tcpCon_pst, const uint8* header_pu8, const uint8* payload_pu8)
{
    BufReq_ReturnType retVal_en = BUFREQ_E_NOT_OK;

    /* Check Routing Activation message handling is not in progress state */
    if (tcpCon_pst->ra_st.comState_en == DOIP_COM_IDLE_E)
    {
        DOIP_MEMCPY(&tcpCon_pst->ra_st.buffer_au8[0], &header_pu8[0], DOIP_HEADER_SIZE);
        DOIP_MEMCPY(&tcpCon_pst->ra_st.buffer_au8[DOIP_HEADER_SIZE], &payload_pu8[0], tcpCon_pst->RX_st.totalPayloadLength_u32);

        /* If Payload length is 11 bytes, then optional 4 bytes of OEM specific bytes has been received */
        tcpCon_pst->ra_st.oemBytesReceived_b =
                    (tcpCon_pst->RX_st.totalPayloadLength_u32 == DOIP_PAYLOAD_LENGTH_ROUTING_ACTIVATION_REQ_11) ? TRUE : FALSE;

        /* Reset oemBytesResponse_b to FALSE if any new message has been received */
        tcpCon_pst->ra_st.oemBytesResponse_b = FALSE;

        retVal_en = BUFREQ_OK;

        /* Set comState_en to DOIP_COM_RECEIVED_E to process in in next main function */
        tcpCon_pst->ra_st.comState_en = DOIP_COM_RECEIVED_E;

        /* set RX.ProcessedLength */
        // a cast to uint16 is ok, because the Routing Actiovation response is smaller than a uint32 msg
        tcpCon_pst->RX_st.ProcessedLength_u16 += (uint16)tcpCon_pst->RX_st.totalPayloadLength_u32;
    }

    return retVal_en;
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"


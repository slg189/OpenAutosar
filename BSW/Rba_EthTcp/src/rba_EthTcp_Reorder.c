

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#ifdef TCPIP_CONFIGURED
#if ( TCPIP_TCP_ENABLED == STD_ON )

#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"
#include "rba_BswSrv.h"

/*
 ***************************************************************************************************
 *+++++++++++++++++++++++++++++ BUFFER HANDLING ALGORITHM FOR TCP REORER +++++++++++++++++++++++++++
 ***************************************************************************************************
 * - Configuration parameters:
 *     TcpIpRbTcpReorderBufferCount : Number of reorder buffer indicates number of TCP sockets capable of handling out of order reception in parallel.
 *     TcpIpRbTcpReorderBufferSize  : Size of each reorder buffer and indicates memory available for individual TCP socket in bytes for out of order reception.
 *                                    Each buffer can store multiple TCP segments belonging to the same TCP socket.
 *                                    Number of TCP segments stored in a buffer depends on the configured value of TcpIpRbTcpReorderBufferSize and
 *                                    availability of enough continuous memory for the incoming out of order segment.
 *                                    For each TCP segment in addition to TCP segment length (TCP header + Payload length) 4 more bytes are consumed to store metadata to handle buffer management.
 *
 * - Each stored TCP segment in reorder buffer can be visualized as a node in a single linked list. To manage this linked list 2 kinds of metadata are used.
 *   -- 1. Per buffer metadata:
 *      --- 2 byte metadata to indicate where the first TCP segment can be found in the buffer.
 *          Only one such metadata is required per buffer and is always located at byte position 0 and 1 of each buffer.
 *      --- For this reason, each buffer is allocated with additional 2 bytes (RBA_ETHTCP_PERBUFFER_REORDER_METADATALEN) than the configured value.
 *          i.e., memory allocated for each buffer RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE = RBA_ETHTCP_REORDER_CONFIGURED_BUFSIZE + RBA_ETHTCP_PERBUFFER_REORDER_METADATALEN.
 *      --- During init, this metadata is initialized to RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE.
 *          When the buffer has a TCP segment stored the value for this meta must be within the range from 0 to RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE - 1.
 *          Initialaization with RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE is required to know if the buffer has enough free space (further explanation is provided in implementation).
 *     --2. Per segment metadata:
 *       --- 4 bytes metadata (RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN) per TCP segment in the buffer.
 *       --- This metadata creates linked list between stored TCP segments.
 *       --- Whenever new TCP segment needs to be stored in the buffer, first this metadata is created and later TCP segment is copied. This means, every TCP segment is preceded by a PerSegment metadata in the buffer.
 *       --- This metadata has 2 parts:
 *          ---- Part1: 2 byte indicating offset (offset from the beginning of the buffer) where next TCP segment can be found.
 *                      Using the concept of linked list, when new TCP segment is stored, value for this metadata will always be copied from
 *                      the corresponding value from the previous metadata in the list (i.e. from the metadata of previous TCP segment in the buffer OR from PerBuffer metadata in case the frame  is stored at the very beginning of the buffer).
 *                      And, corresponding value for the previous metadata / PerBuffer metadata will be replaced with the offset where new metadata is created.
 *                      When this concept of linked list is followed, the value for the last metadata in buffer will always be RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE (which was eventually copied from PerBuffer metadata).
 *          ---- Part2: 2 byte indicating the length of the stored TCP segment.
 *
 * - Once the reorder buffer is assigned to the socket, after storing and releasing out of order TCP frames, frames stored in reorder buffer might look like in scattered.
 * I.e. there could be free space between Per buffer metadata and first frame in buffer, and also between the stored frames.
 ***************************************************************************************************/


/*
 ***************************************************************************************************
 * Macro definitions pertaining only to reorder handling
 ***************************************************************************************************
 */
/* Length of metadata per TCP segment in reorder buffer and is required to establish linked list between the TCP segments in the buffer. */
/* Each stored frame is preceded by 4 bytes of meta-data. */
#define RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN                       4U

/* Length of metadata per reorder buffer to indicate where the first TCP segment can be found in the buffer */
/* In each buffer, byte 0 and 1 contains the offset (from the beginning of the buffer) of first available TCP frame in the buffer.*/
#define RBA_ETHTCP_PERBUFFER_REORDER_METADATALEN                        2U


/* Read the position of next-stored-frame in reorder buffer. This information is available at byte 0 and 1 of the current-frame meta-data. */
/* Each stored frame is preceded by 4 bytes of meta-data (RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN).  */
/* And the position here is the offset of next-stored-frame meta-data from the beginning of the buffer. */
#define RBA_ETHTCP_REORDER_READ_NEXT_OFFSET( BaseAddress )              RBA_ETHTCP_READ_16BITS_AT_POS( BaseAddress, 0U )

/* Read the length of current TCP segment from reorder buffer. Length is stored at byte 2 and 3 of the current-frame meta-data. */
/* Length here refers to only the length of TCP frame (i.e. 4 bytes of meta-data are not considered in the length).*/
#define RBA_ETHTCP_REORDER_READ_CURRENT_LENGTH( BaseAddress )           RBA_ETHTCP_READ_16BITS_AT_POS( BaseAddress, 2U )

/* This is the write functionality correspoding to RBA_ETHTCP_REORDER_READ_NEXT_OFFSET()  */
#define RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET( BaseAddress, Value )      RBA_ETHTCP_WRITE_16BITS_AT_POS( BaseAddress, 0U, Value )

/* This is the write functionality correspoding to RBA_ETHTCP_REORDER_READ_CURRENT_LENGTH()  */
#define RBA_ETHTCP_REORDER_WRITE_CURRENT_LENGTH( BaseAddress, Value )   RBA_ETHTCP_WRITE_16BITS_AT_POS( BaseAddress, 2U, Value )


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

static Std_ReturnType rba_EthTcp_CopyToReorderBuf( const uint8*                         RxData_pcu8,
                                                   uint16                               LenByte_u16,
                                                   uint8*                               ReorderBuf_pu8);
#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_ReorderBufferInit()- This function initializes reorder buffer.                                                   **/
/**                                                                                                                             **/
/** Parameters (in):                None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                   None                                                                                        **/
/*********************************************************************************************************************************/
void rba_EthTcp_ReorderBufferInit(void)
{
    /* Declaration of local variables */
    uint8*      lReorderBuf_pu8;
    uint16      lLoopIdx_u16;

    /* Loop through all reorder buffers and initialize the metadata at byte 0 and byte 1 (PerBuffer metadata) to RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE.             */
    /* This metadata indicates where the first TCP segment in buffer can be found.                                                                                 */
    /* When the buffer has a TCP segment stored the value for this metadata must be within the range from 0 to RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE - 1.           */
    /* Initialaization with RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE is required to know if the buffer has enough free space (further explanation is provided in implementaion while allocating buffer). */
    for(lLoopIdx_u16 = 0; lLoopIdx_u16 < RBA_ETHTCP_REORDER_MAX_BUFCNT; lLoopIdx_u16++)
    {
        /* Initialize with value RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE */
        RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET( rba_EthTcp_ReorderDescriptor_ast[lLoopIdx_u16].ReorderBuf_au8, RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE )

        /* Intialize buffer busy status */
        rba_EthTcp_ReorderDescriptor_ast[lLoopIdx_u16].ReorderBufBusy_b = FALSE;
    }

}

/*********************************************************************************************************************************/
/** rba_EthTcp_ReleaseReorderBuffer()- This function resets buffer to init value and releases it from socket.                   **/
/**                                                                                                                             **/
/** Parameters (in):                None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** DynSockTbl_pst                 - Pointer to TCP dynamic socket table.                                                       **/
/**                                                                                                                             **/
/** Return value:                   None                                                                                        **/
/*********************************************************************************************************************************/
void rba_EthTcp_ReleaseReorderBuffer(rba_EthTcp_DynSockTblType_tst* DynSockTbl_pst)
{
    /* Design decision : Resetting byte 0 and byte 1 of the buffer (PerBufferMetaData) must be done before clearing the busy status(ReorderBufBusy_b).                     */
    /* This is required considering multicore use case.                                                                                                                    */
    /* Consider the case if clearing of the busy status is done before resetting PerBufferMetaData :                                                                       */
    /* - rba_EthTcp_ReleaseReorderBuffer() is called by a closing socket and only first instruction is complete. I.e. clearing of the busy status                          */
    /* - Meantime, rba_EthTcp_HoldOutOfOrderFrame() is called on another socket and is looking for free reorder buffer.                                                    */
    /* - Here there is a risk of allocating above mentioned partially released buffer and will lead to inconsistent data in the buffer.                                    */

    /* Reset PerBufferMetaData */
    RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET( DynSockTbl_pst->ReorderDescriptor_pst->ReorderBuf_au8, RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE )

    /* Reset buffer busy status */
    DynSockTbl_pst->ReorderDescriptor_pst->ReorderBufBusy_b = FALSE;

    /* Release the buffer from socket. */
    DynSockTbl_pst->ReorderDescriptor_pst = NULL_PTR;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_HoldOutOfOrderFrame()- This function holds out of order frame in reorder buffer for later processing             **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** RxData_pcu8                     - Pointer to the start of the Tcp frame                                                     **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/** LenByte_u16                     - Total length of Tcp frame                                                                 **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                   Std_ReturnType                                                                              **/
/**                                   E_OK: Frame was successfully stored ine reorder buffer                                    **/
/**                                   E_NOT_OK: Frame could not be stored because of unavailability of buffer/ enough free space **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
Std_ReturnType rba_EthTcp_HoldOutOfOrderFrame ( const uint8 *           RxData_pcu8,
                                                TcpIp_SocketIdType      TcpDynSockTblIdx_uo,
                                                uint16                  LenByte_u16 )
{
    /* Declaration of local variables */
    Std_ReturnType       lRetVal_en;
    uint16               lLoopIdx_u16;
    uint8*               lReorderBuf_pu8;
    boolean              lBufFound_b;

    /* Initialization of local variables */
    lReorderBuf_pu8    = NULL_PTR;
    lRetVal_en         = E_NOT_OK;
    lBufFound_b        = FALSE;

    /* If the socket has no reorder buffer assigned check if a new buffer can be assigned. */
    if( ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].OutOfOrder_b == TRUE )     &&                           /* Check if the socket has no reorder buffer assigned                         */
       ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst == NULL_PTR ) &&                   /* Check if the socket has no reorder buffer assigned                         */
        ( RBA_ETHTCP_REORDER_CONFIGURED_BUFSIZE >= (LenByte_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN) ) ) /* Check if the configured buffer size is big enough to store the TCP segment */
    {
        /* Loop through all reorder buffers to find out free buffer */
        for(lLoopIdx_u16 = 0; lLoopIdx_u16 <RBA_ETHTCP_REORDER_MAX_BUFCNT; lLoopIdx_u16++)
        {
            /* Design decision : In case of multicore architecture to ensure same buffer is not allocated for more than one socket locking mechanism needs to be implemented. */

            /* Check whether the buffer is free */
            if(rba_EthTcp_ReorderDescriptor_ast[lLoopIdx_u16].ReorderBufBusy_b == FALSE)
            {
                /* Assign the buffer to socket */
                rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst = &rba_EthTcp_ReorderDescriptor_ast[lLoopIdx_u16];

                /* Set the buffer status to busy */
                rba_EthTcp_ReorderDescriptor_ast[lLoopIdx_u16].ReorderBufBusy_b = TRUE;

                /* Terminate further search for buffer*/
                break;
            }
        }
    }

    /* If socket has a buffer allocated, try to copy the received TCP frame to reorder buffer */
    if( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst != NULL_PTR )
    {
       lRetVal_en = rba_EthTcp_CopyToReorderBuf( RxData_pcu8,
                                                 LenByte_u16,
                                                 rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst->ReorderBuf_au8);
    }

    return lRetVal_en;
}

/*********************************************************************************************************************************/
/** rba_EthTcp_FetchOutOfOrderFrame() - This function get a frame which was held for later processing inside the Tcp sector.    **/
/**                                      This function also deletes invalid frames.                                             **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):                                                                                                           **/
/** RxData_ppu8                     - Pointer to the start of the held Tcp frame                                                **/
/** LenByte_pu16                    - Total length of the held Tcp frame                                                        **/
/**                                                                                                                             **/
/** Return value:                   rba_EthTcp_FetchOutOfOrderFrame_ten                                                         **/
/**                                     RBA_ETHTCP_REORDER_NO_ACTION: No action required. This case corresponds to a) when the  **/
/**                                                      socket has no reorder buffer assigned b) No valid frame in the buffer. **/
/**                                     RBA_ETHTCP_REORDER_PROCESSFRAME: Further process the frame as there is a valid          **/
/**                                                                      frame in the reorder buffer                            **/
/**                                     RBA_ETHTCP_REORDER_RELEASEBUF: Release the reorder buffer as the buffer has no more     **/
/**                                                                    frames stored.                                           **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
rba_EthTcp_FetchOutOfOrderFrame_ten  rba_EthTcp_FetchOutOfOrderFrame(   TcpIp_SocketIdType    TcpDynSockTblIdx_uo,
                                                                        uint8                 **RxData_ppu8,
                                                                        uint16                *LenByte_pu16 )
{
    /* Declaration of local variables */
    rba_EthTcp_FetchOutOfOrderFrame_ten     lFetchOutOfOrderFrame_en;    /* Indicates the caller of the API appropriate action */
    rba_EthTcp_SeqVal_ten                   lRcvdSeqNumValidity_en;      /* Validity of the received sequence number           */
    uint32                                  lRcvdSeqNum_u32;             /* Sequence number in the received TCP header         */
    uint16                                  lPreviousBusyRegion_u16;
    uint16                                  lCurrentBusyRegion_u16;
    uint16                                  lNextBusyRegion_u16;
    uint16                                  lPayLoadLen_u16;
    uint16                                  lTcpFrameLen_u16;
    uint8                                   *lReOrderBuf_pu8;
    uint8                                   *lReOrderBufMetadataCurrentFrame_pu8;  /* Points to perSegment metadata in the reorder buffer  */
    uint8                                   *lReOrderBufTcpHdrCurrentFrame_pu8;    /* Points to TCP header in the reorder buffer           */

    /* Traversing through stored TCP frames in reorder buffer:
     * - Each stored TCP frame is traversed in the form of single linked list, each frames sequence number is verified and if it is valid it will be delinked and processed further.
     *
     * - The function makes use of three offsets
     *  a) lPreviousBusyRegion_u16 : This starts with byte 0 i.e. PerBuffer metadata
     *  b) lCurrentBusyRegion_u16  : This starts with contents of PerBuffer metadata
     *  c) lNextBusyRegion_u16     : Contents at offset 2 and 3 of lCurrentBusyRegion_u16
     */

    /* Proceed further only if the socket has reorder buffer allocated */
    if( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst != NULL_PTR )
    {
        /* Initialize local pointer to reorder buffer address */
        lReOrderBuf_pu8 = rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].ReorderDescriptor_pst->ReorderBuf_au8;

        /* 'Previous busy region' is made to point to PerBuffer metadata (byte 0 and 1) */
        lPreviousBusyRegion_u16   = 0U;

        /* Get the offset of first busy region stored in PerBuffer metadata (byte 0 and 1) */
        lCurrentBusyRegion_u16    = RBA_ETHTCP_REORDER_READ_NEXT_OFFSET(lReOrderBuf_pu8);

        /* If there is no more stored TCP segments in the buffer release the buffer from the socket */
        if(lCurrentBusyRegion_u16 == RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE)
        {
            /* Design decision: Release of the buffer from the socket is done after all stored frames are given UL_RxIndication() . */
            /* i.e. Release of the buffer from the socket is not done while processing last stored segment (before calling UL_RxIndication() )*/
            /* With this it is ensured that in case of multicore due to interruption there will not be more than socket owning same reorder buffer. */

            /* Set return value of the function so that rba_EthTcp_RxIndication() will release the buffer */
            lFetchOutOfOrderFrame_en = RBA_ETHTCP_REORDER_RELEASEBUF;
        }
        else
        {   /* If there are some stored TCP segments in the buffer */

            /* Loop through all stored TCP segments until valid TCP frame */
            do{
                /* Initialize the pointer to metadata of the current frame */
                lReOrderBufMetadataCurrentFrame_pu8 = lReOrderBuf_pu8 + lCurrentBusyRegion_u16;

                /* Initialize the pointer to TCP header of the current frame (i.e. immediately after metadata) */
                lReOrderBufTcpHdrCurrentFrame_pu8   = lReOrderBuf_pu8 + lCurrentBusyRegion_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN;

                /* Fetch the sequence number */
                lRcvdSeqNum_u32 = RBA_ETHTCP_READ_32BITS_AT_POS( lReOrderBufTcpHdrCurrentFrame_pu8,
                                                                 (uint8)RBA_ETHTCP_POS_SEQNUM_E );

                /* Fetch the length of TCP frame (TCP header + Payload) */
                lTcpFrameLen_u16 = RBA_ETHTCP_REORDER_READ_CURRENT_LENGTH( lReOrderBufMetadataCurrentFrame_pu8 );

                /* Get payload length */
                lPayLoadLen_u16 = rba_EthTcp_GetPayloadLenFromFrame( lReOrderBufTcpHdrCurrentFrame_pu8,
                                                                     lTcpFrameLen_u16);

                /* Check sequence number validity as per RFC793 p69 */
                lRcvdSeqNumValidity_en = rba_EthTcp_RcvdSeqNumValidity(   TcpDynSockTblIdx_uo,
                                                                       lRcvdSeqNum_u32,
                                                                       lPayLoadLen_u16 );

                /* Design decision: Check for invalid frame.
                 * Stored frame can become INVALID if
                 * a) Duplicate frames are stored in reorder buffer
                 * b) The current received frame (frame from IPvX_RxIndication) completely covers the sequence number of the stored frame.
                 * (Note: This use case here is different than the duplicate frame. In this use case the newly received frame includes missing sequence number as well as sequence number of frame in reorder buffer.) */

                /* If the sequence number is valid/ Special allowance/ Invalid delink the frame */
                if( ( lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_VALID_E ) ||
                    ( lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_SPECIAL_ALLOWANCE_E ) ||
                    ( lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_INVALID_E ) )
                {
                    /* Copy the information about next frame offset from the current frame and place it in the previous frame */
                    lNextBusyRegion_u16 =  RBA_ETHTCP_REORDER_READ_NEXT_OFFSET( lReOrderBufMetadataCurrentFrame_pu8 );

                    RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET(lReOrderBuf_pu8 + lPreviousBusyRegion_u16, lNextBusyRegion_u16)
                }
                else
                {    /* This block corresponds to lRcvdSeqNumValidity_en = RBA_ETHTCP_SEQ_OUT_OF_ORDER_E */

                    /* Following usecases need to be considered here                                                                                                */
                    /* case 1: Sequence number is VALID or SPECIAL_ALLOWANCE                                                                                        */
                    /*         In this case do{}while() loop is terminated and variable lPreviousBusyRegion_u16 carries NO SIGNIFICANCE                             */
                    /* case 2: Sequence number is OUT_OF_ORDER                                                                                                      */
                    /*         In this case do{}while() loop is continued and the offset of previous busy region (lPreviousBusyRegion_u16) MUST BE updated.         */
                    /* case 3: Sequence number is INVALID and is the last frame in the reorder buffer                                                               */
                    /*         In this case do{}while() loop is terminated and variable lPreviousBusyRegion_u16 carries NO SIGNIFICANCE                             */
                    /* case 4: Sequence number is INVALID and there are further frames to be traversed                                                              */
                    /*         In this case do{}while() loop is continued and but the offset of previous busy region (lPreviousBusyRegion_u16) MUST NOT BE updated. */
                    /*         This is beacuse current busy region is delinked from buffer as it was invalid.                                                       */
                    /*         (Updating lPreviousBusyRegion_u16 makes it to get offest of current busy region which is invalid)                                    */

                    /* Update previous busy region with the value of current busy region offset. I.e. move the previous busy region to next node in the linked list. */
                    lPreviousBusyRegion_u16  = lCurrentBusyRegion_u16;
                }

                /* Traverse the next stored frame in the linked list manner. I.e. move the current busy region to next node in the linked list.  */
                lCurrentBusyRegion_u16   = RBA_ETHTCP_REORDER_READ_NEXT_OFFSET( lReOrderBufMetadataCurrentFrame_pu8 );

            }while( (lCurrentBusyRegion_u16 != RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE) &&  /* If traversing of all the stored frames are not complete */
                    (lRcvdSeqNumValidity_en != RBA_ETHTCP_SEQ_VALID_E) &&                            /* Current frame in reorder buffer is not the next expected frame (neither VALID nor SPECIAL_ALLOWANCE)*/
                    (lRcvdSeqNumValidity_en != RBA_ETHTCP_SEQ_SPECIAL_ALLOWANCE_E) );


            /* Update output parameters of the function */
            if( (lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_VALID_E) ||
                (lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_SPECIAL_ALLOWANCE_E) )
            {
                /* If current frame in the reorder buffer is next expected frame (either VALID or SPECIAL_ALLOWANCE) */
                /* Update out parameters and set the return value to indicate the frame to be processed */
                *RxData_ppu8             = lReOrderBufTcpHdrCurrentFrame_pu8;
                *LenByte_pu16            = lTcpFrameLen_u16;
                lFetchOutOfOrderFrame_en = RBA_ETHTCP_REORDER_PROCESSFRAME;
            }
            else if( (lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_INVALID_E) &&
                     ( ( RBA_ETHTCP_REORDER_READ_NEXT_OFFSET(lReOrderBuf_pu8) ) == RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE) )
            {
                /* If the stored frames in the buffer turns to be INVALID and buffer has no more stored frames */
                /* set the return value to indicate the reorder buffer to be leased from the socket. */
                lFetchOutOfOrderFrame_en = RBA_ETHTCP_REORDER_RELEASEBUF;
            }
            else /* if( ( ( lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_INVALID_E ) && ( RBA_ETHTCP_REORDER_READ_NEXT_OFFSET(lReOrderBuf_pu8) != RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE ) ) || */
                 /*     ( lRcvdSeqNumValidity_en == RBA_ETHTCP_SEQ_OUT_OF_ORDER_E ) ) */
            {
                /* If the buffer has some stored frames but no VALID frames at the moment */
                /* set the return value to indicate the no action required. */
                lFetchOutOfOrderFrame_en = RBA_ETHTCP_REORDER_NOACTION_REQUIRED;
            }
        }
    }
    else
    {
        /* If the socket has no reorder buffer assigned set the return value to indicate the no action required. */
        lFetchOutOfOrderFrame_en = RBA_ETHTCP_REORDER_NOACTION_REQUIRED;
    }

    return lFetchOutOfOrderFrame_en;
}

/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */
/*********************************************************************************************************************************/
/** rba_EthTcp_CopyToReorderBuf()   - This function copies the TCP segment to reorder buffer if there are sufficient free space.**/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** RxData_pcu8                     - Pointer to the start of the Tcp frame                                                     **/
/** LenByte_u16                     - Total length of Tcp frame                                                                 **/
/**                                                                                                                             **/
/** Parameters (inout):                                                                                                         **/
/** ReorderBuf_pu8                  - Pointer to the beginning of the reorder buffer                                            **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                   Std_ReturnType                                                                              **/
/**                                     E_OK: Frame was successfully stored in reorder buffer                                  **/
/**                                     E_NOT_OK: Frame could not be stored because of inavailabilty of enough free space      **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static Std_ReturnType rba_EthTcp_CopyToReorderBuf( const uint8*                         RxData_pcu8,
                                                   uint16                               LenByte_u16,
                                                   uint8*                               ReorderBuf_pu8)
{
    /* Declaration of local variables */
    Std_ReturnType      lRetVal_en;
    uint16              lPossibleFreeRegion_u16;
    uint16              lNextBusyRegion_u16;
    uint16              lCurrentBusyRegion_u16;
    uint16              lLenCurrentBusyRegion_u16;   /* Length of the busy region and it does not include RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN */
    uint16              lLoopIdx_u16;

    /* Initialization of local variables */
    lCurrentBusyRegion_u16   = 0U;
    lNextBusyRegion_u16      = RBA_ETHTCP_REORDER_READ_NEXT_OFFSET(ReorderBuf_pu8);
    lPossibleFreeRegion_u16  = RBA_ETHTCP_PERBUFFER_REORDER_METADATALEN;
    lRetVal_en               = E_NOT_OK;

    /* Finding free region to copy the frame:
     * - At any instant of time, reorder buffer can contain scattered TCP segments (busy region) with some free regions in between. Even there could be some free space in the beginning of the buffer.
     *
     * - Each busy region is traversed in the form of linked list to check if there is sufficient free region is available.
     *
     * - The function makes use of three offsets to establish links while storing new TCP frame in the buffer
     *  a) lCurrentBusyRegion_u16: This starts with byte 0 i.e. PerBuffer metadata
     *  b) lNextBusyRegion_u16   : This starts with contents of PerBuffer metadata
     *  c) lPossibleFreeRegion_u16       : This starts with byte position 2 (RBA_ETHTCP_PERBUFFER_REORDER_METADATALEN) as it is first possible free region in the buffer
     *
     *  - The assumed possible free region (lPossibleFreeRegion_u16) is validated to check if it has enough free space to accommodate new frame.
     *    This is achieved by checking the difference between lNextBusyRegion_u16 and lPossibleFreeRegion_u16
     *
     *  - As check in linked list progress, lNextBusyRegion_u16 becomes lCurrentBusyRegion_u16 and contents at offset 2 and 3 of lNextBusyRegion_u16 becomes lNextBusyRegion_u16
     *
     *  - Initialization of PerBuffer metadata with RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE also serves the purpose of checking enough free space incase new frame is stored after the last frame in the buffer.
     *   ( When the concept of linked list is followed, the value for the Part2 of last metadata in the buffer will always be RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE, which was eventually copied from PerBuffer metadata.)
     *
     *  Following scenarios are possible:
     *  Use case 1: Buffer is completely free.
     *  Use case 2: Buffer has scattered TCP segments with enough free region immediately after byte 0 and byte 1 (PerBuffer meta metadata)
     *  Use case 3: Buffer has scattered TCP segments with enough free region available between the two TCP segments.
     *  Use case 4: Buffer has scattered TCP segments OR continuously occupied TCP segments but enough free space available only at the end
     *  Use case 5: Not enough continuous free space available  */

    /* Loop through the free regions of the buffer until sufficient continuous free space is available. */
    /* To search for free region */
    while( ((lNextBusyRegion_u16 - lPossibleFreeRegion_u16) < (LenByte_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN ) ) /* Check if there is enough free region                        */
            && (lNextBusyRegion_u16 != RBA_ETHTCP_REORDER_ALLOCATED_BUFSIZE) )                                            /* Check if there is no more stored TCP segments in the buffer */
    {

        /* 'Next busy region' becomes 'current busy region'  */
        lCurrentBusyRegion_u16  = lNextBusyRegion_u16;

        /* Get the offset of 'next busy region' */
        lNextBusyRegion_u16     = RBA_ETHTCP_REORDER_READ_NEXT_OFFSET( ReorderBuf_pu8 + lCurrentBusyRegion_u16);

        /* Get the length of current busy region */
        lLenCurrentBusyRegion_u16     = RBA_ETHTCP_REORDER_READ_CURRENT_LENGTH( ReorderBuf_pu8 + lCurrentBusyRegion_u16);

        /* Offset where current busy region ends = 'Current busy region' offset + 'Current region length' + metadata length of the 'current busy region'  */
        /* lLenCurrentBusyRegion_u16 is only the length of stored TCP segment and meta dats is not included */
        lPossibleFreeRegion_u16 = lCurrentBusyRegion_u16 + lLenCurrentBusyRegion_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN;

    }

    /* Is there are enough free region after/before the current busy region to place a new frame  */
    /* New frame can be placed before current busy region in a case where Start_BusyRegionOffset_u16 is not holding value 0 */
    if( (lNextBusyRegion_u16 - lPossibleFreeRegion_u16) >= (LenByte_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN )  )
    {
        /* Update linked list property : Link from current busy region to new busy region*/
        RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET( ReorderBuf_pu8 + lCurrentBusyRegion_u16, lPossibleFreeRegion_u16 )

        /* Update linked list property : Link from new busy region to next busy region */
        RBA_ETHTCP_REORDER_WRITE_NEXT_OFFSET( ReorderBuf_pu8 + lPossibleFreeRegion_u16, lNextBusyRegion_u16 )

        /* Update linked list property : New busy region length */
        RBA_ETHTCP_REORDER_WRITE_CURRENT_LENGTH( (ReorderBuf_pu8 + lPossibleFreeRegion_u16), LenByte_u16  )

        /* Copy the TCP segment after newly created metadata */
        ReorderBuf_pu8 += (lPossibleFreeRegion_u16 + RBA_ETHTCP_PERSEGMENT_REORDER_METADATALEN);

        /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. Low Risk */
        (void)rba_BswSrv_MemCopy( ReorderBuf_pu8,
                                  RxData_pcu8,
                                  LenByte_u16 );

        lRetVal_en = E_OK;
    }

    return lRetVal_en;
}

#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* (RBA_ETHTCP_REORDER_ENABLED == STD_ON) */

#endif /* ( TCPIP_TCP_ENABLED == STD_ON ) */
#endif /* TCPIP_CONFIGURED */

/*<BASDKey>
**********************************************************************************************************************
* $History___:$
**********************************************************************************************************************
</BASDKey>*/




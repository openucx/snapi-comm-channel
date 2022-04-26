#ifndef __SNAPI_MQ_H__
#define __SNAPI_MQ_H__

/**
  * @defgroup SNAPI_DT Library data-structures
  * @{
  * Library data-structures
  * @}
  *
  */

/**
  * @defgroup SNAPI_API SNAPI API and interfaces
  * @{
  * Library API and interfaces
  * @}
  *
  */

#define SNAPI_MQ_SERVICE_NAME_MAX 128

/**
 * @ingroup SNAPI_DT
 */
typedef enum {
    SNAPI_MQ_OK                 =  0, //!< Operation finished successfully
    SNAPI_MQ_ERR                = -1, //!< Operation failed to complete
} snapi_mq_status_t;

/**
 * @ingroup SNAPI_DT
 */
struct snapi_device; //!< TBD

/**
 * @ingroup SNAPI_DT
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_init_attr defines the parameters of the MQ for creation
 *
 * @endparblock
 */
struct snapi_mq_init_attr {
    struct snapi_device *device;     //!< device to open MQ on
    uint32_t            flags;       //!< enum snapi_mq_init_flags
    uint32_t            msg_size;    //!< Size of a message in bytes
    uint32_t            queue_size;  //!< Maximum number of outstanding messages in the MQ
};

/**
 * @ingroup SNAPI_DT
 */
typedef struct mq_addr snapi_mq_addr_t;

/**
 * @ingroup SNAPI_DT
 */
enum snapi_mq_init_flags; //!< TBD

/**
 * @ingroup SNAPI_DT
 */
enum snapi_mq_msg_flags; //!< TBD

/**
 * @ingroup SNAPI_DT
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_h is a handle to a message queue. MQ's are created by calling
 * @ref snapi_mq_create. After creation, an MQ can be connected to a peer MQ
 * by using @ref snapi_mq_listen or @ref snapi_mq_connect for server/client MQ's
 * respectively. Connected MQ's can pass messages by using @ref snapi_mq_send
 * and @ref snapi_mq_recv. MQ's must be closed by using @ref snapi_mq_close
 * when the communication is done.
 *
 * @endparblock
 */
typedef struct snapi_mq snapi_mq_h;

/**
 * @ingroup SNAPI_API
 *
 * @brief Creates the message queue.
 *
 *
 * @param [in]   attr  Init attributes for the created MQ
 * @param [out]  mq    Message queue handle
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_create is a nonblocking operation which creates the MQ. The MQ
 * creates all resources required for communication. When an MQ is created,
 * it is not connected to any peer MQ. For communication, connection to a peer MQ
 * is required first.
 *
 * @endparblock
 *
 * @return Error code:
 *              - @ref SNAPI_MQ_OK  - MQ created successfully
 *              - @ref SNAPI_MQ_ERR - failed to initialize resources
 */
snapi_mq_status_t snapi_mq_create(struct snapi_mq_init_attr *attr, snapi_mq_h **mq);

/**
 * @ingroup SNAPI_API
 *
 * @brief Register this MQ as a service and start listening for client messages.
 *
 * @param [in]  mq            Message queue handle
 * @param [in]  service_name  Name of the service this MQ represents,
 *                            SNAPI_MQ_SERVICE_NAME_MAX bytes size null-terminated string
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_listen Registers the MQ as a service MQ, allowing clients to send
 * messages. Connections occur asynchronously. New connections can first be detected
 * when receiving a message from the remote peer. Peers can be rejected using
 * @ref snapi_mq_disconnect_peer.
 *
 * @endparblock

 * @return Error code:
 *              - @ref SNAPI_MQ_OK - MQ successfully registered and is ready to receive messages
 *              - @ref SNAPI_MQ_ERR - Failed to register the MQ
 */
snapi_mq_status_t snapi_mq_listen(snapi_mq_h *mq, const char *service_name);

/**
 * @ingroup SNAPI_API
 *
 * @brief Connects to the service's MQ on the server.
 *
 * @param [in]   mq            Message queue handle
 * @param [in]   service_name  Name of the service to connect to,
 *                             SNAPI_MQ_SERVICE_NAME_MAX bytes size null-terminated string
 * @param [out]  peer_addr     Handle of the remote peer
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_connect interface is used by the client to send a connection
 * request to the server. The service name identifies the server. The calling
 * thread is blocked until the connection is either established or an error
 * occurs.
 *
 * @endparblock

 * @return Error code:
 *              - @ref SNAPI_MQ_OK - MQ successfully connected to server
 *              - @ref SNAPI_MQ_ERR - Failed to connect
 */
snapi_mq_status_t snapi_mq_connect(snapi_mq_h *mq, char *service_name, snapi_mq_addr_t **peer_addr);

/**
 * @ingroup SNAPI_API
 *
 * @brief Sends the message to the peer MQ
 *
 * @param [in]  mq        Message queue handle
 * @param [in]  msg       Message to send
 * @param [in]  msg_size  Size of message
 * @param [in]  flags     enum snapi_mq_msg_flags
 * @param [in]  peer_addr Handle of the remote peer
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_send is a nonblocking operation that sends the data in the
 * data buffer from to the peer MQ. On calling the operation,
 * the message is posted and the call returns immediately.
 *
 * @endparblock
 *
 * @return Error code:
 *              - @ref SNAPI_MQ_OK - Send request registered
 *              - @ref SNAPI_MQ_ERR - Failed to send the message
 */
snapi_mq_status_t snapi_mq_send(snapi_mq_h *mq, const void *msg, size_t msg_size, int flags, const snapi_mq_addr_t *peer_addr);

/**
 * @ingroup SNAPI_API
 *
 * @brief Receive a message from the peer MQ.
 *
 * @param [in]      mq         Message queue handle
 * @param [in]      flags      enum snapi_mq_msg_flags
 * @param [in/out]  msg        Buffer to place message in
 * @param [in/out]  msg_size   Size of buffer / Received message size
 * @param [out]     peer_addr  Handle of the remote peer
 *
 * @b Description
 *
 * @ref snapi_mq_recv is a nonblocking operation that checks for messages received
 * from the peer MQ. On calling the operation, if there are new messages waiting
 * in the MQ, the oldest one will be put in the user supplied buffer.
 *
 * @endparblock
 *
 * @return Error code:
 *              - @ref SNAPI_MQ_OK - Message received into buffer
 *              - @ref SNAPI_MQ_ERR - No messages in MQ
 */
snapi_mq_status_t snapi_mq_recv(snapi_mq_h *mq, int flags, void *msg, size_t *msg_size, snapi_mq_addr_t **peer_addr);

/**
 * @ingroup SNAPI_API
 *
 * @brief Disconnect the MQ from the remote peer
 *
 * @param [in] mq         Message queue handle
 * @param [in] peer_addr  Handle of peer to disconnect
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_disconnect_peer routine disconnects the remote peer from the service MQ.
 * blocks until all resources related to the remote peer are freed.
 *
 * @endparblock
 *
 * @return Error code:
 *              - @ref SNAPI_MQ_OK - remote peer disconnected successfully
 *              - @ref SNAPI_MQ_ERR - Failed to disconnect peer
 *
 */
snapi_mq_status_t snapi_mq_disconnect_peer(snapi_mq_h *mq, snapi_mq_addr_t *peer_addr);

/**
 * @ingroup SNAPI_API
 *
 * @brief Close MQ
 *
 * @param [in] mq  Message queue handle
 *
 * @parblock
 *
 * @b Description
 *
 * @ref snapi_mq_close routine waits for all outstanding operations to complete
 * and then destroys the MQ.
 *
 * @endparblock
 *
 */
snapi_mq_status_t snapi_mq_close(snapi_mq_h *mq);

#endif

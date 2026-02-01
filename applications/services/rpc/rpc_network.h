#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Connect to a remote host via the network proxy.
 *
 * Requires an active RPC session (Flipper app connected via BLE).
 * The host app handles DNS resolution and socket management.
 *
 * @param host          Hostname or IP address (max 255 chars)
 * @param port          Port number (1-65535)
 * @param is_udp        true for UDP, false for TCP
 * @param timeout_ms    Connection timeout in milliseconds (0 = default 30s)
 * @param resolved_ip_out   Buffer to receive resolved IP (can be NULL)
 * @param resolved_ip_size  Size of resolved_ip_out buffer
 * @return              Connection ID on success (>= 0), -1 on failure
 */
int32_t rpc_network_connect(
    const char* host,
    uint16_t port,
    bool is_udp,
    uint32_t timeout_ms,
    char* resolved_ip_out,
    size_t resolved_ip_size);

/**
 * @brief Send data on an established connection.
 *
 * Data larger than 512 bytes will be automatically chunked.
 *
 * @param connection_id Connection ID from rpc_network_connect()
 * @param data          Data buffer to send
 * @param size          Number of bytes to send
 * @return              Number of bytes sent on success, -1 on failure
 */
int32_t rpc_network_send(int32_t connection_id, const uint8_t* data, size_t size);

/**
 * @brief Receive data from a connection.
 *
 * @param connection_id Connection ID from rpc_network_connect()
 * @param buffer        Buffer to receive data
 * @param buffer_size   Size of receive buffer
 * @param timeout_ms    Receive timeout in ms (0 = wait forever)
 * @return              Number of bytes received (>0), 0 on timeout, -1 on error/disconnect
 */
int32_t rpc_network_receive(
    int32_t connection_id,
    uint8_t* buffer,
    size_t buffer_size,
    uint32_t timeout_ms);

/**
 * @brief Close a network connection.
 *
 * @param connection_id Connection ID to close
 */
void rpc_network_close(int32_t connection_id);

/**
 * @brief Check if network proxy is available.
 *
 * Returns true if an RPC session is active (phone connected with Flipper app).
 *
 * @return true if network is available, false otherwise
 */
bool rpc_network_is_available(void);

#ifdef __cplusplus
}
#endif

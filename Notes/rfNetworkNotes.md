# [The RF24Network Layer (RF24Network)](https://nrf24.github.io/RF24Network/index.html)

- Implements OSI Network Layer using the nRF24l01+ radios that are driven by the [RF24 Library](https://nrf24.github.io/RF24/).
- Created as an alternative to ZigBee radios for Arduino communication. Focused on enhancing functionality for maximum efficiency, reliability, and speed.

Features:

- Network ACKs
- Upades the addressing standard
- Optimizations to the core RF24 library
- Built in sleep mode
- Host addressing where each node has a logical address on the local network
- Message forwarding
- Ad-hoc joining

It does not:

- Provide dynamic address assignment (see [RF24Mesh](https://github.com/nRF24/RF24Mesh))
- Layer 4 protocols (TCP/IP - See [RF24Ethernet](https://github.com/nRF24/RF24Ethernet) and [RF24Gateway](https://github.com/nRF24/RF24Gateway))

## [Addressing Format: Understanding Addressing and Topology](https://nrf24.github.io/RF24Network/md_docs_2addressing.html)

### Overview

- nrf24 radio use 40-bit address format
- 5 bytes of storage space per address
- Limited to direct communication with 6 other nodes while using ESB
- RF24Network uses data compression to store addresses in 2 bytes

### Decimal, Octal and Binary formats

Say we want to designate a logical address to a node, using a tree topology as defined by the manufacturer. In the simplest format, we could assign the first node the address of 1, the second 2, and so on. Since a single node can only connect to 6 other nodes (1 parent and 5 children) subnets need to be created if using more than 6 nodes. In this case, the

- children of node 1 could simply be designated as 11, 21, 31, 41, and 51
- children of node 2 could be designated as 12, 22, 32, 42, and 52

The above example is exactly how RF24Network manages the addresses, but they are represented in Octal format.

![RF24Network Topology](https://github.com/nRF24/RF24Network/raw/master/images/topologyImage.jpg)

### Understanding Radio Communication and Topology

- Radios are linked by addresses assigned to pipes
- Each radio can listen to 6 addresses on 6 pipes [1 parent pipe, 4-5 child pipes]
- Tree Structured
- Direct communication between parent and children nodes
- Any other traffic to or from a node must be routed through the network

### Topology of RF24Network

- Similar to IP addressing, with the master being the gateway with a default of 5 connected nodes
- Each node creates a subnet below it with up to 4 additional child nodes _(Nodes can have 5 children if multicast is disabled)_

**Expressing RF24Network Addresses in IP format**

Master Node: 10.10.10.10

- The children nodes of the master would be 10.10.10.1, 10.10.10.2, 10.10.10.3, 10.10.10.4 and 10.10.10.5
- The children nodes of 10.10.10.1 would be 10.10.1.1, 10.10.2.1, 10.10.3.1, 10.10.4.1 and 10.10.5.1

In RF24Network, the master: 00

- Children of master are 01, 02, 03, 04, 05
- Children of 01 are 011, 021, 031, 041, 051

### Multicast

- Enabled by default
- Limits master node to 5 child pipes and other nodes to 4 children
- Arranges node in level with master being level 0, nodes 01 - 05 are level 1, n1-n5 are level 2, etc.
- Level of each node is user configured as desired
- Can be disabled by editing _RF24Network_config.h_

**Example:**
If all nodes are in range of the master node, all nodes can be configured to use multicast level 1, allowing the master node to contact all of them by sending a single payload.

### Routing

- Handled by the network layer
- If address are assigned in accordance with the physical layout of the network, nodes will route traffic automatically
- Individual nodes only route individual fragments, so unless the node itself is receiving the fragmented payloads it does not need it enabled
- when routing data directly between parent and child, network will use built-in ack and retry functions to prevent data loss
- Routing managed using chip built ACK requests and software driven network ACKS.
- Only final node confirms delivery and sends back ACK

**Example:** Node 00 sends to node 01. The nodes use built in auto-retry and auto-ack functions.

**Example:** Node 00 sends to node 011. Node 00 will send to node 01 as before. Node 01 will forward the message to 011. If delivery was successful, node 01 will also forward a message back to node 00, indicating success.

_Note: When retrying failed payloads that have been routed, there is a chance of duplicate payloads if the network-ack is not successful. In this case, it is left up to the user to manage retries and filtereing of duplicate payloads._

Applications can and shold be managed by the application or user. If requresting a response from another node, aan acknowledgement is not required, so a user defined type of 0-64 should be used, to prevent the network from responding with an acknowledgement. If not requesting a response, and wanting to know if the payload was successful or not, users can utilize header types 65-127.

## [Performance and Data Loss: Tuning the Network](https://nrf24.github.io/RF24Network/md_docs_2tuning.html)

### Tuning Overview

- RF24 are half duplex devices with built in auto-retry functions to prevent loss of data
- Values are adjusted automatically by the library on startup but can be further adjusted to reduce data loss and increase throughput of network.

**Auto-Retry Timing**

- The core radio library provides the functionality of adjusting the interal auto-retry interval of radio modules.
- In network configuration radios can be set automatically rety failed transmissions at intervals of 0.5 ms to 4ms
- When operating more than 2 radios, stagger the assigned intervals to preven radios from interferring with each other at the RF layer

The library should provide fairly good working values, as it simply staggers the assigned values within groups of radios in direct communication. This value can be set manually by calling `radio.setRetries(X, 15);` and adjusting the value of X from 1 to 15 (steps of 250us).

**Auto-Retry Count and Extended Timeouts**

- The core radio library also allows adjustment of the internal auto-retry count of the RF24 modules
- Default setting is 15 retries per payload, extended by configuring `network.txTimeout` variable
- Should be left at 15, An interval/retry setting of (15,15) will provide 15 retries at intervals of 4ms, taking up to 60ms per payload
- Staggered timeout periods by default but can be adjusted on per node basis

_Note: The txTimeout variable is used to extend the retry count to a defined duration in milliseconds. See the network.txTimeout variable. Timeout periods of extended duration (500+) will generally not help when payloads are failing due to data collisions, it will only extend the duration of the errors. Extended duration timeouts should generally only be configured on leaf nodes that do not receive data._

### Scenarios

**Example 1:** Network with master node and three leaf nodes that send data to the master node. None of the leaf nodes need to receive data.

1. Master node uses default configuration
2. Leaf nodes ca be configured with extended timeout periods to ensure reception by the master
3. The following configuration will provide a reduction in errors, as the timeouts have been extended and are staggered between devices.

```
Leaf 01: network.txTimeout = 500;
Leaf 02: network.txTimeout = 573;
Leaf 03: network.txTimeout = 653;
```

**Example 2:** Network with master node and three leaf nodes that send data to the master node. The second leaf node needs to receive configuration data from the master at set intervals of 1 second, and send data back to the master node. The other leaf nodes will send basic sensor information every few seconds, and a few dropped payloads will not affect the operation greatly.

1. Master node configured with extended timeouts at 0.5 seconds, and increased retry delay:

```
radio.setRetries(11, 15);
network.txTimeout = 500;
```

2. Second leaf node configured with a similar timeout period and retry delay:

```
radio.setRetries(8, 15);
network.txTimeout = 553;
```

3. First and third leaf nodes configured with default timeout periods or slightly increased timeout periods.

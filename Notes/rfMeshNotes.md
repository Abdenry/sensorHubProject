# [Mesh Networking for RF24 (RF24Mesh)](https://nrf24.github.io/RF24Mesh/md_docs_2general__usage.html)

- Automates the networking process of nrf24 modules
- Assigns nodes a unique number ranging from 1 to 255 (user defined) to act as a static IP
- Addressing, routing, etc. is handled by the library
- Master node keeps track of unique IDs and assigned RF24Network addresses
- Moved or disconnected nodes can automically rejoin the mesh and reconfigure within network
- Uses functionality from RF24 and RF24Network libraries

## [General Usage](https://nrf24.github.io/RF24Mesh/md_docs_2general__usage.html)

### Network Design Options

1. Static Network (no mesh)

   - RF24Network manually configured with static design. RF24Mesh is not used.

2. Static network w/Dynamic Assignment

   - RF24Mesh is only used to acquire an address on startup
   - Nodes are expected to remain stationary
   - Changes to network addressed manually by adding, removing, or resetting nodes
   - Can use the RF24Network functions directly or use RF24Mesh

3. Dynamic Network & Assignment

   - Nodes join the mesh automatically and re-attach as required.
   - Default setting (used for examples)

4. Hybrid Network

   - Utilizes a combination of static & dynamic nodes
   - Requires initlal planning and deployment
   - Can be more stable network easing the use of sleeping nodes

### Network Management

- RF24Network addresses can be viewed as MAC addresses
- RF24Mesh NodeIDs viewed as static IP addresses
- When joining or re-joining network nodes will request a RF24Network address, and are identified via nodeID

#### Raspberry Pi/Linux

- RF24Gateway saves address assignments to file `dhcplist.txt` for restoration even if gateway is restarted
- To force network re-convergence, delete `dhcplist.txt` and restart gateway
- If nodes are configured t overify their connection at set intervals they will come back online in time

#### Arduino/AVR

- Address list is not saved
- To force network re-convergence, restart gateway.
- If nodes are configured t overify their connection at set intervals they will come back online in time
- if node/nodeID is removed permanently, the address hsould be releaser **prior** to removal

### Mesh Communication

- RF24Mesh nodeIDs are unique identifiers
- RF24Network addresses change dynamically within statically defined structure
  - All nodes know the master address (00)
  - The master node maintains a list of all nodes (immediate address lookups)
- Node to Node communication requires address queries to be sent to master node
  - Inheritently inefficient

### Tricks of Trade

- For nodes constantly transmitting (every few seconds at most) it is suitable to check the connection, and/or renew the address when conenctivity fails. Data is not saved by the master node, if the master node goes down, all child nodes must renew their address. In this case, as long as the master node is down for a few seconds, the nodes will all begin requesting an address.
- Nodes not actively transmitting, should be configured to test their connection at predefined intervals to allow them to reconnect as necessary
- Sleeping nodes (nodes only temporarily online) should release their address prior to going offline and request a new one upon waking. _(Note: This can take 10 - 15 ms, up to a few seconds)_
- When transmitting payloads without the network returning a ack-response, nodes should be configured to periodically check connection using `RF24Mesh::checkConnection()`

### RF24Network

Beyond requesting and releasing addresses, usage is outlined in the [RF24Mesh class documentation](https://nrf24.github.io/RF24Mesh/annotated.html), and further information regarding RF24Network is available at [RF24Network documentation](https://nrf24.github.io/RF24Network/index.html).

## [Setup and Config](https://nrf24.github.io/RF24Mesh/md_docs_2setup__config.html)

# Task 1 – Distributed Key–Value Database (C++)

## Objective
The objective of this task is to design and implement a distributed key–value database
that runs on three server nodes and behaves as a single logical database. The system
should allow a client to connect to any server and perform basic database operations,
while ensuring that write operations are replicated across all servers.

---

## System Overview
The system consists of:
- Three independent server nodes
- One client program
- Each server maintains a local in-memory key–value store
- Write operations are replicated to all other servers

The system provides **eventual consistency**, meaning all servers eventually reach
the same state after replication completes.

---

## Supported Commands
The client can issue the following commands to **any server**:

```
SET 
GET 
DELETE
```


### Example

```
SET name Apurv
GET name
DELETE name
```

---

## Replication Mechanism
When a server receives a **SET** or **DELETE** command from a client:

1. The server updates its local key–value store.
2. It sends a `SYNC` message to the other servers.
3. Peer servers apply the update locally.
4. `SYNC` messages are not re-replicated, preventing infinite replication loops.

Read operations (**GET**) are handled locally by the server.

---

## Multithreading and Synchronization
- Each client connection is handled in a separate thread using `std::thread`.
- A `std::mutex` is used to protect access to the shared key–value store.
- This ensures thread safety when multiple clients interact with the server
  concurrently.

---

## How to Run

### Compile the Code
```
g++ server1.cpp -o s1 -pthread
g++ server2.cpp -o s2 -pthread
g++ server3.cpp -o s3 -pthread
g++ client.cpp -o client
```

### Start the Servers
Run each server in a **separate terminal**:

```
./s1 # Port 6100
./s2 # Port 6101
./s3 # Port 6102
```

### Run Client Commands

```
./client 6100 SET name Apurv
./client 6101 GET name
./client 6102 DELETE name
```

### Example

<img width="1230" height="169" alt="image" src="https://github.com/user-attachments/assets/9982b217-ddf2-4e68-b3e7-5a124cc7b043" />

## Learning Outcome
Through this task, I learned:
- Basics of distributed systems
- TCP socket programming in C++
- Multithreading using `std::thread`
- Synchronization using `std::mutex`
- Replication of data across multiple server nodes

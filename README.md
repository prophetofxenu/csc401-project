# csc401-project

P2P system with central index for distributing RFCs, written in C++

## Build

Requirements
* g++
* make
* Crypto++ library

1. Clone repository
2. Clients will attempt to connect to the CI at 127.0.0.1:8080. If you want to change this,
change line 168 of src/server.cpp and line 332 of src/client.cpp
3. Run make
4. Run server
5. Make a directory named something reasonable, and add some RFCs (or any txt file) that follow the
naming scheme "rfc#.txt". E.g. rfc42.txt
7. In another session, run `client <rfc directory> <client address> <client listen port>`. E.g.,
`client rfcs1 127.0.0.1 8081` would start the client with hostname 127.0.0.1, listening on port
8081, and would store RFC files in the rfcs1 folder. It will create this folder if it does not exist
but it cannot create parent folders. When the client connects to the server, it will take note of which
RFCs are in the directory and send this information to the server.
6. For available commands, type help into the client's prompt.
7. Shutdown the client by typing `quit` into the prompt. **Important! The client needs to tell the server**
**it is disconnecting! Don't use Ctrl+C!**
9. Shutdown the server by simply doing Ctrl+C.

## Explanation

This project consists of two parts:

* Central index (server): This is a server that maintains information about what is available on the network.
It tracks which clients are connected to it and are available to download from, and which RFCs are
available. The index keeps track of which RFCs are offered by each client.
* Peers (client): These keep local copies of RFCs in .txt files. Upon connecting, they tell the CI which RFCs they have,
as well as how other peers can connect to them and download those RFCs.

### Deviations from project brief

We employed the following deviations from the project brief **after consulting with the professor**:

* Binary messages: Instead of communication happening by sending HTTP-like strings, we send very specific
streams of binary. For instance, instead of sending a status code of 200 as a string, we take a pointer to
an int holding 200, and send those four bytes across the socket directly. The advantage of this is that we
do not have to generate and parse strings. In our implementation, several \*msg and \*res files exist. Each one
of these represents a message that can be constructed first as an object, and then a to_bytes() method will create
a byte sequence that can be transmitted across the socket. Once the other side receives the bytes, a from_bytes()
method can recreate the object and let the values be accessed. Any shorts or ints are converted to network order
before being sent, and then converted back to the host byte order after they are received.
* No registration: Registration does not need to happen, as it does not prevent any malicious activity on the network.
A client would just register before joining, and this is just an unnecessary step. However, when connecting to the CI,
a client still sends its protocol version, hostname, and port by which it can be reached.
* Don't send unnecessary information in messages: Some headers specified by the project, such as OS, are unnecessary
because the other side doesn't do anything with this information. Also, instead of sending the hostname with every
message, we just send it at the beginning and remember it, as this information will not change until after the client
disconnects. Therefore, we do not send this information.
* Only identify RFCs by their number: The professor told us we did not need to worry about titles, so we didn't.

### Pieces

* CentralIndex: This is a static class that maintains information about who is connected and what RFCs they have.
Access to its members is protected by a mutex. It is used by the server.
* RFCManager: This is used by the client to load and save RFC documents. It stores RFCs in a directory.
* Messages: As mentioned above, Message and Response classes represent messages exchanged between hosts. Specific
messages correspond to specific parts of the program flow. For instance, the GetRFCMessage contains a single field
for the RFC number. This object is serialized to a byte sequence, sent over a socket, and used to reconstruct the
object by the receiver.

### Networking

The BSD socket library of C was used to facilitate basic TCP networking. However, these methods were packaged into classes
to make them more easily usable:

* ClientSocket: This class takes an address and port, at which it will initiate a connection.
* ServerSocketListener: This class takes a port to listen on and a callback function. It will listen at that port. When it
receives a connection, it will start a new thread which will run that function. The callback receives an instance of
ServerSocket that it can use to communicate with the client.
* ServerSocket: This socket is used to talk to a client who has connected to ServerSocketListener.

Both ClientSocket and ServerSocket implement `bool send(void *data, size_t len)` and `bool recv(void *buf, size_t len)` which
do what they suggest. ServerSocketListener is used both by the central index for all peers, and by the peers themselves to listen
for connections when another peer wants to download an RFC.

### Bonus: Encryption

Encrypted versions of the three classes above exist. To the client code, they appear to operate the same. Behind the scenes however,
they transparently encrypt and verify every byte that passes through. The encryption process works like this:

1. As soon as the two sockets connect, a handshake is performed. First, Elliptic Curve Diffie-Hellman is used to exchange a key.
Hash Key Derivation Function with SHA256 is used on this generated key to make another key with a suitable length for the cipher.
Finally, ChaCha20 stream cipher objects are initialized, and a known phrase is sent to make sure everything went smoothly.
2. When a socket sends data to the other side, it first encrypts this data using ChaCha20. It also generates a MAC. Both the
ciphertext and the MAC are then sent.
3. The receives socket uses the ciphertext and MAC to decrypt and verify the data.

This method prevents outside actors from eavesdropping or tampering with communications. However, no identification is performed,
so it can still be vulnerable to Man-in-the-Middle attacks.

### Sources Used

* GeeksForGeeks: Used to learn how to setup sockets in a C/C++ program.
  * https://www.geeksforgeeks.org/socket-programming-cc/
* Crypto++ wiki: Referenced for using cryptographic APIs
  * HCDH: https://www.cryptopp.com/wiki/Elliptic_Curve_Diffie-Hellman
  * HKDF: https://www.cryptopp.com/wiki/HKDF
  * ChaCha20Poly1305: https://www.cryptopp.com/wiki/ChaCha20Poly1305
* Linux man pages used for API documentation

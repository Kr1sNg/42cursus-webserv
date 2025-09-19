
## A Webserver

	1. Listens
	2. on a Port (of server machine)
	3. for a Request
	4. sent via a Transport Protocol
	5. return the Response
	6. containing the requested Resource

- *network host* (or a *host*): any device connected to a network that has a network address.
- *server*: a network host that provides functionality to other hosts.
- *client*: a network host that receives services from servers.

## Socket

Socket is a way to speak to other programs using standard Unix file descriptors.

### Two type of Internet Sockets (DARPA Internet addresses)

- Stream Sockets, `SOCK_STREAM`, are reliable two-way connected communication streams (as in `telnet`, `ssh`, `HTTP`,...), using `TCP` (Transmission Control Protocol) to achieve the high level of data transmission quality. 
- Datagram Sockets, `SOCK_DGRAM`, are connectionless, unreliable, faster way, using `UDP` (User Datagram Protocol) to transfer datagrams (as in `tftp`, multiplayer games, streaming audio, video conferencing,..).

### structs

#### struct `addrinfo`

Struct `addrinfo` (*address info*): is used to prep the socket address structures for subsequent use. It's also used in host name lookups, and service name lookups. Normally, we might not need to write these structures, a call to `getaddrinfor()` will fill it out.

```cpp
struct addrinfo
{
	int				ai_flags;		// AI_PASSIVE, AI_CANONNAME, etc
	int				ai_family;		// AF_INET, AF_INET6, AF_UNSPEC
	int				ai_socktype;	// SOCK_STREAM, SOCK_DGRAM
	int				ai_protocol;	// use 0 for "any"
	size_t			ai_addrlen;		// size of ai_addr in bytes
	struct sockaddr	*ai_addr;		// struct sockaddr_in or sockaddr_in6
	char			*ai_canonname;	// full canonical hostname

	struct addrinfo	*ai_next;		// linked list, next node, because a host can have more than one IP
};
```

#### struct `sockaddr`

struct `sockaddr`: holds *socket address information* for many types of sockets. `sa_data` contains a destination addresss and port number for the socket. Since we don't want to tediously pack the address in `sa_data` by hand, we use `sockaddr_in` (IPv4) and `sockaddr_in6` (IPv6) instead.

```cpp
struct sockaddr
{
	unsigned short	sa_family;		// address family, AF_INET (IPv4) or AF_INET6 (IPv6)
	char			sa_data[14];	// 14 bytes of protocol address
};
```

#### struct `sockaddr_in`

struct `sockaddr_in` (IPv4): a pointer to a `struct sockaddr_in` can be cast to a pointer to a `struct sockaddr` and vice-versa. This structure makes it easier to reference elements of the socket address.

```cpp
struct sockaddr_in
{
	short int			sin_family;		// address family, AF_INET (IPv4)
	unsigned short int	sin_port;		// port number (must be in Network Byte Order - using htons())
	struct in_addr		sin_addr;		// Internet address
	unsigned char		sin_zero[8];	// helps to get the same size as struct sockaddr
};
```

#### struct `in_addr`

struct `in_addr`: IP address, a structure for historical reasons

```cpp
struct in_addr
{
	uint32_t	s_addr;	// 32-bit int (4 bytes) IP address (in Network Byte Order)
}
```


### functions

#### `getaddrinfo()`

`getaddrinfo()` resolves hostname and service into usable `sockaddr` structure. As a host can have more than one IP address, this function gives us results in `res` a pointer to a linked list of one or more `addrinfor` structures (and `ai_next` is point to the next element or `NULL`) - *example 00*

```cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int	getaddrinfo(const char *node,	// hostname or IP address to connect to, e.g. "www.example.com"
				const char *service,	// e.g. "http" or port number of server
				const struct addrinfo *hints, // points to specified criteria of addrinfo which will be returned in res
				struct addrinfo **res);	// points to linked list results

void	freeaddrinfo(struct addrinfor *res);

```

`freeaddrinfo(res)`: free memory allocated by `getaddrinfo()` (linked list in result).


#### `strerror()`, `gai_strerror()`, `errno`

- `errno`: A global variable, which is called when a system call fails. After a function (system call) fails, it returns `-1` or `NULL`, we can check `errno` to see the reason.

- `strerror()`: converts an `errno` integer code into a human-readable string. *e.g. 01*

```cpp
#include <cerrno>	// errno
#include <cstring>	// strerror

char	*strerror(int errno);

```

- `gai_strerror()`: similar to `strerror()` but only for errors from `getaddrinfo()` (because `getaddrinfo()` doesn't set `errno`, instead it returns an error code).

```cpp
#include <netdb.h>

const char	*gai_strerror(int errcode);
```


#### `socket()`

`socket()`: gets the file descriptor. What we want to do is use the values from results of the call to `getaddrinfo()`, and feed them into `socket()` directly.

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int	socket(int domain,	// protocol family PF_INET (IPv4) or PF_INET6 (IPv6)
			int type,	//	SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
			int protocol);	// '0'

getaddrinfo("www.example.com", "http", &hints, &res);
socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

`socket()` returns a *socket descriptor* that we can use in system calls, or `-1` on error. The global variable `errno` is set to the error's value (we can print it to see the detail of error?)


#### `bind()`

`bind()` assigns a port on local machine (specified by `addr`) to a socket. Because when a socket is created (with socket()), it exists in a name space (address family) but has no address assigned to it.

```cpp
#include <sys/types.h>
#include <sys/socket.h>

int	bind(int sockfd,	// socket fd returned by socket()
		struct sockaddr *addr,	// a pointer to a struct sockaddr that contains info about address, namely, port and IP address
		int addrlen);	// length in bytes of that address 

```


#### `setsockopt()`

`setsockopt()` helps to manipulate options for the socket `sockfd`. It helps to avoid "Address already in use" when restarting the server.

```cpp
int	setsockopt(int sockfd,	// socket fd
				int level,	// SOL_SOCKET
				int optname,	// SO_REUSEADDR -> to reuse the address/port
				const void *optval,
				socklen_t optlen);

int	yes = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

```


#### `connect()`

`connect()` initiates a connection on a socket.

```cpp
#include <sys/socket.h>

int	connect(int sockfd,
			const struct sockaddr *addr,	// contains destination port and IP address
			socklen_t addrlen);				// length in bytes of addr

```

#### `listen()`, `accept()`

`listen()` marks socket `sockfd` as a passive socket (server mode), which waits for incomming connection requests using `accept()` and handle them in some way.
`accept()` extracts the first connection request (client) on the queue of incomming connections, creates a new connected socket for this single connection and return fd of this socket. The original socket `sockfd` is unaffected by this call, and it is still listening for more new connections.

```cpp
#include <sys/socket.h>

int listen(int sockfd,
			int backlog);	// SOMAXCONN - number of connections allowed on the incomming queue (waiting until accept())

int	accept(int sockfd,	// the listen() socket descriptor (original from socket())
			struct sockaddr *addr,	// pointer to the client address
			socklen_t *addrlen);	// size of client address

```

As usual, `listen()` and `accept()` returns `-1` and set `errno` on error.
We use the socket fd returned by `accept()` for all `send()` and `recv()` calls.


#### `send()`, `recv()`

- `send()` sends data on a connected socket, while `recv()` receives data from that socket.

```cpp
#include <sys/socket.h>

int	send(int sockfd,			// socket fd we want to send data to (which returned from accept())
			const void *msg,	// pointer to the data we want to send
			int len,			// length of msg in bytes
			int flags);			// 0

int	recv(int sockfd,		// socket fd to read from (which returned from accept())
			void *buf,		// buffer to read the data into
			int len,		// maximum length of the buffer
			int flags);		// 0

```

`send()` returns the number of bytes actually sent out (might be less than `len`).
`recv()` returns the number of bytes actually read into the buffer. If `recv()` returns `0`, it means the remote side has closed the connection on us.

`send()` and `recv()` returns `-1` and sets `errno` on error.



## A Webserver

	1. Listens
	2. on a Port
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
				const char *service,	// e.g. "http" or port number
				const struct addrinfo *hints, // points to specified criteria of addrinfo which will be returned in res
				struct addrinfo **res);	// points to linked list results

void	freeaddrinfo(struct addrinfor *res);

const char	*gai_strerror(int errcode);
```

`freeaddrinfo(res)`: free memory allocated by `getaddrinfo()` (linked list in result).


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

`setsockopt()` helps to manipulate options for the socket `sockfd`

```cpp
int	setsockopt(int sockfd,
				int level,
				int optname,
				const void *optval,
				socklen_t optlen);
```





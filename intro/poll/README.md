### Poll

#### Blocking -> non-blocking

When we first create the socket (with `socket()`), the kernel sets it to blocking. To change it to non-blocking, use `fcntl()`.

```cpp
#include <fcntl.h>

fcntl(socketfd, F_SETFL, O_NONBLOCK);
fcntl(socketfd, F_SETFD, FD_CLOEXEC);
```

*Because you have to use non-blocking file descriptors, it is possible to use read/recv or write/send functions with no poll() (or equivalent), and your server wouldn’t be blocking. But it would consume more system resources.*

- `F_SETFL`: set/change file status flags
- `O_NONBLOCK`: Non-blocking mode (no waiting on reads/writes)
- `F_SETFD`: set/ change descriptor flags
- `FD_CLOEXEC`: Auto-close fd on `execve()`, avoid leaks when launching CGI scripts.


#### `poll()` - Synchronous I/O Multiplexing

`poll()` (newer than `select()`) waits for one of a set of fd to become ready to perform I/O.

```cpp
#include <poll.h>

int poll(struct pollfd fds[],	// array of pollfd with information (which sockets to monnitor for what)
			nfds_t nfds, 		// number of elements in array fds
			int timeout);		// timeout in milliseconds

struct	pollfd
{
	int		fd;			// socket fd
	short	events;		// what we WANT to watch (POLLIN | POLLOUT) both
	short	revents;	// what actually HAPPENED after poll() (POLLIN or POLLOUT or nothing or else)
}
```

So, `POLLIN`, `POLLOUT` are the value of `events` (what we ask poll() to watch) or `revents` (what `poll()` returns in `pollfd`):

- `POLLIN`: there's data which is ready to `recv()` on this socket (means that client has sent data to us (REQUEST))
- `POLLOUT`: the socket is ready for us to `send()` data to client (RESPONSE) 
- `POLLHUP` (only returned in `revents`): the remote (client) closed this connection.

`poll()` returns the numbers of elements in `fds` array for which events have occurred.


#### `epoll()` - Synchronous I/O Multiplexing (Linux Only)

`epoll()` is similar to `poll()`, it monitors multiple fds to see if I/O is possible on any of them. However, it is designed for scalability (thousands of connections), it doen't need to pass the full array of `pollfd` each time in scanning, the kernel will tell us about the active ones.

- `epoll_create()` opens an epoll file descriptor. It returns a fd referring to the new epoll instance. This fd is used for all the subsequent calls to the epoll interface. After using, this fd should be closed by `close()`. On. error, `-1` is returned with `errno`.

```cpp
# include <sys/epoll.h>

int	epoll_create(int size);	// size can be ignored but must be greater than zero
```

- `epoll_ctl()` controls interface for an epoll file descriptor. This system call is used to add, modify, or remove entries in the interest list of the epoll instance.

```cpp
# include <sys/epoll.h>

union	epoll_data
{
	void		*ptr;
	int			fd;
	uint32_t	u32;
	uint64_t	u64;
};
typedef union epoll_data	epoll_data_t;

struct epoll_event
{
	uint32_t		events;	// epoll events -> exactly the same with poll events EPOLLIN/OUT/HUP = POLLIN/OUT/HUP
	epoll_data_t	data;	// user data variable
};

int	epoll_ctl(int epfd,		// list of fds epoll instance
				int op,		// operation to perform	EPOLL_CTR_ADD / _MOD / _DEL
				int fd,		// target fd
				struct epoll_event *event);	// event arguments describe the object lined to fd
```

- `epoll_wait()` waits until one or more fds registered become ready. It returns numbers of ready fds.

```cpp
# include <sys/epoll.h>
int	epoll_wait(int epfd,	// list of fds epoll
				struct epoll_event *event,	// array of events
				int	maxevents,				// size of array
				int	timeout);				// microsecond to wait (-1 = wait forever)
```

#### High-Level Class Design

We use the Reactor Pattern:

- EventLoop (abstract)
	Defines the interface for running the loop.

- EpollLoop (concrete)
	Uses `epoll_create`, `epoll_ctl`, `epoll_wait`.

- EventHandler (interface)
	Something that reacts to an event (fd ready).

- Listener (inherits EventHandler)
	Accepts new connections.

- Connection (inherits EventHandler)
	Handles client I/O (read/write).

- Server
	Owns the loop and registers listeners.



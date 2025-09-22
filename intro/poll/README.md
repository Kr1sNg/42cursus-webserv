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
	short	events;		// bitmap of events we're interested in (requested events)
	short	revents;	// on return, bitmap of events that occurred
}
```

- `POLLIN`: there's data which is ready to `recv()` on this socket
- `POLLOUT`: we can `send()` data to this socket
- `POLLHUP` (only returned in `revents`): the remote (client) closed this connection.

`poll()` returns the numbers of elements in `fds` array for which events have occurred.

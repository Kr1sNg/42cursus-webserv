# HTTP 1.1

## Virtuals hosts

For add a virtual host, we need to add a line like :
127.0.0.1	servername
at this path :
Computer/etc/hosts

## url resolve 

curl -v --resolve example.com:8181:127.0.0.1 http://example.com:8181/

hostname : example.com

## SIEGE / STRESS TEST
end=$((SECONDS+10))
while [ $SECONDS -lt $end ]; do
    for i in {1..10}; do curl -s http://localhost:8080/ > /dev/null & done
done


## HTTP Request

A HTTP Request includes 3 parts:
	- Request-Linemake fl
	- Headers
	- Body (optional)

Each line is separeted by `\r\n` (CRLF)

### Request-Line

```
<METHOD> <URI> <HTTP_VERS>

GET / HTTP/1.1
POST /uploads HTTP/1.1
```

Elements in Request-line are separeted by `space` (SP), and finished by `\r\n`.

Methods:
- `GET`: ask for a resource (html page, image,...)
- `POST`: transfer data to server
- `DELETE`: ask for removing a resource identified by `URI`

### Headers

An HTTP header is a key/value line placed after the request or status line, which provides additional information about the request or response (such as content type, size, host, or client preferences).

```
Header-Name: value

Host: server_name or inteface:port	// <- mandatory -> identify requested server (and compare to server_name)
User-Agent: Chrome		// -> name of HTTP client
Accept: text/html		// -> accepted formats
Content-Type:			// -> type of data sent (useful for POST)
Content-Length			// -> size of body sent
```

- `:` separates the header name and its value; there must be no space between the name and colon `:`, but there can be spaces or tabs after it (usually one space) before value.
- Header-Name: are not case-sensitive: `Header-Name` or `HEADER-NAME` or `header-name` are the same.
- Each header ends with CRLF.
- The Headers part is followed by an empty line (just CRLF).

### Body (optional)

The HTTP body is the optional part of a request or response that contains the actual data being exchanged, such as the content of an HTML page, a file, JSON, or form information.


## HTTP Response

Just like Request, HTTP Response has also 3 parts:
- Status Line
- Headers
- Body (optional)

```
HTTP/1.1 200 OK							// status line
Server: server_name						// header
Content-Type: text/html					// header
Content-Length: 50						// header
Connection: keep-alive					// header

<html><body></body></html>				// body (optional)
```

### Status Line

```
<HTTP_VERS> <CODE> <REASON>

HTTP/1.1 200 OK
HTTP/1.1 404 Not Found
```

Status line must be ended with `\r\n`.

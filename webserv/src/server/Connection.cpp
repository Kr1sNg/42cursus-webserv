/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/29 13:53:29 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/server/Server.hpp"
#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"

// Connection::Connection(void) {}

Connection::Connection(Server *server, int cfd, const Serverconfig &conf):
			_server(server),
			_servConfig(conf),
			_clientFd(cfd),
			_outBuf(),
			_events(POLLIN),
			_request(),
			_response(),
			_connState(CONN_READING_HEADERS), // initial state
			_bodyBytesReceived(0)
{
	setNonBlocking(_clientFd);
	std::cout << "Connection: servConf: server_name: " << _servConfig.getServer_name()[0] << std::endl;
}

Connection::~Connection()
{
	close(_clientFd);
}

int		Connection::getFd(void) const
{
	return (_clientFd);	
}

uint32_t	Connection::getEvents(void) const
{
	return (_events);
}

// void	Connection::recvIntoBuffer(void) //receive !!!!
// {
// 	char 	buf[4096];
// 	ssize_t	bytesRead;

// 	while (true)
// 	{
// 		ssize_t n = recv(_clientFd, buf, sizeof(buf), 0);
// 		if (n <= 0)
// 		{
// 			if (n < 0 || (errno == EAGAIN || errno == EWOULDBLOCK))
// 				break ; // no more data for now (not an error)
// 			_server->markForClose(_clientFd);
// 			return;
// 		}
// 		_inBuf.append(buf, n);
// 		std::cout << "Size " << n << " [RECV] from fd[" << _clientFd << "]: " << buf << "[END_RECV]" << std::endl;
		
// 		if (n < (ssize_t)sizeof(buf))
// 			break ;
// 	}

// 	// Check if we already got headers
// 	// detect a complete HTTP request by "\r\n\r\n"
// 	if (!_headerComplete)
// 	{
// 		size_t pos = _inBuf.find("\r\n\r\n");
// 		if (pos != std::string::npos)
// 		{
// 			_headerComplete = true;
// 			_headerEndPos = pos + 4;
// 			// find content-length
// 			std::string headers = _inBuf.substr(0, pos);
// 			size_t contentPos = headers.find("Content-Length:");
// 			if (contentPos != std::string::npos)
// 			{
// 				std::istringstream iss(headers.substr(contentPos + 16));
// 				iss >> _contentLength;
// 			}
// 			else
// 				_contentLength = 0;
// 		}
// 	}

// 	// If headers complete and full body received
// 	if (_headerComplete)
// 	{
// 		if (_inBuf.size() >= _headerEndPos + _contentLength)
// 		{
// 			// full Request received
// 			std::string fullRequest = _inBuf.substr(0, _headerEndPos + _contentLength);
// 			// Parse HTTP request
// 			Request request(_inBuf);
// 			_request = request;
			
// 			// need a HanderRequest to verify Request and .config

// 			//Build Response
// 			Response response(request, _servConfig);
// 			_response = response;
		
// 			_outBuf = _response.toString();
			
// 			_events = POLLIN | POLLOUT;
// 			_server->setFdEvents(_clientFd, _events);
			
// 			_inBuf.erase(0, _headerEndPos + _contentLength);
// 			_headerComplete = false;
// 			_contentLength = 0;
// 		}
// 	}
// }

void	Connection::handleReadHeaders(void)
{
	char	buf[4096];
	ssize_t	bytesRead = recv(_clientFd, buf, sizeof(buf), 0);
	
	if (bytesRead <= 0)
	{
		if (bytesRead < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
			return; // no more data for now (not an error)
		_server->markForClose(_clientFd);
		return;
	}
	
	//1. Append data to Request's buffer
	_request.append(buf, bytesRead);

	//2. Run the parser
	_request.parse();
	
	//3. Check the parser's state
	if (_request.isError())
	{
		generateErrorResponse(_request.getStatusCode(), _request.getReason());
	}
	else if (_request.isParsingComplete())
	{
		generateResponse();
	}
	else if (_request.getState() == PARSING_BODY)
	{
		_connState = CONN_HANDLING_BODY;
		_bodyBytesReceived = 0;

		// should check config here and open a file if (isUpload) -TODO
		// e.g: _uploadFile.open("/uploads", std::ios::binary);

		handleReadBody();
	}
	//else: state is sill PARSING_REQUEST_LINE and PARSING_HEADER, so we do nothing and wait for the next POLLIN event.
}

void	Connection::handleReadBody(void)
{
	//1. handle Chunked
	if (_request.isChunked())
	{
		// need to handle chunk-parsing loop -TODO
		return ;
	}

	//2. hander Content-Length
	size_t	contentLength = _request.getContentLength();
	
	// first, process any data already in the request's buffer
	std::string &leftover = _request.getBuffer();
	if (!leftover.empty())
	{
		size_t bytesToWrite = std::min(leftover.length(), contentLength - _bodyBytesReceived);

		// --- TODO: Write `bytesToWrite` from `leftover` to your file/CGI ---
        // e.g., _uploadFile.write(leftover.c_str(), bytesToWrite);

		_bodyBytesReceived += bytesToWrite;
		leftover.erase(0, bytesToWrite);
	}

	// second, read new data from the socket (the rest of body)
	char	buf[4096];
	while (_bodyBytesReceived < contentLength)
	{
		ssize_t bytesRead = recv(_clientFd, buf, sizeof(buf), 0);

		if (bytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ; //wait for next POLLIN
			_server->markForClose(_clientFd); //error
			return ;
		}
		if (bytesRead == 0)
		{
			_server->markForClose(_clientFd); //client disconnected
			return ;
		}

		size_t bytesToWrite = std::min((size_t)bytesRead, contentLength - _bodyBytesReceived);
		
		// --- TODO: Write `bytesToWrite` from `buf` to your file/CGI ---
        // e.g., _uploadFile.write(buf, bytesToWrite);
		
		_bodyBytesReceived += bytesToWrite;
		
		//if we read more than the body, save it for next request (pipelining)
		if (_bodyBytesReceived + (size_t)bytesRead > contentLength)
			_request.getBuffer().append(buf + bytesToWrite, bytesRead - bytesToWrite);
	}

	//3. Body is complete
	if (_bodyBytesReceived >= contentLength)
	{
		// TODO: Close your file/CGI
        // e.g., _uploadFile.close();

        // Now we can finally generate the response
        generateResponse();
	}
}

void	Connection::handleWrite(void) //send
{
	//1. send from _outBuf first (headers / small body)
	if (!_outBuf.empty())
	{	
		std::cout << "///////" << std::endl;
		std::cout << _outBuf << std::endl;
		std::cout << "///////" << std::endl;
	
		ssize_t n = send(_clientFd, _outBuf.c_str(), _outBuf.size(), 0);
		if (n < 0)
		{
			if (errno != EWOULDBLOCK && errno != EAGAIN)
				_server->markForClose(_clientFd);
			return ;
		}
		_outBuf.erase(0, n);
		
		// If buffer still has data, wait for next POLLOUT
		if (!_outBuf.empty())
			return ;
	}
	
	//2. if _outBuf is empty, check if we need to build from a file and send it
	if (_response.isBodyFromFile())
	{
		if (!_fileStream.is_open())
		{
			_fileStream.open(_response.getFilePath().c_str(), std::ios::binary);
			if (!_fileStream.is_open())
				return(generateErrorResponse(500, "Permission Deny"));
		}
		
		// Read a chunk from the file
		char	fileBuf[4096];
		_fileStream.read(fileBuf, sizeof(fileBuf));
		std::streamsize	bytesRead = _fileStream.gcount();
		
		if (bytesRead > 0)
		{
			ssize_t n = send(_clientFd, fileBuf, bytesRead, 0);
			if (n < 0)
			{
				if (errno != EWOULDBLOCK && errno != EAGAIN)
				{
					_fileStream.close();
					_server->markForClose(_clientFd);
				}
				else
					_fileStream.seekg(-bytesRead, std::ios::cur);
				return ; //wait for next POLLOUT
			}
			if (n < bytesRead)
			{
				// we only sent a part of chunk, keep continue
				_fileStream.seekg(n - bytesRead, std::ios::cur);
				return ;
			}
			// if (n == bytesRead), we sent the whole chunk, so just return to wait next POLLOUT call
			return ;
		}
		else	//bytesRead <= 0 means we are at the end of file
			_fileStream.close();
	}

	// Done sending
	// we only get here if _outBuf is empty AND we were not sending a file or we finished sending it
	if (_response.getKeepAlive())
	{
		resetConnection();
		_events = POLLIN;	// stop watching for POLLOUT
		_server->setFdEvents(_clientFd, _events);
	}
	else
		_server->markForClose(_clientFd);
}

void	Connection::resetConnection(void)
{
	_request = Request();
	_response = Response();
	_connState = CONN_READING_HEADERS;
	_bodyBytesReceived = 0;
	
	_uploadFilePath.clear();
	_isUploading = false;

	if (_fileStream.is_open())
		_fileStream.close();
}

void	Connection::handleEvent(uint32_t events)
{
	if (events & POLLERR || events & POLLHUP || events & POLLNVAL)
	{
		_server->markForClose(_clientFd);
		return ;
	}

	if (events & POLLIN)
	{
		if (_connState == CONN_READING_HEADERS)
			handleReadHeaders();
		else if (_connState == CONN_HANDLING_BODY)
			handleReadBody();
	}
	if (events & POLLOUT)
	{
		if (_connState == CONN_WRITING_RESPONSE)
			handleWrite();
	}
}

void	Connection::generateErrorResponse(int code, const std::string &reason)
{
	_response.buildError(code, reason, _servConfig);
	_response.setKeepAlive(false);
	_outBuf = _response.getHeaderString();
	
	// append generated body if it's small
	if (!_response.getBody().empty())
		_outBuf.append(_response.getBody());

	// set state to writing
	_connState = CONN_WRITING_RESPONSE;
	_events = POLLOUT;
	_server->setFdEvents(_clientFd, _events);
}

void	Connection::generateResponse(void)
{
	//1- Find the correct location block from config
	Locationconfig *location = _servConfig.matchLocation(_request.getUri());
	if (location == NULL)
			return (generateErrorResponse(404, "Not Found (doesn't match location)"));
	
	//2 - Check config rules (method, redirect)
	// method
	if (!location->isMethodAllowed(_request.getMethod()))
		return (generateErrorResponse(405, "Method Not Allowed (!?)"));

	//redirect
	if (location->hasRedirect())
	{
		_response.setStatus(301, "Moved Permanently");
		_response.setHeader("Location", location->getRedirect());
		_response.setBody(""); // no body for redirect
		_outBuf = _response.getHeaderString();
		_connState = CONN_WRITING_RESPONSE;
		_events = POLLOUT;
		_server->setFdEvents(_clientFd, _events);
		return ; // if we have redirect, we stop here
	}
		
	//3- Handle different request types
	// a. Check for CGI
	// if (location->hasCgi())
	// {
	// 	handleCgi();
	// 	return ;
	// }
	
	// b. Handle POST (which is an upload)
	// handleReadBody already saved the file, so just sent 201 created
	if (_request.getMethod() == "POST")
	{
		_response.setStatus(201, "Created");
		_response.buildFromFile("./notif/upload_success.html", _servConfig);
	}
	
	// c. Handle GET
	else if (_request.getMethod() == "GET")
	{
		//Construct the full file path
		std::string	filePath = _servConfig.getRoot() + location->getRoot() + _request.getUri();
		std::cout << "Connection::generateResponse:: filePath: " << filePath << std::endl;
		
		// check if directory and if autoindex on
		if (isDirectory(filePath))
		{
			std::string indexPath = filePath + "index.html"; // location->getIndex();
			std::cout << "Connection::generateResponse:: indexPath: " << indexPath << std::endl;
			if (fileExists(indexPath))
				_response.buildFromFile(indexPath, _servConfig);
			else if (location->getAutoindex()) // we have autoindex
			{
				std::string	listing = createDirectoryListing(filePath); // TODO
				_response.setBody(listing);
				_response.setHeader("Content-Type", "text/html");
			}
			else
				return (generateErrorResponse(403, "Forbidden"));
		}
		else	// it's a normal file
			_response.buildFromFile(filePath, _servConfig);
	}
	// d. Handle DELETE
	else if (_request.getMethod() == "DELETE")
	{
		// delete file -> TODO
		_response.setStatus(200, "OK");
		_response.buildFromFile("../notif/delete_success.html", _servConfig);
	}
	
	// 4. finalize and set state
	std::string	connHeader = _request.getHeader("Connection");
	if (connHeader == "close")
		_response.setKeepAlive(false);
	else
		_response.setKeepAlive(true);

	_outBuf = _response.getHeaderString();
	// if body is small, append it now
	if (!_response.isBodyFromFile() && !_response.getBody().empty())
		_outBuf.append(_response.getBody());
	// else => build from file in HandleWriter
	
	//set state to writing
	_connState = CONN_WRITING_RESPONSE;
	_events = POLLIN | POLLOUT;
	_server->setFdEvents(_clientFd, _events);
}


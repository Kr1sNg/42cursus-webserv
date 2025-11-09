/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/09 11:23:06 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/server/Server.hpp"
#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"
#include <ctime>
// Connection::Connection(void) {}

Connection::Connection(Server *server, int cfd, const Serverconfig &conf):
			_server(server),
			_servConfig(conf),
			_clientFd(cfd),
			_bodyCGI(""),
			_outBuf(),
			_err_msg(""),
			_events(POLLIN),
			_request(),
			_response(),
			_connState(CONN_READING_HEADERS), // initial state
			_bodyBytesReceived(0),
			_isUploading(false)
{
	setNonBlocking(_clientFd);
	// if (_servConfig.getServer_name().size())
		// std::cout << "Connection: servConf: server_name: " << _servConfig.getServer_name()[0] << std::endl;
	// else
		// std::cout << "Connection: servConf: server_name: Default" << std::endl;
}

Connection::~Connection()
{
	close(_clientFd);
	for (size_t i = 0; i < _cgiConnects.size(); ++i)
	{
		delete _cgiConnects[i];
	}
}

int		Connection::getFd(void) const
{
	return (_clientFd);	
}

uint32_t	Connection::getEvents(void) const
{
	return (_events);
}

std::string& Connection::getCgiOutput(void)
{
	return (_cgiOutput);
}

std::string& Connection::getCGIError(void)
{
	return (_err_msg);
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

void Connection::selectErrorCGI(std::string err_msg)
{
	if (err_msg.find("No such file or directory") != std::string::npos)
	{
		generateErrorResponse(404, "No such file or directory");
	}
	else if (err_msg.find("directory") != std::string::npos)
	{
		generateErrorResponse(404, "No such file or directory");
	}
	else if (err_msg.find("Permission") != std::string::npos)
	{
		generateErrorResponse(403, "Forbidden");
	}
	else if (err_msg.find("permitted") != std::string::npos)
	{
		generateErrorResponse(403, "Forbidden");
	}
	else
	{
		generateErrorResponse(500, "Internal Server Error");
	}
}

void Connection::onCgiComplete()
{
    // std::cout << "[Connection] CGI finished, output size: " << _cgiOutput.size() << std::endl;

	if (_err_msg != "")
	{
		std::cout << "error msg : " << _err_msg << std::endl;
		selectErrorCGI(_err_msg);
		return ;
	}
	else if (!_cgiOutput.size())
		return ;
    // Construire la réponse HTTP avec le contenu du CGI


    _response.buildCGI(_cgiOutput);
    _response.setStatus(200, "OK");
    _response.setHeader("Content-Length", Response::intToStr(_cgiOutput.size()));

    // Placer la réponse dans le buffer d'envoi
    _outBuf = _response.getHeaderString();
    if (!_response.getBody().empty())
        _outBuf.append(_response.getBody());

    // Passer la connexion en état écriture
    _connState = CONN_WRITING_RESPONSE;
    _events = POLLIN | POLLOUT;
    _server->setFdEvents(_clientFd, _events);
}


std::string Connection::trimSpace(const std::string &str)
{
    size_t start = 0;
    size_t end = 0;

    while (start < str.size() && isspace(str[start]))
        ++start;
    if (str.size() == start)
        return "";
    end = str.size() - 1;
    while (end > start && isspace(str[end]))
        end--;
    return (str.substr(start, end - start + 1));
}

bool	Connection::compareHost(std::string hostname)
{
	size_t i = 0;
	while (i < _servConfig.getServer_name().size())
	{
		if (_servConfig.getServer_name()[i] == hostname)
			return (true);
		i++;
	}
	return (false);
}

void	Connection::searchHost(const std::string &line, bool& check)
{
    size_t cmn;

    cmn = line.find(":");
    std::string before = trimSpace(line.substr(0, cmn));
	std::string after = trimSpace(line.substr(cmn + 1));
	if (before == "Host")
	{
		cmn = after.find(":");
		before = trimSpace(after.substr(0, cmn));
		if (before == "127.0.0.1" || before == "0.0.0.0" || before == "localhost")
			check = true;
		else
			check = compareHost(before);
	}
}

bool	Connection::checkHostName(std::string ogRequest)
{
	size_t start = 0;
	bool check= false;

    while (start < ogRequest.size()) {
        size_t index = ogRequest.find("\r\n", start);
        if (index == std::string::npos)
            break ;
        std::string line = ogRequest.substr(start, index - start);
		searchHost(line, check);
        start = index + 2;
    }
	return (check);
}

void	Connection::handleReadHeaders(void)
{
	char	buf[4096];
	ssize_t	bytesRead = recv(_clientFd, buf, sizeof(buf), 0);
	
	if (bytesRead <= 0)
	{
		_server->markForClose(_clientFd);
		return;
	}
	//1. Append data to Request's buffer
	_request.append(buf, bytesRead);
	//1.5 check virtual host
	if (!checkHostName(_request.getBuffer()))
	{
		_server->markForClose(_clientFd);
		return;
	}
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

		handleReadBody();
	}
	//else: state is sill PARSING_REQUEST_LINE and PARSING_HEADER, so we do nothing and wait for the next POLLIN event.
}

void	Connection::handleReadBody(void)
{
	//1. handle Chunked
	if (_request.isChunked())
	{
		// we won't support Transfer-Encoding: chunked
		return (generateErrorResponse(501, "Not Implemented (Transfer-Encoding)"));
	}

	//1.b check if this is an upload
	if (_bodyBytesReceived == 0) // First time in this function
	{
    	_isUploading = false;

		Locationconfig *loc = _servConfig.matchLocation(_request.getUri());
        if (loc && loc->isMethodAllowed("POST") && _request.getMethod() == "POST")
		{
			if (loc->getCgi_pass() != "")
			{
				_isCGI = true;
				_isUploading = false;
			}
			else
			{
				_isCGI = false;
            	_isUploading = true;
				

				// std::map<std::string, std::string>::const_iterator it = _request.getHeaders().find("Content-Disposition");
				// std::cout << "Type : "<< it->second << std::endl;
				// generate a unique name for uploaded file
				
				// std::stringstream ss;
				// ss << "upload_" << std::time(NULL) << "_" << _clientFd;
				// // ss << "upload_" << std::time(NULL) << "_" << _clientFd << extention;
				// std::string uniqueName = ss.str();

				// _uploadFilePath = "www/uploads/" + uniqueName;
				// std::cout << "uploadFilePath: " << _uploadFilePath << std::endl; //
				// _uploadFile.open(_uploadFilePath.c_str(), std::ios::binary);
				// if (!_uploadFile.is_open())
				// 	return (generateErrorResponse(500, "Permission Denied (uploads can't open file)"));
			}
        }
    }

	//2. hander Content-Length
	size_t	contentLength = _request.getContentLength();
	
	// first, process any data already in the request's buffer
	std::string &leftover = _request.getBuffer();
	// std::cout << "requete : " << leftover << std::endl;
	if (!leftover.empty())
	{
		//fonction pour obtenir les infos uploads depuis leftover

		size_t bytesToWrite = std::min(leftover.length(), contentLength - _bodyBytesReceived);
		
		if (_isCGI)
		{
			_bodyCGI.append(leftover.c_str(), bytesToWrite);
		}
		if (_isUploading)
		{
			_reqBody.append(leftover.c_str(), bytesToWrite);
		}
		_bodyBytesReceived += bytesToWrite;
		leftover.erase(0, bytesToWrite);
	}
	
	// second, read new data from the socket (the rest of body)
	char	buf[BUFFER];
	ssize_t bytesRead = 0;
	if (_bodyBytesReceived < contentLength)
	{
		bytesRead = recv(_clientFd, buf, sizeof(buf), 0);
		
		if (bytesRead < 0)
			usleep(500);
		if (bytesRead == 0)
		{
			if (_bodyBytesReceived < contentLength)
				return (generateErrorResponse(400, "Bad Request (Incomplete Body)"));
		}
	}
	if (bytesRead > 0)
	{
		size_t bytesToWrite = std::min((size_t)bytesRead, contentLength - _bodyBytesReceived);
	
		if (_isCGI)
		{
			_bodyCGI.append(buf, bytesToWrite);
		}
		if (_isUploading)
		{
			_reqBody.append(buf, bytesToWrite);
		}
		
		_bodyBytesReceived += bytesToWrite;

		if ((size_t)bytesRead > bytesToWrite)
			_request.getBuffer().append(buf + bytesToWrite, bytesRead - bytesToWrite);
	}

	if (_bodyBytesReceived >= contentLength)
	{
		if (_isUploading)
		{
			if (_request.getContentLength() > _servConfig.getClient_max_size())
			{
				return (generateErrorResponse(400, "Bad Request (client max body size)"));
			}
			std::string contentType = _request.getHeader("Content-Type");
			UploadedFile file = parseMultipartBody(_reqBody, contentType);
			
			if (file.filename.empty())
			{
				return generateErrorResponse(400, "Bad Request (no file found)");
			}
			std::string savePath = "www/uploads/" + file.filename;
			if (fileExists(savePath))
			{
				return generateErrorResponse(400, "Bad Request (file exist)");
			}
			std::ofstream out(savePath.c_str(), std::ios::binary);
			if (!out.is_open())
				return generateErrorResponse(500, "Permission Denied (can't save upload)");
			out.write(file.content.c_str(), file.content.size());
			out.close();

			// std::cout << "Uploaded: " << file.filename
			// 		<< " (" << file.contentType << ")" << std::endl;
		}
        generateResponse();
	}
}


void	Connection::handleWrite(void) //send
{
	//1. send from _outBuf first (headers / small body)
	if (!_outBuf.empty())
	{		
		ssize_t n = send(_clientFd, _outBuf.c_str(), _outBuf.size(), 0);
		if (n <= 0)
		{
			_server->markForClose(_clientFd);
			return (generateErrorResponse(500, "Permission Denied (handle Write)"));
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
				return(generateErrorResponse(500, "Permission Denied"));
		}
		
		// Read a chunk from the file
		char	fileBuf[4096];
		_fileStream.read(fileBuf, sizeof(fileBuf));
		std::streamsize	bytesRead = _fileStream.gcount();
		
		if (bytesRead > 0)
		{
			ssize_t n = send(_clientFd, fileBuf, bytesRead, 0);
			if (n <= 0)
			{
				_fileStream.close();
				_server->markForClose(_clientFd);
				return (generateErrorResponse(500, "Permission Denied (handle Write)"));
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
	// 0- // 1.a check client_max_body_size
	// std::cout << "request content length: " << _request.getContentLength()
	// 			<< " vs serv client max size: " << _servConfig.getClient_max_size() << std::endl;
	if (_request.getContentLength() > _servConfig.getClient_max_size())
	{
		return (generateErrorResponse(400, "Bad Request (client max body size)"));
	}

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
	
	if (location->getCgi_pass() != "")
    {
		_connState = CONN_WAITING_CGI; 
       	cgiHandle(_request, *location, _bodyCGI, _server->getLoop());
		return ;
       	// _response.buildCGI(content);
		// _response.setStatus(200, "OK");
        // _response.setHeader("Content-Length", Response::intToStr(content.size()));
	}
	
	// b. Handle POST (which is an upload)
	// handleReadBody already saved the file, so just sent 201 created
	else if (_request.getMethod() == "POST")
	{
		_response.setStatus(201, "Created");
		_response.buildFromFile("www/notif/upload_success.html", _servConfig);
	}
	
	// c. Handle GET
	else if (_request.getMethod() == "GET")
	{
		// handle session management only in /session/
		if (_request.getUri() == "/session/")
		{
			std::string ses = sessionManagement();
			return (generateVisitCountResponse(ses));
		}
		
		//Construct the full file path
		std::string	filePath = location->getRoot() + _request.getUri(); //_servConfig.getRoot() + location->getRoot() + _request.getUri();
		// std::cout << "generateResponse::GET: filePath: " << filePath << std::endl;
		
		// check if directory -> send index AND if autoindex on -> send directory listing
		if (isDirectory(filePath))
		{
			std::string indexPath = filePath + location->getIndex(); // "index.html";
			// std::cout << "Connection::generateResponse:: indexPath: " << indexPath << std::endl; //
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
		// locate the deleting file (we delete only in /www/uploads/)
		std::string filePath = location->getRoot() + _request.getUri();
		// std::cout << "generateResponse::DELETE: filePath: " << filePath << std::endl;
		
		if (!fileExists(filePath)) // file doesn't exsist
			return (generateErrorResponse(404, "Not Found (deleting file)"));
		if (isDirectory(filePath)) // cant delete directory
			return (generateErrorResponse(403, "Forbidden (deleting directory)"));
		if (std::remove(filePath.c_str()) == 0) // success delete
		{
			_response.setStatus(200, "OK");
			_response.buildFromFile("www/notif/delete_success.html", _servConfig);
		}
		else
			return (generateErrorResponse(403, "Forbidden (deleting file)"));
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

std::string	Connection::sessionManagement()
{
	// handle session
	std::string	sessionId = parseCookie(_request.getHeader("Cookie"), "session_id");	//TODO
	int count = _server->getSessionCount(sessionId);
	if (!count)	//new visitor
	{
		sessionId = createSessionId();
		//_server._sessions[sessionId] = 0;
		_server->setSessionCount(sessionId);
		_response.addCookie("session_id=" + sessionId + "; HttpOnly; Path=/; Max-Age=300");
	}
	else // old visitor
	{
		_server->increaseSessionCount(sessionId);
	}
	return (sessionId);
}

// from Request header: "Cookie: session_id=CK_12345; other=other"
std::string Connection::parseCookie(const std::string &headerLine, const std::string &cookieName)
{
	std::stringstream ss(headerLine);
	std::string cookie; // session_id=CK_12334, other=other...
	std::string key = cookieName + "="; //"session_id="
	
	while (std::getline(ss, cookie, ';'))	//split by semicolon
	{
		// trim whitespace
		size_t start = cookie.find_first_not_of(" ");
		if (start != std::string::npos)
			cookie = cookie.substr(start);
		if (cookie.find(key) == 0) //found the session_id
			return (cookie.substr(key.length()));
	}
	return (""); //not found
}

std::string	Connection::createSessionId()
{
	std::stringstream ss;
	ss << "CK_" << std::time(NULL);
	return (ss.str());
}

void	Connection::generateVisitCountResponse(std::string sesid)
{
	std::stringstream body;
	int count = _server->getSessionCount(sesid);
	
	body << "<html>\r\n";
    body << "<head><title>Visit Count</title></head>\r\n";
    body << "<body>\r\n";
    body << "  <h1>Session Visiting Info</h1>\r\n";
	body << "  <p>Your Session ID: <strong>" << sesid << "</strong>.</p>\r\n";
	body << "  <p>Session time-out: <strong>5</strong> minutes.</p>\r\n";
    body << "  <p>You have visited this page <strong>" << count << "</strong> times.</p>\r\n";
	body << "  <button onclick=\"window.location.href='/'\">Back to Home</button>\r\n";
    body << "</body>\r\n";
    body << "</html>\r\n";

	_response.setStatus(200, "OK");
	_response.setBody(body.str());
	_response.setHeader("Content-Type", "text/html");
	_response.setKeepAlive(true);
	
	_outBuf = _response.getHeaderString();
	_outBuf.append(_response.getBody());
	
	//set state to writing
	_connState = CONN_WRITING_RESPONSE;
	_events = POLLOUT | POLLIN;
	_server->setFdEvents(_clientFd, _events);
}

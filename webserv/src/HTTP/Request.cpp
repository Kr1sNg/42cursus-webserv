#include "../../includes/Request.hpp"
#include "../../includes/webserv.hpp"
//waiting for adding the good _uri (source path)

Request::Request(): _state(PARSING_REQUEST_LINE), _statusCode(200), _reason("OK"), _contentLength(0), _isChunked(false)
{
}

// Request::Request(const std::string &ogRequest):
//         _state(PARSING_REQUEST_LINE),
//         _statusCode(200),
//         _reason("OK"), 
//         _buffer(ogRequest),
//         _contentLength(0),
//         _isChunked(false)
// {
//     parse();
// }

Request::Request(const Request &cpy):
        _state(cpy._state),
        _statusCode(cpy._statusCode),
		_reason(cpy._reason),
        _buffer(cpy._buffer),
        _method(cpy._method),
		_uri(cpy._uri),
		_version(cpy._version),
		_headers(cpy._headers),
		// _body(cpy._body),
		_contentLength(cpy._contentLength),
        _isChunked(cpy._isChunked)
{
}

Request &Request::operator=(const Request &cpy) {
    if (this != &cpy) {
        _state = cpy._state;
        _statusCode = cpy._statusCode;
		_reason = cpy._reason;
        _buffer = cpy._buffer;
        _method = cpy._method;
		_uri = cpy._uri;
		_version = cpy._version;
		_headers = cpy._headers;
		// _body = cpy._body;
		_contentLength = cpy._contentLength;
        _isChunked = cpy._isChunked;
    }
    return *this;
}

Request::~Request() {
}

void	Request::append(const char* buffer, size_t nread)
{
    _buffer.append(buffer, nread);
    // std::cout << "_inBuf: " << _buffer << std::endl;
}


void    Request::parse(void)
{
    bool    stateChanged = true;
    while (stateChanged && _state != PARSING_BODY && _state != PARSING_COMPLETE && _state != PARSING_ERROR)
    {
        stateChanged = false;
        switch (_state)
        {
            case PARSING_REQUEST_LINE:
                if (parsingRequestLine())
                    stateChanged = true;
                break ;
            case PARSING_HEADERS:
                if (parsingHeaders())
                    stateChanged = true;
                break ;
            // case PARSING_BODY:
            //     if (parsingBody())
            //         stateChanged = true;
            //     break ;
            default:    // PARSING_BODY or PARSING_COMPLETE or PARSING_ERROR
                break ;
        }
    }
}

/*helper*/

size_t  Request::findNextCRLF(void)
{
    return (_buffer.find("\r\n"));
}

bool    Request::setError(int code, const std::string &reason)
{
    _state = PARSING_ERROR;
    _statusCode = code;
    _reason = reason;
    return false;
}

std::string Request::trimSpace(const std::string &str) {
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

bool    Request::parsingRequestLine(void)
{
    size_t  crlf_pos = findNextCRLF();
    if (crlf_pos == std::string::npos)
        return (setError(400, "Bad Request"));
    
    std::string line = _buffer.substr(0, crlf_pos);
    _buffer.erase(0, crlf_pos + 2); // remove also \r\n

    std::stringstream ss(line);
    std::string rest;

    //1. Method uri version
    if (!(ss >> _method))
        return (setError(400, "Bad Request (method)"));
    std::cout << "Request method: " << _method << std::endl;

    if (!(ss >> _uri))
        return (setError(400, "Bad Request (uri)"));
    std::cout << "Request uri: " << _uri << std::endl;
    
    if (!(ss >> _version))
        return (setError(400, "Bad Request (HTTP Version)"));
    std::cout << "Request version: " << _version << std::endl;
    
    if (ss >> rest)
        return (setError(400, "Bad Request"));
    
    _state = PARSING_HEADERS;
    return true;
}

bool    Request::parsingHeaders(void)
{
    _contentLength = 0;
    _isChunked = false;

    size_t  crlf_pos;
    while ((crlf_pos = findNextCRLF()) != std::string::npos)
    {
        std::string line = _buffer.substr(0, crlf_pos);
        _buffer.erase(0, crlf_pos + 2);

        // check for empty line (end of headers)
        if (line.empty())
        {
            std::map<std::string, std::string>::iterator te = _headers.find("Transfer-Encoding");
            std::map<std::string, std::string>::iterator cl = _headers.find("Content-Length");

            //check for Transfer-Encoding: chunked
            if (te != _headers.end() && te->second == "chunked")
                _isChunked = true;
                // if chunked is present, Content-Length must be ignored
            else if (cl != _headers.end()) // there's Content-Length
            {
                std::stringstream   ss(cl->second);
                if (!(ss >> _contentLength))// || _contentLength < 0)
                    return (setError(400, "Bad Request"));
            }
            // Decide next state
            if (_isChunked || _contentLength > 0)
                _state = PARSING_BODY;
            else    //there's no Chunked, no ContentLength => no Body to read
                _state = PARSING_COMPLETE;
            return true;    // header parsing was successful
        }

        // parse the header line (key: value)
        size_t  colon_pos = line.find(':');
        if (colon_pos == std::string::npos || colon_pos == 0)
            return (setError(400, "Bad Request")); // malformed, no colon
        
        std::string key = line.substr(0, colon_pos);    // have to use camel type for key
        std::string value = trimSpace(line.substr(colon_pos + 1));
        _headers[key] = value;
    }
    return (false); // not error, it means we need more data, parse() loop stops for now 
}

// bool    Request::parsingBody(void)
// {
//     if (_isChunked)
//         return (parsingChunkedBody());

//     if (_contentLength > 0) // check if the buffer actually contains the promised amount of data
//     {
//         if (_buffer.length() < _contentLength)
//             return (setError(400, "Bad Request (incomplete body)"));
//         _body = _buffer.substr(0, _contentLength);
//         _buffer.erase(0, _contentLength);

//         _state = PARSING_COMPLETE;
//         return true;
//     }

//     // no body was expected => content-length = 0 or absent
//     _state = PARSING_COMPLETE;
//     return true;
// }

// bool    Request::parsingChunkedBody(void)
// {
//     _body = "";
    
//     while (true)
//     {
//         //1. Read chunk size line
//         size_t  crlf_pos = findNextCRLF();
//         if (crlf_pos == std::string::npos)
//             return (setError(400, "Bad Request (malformed chunk - no size line)"));
        
//         std::string sizeLine = _buffer.substr(0, crlf_pos);
//         _buffer.erase(0, crlf_pos + 2);

//         //2. Convert hex size to integer
//         size_t  chunkSize;
//         std::stringstream   ss;
//         ss << std::hex << sizeLine;
//         if (!(ss >> chunkSize))
//             return (setError(400, "Bad Request (malformed chunk - invalid size)"));
        
//         //3. check for final chunk (size 0)
//         if (chunkSize == 0)
//             break ;

//         //4. read chunk data
//         if (_buffer.length() < chunkSize)
//             return (setError(400, "Bad Request (malformed chunk - incomplete data)"));
//         _body.append(_buffer, 0, chunkSize);
//         _buffer.erase(0, chunkSize);

//         //5. Read the chunk's trailing \r\n (final part)
//         if (_buffer.length() < 2 || _buffer.substr(0, 2) != "\r\n")
//             return (setError(400, "Bad Request (malformed chunk - missing trailing CRLF of Chunked Body)"));
//         _buffer.erase(0, 2); // erase the last "\r\n"
//     }
//     //6. Read the final CRLF of the request
//     if (_buffer.length() < 2 || _buffer.substr(0, 2) != "\r\n")
//         return (setError(400, "Bad Request (malformed chunk - missing final CRLF or Request)"));
    
//     _buffer.erase(0, 2);
//     _state = PARSING_COMPLETE;
//     return (true);
// }


/*getters*/
const std::string &Request::getMethod() const {
    return _method;
}

const std::string &Request::getUri() const {
    return _uri;
}

const std::string &Request::getVersion() const {
    return _version;
}

std::string Request::getHeader(const std::string &keyword) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(keyword);
    if (it != _headers.end())
        return (it->second);
    return ("");
}

const std::map<std::string, std::string> &Request::getHeaders() const {
    return _headers;
}

ParseState	Request::getState(void)
{
    return _state;
}

int Request::getStatusCode(void)
{
    return _statusCode;
}

const std::string &Request::getReason(void) const
{
    return _reason;
}

size_t  Request::getContentLength(void)
{
    return _contentLength;
}

std::string &Request::getBuffer(void)
{
    return _buffer;
}

bool    Request::isChunked(void) const
{
    return _isChunked;
}

bool    Request::isParsingComplete(void) const
{
    return (_state == PARSING_COMPLETE);
}

bool    Request::isError(void) const
{
    return (_state == PARSING_ERROR);
}
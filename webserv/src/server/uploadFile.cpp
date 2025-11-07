
#include "../../includes/webserv.hpp"

static std::string trim(const std::string &s)
{
	size_t start = s.find_first_not_of(" \t\r\n");
	size_t end = s.find_last_not_of(" \t\r\n");
	if (start == std::string::npos || end == std::string::npos)
		return "";
	return s.substr(start, end - start + 1);
}

UploadedFile parseMultipartBody(const std::string &body, const std::string &contentTypeHeader)
{
	UploadedFile file;
	std::string boundary;
	size_t bpos = contentTypeHeader.find("boundary=");
	if (bpos == std::string::npos)
	{
		std::cerr << "No boundary found in Content-Type" << std::endl;
		return file;
	}
	boundary = "--" + contentTypeHeader.substr(bpos + 9);

	//start of first part
	size_t start = body.find(boundary);
	if (start == std::string::npos)
		return file;
	start += boundary.size() + 2; // skip boundary + CRLF

	// end of headers
	size_t headerEnd = body.find("\r\n\r\n", start);
	if (headerEnd == std::string::npos)
		return file;
	std::string headers = body.substr(start, headerEnd - start);

	// extract filename
	size_t fnPos = headers.find("filename=\"");
	if (fnPos != std::string::npos)
	{
		size_t fnStart = fnPos + 10;
		size_t fnEnd = headers.find("\"", fnStart);
		file.filename = headers.substr(fnStart, fnEnd - fnStart);
		size_t slash = file.filename.find_last_of("/\\");
		if (slash != std::string::npos)
			file.filename = file.filename.substr(slash + 1);
	}

	// extract content-type
	size_t ctPos = headers.find("Content-Type:");
	if (ctPos != std::string::npos)
	{
		size_t ctStart = ctPos + 13;
		size_t ctEnd = headers.find("\r\n", ctStart);
		file.contentType = trim(headers.substr(ctStart, ctEnd - ctStart));
	}

	// extract content (until the next boundary)
	size_t contentStart = headerEnd + 4;
	size_t contentEnd = body.find(boundary, contentStart);
	if (contentEnd == std::string::npos)
		contentEnd = body.size();
	file.content = body.substr(contentStart, contentEnd - contentStart);

	// remove CRLF if present
	while (!file.content.empty() &&
		(file.content[file.content.size() - 1] == '\n' ||
		 file.content[file.content.size() - 1] == '\r'))
		file.content.erase(file.content.size() - 1);

	return file;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int PORT = 8080;

// Function to read the request body
std::string read_body(int socket_fd, size_t content_length) {
    char buffer[1024];
    std::string body = "";
    size_t bytes_read = 0;
    
    while (bytes_read < content_length) {
        ssize_t n = read(socket_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;
        bytes_read += n;
        buffer[n] = '\0';  // Null-terminate the buffer
        body.append(buffer);
    }

    return body;
}

#include <sys/stat.h>   // mkdir
#include <algorithm>    // find_if
#include <vector>

std::string sanitize_filename(const std::string &name) {
    // Keep only safe characters: letters, digits, dot, dash, underscore
    std::string out;
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '_') {
            out.push_back(c);
        }
    }
    // If empty, return fallback name
    if (out.empty()) return "upload.bin";
    return out;
}

void ensure_upload_dir(const std::string &dir) {
    struct stat st{};
    if (stat(dir.c_str(), &st) != 0) {
        // Try to create directory (permissions: rwx for owner)
        mkdir(dir.c_str(), 0700);
    }
}


// Function to parse multipart/form-data
bool parse_multipart(const std::string& body, const std::string& boundary, const std::string& upload_dir = "uploads") {
    std::string real_boundary = "--" + boundary; // parts start with --boundary
    ensure_upload_dir(upload_dir);

    size_t pos = 0;
    while ((pos = body.find(real_boundary, pos)) != std::string::npos) {
        size_t header_start = pos + real_boundary.size();
        if (body.compare(header_start, 2, "\r\n") == 0) header_start += 2;

        size_t end_of_header = body.find("\r\n\r\n", header_start);
        if (end_of_header == std::string::npos) break;

        std::string header = body.substr(header_start, end_of_header - header_start);

        // Look for filename in Content-Disposition
        size_t fn_pos = header.find("filename=\"");
        if (fn_pos != std::string::npos) {
            size_t fn_start = fn_pos + 10;
            size_t fn_end = header.find("\"", fn_start);
            std::string filename = header.substr(fn_start, fn_end - fn_start);
            filename = sanitize_filename(filename);

            // file content begins after the blank line
            size_t file_start = end_of_header + 4;
            // file end is before the next boundary (preceded by CRLF)
            size_t file_end = body.find("\r\n" + real_boundary, file_start);
            if (file_end == std::string::npos) {
                // final boundary might be followed by "--"
                file_end = body.find(real_boundary + "--", file_start);
                if (file_end == std::string::npos) break;
            }

            std::string file_data = body.substr(file_start, file_end - file_start);

            // Write to disk
            std::string path = upload_dir + "/" + filename;
            std::ofstream outfile(path, std::ofstream::binary);
            if (!outfile) {
                std::cerr << "Failed to open " << path << " for writing\n";
            } else {
                outfile.write(file_data.data(), file_data.size());
                outfile.close();
                std::cout << "Saved uploaded file: " << path << " (" << file_data.size() << " bytes)\n";
            }
        }
        pos = end_of_header; // continue looking for next part
    }
    return true;
}



// Handle incoming requests
void handle_request(int new_socket) {
    char buffer[1024];
    ssize_t read_size = read(new_socket, buffer, sizeof(buffer) - 1);
    if (read_size <= 0) return;
    buffer[read_size] = '\0';

    std::string request(buffer);
    std::string method = request.substr(0, request.find(' '));
    std::string url = request.substr(request.find(' ') + 1, request.find(' ', request.find(' ') + 1) - request.find(' ') - 1);

    // Extract content length (for POST)
    size_t content_length = 0;
    size_t content_length_pos = request.find("Content-Length: ");
    if (content_length_pos != std::string::npos) {
        content_length_pos += 16; // Move past the "Content-Length: " string
        size_t content_length_end = request.find("\r\n", content_length_pos);
        content_length = std::stoi(request.substr(content_length_pos, content_length_end - content_length_pos));
    }

    // Extract boundary (for multipart POST requests)
    std::string boundary;
    size_t boundary_pos = request.find("boundary=");
    if (boundary_pos != std::string::npos) {
        size_t boundary_start = boundary_pos + 9;  // Skip "boundary="
        size_t boundary_end = request.find(";", boundary_start);
        boundary = request.substr(boundary_start, boundary_end - boundary_start);
    }

    if (method == "POST") {
        std::cout << "Received a POST request" << std::endl;

        // Check if content type is multipart/form-data
        size_t content_type_pos = request.find("Content-Type: multipart/form-data;");
		if (content_type_pos != std::string::npos) {
    		std::string body = read_body(new_socket, content_length);
    		parse_multipart(body, boundary, "uploads");
        }

        // Respond to client
        std::string response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>File Uploaded Successfully!</h1></body></html>";

        write(new_socket, response.c_str(), response.size());
    } 
	else if (method == "GET") {
    std::cout << "Received a GET request for: " << url << std::endl;

    std::string upload_prefix = "/uploads/";
    std::string file_path;
    std::string content_type = "application/octet-stream";

    if (url == "/") {
        file_path = "index.html";
        content_type = "text/html";
    	} 
	else if (url.rfind(upload_prefix, 0) == 0) { // starts with /uploads/
        // map to uploads/<filename>
        std::string requested = url.substr(upload_prefix.size());
        requested = sanitize_filename(requested); // sanitize again
        file_path = "uploads/" + requested;
        // Basic content-type detection by extension
        if (requested.size() >= 4) {
            if (requested.find(".html") != std::string::npos) content_type = "text/html";
            else if (requested.find(".css") != std::string::npos) content_type = "text/css";
            else if (requested.find(".js") != std::string::npos) content_type = "application/javascript";
            else if (requested.find(".png") != std::string::npos) content_type = "image/png";
            else if (requested.find(".jpg") != std::string::npos || requested.find(".jpeg") != std::string::npos) content_type = "image/jpeg";
            else if (requested.find(".txt") != std::string::npos) content_type = "text/plain";
    } 
	else {
        // serve other static files from current dir
        file_path = (url.size() > 1 ? url.substr(1) : "index.html");
    }

    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) {
        std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        std::ostringstream resp;
        resp << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << content_type << "\r\n"
             << "Content-Length: " << file_contents.size() << "\r\n"
             << "Connection: close\r\n\r\n";
        std::string header = resp.str();
        write(new_socket, header.c_str(), header.size());
        write(new_socket, file_contents.data(), file_contents.size());
    } else {
        std::string response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body><h1>Page Not Found</h1></body></html>";
        write(new_socket, response.c_str(), response.size());
    }
}
}}

// Main server setup
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error opening socket." << std::endl;
        return 1;std::cout << "Received a POST request" << std::endl;

    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket." << std::endl;
        return 1;
    }

    listen(server_fd, 5);
    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (new_socket < 0) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        handle_request(new_socket);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

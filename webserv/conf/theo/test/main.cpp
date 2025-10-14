#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <algorithm>
#include <vector>
#include <cctype>

const int PORT = 8080;

// Sanitize filename to prevent path traversal
std::string sanitize_filename(const std::string &name) {
    std::string out;
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '_') {
            out.push_back(c);
        }
    }
    return out.empty() ? "upload.bin" : out;
}

// Ensure upload directory exists
void ensure_upload_dir(const std::string &dir) {
    struct stat st{};
    if (stat(dir.c_str(), &st) != 0) {
        mkdir(dir.c_str(), 0700);
    }
}

// Read request body
std::string read_body(int socket_fd, size_t content_length) {
    char buffer[1024];
    std::string body;
    size_t bytes_read = 0;

    while (bytes_read < content_length) {
        ssize_t n = read(socket_fd, buffer, sizeof(buffer));
        if (n <= 0) break;
        body.append(buffer, n);
        bytes_read += n;
    }
    return body;
}

// Parse multipart form-data
bool parse_multipart(const std::string& body, const std::string& boundary, const std::string& upload_dir = "uploads") {
    std::string real_boundary = "--" + boundary;
    ensure_upload_dir(upload_dir);

    size_t pos = 0;
    while ((pos = body.find(real_boundary, pos)) != std::string::npos) {
        size_t header_start = pos + real_boundary.size();
        if (body.compare(header_start, 2, "\r\n") == 0) header_start += 2;

        size_t end_of_header = body.find("\r\n\r\n", header_start);
        if (end_of_header == std::string::npos) break;

        std::string header = body.substr(header_start, end_of_header - header_start);

        // Extract filename
        size_t fn_pos = header.find("filename=\"");
        if (fn_pos != std::string::npos) {
            size_t fn_start = fn_pos + 10;
            size_t fn_end = header.find("\"", fn_start);
            std::string filename = sanitize_filename(header.substr(fn_start, fn_end - fn_start));

            size_t file_start = end_of_header + 4;
            size_t file_end = body.find("\r\n" + real_boundary, file_start);
            if (file_end == std::string::npos) {
                file_end = body.find(real_boundary + "--", file_start);
                if (file_end == std::string::npos) break;
            }

            std::string file_data = body.substr(file_start, file_end - file_start);
            std::string path = upload_dir + "/" + filename;

            std::ofstream outfile(path, std::ofstream::binary);
            if (!outfile) {
                std::cerr << "Failed to write to " << path << "\n";
            } else {
                outfile.write(file_data.data(), file_data.size());
                outfile.close();
                std::cout << "Saved file: " << path << " (" << file_data.size() << " bytes)\n";
            }
        }
        pos = end_of_header;
    }
    return true;
}

// Handle HTTP request
void handle_request(int new_socket) {
    char buffer[4096];
    ssize_t read_size = read(new_socket, buffer, sizeof(buffer) - 1);
    if (read_size <= 0) return;

    buffer[read_size] = '\0';
    std::string request(buffer);

    // Parse method and path
    std::string method = request.substr(0, request.find(' '));
    size_t path_start = request.find(' ') + 1;
    size_t path_end = request.find(' ', path_start);
    std::string url = request.substr(path_start, path_end - path_start);

    // Parse content length
    size_t content_length = 0;
    size_t cl_pos = request.find("Content-Length: ");
    if (cl_pos != std::string::npos) {
        cl_pos += 16;
        size_t cl_end = request.find("\r\n", cl_pos);
        content_length = std::stoul(request.substr(cl_pos, cl_end - cl_pos));
    }

    // Parse boundary
    std::string boundary;
    size_t b_pos = request.find("boundary=");
    if (b_pos != std::string::npos) {
        size_t b_start = b_pos + 9;
        size_t b_end = request.find("\r\n", b_start);
        boundary = request.substr(b_start, b_end - b_start);
    }

    if (method == "POST") {
        std::cout << "Received POST request\n";
        std::string body = read_body(new_socket, content_length);

        if (!boundary.empty()) {
            parse_multipart(body, boundary);
        }

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>File Uploaded Successfully!</h1></body></html>";

        write(new_socket, response.c_str(), response.size());
    }
    else if (method == "GET") {
        std::cout << "Received GET request for: " << url << "\n";

        std::string file_path;
        std::string content_type = "application/octet-stream";

        if (url == "/") {
            file_path = "index.html";
            content_type = "text/html";
        } else if (url.rfind("/uploads/", 0) == 0) {
            std::string filename = sanitize_filename(url.substr(9));
            file_path = "uploads/" + filename;

            if (filename.find(".html") != std::string::npos) content_type = "text/html";
            else if (filename.find(".css") != std::string::npos) content_type = "text/css";
            else if (filename.find(".js") != std::string::npos) content_type = "application/javascript";
            else if (filename.find(".png") != std::string::npos) content_type = "image/png";
            else if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos) content_type = "image/jpeg";
            else if (filename.find(".txt") != std::string::npos) content_type = "text/plain";
        } else {
            file_path = sanitize_filename(url.substr(1));
        }

        std::ifstream file(file_path, std::ios::binary);
        if (file.is_open()) {
            std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n"
                     << "Content-Type: " << content_type << "\r\n"
                     << "Content-Length: " << file_contents.size() << "\r\n"
                     << "Connection: close\r\n\r\n";

            std::string header = response.str();
            write(new_socket, header.c_str(), header.size());
            write(new_socket, file_contents.data(), file_contents.size());
        } else {
            std::string response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n\r\n"
                "<html><body><h1>404 - File Not Found</h1></body></html>";
            write(new_socket, response.c_str(), response.size());
        }
    }
}

// Main function
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error creating socket.\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket.\n";
        return 1;
    }

    listen(server_fd, 5);
    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Error accepting connection.\n";
            continue;
        }

        handle_request(client_socket);
        close(client_socket);
    }

    close(server_fd);
    return 0;
}

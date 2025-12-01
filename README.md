<h1 align="center"> 
	🌐🌐🌐 &emsp; Webserv - HTTP Server &emsp; 🔗🔗🔗
</h1>

<img width="1260" height="909" alt="image" src="https://github.com/user-attachments/assets/a389c05a-8a99-46e5-ae64-52007b888c9b" />
<h4 align="right">
  
  🧑🏻‍💻👩🏻‍💻🧑🏻‍💻 Made by [@KrisNg](https://github.com/Kr1sNg), [@Mileumm](https://github.com/Mileumm) and [@KuroeProg](https://github.com/KuroeProg)  

</h4>

---

## ℹ️ &ensp; Introduction

**Webserv** is a lightweight **HTTP server** written in **C++98** as part of the school 42's curriculum.

The goal of the project is to re-create core features of a real web server—similar to *Nginx*, including parsing *configuration files*, handling *multiple client connections*, serving *static files*, managing *CGI execution*, and supporting different *HTTP methods*.


## 📁 &ensp; Project Structure

```dir
webserv/
│── src/            # source files
│── includes/       # header files
│── www/            # static website directory
│── conf/           # sample config files
│── Makefile
└── README.md
```

## 🚀 &ensp; Features

- Non-blocking I/O using **poll()**
- Supports **HTTP/1.1**
- Config file parsing similar to **Nginx**
- **Multiple servers** handling
- Routing with location blocks
- Supported HTTP methods: **GET**, **POST**, **DELETE**
- CGI execution (Python / Bash)
- Static file serving
- Automatic error pages (customizable)
- Directory listing (**autoindex**)
- Multiple virtual servers via different **server_name** or listen values
- File **upload** / **delete** handling
- Cookie / Session handling
- Connection timeout management


## 🛠️ &ensp; Running requirements

Before building the server, ensure you have:

- C++98 compatible compiler (clang++ or g++)
- Make
- (Optional) Python3 and BASH for CGI testing
- Tested on: macOS / Linux (Ubuntu, Debian)


## ▶️ &ensp; Run the Webserv

### Build the Server

In the root directory of `webserv`:

```bash
make
# you can use also these commands below to clean
make clean
make fclean
make re
```

### Run the Server

Basic use:

```bash
./webserv conf/default.conf
```

Multi-servers:

```bash
./webserv config/multi.conf
```

### Connect to the Server

In your web browser, connect to: http://localhost:8080 if you're running `conf/default.conf`

In case of multi-servers `conf/multi.conf`, try to connect to three servers by three different tabs:
- http://localhost:8080
- http://localhost:8181
- http://localhost:8282


## 📚 &ensp; How to Extend the Project

This project still has potential for more features in the future:

- more HTTP methods
- WebSocket upgrade logic
- caching system
- load balancing (multiple workers)

The modular structure allows for new features without breaking the existing core.


## 🐛 &ensp; Issues and Bugs

Please feel free to create a new issue with the title and description on the issues page of this `42cursus-webserv` repository.

If you have already found the solution to the problem, we would love to review your `pull request`!

Thank you in advance and `<h1>Happy coding!<h1>`


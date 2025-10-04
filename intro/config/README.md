Config Parser for Webserv

This module is responsible for reading and interpreting the configuration file (.conf) used by webserv.
The syntax is inspired by Nginx configuration files, but only a subset of blocks and directives are supported.


🔹 Overview

The parser has two main responsibilities:

Tokenization – Split the configuration file into atomic tokens (keywords, braces, semicolons, arguments, etc.).

Hierarchization – Organize tokens into a structured tree of Block and Directive objects that represent the configuration hierarchy.



🔹 Tokenization

During this phase, the configuration file is read line by line and each word/symbol is extracted into a token list.

Spaces and tabs are ignored (except as separators).

Comments (# ...) are skipped.

Special characters ({, }, ;) are treated as individual tokens.

Sequences of non-space characters are grouped into a single token.

Example input:

server {
    listen 8080;
    server_name localhost;
}

Token list:

["server", "{", "listen", "8080", ";", "server_name", "localhost", ";", "}"]



🔹 Hierarchization

Once we have a list of tokens, we build a tree-like structure representing blocks and directives.

Objects used

Directive

Represents a simple statement ending with ;.
Has:

    A name (e.g. listen, server_name).

    A list of arguments (e.g. 8080, localhost).



Block

Represents a group of directives and/or sub-blocks surrounded by { ... }.
Has:

    A name (e.g. server, location).

    A list of arguments (optional, e.g. /uploads in location /uploads {}).

    A list of directives.

    A list of child blocks.


Example

Input:

server {
    listen 8080;
    root ./www;

    location /uploads {
        autoindex on;
    }
}


Hierarchy:

Block "server"
  Directive "listen" -> ["8080"]
  Directive "root" -> ["./www"]
  Block "location"
    Arg -> ["/uploads"]
    Directive "autoindex" -> ["on"]



🔹 Error Handling

The parser checks for common syntax and semantic errors:

Missing semicolons (;).

Unmatched braces ({ without }).

Unknown directives.

Invalid arguments (e.g. listen abc;).

Forbidden placement (e.g. listen inside a location).



🔹 Why this matters

The parser ensures that the configuration file is valid and consistent before the server starts.
Once the parsing is done, the server can rely on a clean and structured representation of the configuration to:

Open sockets (listen).

Define root directories (root).

Serve error pages (error_page).

Handle routes (location).

Execute CGI scripts (cgi_pass).
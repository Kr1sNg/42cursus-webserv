#!/usr/bin/env python3
import os
import sys
import urllib.parse

# En-têtes obligatoires
print("Content-Type: text/html")
print("")  # Ligne vide très importante

# Corps HTML
print("<html><body>")
print("<h1>CGI Python Test</h1>")

# GET query string
query = os.environ.get("QUERY_STRING", "")
if query:
    print(f"<p>GET query: {query}</p>")
else:
    print("<p>No GET query string provided.</p>")

# POST data
if os.environ.get("REQUEST_METHOD") == "POST":
    length = int(os.environ.get("CONTENT_LENGTH") or 0)
    if length > 0:
        body = sys.stdin.read(length)
        print(f"<p>POST data: {body}</p>")
    else:
        print("<p>No POST data provided.</p>")

# Variables CGI (facultatif)
print("<h2>CGI Environment Variables:</h2><ul>")
for var in ["REQUEST_METHOD","CONTENT_LENGTH","CONTENT_TYPE","SCRIPT_NAME","PATH_INFO"]:
    print(f"<li>{var}: {os.environ.get(var,'')}</li>")
print("</ul></body></html>")

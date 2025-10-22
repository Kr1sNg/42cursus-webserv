#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cgi
import cgitb
import os
import sys

# Active le debug en CGI (affiche erreurs dans le navigateur)
cgitb.enable()

# En-tête HTTP obligatoire
print("Content-Type: text/html")
print("")  # Ligne vide pour séparer headers et body

# Commence la page HTML
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI Python Test</h1>")

# --- Gestion GET ---
query_string = os.environ.get("QUERY_STRING", "")
if query_string:
    print("<h2>GET Query string:</h2>")
    print("<p>{}</p>".format(query_string))
else:
    print("<p>No GET query string provided.</p>")

# --- Gestion POST ---
form = cgi.FieldStorage()
if form:
    print("<h2>POST Data:</h2>")
    for key in form.keys():
        print("<p>{}: {}</p>".format(key, form.getvalue(key)))
else:
    print("<p>No POST data provided.</p>")

# --- Infos CGI ---
print("<h2>CGI Environment Variables:</h2>")
print("<ul>")
for key in ["REQUEST_METHOD", "CONTENT_LENGTH", "CONTENT_TYPE", "SCRIPT_NAME", "PATH_INFO"]:
    value = os.environ.get(key, "")
    print("<li>{}: {}</li>".format(key, value))
print("</ul>")

# Fin de la page
print("</body>")
print("</html>")

# Flush pour que le serveur parent lise tout
sys.stdout.flush()

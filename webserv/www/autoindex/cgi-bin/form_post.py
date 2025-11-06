#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cgi
import cgitb
import os
import sys

# Active le débogage (affiche les erreurs dans le navigateur)
cgitb.enable()

# Création de l'objet FieldStorage (parse les données POST ou GET)
form = cgi.FieldStorage()

# En-tête HTTP (obligatoire)
print("Content-Type: text/html; charset=utf-8")
print()  # ligne vide = fin des en-têtes

print("<html><body>")
print("<h2>Requête POST reçue ✅</h2>")

# --- Affichage du contenu reçu ---
if form:
    print("<h3>Données reçues :</h3>")
    print("<ul>")
    for key in form.keys():
        print(f"<li><b>{key}</b> = {form.getvalue(key)}</li>")
    print("</ul>")
else:
    print("<p>Aucune donnée reçue.</p>")

# --- Debug : affichage de l'environnement CGI ---
print("<h3>Environnement CGI :</h3>")
print("<pre>")
for k, v in sorted(os.environ.items()):
    print(f"{k} = {v}")
print("</pre>")

print("</body></html>")

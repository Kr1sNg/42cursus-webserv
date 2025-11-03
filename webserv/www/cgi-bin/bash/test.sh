#!/usr/bin/env bash

echo "Content-Type: text/html"
echo ""  # ligne vide obligatoire

echo "<html><body>"
echo "<h2>CGI Bash ✅</h2>"

if [ "$REQUEST_METHOD" = "POST" ]; then
  echo "<h3>Données reçues :</h3><pre>"
  read -n "$CONTENT_LENGTH" POST_DATA
  echo "$POST_DATA"
  echo "</pre>"
else
  echo "<p>Aucune donnée POST reçue.</p>"
fi

echo "<h3>Environnement CGI :</h3><pre>"
env | sort
echo "</pre>"

echo "</body></html>"



































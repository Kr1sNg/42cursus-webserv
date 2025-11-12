#!/usr/bin/env perl
use strict;
use warnings;
use CGI;
use utf8;  # active l’UTF-8 dans le script
binmode STDOUT, ':encoding(UTF-8)';  # force la sortie en UTF-8

# Création de l’objet CGI (parse automatiquement GET ou POST)
my $q = CGI->new;

# En-tête HTTP avec charset UTF-8
print $q->header(-type => 'text/html', -charset => 'UTF-8');

# Début du HTML
print "<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <title>Webserv — CGI Tests</title>
  <style>
    body {
      font-family: 'Poppins', Arial, sans-serif;
      background: url('../../CGI_menu.jpg') no-repeat center 45% fixed;
      background-size: 100%;
      margin: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
      overflow: hidden;
      animation: floatBackground 12s ease-in-out infinite;
    }
    \@keyframes floatBackground { 0% { background-position: center 45%; } 50% { background-position: center 48%; } 100% { background-position: center 45%; } }
    header { width: 100%; max-width: 1100px; display: flex; justify-content: space-between; align-items: center; padding: 1.2rem 2.5rem; background: rgba(255,255,255,0.85); box-shadow: 0 2px 8px rgba(0,0,0,0.05); border-radius: 0 0 20px 20px; position: relative; z-index: 2; }
    header h1 { font-size: 1.8rem; color: #444; margin: 0; }
    nav { display: flex; gap: 2rem; }
    nav a { color: #555; text-decoration: none; font-weight: 500; transition: color 0.2s ease, transform 0.2s ease; }
    nav a:hover { color: #ff8c8c; transform: translateY(-2px); }
    main { flex: 1; display: flex; flex-direction: column; align-items: center; justify-content: flex-start; gap: 40px; width: 100%; max-width: 600px; padding: 50px 20px; box-sizing: border-box; }
    section { width: 100%; background: rgba(255,255,255,0.85); backdrop-filter: blur(6px); padding: 30px 40px; border-radius: 15px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); animation: floatForm 6s ease-in-out infinite; }
    \@keyframes floatForm { 0% { transform: translateY(0px); } 50% { transform: translateY(-10px); } 100% { transform: translateY(0px); } }
    h2 { background-color: white; color: #333; padding: 10px 15px; border-radius: 8px; text-align: center; margin-bottom: 20px; box-shadow: 0 2px 6px rgba(0,0,0,0.1); }
    section { display: flex; flex-direction: column; gap: 15px; }
    label { font-weight: 500; color: #444; }
    input[type='text'] { padding: 8px 10px; border: 1px solid #ccc; border-radius: 6px; outline: none; }
    input[type='text']:focus { border-color: #ff9b9b; }
    input[type='submit'] { background-color: #ffb6b6; color: #444; border: none; padding: 10px 20px; border-radius: 8px; cursor: pointer; transition: background 0.3s ease, transform 0.2s ease; }
    input[type='submit']:hover { background-color: #ff9b9b; transform: scale(1.05); }
    footer { text-align: center; padding: 1rem; color: #999; font-size: 0.9rem; }
  </style>
</head>
<body>

<header>
  <h1>Webserv</h1>
</header>

<main>

<h2>CGI Perl ✅</h2>
<section>";
# --- Traitement des données POST ---
if ($ENV{'REQUEST_METHOD'} eq 'POST') {
    print "<h3>Données reçues :</h3><ul>";

    # Récupère tous les champs transmis
    foreach my $key ($q->param) {
        my $value = $q->param($key);
        print "<li><b>$key</b> = $value</li>";
    }

    print "</ul>";
} else {
    print "<p>Aucune donnée POST reçue.</p>";
}

# --- Affichage de l’environnement CGI ---
print "<h3>Environnement CGI :</h3><pre>";
foreach my $key (sort keys %ENV) {
    print "$key = $ENV{$key}\n";
}
print "</pre>
</section>
</main>


<footer>© 2025 Webserv — Made by 3 weebs</footer>

</body>
</html>";
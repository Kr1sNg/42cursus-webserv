#!/usr/bin/env python3
import sys
import time

# En-tête HTTP minimal
print("Content-Type: text/plain\n")

print("Starting infinite loop...", flush=True)

# Boucle infinie
while True:
    print("Still running...", flush=True)
    time.sleep(1)

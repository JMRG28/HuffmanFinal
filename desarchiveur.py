#!/usr/bin/env python3

import os,sys,subprocess

subprocess.call(["./dehuff",sys.argv[1],"tempfile"])
file_to_decomp=open("tempfile","r",encoding="ISO-8859-1")
tabLignes = file_to_decomp.readlines()
numLines = len(tabLignes)
i=0
while(i < numLines):
    nom = tabLignes[i]
    new_file=open(nom,"w",encoding="ISO-8859-1")
    i+=1
    nbrLignes = tabLignes[i].split("\n")
    n=int(nbrLignes[0])
    i+=1
    for j in range(n):
        new_file.write(tabLignes[i])
        i+=1
    new_file.close()

file_to_decomp.close()

os.remove("tempfile")

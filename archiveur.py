#!/usr/bin/env python3

import os,sys,subprocess

def nbL(fichier):
    count = 0
    curFile= open(fichier,"r",encoding="ISO-8859-1")
    for line in curFile.readlines():
        count+=1
    curFile.close()

    return count

def tree(fichier,sortie):
    #print(fichier)
    liste = os.listdir(fichier)
    for f in liste :
        if os.path.isdir(fichier+"/"+f):
            tree(fichier+"/"+f,sortie)
        else:

            if str(sys.argv[2]) in f:
                comp = False
            else :
                comp = True

            if comp:
                print(fichier+"/"+f)
                currentFile = open(fichier+"/"+f,"r", encoding="ISO-8859-1")
                lines_count = nbL(fichier+"/"+f)
                sortie.write(fichier+"/"+f+"\n")
                sortie.write((str(lines_count)) + "\n" )
                for line in currentFile.readlines():
                    sortie.write(line)
                currentFile.close()


output=open(sys.argv[2],"w",encoding="ISO-8859-1")
tree(sys.argv[1],output)
output.close()
subprocess.call(["./huff",sys.argv[2],sys.argv[2]])
os.remove(sys.argv[2])

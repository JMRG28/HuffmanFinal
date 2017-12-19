#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>

//Converti un entier n en binaire sur t bits (t étant une variable pour le dernier cas ou on aura pas 8bits)
char* convB(int n,int t){
  char* res = (char*)malloc(t);
  for (int i=0;i<t;i++){
    res[i]='0';
  }
  int z=t-1;
  while(n>0){
    res[z]=(n%2) + '0';
    n=n/2;
    z--;
  }
  return res;
  free(res);
}
//fonction qui ajoute un caractère a une chaîne de caractère (dans le but de comparer ensuite la chaîne avec le dictionnaire)
char *  ajoutChar(char*s, char c) {

  int len = strlen(s);
  char* a=(char*)malloc(len +2);
  for(int i=0;i<len;i++){
    a[i]=s[i];
  }
  a[len] = c;
  a[len+1] = '\0';
  return a;
  free(a);
}
void manuel(){
  printf("Pour compresser: \n");
  printf("./huff<espace>nom_du_fichier_a_compresser<espace><nom_de_sortie> \n");
  printf("Pour decompresser: \n");
  printf("./dehuff<espace>nom_du_fichier_a_decompresser<espace><nom_de_sortie> \n");
}
int main(int argc,char** argv){
  if(((argc==1) && !strcmp(argv[1],"-h")) || argc !=3){
    manuel();
exit(EXIT_FAILURE);
  }
  //début de l'horloge (lancement du programme)
  clock_t begin = clock();
  //création/initialisation du tableau d'encodage des caractères ASCII en binaire
  char * codes[256];
  for(int i=0;i<256;i++){
    codes[i] = NULL;

  }
  //Ouverture/Lecture du fichier à décompresser
  FILE* fd = fopen(argv[1],"r+");
  unsigned char buffer[2];

  unsigned char btemp;
  char* co = "";
  int ttemp;
  unsigned stop =1;
  if(fd !=NULL){
    //calcul de la taille du fichier
    double sizeofF=0;
    while(fread(buffer,1,1,fd)){
      sizeofF++;
    }

    //remet la tete de lecture au début
    rewind(fd);
    //réencodage du dictionnaire utilisé pour compresser et donc pour décompresser
    while(stop == 1){
      fread(buffer,1,1,fd);
      btemp = buffer[0];
      fread(buffer,1,1,fd);
      if(buffer[0] =='!' && btemp == '!') {
        stop = 0;
        continue;
      }
      ttemp=(int)buffer[0];
      codes[(int)btemp] = (char*)malloc(ttemp*sizeof(char));
      for(int i=0;i<ttemp;i++){
        fread(buffer,1,1,fd);
        co = ajoutChar(co,buffer[0]);
      }
      strcpy(codes[(int)btemp],co);
      co = "";
    }
    //recherche dans le tableau de codes le premier et le dernier éléments non nuls pour éviter de parcourir tout le tableau pour rien par la suite
    int first=-1;
    int last=-1;
    //longueur du plus petit code bin pour ne pas chercher en dessous
    int minStrL = 257;
    for(int i=0;i<256;i++){
      if(codes[i] != NULL){
        if(first==-1)first = i;
        last =i;
        if(strlen(codes[i])<minStrL){
          minStrL=strlen(codes[i]);
        }
      }
    }
/*on converti les caracteres du fichier comp
 en entier puis en binaire que l'on place dans un fichier temporaire */
    unsigned int entChar;
    char* binary;
    int fin;
    FILE * ftemp = fopen("ctemp","w+");
    while(fread(buffer,1,1,fd)){

      entChar=(unsigned int)buffer[0];

      if(ftell(fd)==sizeofF-1){
        fin = (int)buffer[0];
      }
      else if(ftell(fd)<sizeofF){
        binary=convB(entChar,8);
        fputs(binary,ftemp);
      }
      if(ftell(fd)==sizeofF){
        binary=convB(entChar,fin);
        fputs(binary,ftemp);
      }
    }
    fclose(ftemp);
//fichier final
    FILE * final;
    //fichier temporaire contenant la traduction en binaire
    FILE * tem;
    //chaine de caracteres (binaire) que l'on va comparer avec le dictionnaire
    char * temp_binary="";
    final = fopen(argv[2],"w+");
    tem=fopen("ctemp","r+");
    //on obtient la taille du fichier temporaire pour pouvoir calculer le pourcentage
    struct stat tempF;
    stat("ctemp", &tempF);
    float tf =(float)tempF.st_size;

    char buffer4[2];
    //indice de parcours du dictionnaire
    int y;
    //entier (booleen) pour savoir si on a trouve un code correspondant
    int found;
    int pourcentage;
    int lastPourcentage;
    char * bar = "";
    printf("Decompression en cours \n");
    printf("%s",bar);
    while(fread(buffer4,1,1,tem)){
      pourcentage = (int)((ftell(tem)/tf)*100) ;
      if(pourcentage%5 == 0 && pourcentage != lastPourcentage){
        bar=ajoutChar(bar,'#');
        printf("\r%s %d%%",bar,pourcentage);
        fflush(stdout);
      }
      lastPourcentage=pourcentage;
//on ajoute le caractere present dans buffer4 a la fin de temp_binary
      temp_binary=ajoutChar(temp_binary,buffer4[0]);

      y=first;
      found=0;
      if(strlen(temp_binary)>minStrL-1){
      while(found==0 && y<last+1){
        if (codes[y]!=NULL){
         if(temp_binary[0]==codes[y][0]){
           if(strlen(temp_binary)==strlen(codes[y])){

              if(!strcmp(temp_binary,codes[y])){

                found=1;

                fputc((char)y,final);

                temp_binary="";
              }
            }
         }
        }
        y++;
      }
    }
    }
    fclose(final);
    fclose(tem);
  remove("ctemp");

  }
  fclose(fd);

  printf("\n");
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Temps d'execution: %f secondes \n", time_spent);
  return 0;
}

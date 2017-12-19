#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
// Tableau de fréquence de caractère
double charFreq[256];
//structure de l'arbre
typedef struct{
  int pere;
  int fg;
  int fd;
  double freq;
}noeud;
//converti le buffer en entier avant de l'inserer dans le fichier compréssé
unsigned char convBuff (unsigned char a[],int t){
  unsigned int r=0;
  unsigned char ab;
  for(int i=0;i<t;i++){
    r+= ((unsigned int)(a[i] - '0')) * pow(2,(t-1-i));
  }
  ab=(unsigned char)(r);
  return ab;
}
//ajoute le caractère c a la fin de la chaine de caractere
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
//déplace la tête de lecture de a char vers la gauche
void resetBuff(unsigned char* s,int a){
  int len = strlen((char*)s);
  char* res1=(char*)malloc(len);
  for(int j=0;j<len;j++){
    res1[j]='\0';
  }
  for(int i=0;i<a;i++){
    res1[i]=s[len-a+i];
  }
  for(int j=0;j<len;j++){
    s[j]=res1[j];
  }
  free(res1);
}
//calcul la fréquence d'apparition de chaque caractères dans le fichier texte a compresser
void calFreq (char* file){
  unsigned char buffer[2];
  double count=0;
  for(int i=0;i<256;i++){
    charFreq[i]=0;
  }
  FILE* fd =fopen(file,"r");
  if(fd!=NULL){
    while(fread(buffer,1,1,fd)){
      charFreq[(unsigned int)buffer[0]]++;
      count++;
    }
    //  printf("%f\n",count);
    fclose(fd);
    for(int i=0;i<256;i++){
      charFreq[i]=(double)charFreq[i]/count;
    }
  }
  else{
    printf("ERREUR\n");
  }
}
// FONCTION DEBBOGAGE: affiche un arbre
void printA(noeud a[],int n){
  for(int i=0;i<n;i++){
    printf("indice: %d,pere: %d, fg: %d, fd: %d, freq: %f \n",i,a[i].pere,a[i].fg,a[i].fd,a[i].freq);
    printf("---------------------------------------------- \n");
  }
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
  //renvoie le temps au moment où le programme est lancé
  clock_t begin = clock();
  //pour obtenir la taille initiale du fichier
  struct stat original;
  stat(argv[1], &original);
  float or =(float)original.st_size;
  //dictionnaire de codes binaires (tableau de chaine de char)
  char * dict[256];

  char* outName = "";
  //tableau de noeuds (arbre)
  noeud arbre[511];
  //initialisation du dictionnaire a NULL
  for(int i=0;i<256;i++){
    dict[i]=NULL;
  }
  //calcule la frequence de chaque caracteres du fichier mis en parametre
  calFreq(argv[1]);
  //initialisation de l'arbre
  for(int i=0;i<256;i++){
    arbre[i].pere=-1;
    arbre[i].fg=-1;
    arbre[i].fd=-1;
    arbre[i].freq=charFreq[i];
  }
  for(int i=256;i<511;i++){
    arbre[i].pere=-1;
    arbre[i].fg=-1;
    arbre[i].fd=-1;
    arbre[i].freq=0.0;
  }
  // p: premiere case 'libre' de l'arbre (car 256 caractères)
  int p =256;
  //construction de l'arbre
  do{
    float min1=2;
    int saveind1=-1;
    for(int j=0;j<511;j++){
      if(arbre[j].freq<min1 && arbre[j].freq !=0 ){
        min1=arbre[j].freq;
        saveind1=j;
      }
    }
    arbre[saveind1].freq=0;

    float min2=2;
    int saveind2=-1;
    for(int k=0;k<511;k++){
      if(arbre[k].freq<min2 && arbre[k].freq !=0 ){
        min2=arbre[k].freq;
        saveind2=k;
      }
    }
    arbre[saveind2].freq=0;

    arbre[saveind1].pere=p;
    arbre[p].fg=saveind1;
    //pour eviter des erreurs de segmentation si le fichier ne contient qu'un seul caractere
    if (min2<2){
      arbre[saveind2].pere=p;
      arbre[p].fd=saveind2;
      arbre[p].freq=min1+min2;
    }
    else{
      arbre[p].freq=min1;
    }
    p++;
  }while(arbre[p-1].freq<0.99);

  //fontion qui parcours l'arbre afin de construire les codes binaires
  void parcours(int i,char* code){
    if(arbre[i].fg!=-1){
      parcours(arbre[i].fg,ajoutChar(code,'1'));
      parcours(arbre[i].fd,ajoutChar(code,'0'));
    }
    else {
      dict[i] = (char*)malloc(strlen(code)*sizeof(char));
      strcpy(dict[i],code);
    }
  }
  //effectue le parcours a partir de p-1 : la racine de l'arbre
  parcours(p-1,"");

  //fichier comp
  FILE *fp;
  //ajout de l'extension ".huff" a la fin du fichier comp
  char* filename = (char*)malloc(strlen(argv[2]) + 5);
  outName = (char*)malloc(strlen(argv[2]) + 5);
  strcpy(filename,argv[2]);
  strcat(filename,".huff");
  strcpy(outName,filename);
  printf("Nom du fichier de sortie : %s\n",outName);
  fp = fopen(filename, "w+");

  //on place le dictionnaire au debut du fichier comp
  for(int i=0;i<256;i++){
    if(dict[i] != NULL){
      fputc((char)i,fp);
      fputc(strlen(dict[i]),fp);
      fputs(dict[i],fp);
    }
    if(i==255)  fputs("!!",fp);
  }
  /*buffer temporaire de taille 263 car un code binaire peut ,au max, contenir 256
  caracteres et le buffer peut deja en contenir 7 (256+7=263) */
  unsigned char buffer3 [263];
  for (int i=0;i<263;i++){
    buffer3[i]='\0';
  }

  //taille du buffer courant (a l'etape n)
  int bsize=0;
  //taille du buffer a l'etape n-1
  int lastBS=0;
  //caractere qui sera insere dans le fichier comp
  unsigned char c;
  //pourcentage de compression ... pour la barre de chargement
  int pourcentage;
  int lastPourcentage;
  //barre de chargement
  char * bar = "";
  printf("Compression en cours \n");
  printf("%s",bar);
  unsigned char buffer2[2];
  FILE* fd2 =fopen(argv[1],"r");
  if(fd2!=NULL){
    while(fread(buffer2,1,1,fd2)){
      pourcentage = (int)((ftell(fd2)/or)*100) ;
      if(pourcentage%5 == 0 && pourcentage != lastPourcentage){
        bar=ajoutChar(bar,'#');
        printf("\r%s %d%%",bar,pourcentage);
        //permet d'afficher la barre sans avoir a utiliser un '\n' ... debuffurise
        fflush(stdout);
      }
      lastPourcentage=pourcentage;
      //actualisationn de la taille du buffer3
      bsize+=strlen(dict[(unsigned int)buffer2[0]]);
      //insertion du code du caractere courant dans le buffer3
      for(int z=lastBS;z<bsize;z++){
        buffer3[z]=dict[(unsigned int)buffer2[0]][z-lastBS];
      }
      lastBS=bsize;
      /*des que la taille du buffer >= 8 , on converti le contenu (8bits) en
      entier puis en un caractère ASCII, que l'on place dans le fichier comp  */
      while(bsize >=8){
        c=convBuff(buffer3,8);
        fputc(c, fp);
        bsize-=8;
        lastBS-=8;
        resetBuff(buffer3,bsize);
      }

    }
    //on vide les derniers caracteres qui sont restes dans le buffer a la fin
    resetBuff(buffer3,bsize);
    fputc(bsize,fp);
    c=convBuff(buffer3,bsize);
    fputc(c, fp);
  }
  fclose(fd2);
  fclose(fp);
  printf("\n");
  //pour obtenir la taille du fichier comp
  struct stat compressed;
  stat(outName, &compressed);
  float co = (float)compressed.st_size;

  printf("Taille du fichier original : %.0f octets \nTaille du fichier compressE: %.0f octets \n",or ,co );
//calcul du gain
  float gain = (1-(co/or)) *100;
  printf("Gain: %.2f %% \n",gain);
  //renvoie le temps a la fin du programme
  clock_t end = clock();
  //calcul du temps écoulé
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Temps d'execution: %f secondes \n", time_spent);
  return 0;
}

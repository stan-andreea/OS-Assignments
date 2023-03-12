#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>


typedef struct _SECTION_HEADER{
    char SECT_NAME[50];
    int SECT_TYPE;
    int SECT_OFFSET;
    int SECT_SIZE;

}SECTION_HEADER;



typedef struct _HEADER{
    int version;
    unsigned int no_of_sections;
    SECTION_HEADER *header_sections;
    int header_size;
    char magic[5];

}HEADER;



int checkEnding(char *string, char* ending){
    int lengthString=strlen(string);
    int lengthEnding=strlen(ending);


    if(lengthEnding>lengthString)
        return 0;
    
    for(int i=0;i<lengthEnding;i++){
        if(string[i+lengthString-lengthEnding]!=ending[i])
            return 0;
    }
    return 1;

}


int checkStart(char *string, char* start){
    int lengthString=strlen(string);
    int lengthStart=strlen(start);


    if(lengthStart>lengthString)
        return 0;
    
    for(int i=0;i<lengthStart;i++){
        if(string[i]!=start[i])
            return 0;
    }
    return 1;

}


void listNonRec(char *dir_name, int filter, char *ending){

    DIR* dir;
    struct dirent *dirEntry;
    char name[1000];
    struct stat buffer;


    dir = opendir(dir_name);
    if (dir == 0) {
        printf("ERROR\ninvalid directory path\n");
        exit(4);
    }

    printf("SUCCESS\n");
    while ((dirEntry=readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!= 0){
            sprintf(name,"%s/%s",dir_name,dirEntry->d_name);
            lstat(name, &buffer);
            if(filter==0) 
                printf("%s\n", name);
            else if(filter==1 && checkEnding(name,ending)==1)
                printf("%s\n", name);
            else if(filter==2 && buffer.st_mode & 0100)
                printf("%s\n", name);
        }
       }

    closedir(dir);
  
}

void listRec(char *dir_name, int filter, char *ending){

    DIR* dir;
    struct dirent *dirEntry;
    char name[500];
    struct stat buffer;


    dir = opendir(dir_name);
    if (dir == 0) {
        printf("ERROR\ninvalid directory path\n");
        exit(4);
    }
    
    
     while( (dirEntry = readdir(dir))!=0 ){
                if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!= 0){
                    sprintf(name,"%s/%s",dir_name,dirEntry->d_name);
                    lstat(name, &buffer);
                    if(filter==0) 
                        printf("%s\n", name);
                    else if(filter==1 && checkEnding(name,ending)==1)
                        printf("%s\n", name);
                    else if(filter==2 && buffer.st_mode & 0100)
                        printf("%s\n", name);
    
                    if(dirEntry->d_type == DT_DIR)
                    {
                        listRec(name,filter,ending);                    
                    }
                }
            }
    

    closedir(dir);
     
}

int checkHeaderExtract(HEADER header){
    
    if(strcmp(header.magic,"yeEI")!=0){
        printf("ERROR\nwrong magic\nmagic: %s\n",header.magic);
        exit(4);
    }
    if(header.version<121 || header.version>153){
        printf("ERROR\nwrong version\n");
        exit(4);
    }
    if(header.no_of_sections<4 || header.no_of_sections>17){
        printf("ERROR\nwrong sect_nr\n");
        exit(4);
    }
    int array[]={20,53,21,17,97,43,97};
    int ok=1;
    for(int i=0;i<header.no_of_sections && ok==1 ;i++){
    
    int okk=0;
    
    for(int j=0;j<7;j++){
        if(header.header_sections[i].SECT_TYPE==array[j]){
            okk=1;
        }
    }
    if(okk==0) ok=0;

    }
    if(ok==0){
        printf("ERROR\nwrong sect_types\n");
        exit(4);
    }
    return 1;
}


int checkSF(HEADER header){
    
    if(strcmp(header.magic,"yeEI")!=0){
       return 0;
    }
    if(header.version<121 || header.version>153){
       return 0;
    }
    if(header.no_of_sections<4 || header.no_of_sections>17){
        return 0;
    }
    int array[]={20,53,21,17,97,43,97};
    int ok=1;
    for(int i=0;i<header.no_of_sections && ok==1 ;i++){
    
    int okk=0;
    
    for(int j=0;j<7;j++){
        if(header.header_sections[i].SECT_TYPE==array[j]){
            okk=1;
        }
    }
    if(okk==0) ok=0;

    }
    if(ok==0){
        return 0;
    }
    return 1;
}


int checkHeader(HEADER header){
    if(strcmp(header.magic,"yeEI")!=0){
        free(header.header_sections);
        printf("ERROR\nwrong magic\n");
        exit(4);
    }
    if(header.version<121 || header.version>153){
        free(header.header_sections);
        printf("ERROR\nwrong version\n");
        exit(4);
    }
    if(header.no_of_sections<4 || header.no_of_sections>17){
        printf("ERROR\nwrong sect_nr\n");
        free(header.header_sections);
        exit(4);
    }
    int array[]={20,53,21,17,97,43,97};
    int ok=1;
    for(int i=0;i<header.no_of_sections && ok==1 ;i++){
    
    int okk=0;
    
    for(int j=0;j<7;j++){
        if(header.header_sections[i].SECT_TYPE==array[j]){
            okk=1;
        }
    }
    if(okk==0) ok=0;

    }
    if(ok==0){
        free(header.header_sections);
        printf("ERROR\nwrong sect_types\n");
        exit(4);
    }
    return 1;
}



int read_header(char *path, HEADER *header)
{
    int fd = open(path, O_RDONLY);

    char magic[5];
    lseek(fd,-4, SEEK_END);
    read(fd,magic,4);
    magic[4]=0;
    strcpy(header->magic,magic);


    int buffer=0;
    lseek(fd,-6, SEEK_END);
    read(fd,&buffer,2);
    header->header_size=buffer;


    lseek(fd,-header->header_size,SEEK_END);
    int version=0;
    read(fd,&version,4);
    header->version=version;


    int sections=0;
    read(fd,&sections,1);
    header->no_of_sections=sections;
    

    char name[7];
    int type=0,offset=0, size=0;

    header->header_sections=realloc(header->header_sections,sections*sizeof(SECTION_HEADER));

    for(int i=0;i<sections;i++){
        read(fd,name,6);
        name[6]=0;
        strcpy(header->header_sections[i].SECT_NAME,name);
   
        read(fd,&type,4);
        header->header_sections[i].SECT_TYPE=type;

        read(fd,&offset,4);
        header->header_sections[i].SECT_OFFSET=offset;
 
        read(fd,&size,4);
        header->header_sections[i].SECT_SIZE=size;

    }


    return 1;

}


void sectionFiltering(char *dir_name){

    DIR* dir;
    struct dirent *dirEntry;
    char name[500];
    HEADER header;
    header.header_sections=(SECTION_HEADER*)calloc(sizeof(SECTION_HEADER),1);


    dir = opendir(dir_name);
    if (dir == 0) {
        printf("ERROR\ninvalid directory path\n");
        exit(4);
    }
    
    
     while( (dirEntry = readdir(dir))!=0 ){
                if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!= 0){
                    sprintf(name,"%s/%s",dir_name,dirEntry->d_name);
                    
                   read_header(name,&header);
                   int fd=open(name, O_RDONLY);
                   int ok=0;
                   char *aux=NULL;
                   for(int i=0;i<header.no_of_sections;i++){
                        lseek(fd,header.header_sections[i].SECT_OFFSET, SEEK_SET);

                        aux=realloc(aux,(header.header_sections[i].SECT_SIZE+1)*sizeof(char));

                        read(fd,aux,header.header_sections[i].SECT_SIZE);
                
                        char *p=NULL;
                        aux[header.header_sections[i].SECT_SIZE]=0;
                        p=strtok(aux,"\r\n");
                        
                        int count=0;
                       
                        while (p)
                        {   count++;
                            p=strtok(NULL,"\r\n");
                        }

                        if(count==13) ok=1;
                        //printf("%d %d\n",count,i);
                        
                   }

                   free(aux);

                   if(ok==1 && checkSF(header)==1) printf("%s\n",name);
                    
                    if(dirEntry->d_type == DT_DIR)
                    {
                        sectionFiltering(name);                   
                    }
                    
                    }
                }
            
    free(header.header_sections);

    closedir(dir);
     
}



int main(int argc, char **argv){

    HEADER header;

    if(argc >= 2){
        
        if(strcmp(argv[1], "variant") == 0){
            printf("22768\n");
            return 0;
        }

        for(int i=1;i<argc;i++){
            
            if(strcmp(argv[i],"list")==0){
                char *path=NULL;
                char *aux=NULL;
                int recursive=0;
                int filter=0;
                for(int j=1;j<argc;j++){
                    if(strcmp(argv[j],"recursive")==0){
                        recursive=1;
                    }
                   else if(checkStart(argv[j],"path=")){
                        path=argv[j]+5;
                    }
                   else if(checkStart(argv[j],"name_ends_with=")){
                        filter=1;
                        aux=argv[j]+15;
                        //printf("%s\n",aux);
                        
                    }
                   else  if(strcmp(argv[j],"has_perm_execute")==0){
                        filter=2;
                    }
                }

               if(!recursive)
               listNonRec(path,filter,aux);
               else{
                

                DIR* dir;
                dir = opendir(path);
              
                if (dir != 0) printf("SUCCESS\n");
                  closedir(dir);
                listRec(path,filter,aux);
               

               }
             return 0;
            }
            
            else if(strcmp(argv[i],"parse")==0){
                char *path=NULL;
                for(int j=1;j<argc;j++){
                    if(checkStart(argv[j],"path=")){
                            path=argv[j]+5;
                           
                    }

            }

                 header.header_sections=(SECTION_HEADER*)malloc(sizeof(SECTION_HEADER));
                read_header(path,&header);
                checkHeader(header);
                printf("SUCCESS\n");
                printf("version=%d\n", header.version);
                printf("nr_sections=%d\n", header.no_of_sections);

                for(int i=0;i<header.no_of_sections;i++){
                    printf("section%d: %s %d %d\n",i+1,header.header_sections[i].SECT_NAME, header.header_sections[i].SECT_TYPE, header.header_sections[i].SECT_SIZE) ; 
                                   }
                printf("\n");
                free(header.header_sections);
                return 0;

            }

            else if(strcmp(argv[i],"extract")==0){
                char *sect, *ln, *path;
                int section,line;
                for(int j=1;j<argc;j++){
                    if(checkStart(argv[j],"path=")){
                            path=argv[j]+5;
                          
                    }

                    if(checkStart(argv[j],"section=")){
                            sect=argv[j]+8; 
                            section=atoi(sect);
                    }

                     if(checkStart(argv[j],"line=")){
                            ln=argv[j]+5;
                            line=atoi(ln);
                    }
            }

            int fd;
            fd = open(path, O_RDONLY);
              
            if (fd == -1) {
                printf("ERROR\ninvalid file\n");
                exit(4);
            }
            
            header.header_sections=(SECTION_HEADER*)malloc(sizeof(SECTION_HEADER));
            read_header(path,&header);
            checkHeaderExtract(header);
            if(section>header.no_of_sections){
                printf("ERROR\ninvalid section\n");
                exit(4);
            }

            lseek(fd,header.header_sections[section-1].SECT_OFFSET, SEEK_SET);

            char *aux=(char*)calloc(sizeof(char),(header.header_sections[section-1].SECT_SIZE+1));

            read(fd,aux,header.header_sections[section-1].SECT_SIZE);
     
            char *p=NULL;
            p=strtok(aux,"\r\n");

            int count=1;
            int ok=0;
            while (p && !ok)
            {
                if(count==line){
                    printf("SUCCESS\n");
                    for(int i=strlen(p)-1;i>=0;i--)
                        printf("%c",p[i]);
                    printf("\n");
                    ok=1;
                }
                count++;
                p=strtok(NULL,"\r\n");
            }
            if(ok==0){
                printf("ERROR\ninvalid line\n");
                exit(4);
            }

            free(aux);
            free(header.header_sections);

            close(fd);
            return 0;

        }

        else if(strcmp(argv[i],"findall")==0){
                char *path;
                for(int j=1;j<argc;j++){
                    if(checkStart(argv[j],"path=")){
                            path=argv[j]+5;
                          
                    }
            }

       
            DIR* dir;
            dir = opendir(path);
              
            if (dir != 0) printf("SUCCESS\n");
                  closedir(dir);
            sectionFiltering(path);
            return 0;

        }

        }

       
     }
    
    return 0;
}
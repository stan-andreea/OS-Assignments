#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>


int freq, fresp;

void write_response(char *message, bool status){
  
    int length=strlen(message);
      printf("length is %d\n\n",length);
    write(fresp, &length,1);
    write(fresp,message,length);
    if(status){
        length=7;
        write(fresp, &length,1);
        write(fresp,"SUCCESS",7);
    }else{
        length=5;
        write(fresp, &length,1);
        write(fresp,"ERROR",5);
    }

}

int main(){
   
    if(mkfifo("RESP_PIPE_22768", 0600)!=0){
        printf("ERROR\ncannot create the response pipe\n");   
        exit(1);
    }

    freq = open("REQ_PIPE_22768", O_RDONLY);
    fresp = open("RESP_PIPE_22768", O_WRONLY);

    if (fresp < 0) {
        printf("ERROR\ncannot open the response pipe\n");   
        exit(1);
    }

     if (freq < 0) {
        printf("ERROR\ncannot open the request pipe\n");   
        exit(1);
    }
    

    char length=7;

    write(fresp, &length,1);
    write(fresp, "CONNECT", 7);
    printf("SUCCESS\n");
    int size;
    char command[100];
    int shm_fd;
    int fd;
    char *shared_data;
    char *data;
    int header_size=0,version,no_of_sections=0;
    int section_size,section_offset;
    int nr;
    while(1){
        
        
  
        char len;
        read(freq,&len,1);
        int n=read(freq,command,(int)len);
        command[n]=0;
  

        if(strcmp(command,"EXIT")==0){
            break;
        }
        else if(strcmp(command,"PING")==0){
            char l=4;
            write(fresp, &l,1);
            write(fresp,"PING",4);
            write(fresp, &l,1);
            write(fresp,"PONG",4);
            unsigned int nr=22768;
            write(fresp,&nr,sizeof(unsigned int));
        }
        else if(strcmp(command,"CREATE_SHM")==0){
           
            read(freq,&nr,4);
            
            shm_fd = shm_open("/2EjJwK", O_CREAT | O_RDWR, 0664);
            if(shm_fd < 0) {
                write_response("CREATE_SHM",false);
            }else{

                ftruncate(shm_fd, nr);

                shared_data  = (char*)mmap(0, nr*sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
                if(shared_data == MAP_FAILED){
                    write_response("CREATE_SHM",false);
                }else{
                    write_response("CREATE_SHM",true);
                }
            }
        }

        else if(strcmp(command,"WRITE_TO_SHM")==0){
            unsigned int offset,value;
        
            read(freq,&offset,4);
            read(freq,&value,4);

            if(!(offset>=0 && offset<=2407090)){
                write_response("WRITE_TO_SHM",false);
            }else{

                if(!((offset+sizeof(value))>=0 && (offset+sizeof(value))<=2407090)){
                    write_response("WRITE_TO_SHM",false);
                }else{
                
                    memcpy(shared_data+offset,  (char*)&value, sizeof(value));
                    write_response("WRITE_TO_SHM",true);
                }
            }
        }

        else if(strcmp(command,"MAP_FILE")==0){
            char len;
            char path[100];
            read(freq,&len,1);
            n=read(freq,path,(int)len);
            path[n]=0;

            fd = open(path, O_RDWR);
            
            if(fd == -1) {
                write_response("MAP_FILE",false);
            }else{
            
            size = lseek(fd, 0, SEEK_END);
            lseek(fd, 0, SEEK_SET);

            data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
            if(data == (void*)-1) {
                write_response("MAP_FILE",false);
	        }else{
                write_response("MAP_FILE",true);
            }
            }
        }

        else if(strcmp(command,"READ_FROM_FILE_OFFSET")==0){
            unsigned int offset,no_of_bytes;

            read(freq,&offset,4);
            read(freq,&no_of_bytes,4);
           
            char aux[10];

            if((offset+no_of_bytes)>size || fd==-1 || data == (void*)-1 || shared_data == MAP_FAILED) {
                write_response("READ_FROM_FILE_OFFSET",false);

            }else{
                memcpy(aux, data+offset, no_of_bytes);
                memcpy(shared_data, aux, no_of_bytes);
                write_response("READ_FROM_FILE_OFFSET",true);
            }
        }

        else if(strcmp(command,"READ_FROM_FILE_SECTION")==0){
            unsigned int offset,no_of_bytes,section_no;
           
            read(freq,&section_no,4);
            read(freq,&offset,4);
            read(freq,&no_of_bytes,4);
           
            
            memcpy(&header_size, data+(size-6), 2);
            memcpy(&version,data+size-header_size,4);
            memcpy(&no_of_sections,data+size-header_size+4,1);
            memcpy(&section_offset,data+size-header_size+5+(section_no-1)*18+10,4);
            memcpy(&section_size,data+size-header_size+5+(section_no-1)*18+14,4);
           
            if(!(section_no>=1 && section_no<=no_of_sections)) {
                write_response("READ_FROM_FILE_SECTION",false);
            }
            else{
    
                memcpy(shared_data, data+section_offset+offset, no_of_bytes);
                write_response("READ_FROM_FILE_SECTION",true);
            }

        }

        else if(strcmp(command,"READ_FROM_LOGICAL_SPACE_OFFSET")==0){
            unsigned int logical_offset,no_of_bytes;
           
            read(freq,&logical_offset,4);
            read(freq,&no_of_bytes,4);
           
            
            memcpy(&header_size, data+(size-6), 2);
            memcpy(&version,data+size-header_size,4);
            memcpy(&no_of_sections,data+size-header_size+4,1);

            int aux_offset;
            int start_logical=0;
            int nr_pages=0;

            for(int i=0;i<no_of_sections;i++){
                memcpy(&section_size,data+size-header_size+5+i*18+14,4);
                memcpy(&aux_offset,data+size-header_size+5+i*18+10,4);
                if(section_size%2048==0){
                    nr_pages+=section_size/2048;
                }
                else{
                    nr_pages+=section_size/2048+1;
                }
                if(nr_pages*2048>logical_offset)
                    break;
                else 
                    start_logical=nr_pages*2048;
            }
     

            int start;
            start=logical_offset-start_logical;


            if(start+no_of_bytes>section_size || fd==-1 || data == (void*)-1 || shared_data == MAP_FAILED){
                write_response("READ_FROM_LOGICAL_SPACE_OFFSET",false);
            }else{

                memcpy(shared_data, data+aux_offset+start, no_of_bytes);
                write_response("READ_FROM_LOGICAL_SPACE_OFFSET",true);
            }
            
        }

    }
    close(fresp);
    close(freq);
    munmap(shared_data,nr);
    munmap(data,size);
    unlink("RESP_PIPE_22768");
    close(shm_fd);
    close(fd);
    return 0;
}
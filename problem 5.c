#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int read_bytes(int my_file, void* buf, int n) {

	int rr = 0;
    int a = 0;
    int bytes = 0;
    while(bytes != n){
		n = n - bytes;
		rr = read(my_file,buf, n);
        
        if (rr == -1) {
            if (errno == EINTR || errno == EAGAIN)  {
				if (a == -1)
					return -1;
				a = -1;
                continue; 
            }
            else {
                perror("Error while reading!");
				return -2;
            }
        }
		a = 0;
        if(rr == 0) {
            perror("Error! End of file You enter wrong value.");
	        return 0;
        }
       bytes+= rr;
       buf+= rr; 
	
    }
}

int create_rows_table (int file_size, char* file_data, off_t* offsets, off_t* line_length)
{
	int line_number = 0;
	int i;
	for(i = 0; i < file_size; i++) {
        if( file_data[i] == '\n') {
             
            if(line_number == 0) {
                offsets[line_number] = 0;
                line_length[line_number] = i+1;
            }
            else {
                offsets[line_number] = offsets[line_number - 1] + line_length[line_number - 1];
                line_length[line_number] = i + 1 - offsets[line_number];
            }
            line_number++;
        }
    }
	return line_number;
}

int main(int argc , char* argv[])
{
    int my_file;
	int i;
    int line_number = 0;
    int need_line = 1;
    if((my_file = open(argv[1],O_RDONLY)) == -1) {
        perror("File is not open\n");
        return 1;
    }  
    int file_size = lseek(my_file, 0, 2);
    lseek(my_file,0,0);
    char* file_data = malloc(sizeof(char)*file_size);
	off_t* offsets = malloc(sizeof(off_t)*file_size);
	off_t* line_length = malloc(sizeof(off_t)*file_size);
    read_bytes(my_file,file_data, file_size);
    line_number = create_rows_table(file_size, file_data, offsets, line_length);
   
    printf("count of lines : %d\n ", line_number);
    while(1){
        printf("What line do you need? ");
        scanf("%d", &need_line);
        if(need_line == 0) {
            if(close(my_file) == -1) perror("Error while closing");
            return 0;
        }
        if((need_line > line_number) || (need_line < 0)) {
            perror("Error! We haven't this line!");
			continue;
        }
        for( i = offsets[need_line -1]; i < offsets[need_line -1] + line_length[need_line - 1]; i++)
            printf("%c", file_data[i]);
    }
}
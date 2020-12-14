#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#define BUF_SIZE 100
#define LINE_SIZE 100

int create_rows_table (off_t* offsets, char* buf, int file_size)
{
	int line_number = 1;
	int i;
	int a = 0;
	int line_length;
	
	for(i = 0; i < file_size; i++) {
		if( buf[i] == '\n') {
		offsets[line_number] = i+1;
		line_number++;
		}
	}
	return line_number - 1;
}

int print_row(off_t* offsets, int line_number, char* buf, int my_file){
	int need_line;
	int i;
	while(1){
        printf("What line do you need? ");
        scanf("%d", &need_line);
        if(need_line == 0) {
			printf("Goodbye!\n");
            return 0;
        } else{
			if((need_line > line_number) || (need_line < 0)) {
				perror("Error! We haven't this line!");
				continue;
			} else{
				for(i = offsets[need_line-1]; i < offsets[need_line]; ++i){
					printf("%c",buf[i]);
				}
			}
		}
    }
}


int main(int argc , char* argv[])
{
    int my_file;
	int i;
    int line_number = 0;
	
    if((my_file = open(argv[1],O_RDONLY)) == -1) {
        perror("File is not open\n");
        return -1;
    }  
	int file_size = lseek(my_file,0,SEEK_END);
	char* buf;
	if ((buf = mmap(0,file_size,PROT_READ, MAP_SHARED, my_file, 0)) == MAP_FAILED)
	{
		printf("Error of mmap\n");
		close(my_file);
		return -2;
	}
    
	off_t* offsets = malloc(sizeof(off_t) * BUF_SIZE);
	if(offsets == NULL){
		close(my_file);
		munmap(buf, file_size);
		return -3;
	}
    line_number = create_rows_table(offsets, buf, file_size);
    printf("count of lines : %d\n ", line_number);
	print_row(offsets, line_number, buf, my_file);
  
	free (offsets);
	close(my_file);
	munmap(buf, file_size);
}

/*
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "mem_collector.h"
#include "fkw_seg.h"

void print_time(const char *name){
	time_t now;
	now = time(NULL);

	struct tm *now_tm;
	now_tm = localtime(&now);

	char buf[64];
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S\n", now_tm);
	printf("%s\t%s", name, buf);
}

int main(int argc, char **argv)
{
	char dict_path[]  = "word.txt";
	char text_path[]  = "mogu.txt";

	char new_word[] =  "new_word";

	int hash_size = 50;
	unsigned char buf[8192];

	int fd;
	ssize_t read_size;
	int j;

	fkw_word_list_t *word_list;
	fkw_word_t *start;

	fkw_str_t text;

	fkw_dict_array_t *dict_array;

	dict_array = fkw_dict_array_init(hash_size);

	print_time("load dict start");
	dict_array = fkw_load_dict(dict_array, dict_path);
	fkw_add_dict_word(dict_array, new_word, strlen(new_word));
	print_time("load dict end");
	
	fd = open(text_path, O_RDONLY);
	read_size = read(fd, buf, sizeof(buf));
	printf("read_size=%d\n", read_size);

	text.len  = read_size;
	text.data = buf;

	print_time("before seg");
	word_list = fkw_full_seg(&text, dict_array);
	print_time("end seg");

	//return;
	if (word_list->start == NULL){
		close(fd);
		printf("no found");
		return 0;
	}

	start = word_list->start;
	while(start){
		printf("%s ", start->value.data);
		start = start->next;
	}

	if (fkw_dict_search(dict_array, new_word, strlen(new_word))){
		printf("found new_word\n");
	} else {
		printf("no found new word\n");
	}

	// free res
	close(fd);

	fkw_free_dict(&dict_array);

	fkw_free_word_list(&word_list);

	return 0;
}

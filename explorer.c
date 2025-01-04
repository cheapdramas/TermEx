#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int string_compare(char *a, char *b){
	return strcmp(a,b);
}


struct dirent** directory_entries(char *path,int *dirlen){
	DIR *dir;
	struct dirent *entry;
	//count files
	dir = opendir(path);	
	while ((entry = readdir(dir))!= NULL){
		char *file_name = (entry->d_name);
		if (string_compare(file_name,".") && string_compare(file_name,"..")){
			*dirlen+=1;
		}
	}
	closedir(dir);

	//build array with directory entries
	int counter = 0;
	dir = opendir(path);	

	struct dirent** entries = malloc(sizeof(struct dirent *) * (*dirlen));
	while ((entry = readdir(dir))!= NULL){
		char *file_name = (entry->d_name);
		if (string_compare(file_name,".") && string_compare(file_name,"..")){
			entries[counter] = entry;
			counter +=1;
		}
	}	


	closedir(dir);

	return entries;
}

void update_win(){
	clear();
	refresh();
}

void quit(){
	endwin();
	exit(EXIT_SUCCESS);
}

void draw_files(struct dirent** dir_entries,int dirlen,int current_item){
	for (int i =0;i<dirlen;i++){
		struct dirent *entry = dir_entries[i];

		char *filename = entry->d_name;
		char *filetype = "F";
		if (entry->d_type == DT_DIR){filetype="D";}		

		
		
		if (i == current_item){
			printw("%s  --> %s\n",filetype,filename);
		}else{
			printw("%s      %s\n",filetype,filename);
		}

	}
}

void user_controls(char user_input,int* current_item,int dirlen){
	if (user_input == *"j" && *current_item < dirlen - 1){
		*current_item += 1;
	}
	if (user_input == *"k" && *current_item > 0){
		*current_item -= 1;
	}
	if (user_input == *"q"){
		quit();
	}


}




void main(){
	initscr();
	//draw related
	int current_item = 0;

	//files related	
	char *path = ".";
	int dirlen = 0;
	struct dirent**  dir_entries=directory_entries(path,&dirlen);	

	
	//screen related
	curs_set(0);
	int maxX = 0;
	int maxY = 0;



	//main loop
	while(1){
		draw_files(
				dir_entries,
				dirlen,
				current_item
		);





		char user_input = getch();
		user_controls(user_input,&current_item,dirlen);	

		update_win();
	}
	

	quit();
}



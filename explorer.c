#include <linux/limits.h>
#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


//fileactions
//d = delete
//e = enter
//f = find


//here we will display the files,here we will know which file did user select and etc..
void file_control(
	int *highlight,
	int *dirlen,
	int maxY,
	int *do_not_show,
  char **fileaction,
  char *file_name
)
{
  nodelay(stdscr,false);
  DIR *dir;
	struct dirent *entry;
	dir = opendir(".");
	*dirlen=0;

		

	int iterator = 0;
	//main loop for file manipulations
	while ((entry = readdir(dir))!= NULL)
	{	
				
		char *filename = (entry->d_name);
		char *filetype = "";
    //detecting file type
    switch((entry->d_type)){
      case DT_REG:
        filetype="F";
        break;
      case DT_DIR:
        filetype="D";
        break;
      case DT_LNK:
        filetype="D";
        break;
      default:
        break;
    }







		// if entry is not . and .. dir,than:
		if (strcmp(filename,".") && strcmp(filename,"..")){
			//clearing files at the top
			if (iterator > *do_not_show)
			{
				if (iterator < (maxY -3) +*do_not_show)
				{
				
					if (iterator == *highlight)
					{
						attron(A_REVERSE);
					}
					mvprintw(iterator+(-1*(*do_not_show)),1,"%s  %s",filetype,filename);
					attroff(A_REVERSE);
					refresh();
				}
			}

      if (*fileaction == "d"){
        if (*highlight == iterator){
          remove(filename);
          *fileaction="";
          clear();
          nodelay(stdscr,true);
        if (*highlight > 0){
          *highlight -= 1;
        }
        iterator --; 
        }
      }
      if (*fileaction == "e"){
        if (*highlight==iterator){
          if (strcmp(filetype,"F") == 0){
            char *syscall = malloc( sizeof(char) * (strlen(filename) + 6)  );
            int formatting= snprintf(syscall,sizeof(char) * strlen(filename) + 6,"lvim %s",filename);


            def_prog_mode();
            endwin();
            system(syscall);
            refresh();
            free(syscall);
          }else{
            
            chdir(filename);
            clear();
            *fileaction="";
            nodelay(stdscr,true);
            *highlight = 0;
            *do_not_show = -1;

            break;
            
          }

          *fileaction = "";
        }
      }
      //display found file
      if (strcmp(*fileaction,"f")==0){
        if (strcmp(filename,file_name)==0){
          free(file_name);
          *highlight= iterator;

          if (*highlight > (maxY -4) + *do_not_show){
            *do_not_show += (*highlight - *do_not_show) - 1;
          }
          if (*highlight <= *do_not_show){
            *do_not_show = *highlight -1;
          }
            
  

          nodelay(stdscr,true);
          clear();
          refresh();
        }
      }

    
			iterator ++;
    }
	}	


	closedir(dir);
	//directoruy length is our iterator by the end of the loop
	*dirlen = iterator;
}

char *fzf(){
  FILE *fp;
  char *filename = malloc(PATH_MAX);


  fp = popen("find . -maxdepth 1 | sed 's/^..//' | fzf","r");
  if (fp == NULL){
    return NULL;
  }
  fgets(filename,PATH_MAX,fp);
  filename[strlen(filename) -1] = 0;
  pclose(fp);
  return filename;

  
}


void user_controls(
	int user_input,
	int *highlight,
	int dirlen,
	int maxY,
	int *do_not_show,
  char **fileaction,
  char **file_name
)
{
	//arrows pressed check
	switch(user_input)
	{
		case KEY_UP:
			if (*highlight > 0)
		 	{
				if (*highlight-1 <= *do_not_show) {
					*do_not_show-=1;
					clear();
				}
				*highlight-=1;
			
			};
			break;
		case KEY_DOWN:
			//if we are not on the last item
			if (*highlight < dirlen - 1) 
			{
				if (*highlight > (maxY -6) + (*do_not_show+1)){
					*do_not_show+=1;
					clear();
				}

				*highlight+=1;
			
			};
			break;
		case KEY_RESIZE:
			clear();
	}
	//we are here if none of the arrows have been pressed
	//cd to dir, or vim to file
  if (user_input == *"\n"){
    *fileaction = "e";
    clear(); 
  }

	if (user_input == *"b"){
		chdir("..");
		*highlight=0;
		*do_not_show=-1;
		clear();
	}
	//creating file or directory
	if (user_input == *"a"){
		
    printw("\nEnter file type: (d)irectory (f)ile:  ");
    echo();
    int filetype = getch();
    //creating file
    if (filetype == *"f"){
      printw("\nEnter file name: ");
      char filename[256];
      scanw("%s",filename);
      clear();
      char *syscall = malloc((sizeof(char) * 7) + sizeof(filename));
      int formatting= snprintf(syscall,sizeof(filename),"touch %s",filename);
      system(syscall);
      free(syscall);
    }
    //creating dir
    if (filetype == *"d"){
      printw("\nEnter directory name: ");
      char filename[256];
      scanw("%s",filename);
      clear();
      char *syscall = malloc((sizeof(char) * 7) + sizeof(filename));
      int formatting= snprintf(syscall,sizeof(filename),"mkdir %s",filename);
      system(syscall);
      free(syscall);
    }else{
      clear();
    }
    noecho();
	}
  //find file
  if (user_input == *"f"){
    *fileaction="f";
    def_prog_mode();   
    *file_name = fzf();
    printw("\n Found file: %s",*file_name);
    endwin();
    refresh();
  }
	
  if (user_input == *"d"){
    printw("\nDelete ? (y | n): \n");
    int confirm = getch();
    echo();
    if (confirm == *"y"){
      *fileaction = "d";
      clear();
    }
    else{
      clear();
    }
    noecho();

  }

  if (user_input == *"t"){
    printw("\nEnter terminal command: \n");
    char terminal_command[256];
    echo();
    scanw("%63[^\n]",terminal_command);
    def_prog_mode();
    endwin();
    system(terminal_command);
    refresh();
    clear();




    noecho();      
  }
    

	if (user_input == *"q"){
		endwin();
		exit(EXIT_SUCCESS);
	}	


}



int main(){
	//screen related
	initscr();
	curs_set(0);
	noecho();
	int maxY,maxX;
	int do_not_show =-1;
	
	//file related
	int highlight = 0;
	int dirlen;
	char *fileaction = "";
  char *file_name;

	//controls related
	int user_input;
	keypad(stdscr,true);


	while(1){
		getmaxyx(stdscr,maxY,maxX);

		file_control(
			&highlight,
			&dirlen,
			maxY,
			&do_not_show,
      &fileaction,
      file_name
		);


		user_input = getch();
		

		user_controls(
			user_input,
			&highlight,
			dirlen,
			maxY,
			&do_not_show,
      &fileaction,
      &file_name
		);


	}

	endwin();
  
  return 0;
	
}







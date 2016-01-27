
#include "sim.h"
#include <string.h>

#define COL_TEXT_ALIVE 1
#define COL_TEXT_ACTIVE 2
#define COL_TEXT_DEAD 3
#define COL_BORDER 4

typedef 

struct gui {
   WINDOW **windows;
};

typedef struct gui GUI;

void init_gui (struct gui *GUI)
{  

   /** Setup ncurses **/
   initscr(); // establish ncurses.
   noecho(); // disable input feedback.
   curs_set(FALSE); // disable cursor.
   keypad(stdscr, TRUE); // register function keys.
   timeout(-1); // non-blocking input.

   /** Set up colours **/
   if (has_colors() == FALSE) {
      fprintf(stderr, "No colour--exiting program\n");
      abort();   
   }
   start_color();
   init_pair(COL_TEXT_ALIVE, COLOR_WHITE, COLOR_BLACK);
   init_pair(COL_TEXT_ACTIVE, COLOR_GREEN, COLOR_BLACK);
   init_pair(COL_TEXT_DEAD, COLOR_RED, COLOR_BLACK);
   init_pair(COL_BORDER, COLOR_YELLOW, COLOR_BLACK);

   /** Set up windows. **/
   int width, height;
   getmaxyx(stdscr, height, width);
   GUI->windows = malloc (sizeof (WINDOW) * 3);
   GUI->windows[0] = newwin(height / 2, width / 2 , 0, 0);
   GUI->windows[1] = newwin(height / 2, width / 2, 0, width / 2);
   GUI->windows[2] = newwin(height / 2, width, 0 
}

void draw_menu (WINDOW *window)
{
   char *menu[3];
   menu[0] = "hello, world!";
   menu[1] = "run the game";
   menu[2] = "fucks sake";
   int i;
   for (i=0; i < 3; i++) {
      mvwprintw(GUI->window, 5 + i, 5, menu[i]);
   }
}

void gui_borders (GUI *gui)
{
   WINDOW **windows = gui->windows;
   for (int i = 0; i < 3; i++) {
      wattron(windows[i], COLOR_PAIR(col_value))
      wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
      wattroff(windows[i], COLOR_PAIR(col_value));
   }
}

void end_gui (struct gui *gui)
{
   wclear(gui->window);
   delwin(gui->window);
   endwin();
}

int main (int argc, char **argv)
{

   struct gui *GUI = malloc(sizeof(struct gui));
   init_gui(GUI);
  
   getch();
   return 0;
}

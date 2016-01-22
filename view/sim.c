
#include "sim.h"

#define COL_TEXT_ALIVE 1
#define COL_TEXT_ACTIVE 2
#define COL_TEXT_DEAD 3

struct gui {
   WINDOW *window;
};

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
   init_pair(COL_TEXT_ALIVE, COLOR_WHITE, COLOR_BLACK);
   init_pair(COL_TEXT_ACTIVE, COLOR_GREEN, COLOR_BLACK);
   init_pair(COL_TEXT_DEAD, COLOR_RED, COLOR_BLACK);

   /** Set up GUI. **/
   GUI->window = newwin(30, 30, 0, 0);

}

void end_gui (struct gui *gui)
{
   wclear(gui->window);
   delwin(gui->window);
   endwin();
}

int main (int argc, char **argv)
{

   struct gui *GUI = malloc(sizeof (struct gui));

   init_gui(GUI);

   char *button_names[5];
   button_names[0] = "load";
   button_names[1] = "inputs";
   button_names[2] = "step";
   button_names[3] = "speed";
   button_names[4] = "run";

   while (1) {
      int i;
      for (i=0 ; i < 5 ; i++) {
         wattron(GUI->window, COL_TEXT_ALIVE);
         mvwprintw(GUI->window, 10 + i, 10, button_names[i]);
      }
      wrefresh(GUI->window);
   }

   printf("%s\n", button_names[0]);

   end_gui(GUI);

   return 0;
}

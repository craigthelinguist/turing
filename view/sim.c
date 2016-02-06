
#include "sim.h"

#define COL_TEXT_ALIVE 1
#define COL_TEXT_ACTIVE 2
#define COL_TEXT_DEAD 3
#define COL_BORDER 4

typedef void (*DRAWER)(WINDOW *window);

struct gui {
   WINDOW **windows;
   DRAWER *drawers;
   int num_windows;
};

typedef struct gui GUI;

void draw_menu (WINDOW *window);
void draw_prog (WINDOW *window);
void draw_sim (WINDOW *window);

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
   GUI->num_windows = 3;

   /** Make the windows. **/
   GUI->windows = malloc (sizeof (WINDOW) * GUI->num_windows);
   GUI->windows[0] = newwin(height / 2, width / 2 , 0, 0);
   GUI->windows[1] = newwin(height / 2, width / 2, 0, width / 2);
   GUI->windows[2] = newwin(height / 2, width, height / 2, 0);

   /** Give them drawing functions. **/
   GUI->drawers = malloc (sizeof (DRAWER) * GUI->num_windows);
   GUI->drawers[0] = &draw_menu;
   GUI->drawers[1] = &draw_prog;
   GUI->drawers[2] = &draw_sim;

}

void draw_menu (WINDOW *window)
{
   int num_items = 6;
   char *menu[num_items];

   menu[0] = "Load Machine";
   menu[1] = "Step";
   menu[2] = "Run Machine";
   menu[3] = "Simulation Speed";
   menu[4] = "Reset Machine";
   menu[5] = "Exit Program";
   
   int i;
   for (i=0; i < num_items; i++) {
      mvwprintw(window, 5 + i, 5, menu[i]);
   }
}

void draw_prog (WINDOW *window)
{
}

void draw_sim (WINDOW *window)
{
}

void gui_draw (GUI *gui)
{

   // Drawing parameters.
   WINDOW **windows = gui->windows;
   DRAWER *drawers = gui->drawers;
   int num_windows = gui->num_windows;

   // Iterator.
   int i;

   // Draw each window.
   for (i = 0; i < num_windows; i++) {
      DRAWER window_drawer = drawers[i];
      (*window_drawer)(windows[i]);
   }

   // Draw borders.
   for (i = 0; i < 3; i++) {
      wattron(windows[i], COLOR_PAIR(COL_BORDER));
      wborder(windows[i], '|', '|', '-', '-', '+', '+', '+', '+');
      wattroff(windows[i], COLOR_PAIR(COL_BORDER));
   }

   // Refresh each window.
   for (i = 0; i < 3; i++) {
      wrefresh(windows[i]);   
   }

}

void end_gui (GUI *gui)
{

   int i;
   for (i=0; i < gui->num_windows; i++) {
      wclear(gui->windows[i]);
      delwin(gui->windows[i]);   
   }

   free(gui->windows);
   free(gui->drawers);
   free(gui);
   endwin();
}

int main (int argc, char **argv)
{

   // Check for correct number of arguments.
   if (argc < 2) {
      fprintf(stderr, "Usage: sim.c <prog> <args>\n");
      return 1;
   }
   
   // Get filename, parse contents.
   Str *fname = Str_Make(argv[1]);
   Program *prog = Parser_ProgFromFile(fname);
   Str_Free(fname);
   
   // Check we have correct number of inputs to program.
   int num_inputs = Prog_NumInputs(prog);
   if (argc - 2 != num_inputs) {
      fprintf(stderr, "Error: expected %d input(s) but received %d.\n", num_inputs, argc - 2);
      Prog_Free(prog);
      return 2;
   }
   
   // Load the inputs to the program.
   int inputs[num_inputs];
   int i;
   for (i = 2; i < 2 + num_inputs; i++) {
   
      // Check that this is a number.
      int k;
      for (k = 0; argv[i][k] != '\0'; k++) {
         if (!isdigit(argv[i][k])) {
            fprintf(stderr, "Error: the argument \"%s\" is not a number.\n", argv[i]);
            Prog_Free(prog);
            return 3;
         }
      }
      
      // Convert char * to number and load into inputs array.
      inputs[i-2] = atoi(argv[i]);
   }
   
   // Construct the turing machine.
   Machine *machine = M_Make(inputs, num_inputs);
   
   // Tear down everything.
   Prog_Free(prog);
   M_Del(machine);
   
   fprintf(stderr, "done\n");
   return 0;
   
   
   /*
   GUI *gui = malloc(sizeof(struct gui));
   init_gui(gui);

   while (1) {
      gui_draw(gui);
   }

   end_gui(gui);
   getch();
   return 0;
   */
   
}

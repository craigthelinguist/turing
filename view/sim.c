
#include "sim.h"

#define COL_TEXT_ALIVE 1
#define COL_TEXT_ACTIVE 2
#define COL_TEXT_DEAD 3
#define COL_BORDER 4

#define KEY_ESC 27

struct gui {
   WINDOW *menu;
   WINDOW *tape;
   Machine *machine;
};

typedef struct gui GUI;

void init_gui (GUI *gui, Machine *m);
void draw_menu (GUI *gui);
void draw_tape (GUI *gui);

void init_gui (GUI *gui, Machine *m)
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

   /** Make the windows. **/
   gui->menu = newwin(height / 2, width, 0, 0);
   gui->tape = newwin(height / 2, width, height / 2, 0);

   /** Associate gui with a machine. **/
   gui->machine = m;

}

void draw_menu (GUI *gui)
{

   // Get the window.
   WINDOW *window = gui->menu;

   // The menu is an array.
   int num_items = 2;
   char *menu[num_items];
   menu[0] = "Step";
   menu[1] = "Run";

   // Draw the menu.
   int i;

   int offset_top = 2;
   int offset_left = 3;

   for (i=0; i < num_items; i++) {
      mvwprintw(window, offset_top + i, offset_left, menu[i]);
   }

}

void draw_tape (GUI *gui)
{

   // Get data structures for drawing.
   WINDOW *w = gui->tape;
   Machine *m = gui->machine;

   // Get dimensions of window.
   int rows, cols;
   getmaxyx(w, rows, cols);

   // Draw the head.
   mvwaddch(w, 0, cols/2, 'v');
  
   // Draw the top & bottom of the tape.
   int i;
   for (i=0; i < cols; i++) {
      mvwaddch(w, 1, i, '-');
      mvwaddch(w, 3, i, '-');   
   }

   // Draw contents of tape left of the head.
   int offset = 0;
   int should_draw = 1;
   for (i=cols/2; i >= 0; i--) {
      if (should_draw == 1) {
         char c = M_CharAtHead(m, offset);
         mvwaddch(w, 2, i, c);         
         offset--;
         should_draw = 0;
      }
      else {
         should_draw = 1;
         mvwaddch(w, 2, i, '|');
      }
   }

   // Draw contents of tape right of the head.
   offset = 0;
   should_draw = 1;
   for (i=cols/2; i < cols; i++) {
      if (should_draw == 1) {
         char c = M_CharAtHead(m, offset);
         mvwaddch(w, 2, i, c);         
         offset++;
         should_draw = 0;
      }
      else {
         should_draw = 1;
         mvwaddch(w, 2, i, '|');
      }
   }


}

void gui_draw (GUI *gui)
{

   // Clear each window.
   wclear(gui->menu);
   wclear(gui->tape);

   // Draw each window.
   draw_menu(gui);
   draw_tape(gui);

   // Refresh each window.
   wrefresh(gui->menu);
   wrefresh(gui->tape);

}

void end_gui (GUI *gui)
{
   wclear(gui->menu);
   wclear(gui->tape);
   delwin(gui->menu);
   delwin(gui->tape);

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

   // Parse file, checking for an IO error.
   if (prog == NULL) {
      char *s = Str_Guts(fname);
      fprintf(stderr, "Error reading file: %s\n", s);
      free(s);
      return 1;
   }

   // Free the file name.
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


   // Construct the turing machine and the gui.
   Machine *machine = M_Make(prog, inputs);
   GUI *gui = malloc(sizeof(GUI));
   init_gui(gui, machine);
   

   while (1) {
   
      // Draw and process user input.
      gui_draw(gui);   
      int input = getch();

      if (input == KEY_RIGHT) I_Step(machine, prog);      
      else if (input == KEY_ESC) break;
      
   }

   // Tear down everything.
   end_gui(gui);
   Prog_Free(prog);
   M_Del(machine);

   return 0;


}

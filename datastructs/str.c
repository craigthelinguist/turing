

// Headers.
// ============================================================


#include "str.h"


// Definitions.
// ============================================================

struct str {
   int len;
   char *chars;
};

static inline void out_of_bounds (void);
char *Str_Guts(Str *s1);



// Internal functions.
// ============================================================

static inline void out_of_bounds ()
{
   fprintf(stderr, "Trying to access list at illegal index.");
   abort();
}



// Public functions.
// ============================================================

char Str_CharAt (Str *str, int index)
{
   if (index < 0 || index >= str->len)
      out_of_bounds();
   return str->chars[index];
}

int Str_Len (Str *str)
{
   return str->len;
}

Str *Str_Make (char *contents)
{
   int count = 0;
   while (*(contents+count) != '\0')
      count++;
   struct str *str = malloc(sizeof (struct str));
   str->len = count;
   str->chars = malloc(sizeof (char) * count);
   memcpy(str->chars, contents, sizeof (char) * count);
   return str;
}

Str *Str_Copy (Str *toCopy)
{
   Str *s = malloc(Str_SizeOf());
   s->len = toCopy->len;
   s->chars = malloc(sizeof(char) * s->len);
   memcpy(s->chars, toCopy->chars, toCopy->len);
   return s;
}

void Str_Free (Str *str)
{
   free(str->chars);
   str->chars = NULL;
   str = NULL;
}

int Str_Cmp (Str *str1, Str *str2)
{

   int minimum = str1->len < str2->len ? str1->len : str2->len;
   int maximum = str1->len < str2->len ? str2->len : str1->len;
   int difference = maximum - minimum;
   int i;
   for (i=0; i < minimum; i++) {
      if (str1->chars[i] != str2->chars[i]) difference++;
   }
   return difference;
}

char *Str_Guts (Str *str)
{
   char *s = malloc(Str_Len(str) + 1);
   int len = Str_Len(str);
   memcpy(s, str->chars, len);
   s[len] = '\0';
   return s;
}

int Str_ToInt (Str *str)
{

   // Preliminary checks.
   if (str == NULL) goto null;
   if (str->len == 0) goto err;
   char *txt = str->chars;   

   // Check for the sign.
   int sign = 1;
   if (txt[0] == '-') sign = -1;

   // Go through each digit.
   char digit;
   int num = 0;
   int i;
   for (i=0; i < str->len; i++) {
      digit = txt[i];
      if (digit < '0' || digit > '9') goto err;
      num *= 10;
      num += digit - '0';
   }
   return sign * num;

   // Error handling.
   err:
      fprintf(stderr, "Illegal conversion of string %s to integer.", str->chars);
      abort();
   null:
      fprintf(stderr, "Trying to convert null Str to itneger.");
      abort();

}


int Str_Eq (Str *str, char *txt)
{
   int i = 0;
   while (txt[i] != '\0' && i < str->len) {
      if (txt[i] != str->chars[i]) return 0;
      i++;
   }
   return i == str->len && txt[i] == '\0';
}

int Str_EqIgnoreCase (Str *str, char *txt)
{
   int i = 0;
   while (txt[i] != '\0' && i < str->len) {
      if (tolower(txt[i]) != tolower(str->chars[i])) return 0;
      i++;
   }
   return i == str->len && txt[i] == '\0';
}

int Str_SizeOf ()
{
   return sizeof(struct str);
}

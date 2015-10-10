
typedef struct str Str;


char Str_CharAt (Str *str,
                 int index);

int Str_Len (Str *str);

int Str_Cmp (Str *str1, Str *str2);

Str *Str_Make (char *contents);

void Str_Free (Str *str);

char *Str_Guts (Str *str);

int Str_ToInt (Str *str);

int Str_Eq (Str *str, char *txt);

int Str_EqIgnoreCase (Str *str, char *txt);

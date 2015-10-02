


#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "minunit.h"

#include "list.h"



#define SIZE_TEST(x) mu_assert(List_Size(list) == x, "List should have 3 things in it.");
#define NULL_CHECK(x) mu_assert(x != NULL, "Element shouldn't be null.");
#define INDEX_TEST(x, y) mu_assert(List_IndexOf(list, x) == y, "Element in wrong index.");
#define CONTAIN_TEST(x) mu_assert(List_Contains(list, x), "Element not found in list.");
#define MISSING_TEST(x) mu_assert(!List_Contains(list, x), "Element should be missing.");

struct Person {
   char *name;
   int age;
};

   /**
      Checks if two numbers are equal to within a certain threshold. **/
int fuzzy_eq (void *n1, void *n2)
{
   int x, y;
   x = *(int*)n1; y = *(int*)n2;
   int r = x - y;
   if (r < 0) r = -r;
   return r < 3 ? 0 : 1;
}

   /**
      Checks if two persons are equal by whether they have the same name. **/
int name_eq (void *p, void *q)
{
   struct Person *p1 = (struct Person *)p;
   struct Person *q1 = (struct Person *)q;
   return strcmp(p1->name, q1->name);
}

List *list;

void Setup () {
   list = NULL;
}

void Teardown () {
   list = NULL;
}

   /*
      Check the constructor gives you a list with a sensible initial state.
   */
MU_TEST(test_constructor) {
   list = List_Make(10, sizeof(int), NULL, NULL);
   NULL_CHECK(list);
   SIZE_TEST(0);
}

MU_TEST(test_add) {

   // Construct a list and add x.
   list = List_Make(10, sizeof(int), NULL, NULL);
   int x = 4;
   List_Append(list, &x);

   // Tests.
   SIZE_TEST(1);
   INDEX_TEST(&x, 0);
   CONTAIN_TEST(&x);
}

MU_TEST(test_add_2) {
   
   // Make a list, add multiple things.
   list = List_Make(10, sizeof(char), NULL, NULL);
   char chars[6] = { 34, 120, 66, 76, 41, 8 };
   int i;
   for (i=0; i < 6; i++) {
      List_Append(list, &chars[i]);
   }
   
   // List should have those things in order of insertion.
   SIZE_TEST(6);
   for (i=0; i < 6; i++) {
      INDEX_TEST(&chars[i], i);
      CONTAIN_TEST(&chars[i]);
   }

}

   /**
      Add and retrieve with structs.
    **/
MU_TEST(test_add_3) {

   // Make a list, make structs, add people.   
   list = List_Make(10, sizeof(struct Person), NULL, NULL);
   struct Person p = { "rawinia", 34 };
   struct Person q = { "cook", 88 };
   List_Append(list, &p);
   List_Append(list, &q);

   // List size should be correct. Things should be in right order.
   SIZE_TEST(2);
   INDEX_TEST(&p, 0);
   INDEX_TEST(&q, 1);
   CONTAIN_TEST(&p);
   CONTAIN_TEST(&q);

}


   /**
      Add and retrieve primitives with custom comparator functions.
    **/
MU_TEST(test_add_4) {

   // Make a list, add numbers.
   list = List_Make(10, sizeof(struct Person), &fuzzy_eq, NULL);
   int ints[] = { 43, 50, 106, 88, -13 };
   int i;
   for (i=0; i < 5; i++) {
      List_Append(list, &ints[i]);   
   }

   // Everything in the list should register.
   for (i=0; i < 5; i++) {
      INDEX_TEST(&ints[i], i);
      CONTAIN_TEST(&ints[i]);   
   }

   // Some things not in the list which should not register.
   int bad[] = { -100, 55, 3, 67, 24 };
   for (i=0; i < 5; i++) {
      INDEX_TEST(&bad[i], -1);
      MISSING_TEST(&bad[i]);
   }

   // Items which match things in the list according to our unique
   // notion of equality.
   int ok[] = { 44, 51, 104, 90, -14 };
   for (i=0; i < 5; i++) {
      INDEX_TEST(&ok[i], i);
      CONTAIN_TEST(&ok[i]);   
   }

}

   /**
      Add and retrieve structs with custom comparator functions.
    **/
MU_TEST(test_add_5) {

   // Make and populate list.
   list = List_Make(10, sizeof (struct Person), name_eq, NULL);
   struct Person p = { "ron", 34 };
   struct Person q = { "ayako", 4 };
   List_Append(list, &p);
   List_Append(list, &q);

   // Should be able to find those things again.
   INDEX_TEST(&p, 0); INDEX_TEST(&q, 1);
   CONTAIN_TEST(&p); CONTAIN_TEST(&q);

   // Should able to find by different things that are equal according
   // to our notion of equality.
   struct Person r = { "ron", 100 };
   struct Person s = { "ayako", -5 };
   INDEX_TEST(&r, 0); INDEX_TEST(&s, 1);
   CONTAIN_TEST(&r); CONTAIN_TEST(&s);

   // Should have matched to correct thing.
   int idx = List_IndexOf(list, &r);
   struct Person a = *(struct Person *)List_Get(list, idx);
   mu_assert(a.age == 34, "Should have matched the right thing.");
 
   // And now, for something random.
   struct Person t = { "jon", 34 };
   INDEX_TEST(&t, -1);
   MISSING_TEST(&t);

}

MU_TEST(test_aliasing) {

   // Add to list, pull it out again.
   list = List_Make(10, sizeof(int), NULL, NULL);
   int x = 40;
   List_Append(list, &x);
   int *y = (int *)List_Get(list, 0);
   mu_assert(x==*y, "Should be same item.");

   // Modify the thing you got back. You should still be able to find
   // it in the list with its original value.
   *y = 100;
   int *z = (int *)List_Get(list, 0);
   mu_assert(*z==x, "Should not have been modified.");
   
   // Now check that the original value is the same.
   mu_assert(x==40, "Should not have been modified.");
   free(y);
   free(z);
      
}

MU_TEST(test_aliasing_2) {

   // Add to list, pull it out again.
   list = List_Make(10, sizeof(struct Person), NULL, NULL);
   struct Person P = { "hemi", 14 };
   struct Person Q = { "jane", 21 };
   List_Append(list, &P);
   List_Append(list, &Q);
   struct Person *y = (struct Person *)List_Get(list, 1);
   mu_assert(!memcmp(&Q, y, sizeof(struct Person)), "Should be same item.");

   // Modify the thing you got back. You should still be able to find
   // it in the list with its original value.
   y->name = "billy";
   struct Person *z = (struct Person *)List_Get(list, 1);
   mu_assert(!strcmp(Q.name, z->name) && Q.age == z->age,
      "Should not have been modified.");
   
   // Now check the original is the same.
   mu_assert(!strcmp("jane", Q.name) && Q.age == 21,
      "Should not have been modified.");
   free(y);
   free(z);

}

MU_TEST_SUITE(test_suite) {
   MU_SUITE_CONFIGURE(&Setup, &Teardown);
   MU_RUN_TEST(test_constructor);
   MU_RUN_TEST(test_add);
   MU_RUN_TEST(test_add_2);
   MU_RUN_TEST(test_add_3);
   MU_RUN_TEST(test_add_4);
   MU_RUN_TEST(test_add_5);
   MU_RUN_TEST(test_aliasing);
   MU_RUN_TEST(test_aliasing_2);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}


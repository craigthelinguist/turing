


#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "list.h"

// Unit testing macros and imports.
// ======================================================================

#include "minunit.h"
#define SIZE_TEST(x) mu_assert(List_Size(list) == x, "List should have 3 things in it.");
#define NULL_CHECK(x) mu_assert(x != NULL, "Element shouldn't be null.");
#define INDEX_TEST(x, y) mu_assert(List_IndexOf(list, x) == y, "Element in wrong index.");
#define CONTAIN_TEST(x) mu_assert(List_Contains(list, x), "Element not found in list.");
#define MISSING_TEST(x) mu_assert(!List_Contains(list, x), "Element should be missing.");


// Structs and functions for unit testing.
// ======================================================================

struct Person {
   char *name;
   int age;
};

   /**
      Checks if two numbers are equal to within a certain threshold. **/
static int fuzzy_eq (void *n1, void *n2)
{
   int x, y;
   x = *(int*)n1; y = *(int*)n2;
   int r = x - y;
   if (r < 0) r = -r;
   return r < 3 ? 0 : 1;
}

   /**
      Checks if two persons are equal by whether they have the same name. **/
static int name_eq (void *p, void *q)
{
   struct Person *p1 = (struct Person *)p;
   struct Person *q1 = (struct Person *)q;
   return strcmp(p1->name, q1->name);
}



// Set up stuff, testing variables.
// ======================================================================

static List *list;

static void Setup () {
   list = NULL;
}

static void Teardown () {
   list = NULL;
}


// Unit tests.
// ======================================================================

   /**
      Check the constructor gives you a list with a sensible initial state.
   **/
MU_TEST(test_constructor) {
   list = List_Make(10, sizeof(int), NULL, NULL);
   NULL_CHECK(list);
   SIZE_TEST(0);
}

   /**
      Test adding and retrieving a single element.
   **/
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

   /**
      Test adding and retrieving multiple elements. Check that their location
      in the list matches how they were added.
   **/
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

   /**
      Test aliasing. That is, add some stuff, pull it out again, modify
      the thing that you pulled out, and make sure it doesn't affect the
      original thing that was added, or the copy stored in the list. **/
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

   /**
      Test aliasing with structs. **/
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

MU_TEST(test_del) {

   // Initialise list for testing.
   list = List_Make(10, sizeof(int), NULL, NULL);
   int x, y;
   x = 4; y = 11;
   List_Append(list, &x);
   List_Append(list, &y);
   SIZE_TEST(2);
   
   // Delete stuff. Make sure elements get shifted down.
   List_Del(list, 0);
   SIZE_TEST(1);
   MISSING_TEST(&x);
   INDEX_TEST(&x, -1);
   CONTAIN_TEST(&y);
   INDEX_TEST(&y, 0);

   // Put stuff back in. Make sure in right order.
   List_Append(list, &x);
   SIZE_TEST(2);
   CONTAIN_TEST(&x);
   INDEX_TEST(&x, 1);

   // Remove everything by searching for value, not name.
   List_Remove(list, &x);
   SIZE_TEST(1);
   CONTAIN_TEST(&y);
   MISSING_TEST(&x);

   // Remove other thing.
   List_Remove(list, &y);
   SIZE_TEST(0);
   MISSING_TEST(&y);

}

MU_TEST(test_del_2) {

   // Initialise list.
   list = List_Make(5, sizeof(int), NULL, NULL);
   int ints[] = { 0, 1, 2, 3, 4 };
   int i;
   for (i=0; i < 5; i++) {
      List_Append(list, &ints[i]);   
   }
   SIZE_TEST(5);

   // Delete some stuff, make sure everything shifted down.
   // After deletion should look like: [1,2,3]
   List_Remove(list, &ints[0]);
   List_Remove(list, &ints[4]);
   SIZE_TEST(3);
   for (i=1; i<=3; i++) CONTAIN_TEST(&ints[i]);
   MISSING_TEST(&ints[0]);
   MISSING_TEST(&ints[4]);
   INDEX_TEST(&ints[1], 0);
   INDEX_TEST(&ints[2], 1);
   INDEX_TEST(&ints[3], 2);

   // Add some more stuff.
   // List should look like: [1,2,3,0,4]
   List_Append(list, &ints[0]);
   List_Append(list, &ints[4]);
   SIZE_TEST(5);
   INDEX_TEST(&ints[0], 3);
   INDEX_TEST(&ints[4], 4);
   CONTAIN_TEST(&ints[0]);
   CONTAIN_TEST(&ints[4]);

}

MU_TEST(test_del_3) {
}

MU_TEST(test_del_4) {
}

   /**
      Add enough things to cause the table to have to rebuild. Make sure
      everything still works, in correct order, etc. **/
MU_TEST(test_rebuild) {

   // Add stuff, get ready to resize.
   list = List_Make(10, sizeof (int), NULL, NULL);
   int ints[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   int i;
   for (i=0; i < 10; i++) {
      List_Append(list, &ints[i]);
   }
   for (i=0; i < 10; i++) {
      INDEX_TEST(&ints[i], i);
      CONTAIN_TEST(&ints[i]);
   }
   SIZE_TEST(10);
   
   // Add one more thing. Check everything still in right order.
   int z = 50;
   List_Append(list, &z);
   for (i=0; i < 10; i++) {
      INDEX_TEST(&ints[i], i);
      CONTAIN_TEST(&ints[i]);
   }
   INDEX_TEST(&z, 10);
   CONTAIN_TEST(&z);
   SIZE_TEST(11);

}

   /**
      Add a non-trivial amount of items.
   **/
MU_TEST(test_big) {

   // Add heaps of stuff.
   list = List_Make(10, sizeof (int), NULL, NULL);
   const int NUM_ITEMS = 1000;
   int ints[NUM_ITEMS];
   int i;
   for (i=0; i < NUM_ITEMS; i++) {
      ints[i] = i;
      List_Append(list, &ints[i]);
   }
   
   // Check things are in there.
   SIZE_TEST(NUM_ITEMS);
   for (i=0; i < NUM_ITEMS; i++) {
      INDEX_TEST(&ints[i], i);
      CONTAIN_TEST(&ints[i]);
   }

   // Remove everything
   for (i=0; i < NUM_ITEMS; i++) {
      List_Del(list, 0);   
   }
   SIZE_TEST(0);
   List_Free(list);

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
   MU_RUN_TEST(test_rebuild);
   MU_RUN_TEST(test_big);
   MU_RUN_TEST(test_del);
   MU_RUN_TEST(test_del_2);
   MU_RUN_TEST(test_del_3);
   MU_RUN_TEST(test_del_4);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}


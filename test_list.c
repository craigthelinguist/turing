


#include "stdlib.h"

#include "list.h"
#include "minunit.h"

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
   list = ListMake(sizeof(int), NULL, NULL);
   mu_assert(list != NULL, "Constructor should not return null pointer.");
   mu_assert(ListSize(list) == 0, "Empty list should have size of 0.");
}

   /*
      Test adding to a list.
      Make sure list doesn't break if you add something to list and then update the
      thing which was added to the list.
   */ 
MU_TEST(test_add) {

   // Construct a list and add x.
   list = ListMake(sizeof(int), NULL, NULL);
   int x = 4;
   ListPrepend(list, &x);
   mu_assert(ListSize(list) == 1, "List size should be 1 after adding an element.");
   mu_assert(ListContains(list, &x), "List should contain integer after adding it.");
   
   // If we update x, this shouldn't affect the 'x' in the list.
   // That is, adding to the list has pass-by-value semantics.
   int z = 4;
   x = 5;
   mu_assert(ListContains(list, &z), "If you add x to list and then update x, its original value should still be in the list.");
   mu_assert(!ListContains(list, &x), "If you add to list and then update x, its new value should not be in the list.");
}

MU_TEST(test_add_2) {

   // Construct a list and add three things.
   list = ListMake(sizeof(int), NULL, NULL);
   int x = 4;
   int y = 5;
   int z = 5;
   ListPrepend(list, &x);
   ListPrepend(list, &y);
   ListPrepend(list, &z);
   
   // The list should now have three things in it.
   mu_assert(ListSize(list) == 3, "List should be size 3 after adding 3 things.");
   mu_assert(ListContains(list, &x), "List should contain the element you added.");
   mu_assert(ListContains(list, &y), "List should contain the element you added.");
   mu_assert(ListContains(list, &z), "List should contain the element you added.");
   
   
   // If you retrieve the contents of the list it should be the same as what you added.
   int *ptr = malloc(sizeof (int));
   ListGet(ptr, list, &x);
   mu_assert(*ptr == x,  "If you add element and retrieve it, it should be the same.");

   ListGet(ptr, list, &y);
   mu_assert(*ptr == y, "If you add element and retrieve it, it should be the same.");

   ListGet(ptr, list, &z);
   mu_assert(*ptr == z, "If you add element and retrieve it, it should be the same.");
}


struct Triple {
   int x;
   char y;
   float z;
};

MU_TEST(test_add_3) {

   // Make the list. Make some items to add.
   list = ListMake(sizeof(struct Triple), NULL, NULL);
   struct Triple t1 = { 2, 3, 6 };
   struct Triple t2 = { 4, 5, 1 };

   
   // Add items to the list.
   ListPrepend(list, &t1);
   ListPrepend(list, &t2);

   // Size of list should be two. Elements you added should be there.
   mu_assert(ListSize(list) == 2, "List size after adding should be two.");

   // Retrieve the triples. They should be equal to what you added.
   struct Triple *rPtr = malloc(sizeof (struct Triple));

   ListGet(rPtr, list, &t1);
   mu_assert(rPtr->x == t1.x && rPtr->y == t1.y && rPtr->z == t1.z, "Struct you put in map should be equal to struct retrieved from map.");

   ListGet(rPtr, list, &t2);
   mu_assert(rPtr->x == t2.x && rPtr->y == t2.y && rPtr->z == t2.z, "Struct you put in map should be equal to struct retrieved from map.");

}


struct Person {
   int age;
   char *name;
};

int PersonComp (void *p1, void *p2)
{
   char *s1 = ((struct Person *)p1)->name;
   char *s2 = ((struct Person *)p2)->name;
   int i, j;
   for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++) {
      if (s1[i] != s2[i]) return 1;
   }
   return s1[i] != '\0' || s2[i] != '\0';
}

MU_TEST(test_eq_func) {
   struct Person p1 = { 10, "billy" };
   struct Person p2 = { 10, "john" };
   struct Person p3 = { 5, "billy" };
   mu_assert(PersonComp(&p1, &p2), "People with different names not equal.");
   mu_assert(!PersonComp(&p1, &p3), "People with same names equal.");
   mu_assert(PersonComp(&p2, &p3), "People with different names not equal.");
}

MU_TEST(test_eq_func_2) {

   // Make some people.
   struct Person p1 = { 10, "billy" };
   struct Person p2 = { 10, "johnny" };   
   struct Person p3 = { 5, "billy" };

   // Add to a list.
   List *list = ListMake(sizeof(struct Person), NULL, &PersonComp);
   ListPrepend(list, &p1);
   ListPrepend(list, &p2);
   ListPrepend(list, &p3);

   // Should be able to find the item using custom equality function.
   struct Person *rPtr = malloc(sizeof (struct Person));
   ListGet(rPtr, list, &p2);
   struct Person q = *rPtr;

   mu_assert(ListContains(list, &p1), "Should be in list after adding.");
   mu_assert(ListContains(list, &p2), "Should be in list after adding.");
   mu_assert(ListContains(list, &p3), "Should be in list after adding.");

   mu_assert(!PersonComp(&p2, &q), "Thing retrieved from map should be equal to thing you searched for.");
   mu_assert(q.age == p2.age, "Thing retrieved from map should be equal to thing you searched for.");

   // If I make something which hasn't added to the map, but it is equal to something in the map
   // according to the comparison function, should return a positive match.

   struct Person r = { 100, "billy" };
   mu_assert(ListContains(list, &r), "Item equivalent by comparison function should be in list.");

}

MU_TEST(test_aliasing) {

   // Add person to list.
   struct Person p1 = { 40, "rawinia" };
   List *list = ListMake(sizeof (struct Person), NULL, &PersonComp);
   ListPrepend(list, &p1);
   mu_assert(ListContains(list, &p1), "Should be in list after adding.");
   
   // Check aliasing of insert.
   struct Person *q = malloc(sizeof (struct Person));
   ListGet(q, list, &p1);
   q->age = 10;
   mu_assert(q->age != p1.age, "Item in list shouldn't refer to same thing as inserted value.");

   // Check aliasing of return.
   struct Person *r = malloc(sizeof (struct Person));
   ListGet(r, list, &p1);
   mu_assert(q->age != r->age, "Item in list shouldn't refer to same thing as return value.");
   
}

MU_TEST_SUITE(test_suite) {
   MU_SUITE_CONFIGURE(&Setup, &Teardown);
   MU_RUN_TEST(test_constructor);
   MU_RUN_TEST(test_add);
   MU_RUN_TEST(test_add_2);
   MU_RUN_TEST(test_add_3);
   MU_RUN_TEST(test_eq_func);
   MU_RUN_TEST(test_eq_func_2);
   MU_RUN_TEST(test_aliasing);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}


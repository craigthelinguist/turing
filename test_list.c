


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
   mu_assert(*(int *)ListGet(list, &x) == x, "If you add element and retrieve it, it should be the same.");
   mu_assert(*(int *)ListGet(list, &y) == y, "If you add element and retrieve it, it should be the same.");
   mu_assert(*(int *)ListGet(list, &z) == z, "If you add element and retrieve it, it should be the same.");
   
}

MU_TEST_SUITE(test_suite) {
   MU_SUITE_CONFIGURE(&Setup, &Teardown);
   MU_RUN_TEST(test_constructor);
   MU_RUN_TEST(test_add);
   MU_RUN_TEST(test_add_2);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return 0;
}


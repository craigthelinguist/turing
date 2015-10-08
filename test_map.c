
#include <stdlib.h>
#include <string.h>

#include "map.h"

// Unit testing stuff.
// ======================================================================

#include "minunit.h"

#define SIZE_TEST(x) mu_assert(Map_Size(map) == x, "Wrong number of items in map.")
#define NULL_CHECK(x) mu_assert(x != NULL, "Thing shouldn't be null.")
#define NULL_TEST(x) mu_assert(x == NULL, "Thing should be null.")
#define CONTAINS_TEST(x) mu_assert(Map_Contains(map, x), "Map should contain item.")
#define MISSING_TEST(x) mu_assert(Map_Contains(map, x), "Map should not contain item.")
#define DELETED_TEST(x) mu_assert(Map_Del(map, x), "Map should have deleted something.")
#define NOT_DELETED_TEST(x) mu_assert(Map_Del(map, x), "Map should not have deleted anything.")


// Set up.
// ======================================================================

static Map *map;
static const int szInt = sizeof(int);
static void Setup () {}
static void Reset () {
   Map_Free(map);
   map = NULL;
}

// Unit tests.
// ======================================================================

unsigned int BadHash (void *i1) 
{
   return 0;
}

unsigned int IdentityHash (void *i1)
{
   int i = *(int *)i1;
   if (i < 0) return -i;
   else return i;
}

void
IntToIntMap (unsigned int (*Hash)(void *))
{
   map = Map_Make(10, szInt, szInt, Hash, NULL, NULL, NULL, NULL);
}

MU_TEST (test_constructor) {
   IntToIntMap(&BadHash);
   NULL_CHECK(map);
   SIZE_TEST(0);
}

MU_TEST (test_badhash_put) {

   // Make a map, put a single pair in it.
   IntToIntMap(&BadHash);
   int k, v;
   k = 10; v = 20;
   Map_Put(map, &k, &v);
   
   // Size of map should be affected.
   SIZE_TEST(1);
   CONTAINS_TEST(&k);
   
   // Should be able to retrieve value from key.
   int *result = Map_Get(map, &k);
   mu_assert(*result == v, "Should be able to retrieve value from key.");
   free(result);

}

MU_TEST (test_badhash_overwrite) {
   
   // Make a map, put a single pair in it.
   IntToIntMap(&BadHash);
   int k, v;
   k = 10; v = 20;
   Map_Put(map, &k, &v);
   
   // Size of map should be affected.
   SIZE_TEST(1);
   CONTAINS_TEST(&k);
   
   // Should be able to retrieve value from key.
   int *result = Map_Get(map, &k);
   mu_assert(*result == v, "Should be able to retrieve value from key.");
   free(result);   

   // Add another value with same key.
   int v2 = 100;
   Map_Put(map, &k, &v2);
   
   // Should not have affected map and key.
   SIZE_TEST(1);
   CONTAINS_TEST(&k);
   
   // Value retrieved should be different.
   result = Map_Get(map, &k);
   mu_assert(*result == v2, "Should have updated value in map.");
   free(result);

}

MU_TEST (test_badhash_collision) {
   
   // Make a map, put two pairs in it.
   IntToIntMap(&BadHash);
   int k1, k2, v1, v2;
   k1 = 10; k2 = -10;
   v1 = 100; v2 = -100;
   Map_Put(map, &k1, &v1);
   Map_Put(map, &v1, &v2);
   
   // Size of map should be affected.
   SIZE_TEST(2);
   CONTAINS_TEST(&k1);
   CONTAINS_TEST(&k2);
   
   // Should be able to retrieve values from keys.
   int *r1 = Map_Get(map, &k1);
   int *r2 = Map_Get(map, &k2);
   mu_assert(*r1 == v1, "Should be able to retrieve value from key.");
   mu_assert(*r2 == v2, "Should be able to retrieve value from key.");
   free(r1);
   free(r2);

}

MU_TEST (test_identityhash) {

   // Make a map, put a bunch of stuff into it.
   IntToIntMap(&IdentityHash);
   int ints[] = { 0, 2, 4, 6, 8 };
   int i;
   for (i=0 ; i < 5; i++) {
      int dub = ints[i] * 2;
      Map_Put(map, &ints[i], &dub);
   }

   // Check map is allg
   SIZE_TEST(5);
   for (i=0 ; i < 5; i++) {
      CONTAINS_TEST(&ints[i]);
      int *r = Map_Get(map, &ints[i]);
      mu_assert(*r == ints[i] * 2, "Should retrieve correct value from key.");
      free(r);
   }

}

MU_TEST (test_outofbounds) {
   
   // Make a map, but some stuff in it. The hash function will hash -ve out of
   // bounds values so map has to be able to handle it.
   IntToIntMap(&IdentityHash);
   int ints[] = { 10, 15, 25, 50 };
   int i;
   for (i=0; i < 4; i++) {
      Map_Put(map, &ints[i], &ints[i]);   
   }

   // Check map is allg.
   SIZE_TEST(4);
   for (i=0; i < 4; i++) {
      CONTAINS_TEST(&ints[i]);
      int *r = Map_Get(map, &ints[i]);
      mu_assert(*r == ints[i], "Should retrieve correct value from key.");
      free(r);
   }

}

MU_TEST (test_rebuild) {
   
   // Make a map, put enough stuff in it so the table has to rebuild.
   const int NUM_TEST = 100;

   IntToIntMap(&IdentityHash);
   int ints[NUM_TEST];
   int squares[NUM_TEST];
   int i;
   for (i = 0; i < NUM_TEST; i++) {
      ints[i] = i;
      squares[i] = i * i;
      Map_Put(map, &ints[i], &squares[i]);
   }

   // Size should be 100.
   SIZE_TEST(NUM_TEST);

   // Should be able to get everything back out.
   for (i=0; i < NUM_TEST; i++) {
      CONTAINS_TEST(&ints[i]);
      int *r = Map_Get(map, &ints[i]);
      mu_assert(*r == squares[i], "Should retrieve correct value from key."); 
      free(r);  
   }

}

// Running everything.
// ======================================================================


MU_TEST_SUITE (test_suite) 
{
   MU_SUITE_CONFIGURE(&Setup, &Reset);
   MU_RUN_TEST(test_constructor);
   MU_RUN_TEST(test_badhash_put);
   MU_RUN_TEST(test_badhash_overwrite);
   MU_RUN_TEST(test_identityhash);
   MU_RUN_TEST(test_outofbounds);
   MU_RUN_TEST(test_rebuild);
}

int main (int argc, char **argv)
{
   MU_RUN_SUITE(test_suite);
   MU_REPORT();
   return 0;
}

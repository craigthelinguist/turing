Name: add.
Inputs: 2.
Init: init.

init:
   blank -> right, first_arg_zero_1.
   1 -> right, check_second_arg_1.

first_arg_zero_1:
   blank -> right, halt.

check_second_arg_1:
   1 -> right, check_second_arg_1.
   blank -> right, check_second_arg_2.

check_second_arg_2:
   blank -> left, second_arg_zero_1.
   1 -> left, normal_add_1.

second_arg_zero_1:
   blank -> left, second_arg_zero_2.

second_arg_zero_2:
   1 -> left, second_arg_zero_2.
   blank -> right, halt.

normal_add_1:
   blank -> 1, normal_add_1.
   1 -> left, normal_add_2.

normal_add_2:
   1 -> left, normal_add_2.
   blank -> right, halt.

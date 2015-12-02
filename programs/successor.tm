Name: successor.
Inputs: 1.
Init: q0.

q0:
	1 -> right, q0.
	blank -> 1, q1.

q1:
	1 -> left, q1.
	blank -> right, halt.

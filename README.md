# Befunge93-vm
A virtual manchine for the Befunge93 programming language in C.

## Branch gc:
A Garbage collector for the Befunge93+ vm

### Befunge-93+
<b>Befunge-93+</b> is an extension of Befunge-93 with three new instructions, <i>c</i>, <i>h</i> and <i>b</i>.

* <b>c (cons)</b>: removes from the top of the stack in turn two elements, <i>b</i> and <i>a</i>, creates in the heap a new pair (cons cell) containing these two elements (<i>a</i> in the first position and <i>b</i> in the second) and pushes on top of the stack the pair’s location.

* <b>h (head)</b>: removes an element from the top of the stack, which must be the location of a pair (cons cell) in the heap; it pushes on top of the stack the value that is contained in the first position of this pair (head).

* <b>t (tail)</b>: same as <i>h</i> but for the second position of the pair (tail).

#### This project is an exercise for the [pl2](https://courses.softlab.ntua.gr/pl2/) course in ECE NTUA.

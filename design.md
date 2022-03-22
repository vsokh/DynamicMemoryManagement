SharedPointer class:
Responsibility: Aquaire the pointer to the object and release it after own destruction. Mimic the API of a C-style pointer.
API:
- ctor(Obj)
- dtor - decrease the reference counter, and destroy the obj, if it's zero
- c-ctor - same as =
- -> - access to an object's fields, methods
- * - access to an object
- = - copy a SP and point to SP's acuired obj, increment reference counter

Bonus: Implement make_shared func.


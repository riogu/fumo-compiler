// param is array of ints; the arg passed automatically "adjusts" (frequently said
// informally as "decays") from `int []` (array of ints) to `int *` 
// (ptr to int)
void arraytest(int a[]);
// ptr to int
void arraytest(int *a);

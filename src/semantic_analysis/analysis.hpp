// make:
// symbol table
// type checker
// declaration checker

/*
i want each scope to be linked to each other after the analysis phase
    > maybe we can uniquely name each scope
 
fn somefunc(x: i32, y: i64) {
    { // this would be somefunc::0
        
        { // this is somefunc::0::0
            this is always a unique scope name
        }
        struct e{}; somefunc::0::e // i guess (?)
        // this makes it easier to allow namespaces to be a thing
        // we can allow this kind of access only on namespaced stuff (not functions ofc)
    
    }
    { // somefunc::1
    }
    if () { // somefunc::2
    }
    if () { // somefunc::3 
    }

    {
        int x;
    }
    {
        {
            x++; // this doesnt work 
                 // we can only go backwards

        }
        {

        }
    }
}

the global namespace would have:
    all the global declarations, 
    all the function declarations
    all the struct/enum declarations
    all the namespaces
*/

struct e {
    int x;
};

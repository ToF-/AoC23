require ffl/tst.fs
require seed-fertilizer.fs

t{
    ." max range is 2500" cr
    maxrange 2500 ?s

    ." a range set is initially empty" cr
    rangeset foo
    foo itemcount 0 ?s
    
    ." items in a range set are 2 cells long" cr
    foo itemsize 2 ?s
    
    ." after adding a range to a range set the range set has it"
    23 17 foo hasrange? ?false
    23 17 foo addrange
    23 17 foo hasrange? ?true
    49 15 foo hasrange? ?false
    49 15 foo addrange
    49 15 foo hasrange? ?true
    23 17 foo hasrange? ?true

    ." nextrange pops a range from the range set"
    foo nextrange ?true 23 17 ?d
    foo nextrange ?true 49 15 ?d
}t
bye

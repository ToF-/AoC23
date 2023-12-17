require ffl/tst.fs
require seed-fertilizer.fs

    ." max range is 2500" cr
    maxrange 2500 ?s

    ." a range set is initially empty" cr
    rangeset foo
    foo set>capacity maxrange ?s
    foo set>count 0 ?s

    ." items in a range set are 2 cells long" cr
    foo set>itemsize 2 ?s

    ." after adding a range to a range set the range set has it" cr
    23 17 foo hasrange? ?false
    23 17 foo addrange
    23 17 foo hasrange? ?true
    49 15 foo hasrange? ?false
    49 15 foo addrange
    49 15 foo hasrange? ?true
    23 17 foo hasrange? ?true

    foo emptyranges

    ." nextrange pops a range from the range set" cr
    49 15 foo addrange
    23 17 foo addrange
    foo nextrange ?true 49 15 ?d
    foo nextrange ?true 23 17 ?d
    foo nextrange ?false

    ." addranges add ranges from a set to a set" cr
    1 2 foo addrange
    rangeset bar
    4807 23 bar addrange
    1905 18 bar addrange
    bar foo addranges
    foo nextrange ?true 1 2 ?d
    foo nextrange ?true 4807 23 ?d
    foo nextrange ?true 1905 18 ?d
    foo nextrange ?false

    ." copyranges copy ranges from a set to a set" cr
    bar set>count 2 ?s
    foo set>count 0 ?s
    bar foo copyranges
    foo nextrange ?true 4807 23 ?d
    foo nextrange ?true 1905 18 ?d
    foo nextrange ?false

    ." convert range, no intersection" cr
    rangeset result
    rangeset remaining

    ." max converter is 100" cr
    maxconverter 100 ?s

    ." a converter set is initially empty" cr
    converterset loo
    loo set>capacity maxconverter ?s
    loo set>count 0 ?s
    
    ." items in a converter set are 3 cells long" cr
    loo set>itemsize 3 ?s

    ." after adding a converter to a converter set it can be found with nthconverter" cr
    10 11 12 loo addconverter
    20 21 22 loo addconverter
    1 2 3 loo addconverter
    0 loo nthconverter
    11 12 ?d 10 ?s
    1 loo nthconverter
    21 22 ?d 20 ?s
    2 loo nthconverter
    2 3 ?d 1 ?s
bye

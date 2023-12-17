2500 constant maxrange
100 constant maxconverter

: set ( capacity itemsize <name> -- )
    create 0 , 2dup 32 lshift or , * allot ;

: set>itemsize ( set -- n )
    cell+ @ 32 rshift ;

: set>count ( set - n )
    @ ;

: set>capacity ( set -- n )
    cell+ @ 32767 and ;

: set>items ( set - adr )
    cell+ cell+ ;

: rangeset ( <name> -- )
    maxrange 2 set ;

: set>end ( set -- adr )
    dup set>itemsize
    over set>count * cells 
    swap set>items + ;

: range@ ( adr -- n,m )
    2@ ;

: range! ( n,m,addr -- )
    2! ;

: (hasrange?) ( n,m,set -- f )
    >r false -rot r>
    dup set>items
    swap set>end
    swap do
        i range@
        2over d= if
            2drop drop true 0 0
        then
    2 cells +loop
    2drop ;

: hasrange? ( n,m,set -- f )
    dup set>count if
        (hasrange?)
    else
        drop 2drop false
    then ;

: (addrange) ( n,m,set -- )
    dup >r set>end range!
    r> dup set>count 1+ swap ! ;

: addrange ( n,m,set -- )
    dup set>capacity
    over set>count <= if
       s" range set out of capacity" exception throw
    then
    dup 2over rot hasrange? 0= if
        (addrange)
    else
        drop 2drop
    then ;

: emptyranges ( set )
    0 swap ! ;

: (deletefirstrange) ( set -- )
    dup set>items over       ( set,dest,set )
    over cell+ cell+         ( set,dest,set,srce )
    -rot set>count cells 2*  ( set,srce,dest,size )
    cmove
    dup set>count 1- swap ! ;

: firstrange ( set -- n,m )
    cell+ cell+ 2@ ;

: (nextrange) ( set -- n,m)
    dup firstrange rot
    (deletefirstrange) ;

: nextrange ( set -- n,m|f )
    dup set>count if (nextrange) true else drop false then ;

: (addranges) ( src,dest -- )
    swap
    dup set>end
    swap set>items do
        dup i 2@ rot addrange
        2 cells +loop
    drop ;

: addranges ( src,dest -- )
    over set>count if
       (addranges)
    else
        2drop
    then ;

: copyranges ( src,dest -- )
    dup emptyranges
    addranges ;

: converterset ( <name> -- )
    maxconverter 3 set ;

: converter! ( c,n,m,addr -- )
    dup >r range!
    r> 2 cells + ! ;

: (addconverter) ( c,n,m,set -- )
    dup >r set>end converter!
    r> dup set>count 1+ swap ! ;

: addconverter ( c,n,m,set -- )
    dup set>capacity
    over set>count <= if
        s" converter set out of capacity" exception throw
    then
    (addconverter) ;

: converter@ ( addr -- c,n,m )
    dup >r 2 cells + @
    r> range@ ;
    
: nthconverter ( n,set -- c,n,m )
    2dup set>count >= if
        s" converter index out of range" exception throw
    then
    dup set>itemsize rot * cells
    swap set>items + converter@ ;
    

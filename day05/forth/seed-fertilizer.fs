2500 constant maxrange

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

: (hasrange?) ( n,m,set -- f )
    >r false -rot r>
    dup set>items
    swap set>end
    swap do
        i 2@ 
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
    dup >r set>end 2!
    r> dup set>count 1+ swap ! ;

: addrange ( n,m,set -- )
    dup set>capacity
    over set>count <= if
       s" range out of capacity" exception throw
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
    dup set>count if
        (nextrange) true
    else
        drop false
    then ;

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

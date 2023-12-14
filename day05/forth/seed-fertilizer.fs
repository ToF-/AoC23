2500 constant maxrange

: set ( n <name> -- )
    create 0 , , ;

: itemsize ( set -- n )
    cell+ @ ;

: itemcount ( set - n )
    @ ;

: set>items ( set - adr )
    cell+ cell+ ;

: rangeset ( <name> -- )
    2 set ;

: set>end ( set -- adr )
    dup 2@ * cells
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
    dup itemcount if
        (hasrange?)
    else
        drop 2drop false
    then ;

: (addrange) ( n,m,set -- )
    dup itemcount 1+ over !
    set>end 2 cells - 2! ;

: addrange ( n,m,set -- )
    dup 2over rot hasrange? 0= if
        (addrange)
    else
        drop 2drop
    then ;

: (nextrange) ( set -- n,m)
    dup set>items 2@ 2>r
    dup itemcount 1- over !
    dup set>items
    swap set>end
    swap ?do
        i dup 2 cells + 2@ rot 2!
    2 cells +loop
    2r> ;

: nextrange ( set -- n,m|f )
    dup itemcount if
        (nextrange) true
    else
        drop false
    then ;

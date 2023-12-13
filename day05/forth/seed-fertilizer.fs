2500 constant maxrange

: range-set ( <name> -- )
    create 0 ,
    maxrange 0 do 0 0 2, loop ;

: next-range ( range-set -- range|true )
    dup @ 0= if
        drop false
    else
        cell+ 2@ true
    then ;

: add-range ( range-set, range -- )
    rot dup @                  \ range,range-set,count
    2dup cells 2* +            \ range,range-set,count,dest
    -rot 1+ swap !             \ range,dest
    2! ;
        
           

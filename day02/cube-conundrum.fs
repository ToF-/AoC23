
require ffl/tst.fs

variable red# variable green# variable blue#

: init
    red# off green# off blue# off ;

: game ( <id>  n -- n+1 )
    parse-name 2drop ( we don't care about the id )
    init
    1+ ;

: max! ( n,addr -- )
    dup @
    rot max
    swap ! ;

: red,   ( n -- ) red# max! ;
: red;   ( n -- ) red, ;
: green, ( n -- ) green# max! ;
: green; ( n -- ) green, ;
: blue,  ( n -- ) blue# max! ;
: blue;  ( n -- ) blue, ;

: end-game-1 ( sum,id -- sum' )
    red# @ 12 <= green# @ 13 <= blue# @ 14 <= and and
    if dup -rot + swap then ;

: end-game-2 ( sum,id -- sum' )
    red# @ green# @ blue# @ * *
    rot + swap ;

defer end-game

: red   ( n -- ) red, end-game ;
: green ( n -- ) green, end-game ;
: blue  ( n -- ) blue, end-game ;

' end-game-1 is end-game
0 0
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
drop 
8 ?s

0 0
include data/puzzle.txt
drop 2006 ?s

' end-game-2 is end-game
0 0
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
drop
2286 ?s
0 0
include data/puzzle.txt
drop
84911 ?s
bye


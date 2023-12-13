require ffl/tst.fs
require seed-fertilizer.fs

t{
    maxrange 2500 ?S

    range-set foo
    foo dbg next-range false ?S

    foo 100 dump
    foo 79 48 dbg add-range
    foo dbg next-range ?true 79 48 ?D
    foo 100 dump
}t
bye

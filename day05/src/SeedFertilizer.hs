module SeedFertilizer
    where

import Data.Maybe
import Data.List

type Range = (Int,Int)

data Almanach = Almanach { seeds      :: [Int],
                           maps       :: [Map] }
    deriving (Eq, Show)

data Map = Map [Converter]
    deriving (Eq, Show)

data Converter = Converter { dest  :: Int,
                             range :: Range }
    deriving (Eq, Show)


readAlmanach :: [String] -> Almanach
readAlmanach ss = Almanach { seeds      = map read (tail (words (head ss))),
                             maps       = readMaps (Map []) (tail(tail ss)) }
    where
        readMaps :: Map -> [String] -> [Map]
        readMaps m []        = [m]
        readMaps m (s:ss) | null s  = m : readMaps (Map []) ss
        readMaps m (s:ss) | (words s)!!1 == "map:" = readMaps m ss
        readMaps (Map converters) (s:ss) =
            readMaps (Map converters') ss
                where
                    converters' = converters <> [Converter { dest = dst, range = (srce, len) }]
                    dst  = ints!!0
                    srce = ints!!1
                    len  = ints!!2
                    ints = map read (words s)

seedRanges :: Almanach -> [Range]
seedRanges al = seedRanges' (seeds al)
    where
        seedRanges' [] = []
        seedRanges' (s:l:ss) = (s,l) : seedRanges' ss

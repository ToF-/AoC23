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

convert :: Int -> Converter -> Maybe Int
convert n (Converter dst (start,len)) =
    let offset = n - start
        within = offset >= 0 && offset < len
        result = dst + offset
    in if within then Just result else Nothing

mapping :: Int -> Map -> Int
mapping n (Map []) = n
mapping n (Map (c:cs)) =
    let result = convert n c
     in case result of
          (Just m) -> m
          Nothing -> mapping n (Map cs)

lowestLocation :: Almanach -> Int
lowestLocation sf = minimum $ foldl allMappings (seeds sf) [0..6]
    where
        allMappings seedList i = map (\seed -> mapping seed ((maps sf)!!i)) seedList

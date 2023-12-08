module SeedFertilizer
    where

import Data.Maybe
import Data.List

type Length = Int
type Interval = (Int, Int)

data Range = R { start :: Int, len :: Length }
    deriving (Eq, Show)

data Almanach = Almanach { seeds      :: [Int],
                           maps       :: [Map] }
    deriving (Eq, Show)

data Map = M [Converter]
    deriving (Eq, Show)

data Converter = C { dest  :: Int,
                             range :: Range }
    deriving (Eq, Show)


readAlmanach :: [String] -> Almanach
readAlmanach ss = Almanach { seeds      = map read (tail (words (head ss))),
                             maps       = readMaps (M []) (tail(tail ss)) }
    where
        readMaps :: Map -> [String] -> [Map]
        readMaps m []        = [m]
        readMaps m (s:ss) | null s  = m : readMaps (M []) ss
        readMaps m (s:ss) | (words s)!!1 == "map:" = readMaps m ss
        readMaps (M converters) (s:ss) =
            readMaps (M converters') ss
                where
                    converters' = converters <> [C { dest = dst, range = R srce len }]
                    dst  = ints!!0
                    srce = ints!!1
                    len  = ints!!2
                    ints = map read (words s)

seedRanges :: Almanach -> [Range]
seedRanges al = seedRanges' (seeds al)
    where
        seedRanges' [] = []
        seedRanges' (s:l:ss) = R s l : seedRanges' ss

convert :: Int -> Converter -> Maybe Int
convert n (C dst (R start len)) =
    let offset = n - start
        within = offset >= 0 && offset < len
        result = dst + offset
    in if within then Just result else Nothing

mapping :: Int -> Map -> Int
mapping n (M []) = n
mapping n (M (c:cs)) =
    let result = convert n c
     in case result of
          (Just m) -> m
          Nothing -> mapping n (M cs)

lowestLocation :: Almanach -> Int
lowestLocation sf = minimum $ foldl allMappings (seeds sf) [0..6]
    where
        allMappings seedList i = map (\seed -> mapping seed ((maps sf)!!i)) seedList

isValidInterval :: Interval -> Bool
isValidInterval  = (Nothing /=) . rangeFromInterval

intervalFromRange :: Range -> Interval
intervalFromRange (R a l) = (a, a + l -1)

rangeFromInterval :: Interval -> Maybe Range
rangeFromInterval (a,z) | a <= z = Just $ R a (z - a + 1)
                        | otherwise = Nothing

offset :: Int -> Interval -> Interval
offset off (a,z) = if isValidInterval (a,z) then (a + off, z + off) else invalidInterval

invalidInterval:: (Int, Int)
invalidInterval = (1,-1)

convertR :: Range -> Converter -> [Range]
convertR r (C dst cr) =
    let
        (a0,z0) = intervalFromRange r
        (a1,z1) = intervalFromRange cr
        (a2,z2) = (max a0 a1, min z0 z1)
        (a3,z3) = if isValidInterval (a2,z2) then (a0,a1-1) else invalidInterval
        (a4,z4) = if isValidInterval (a2,z2) then (z1+1,z0) else invalidInterval
        R srce  _ = cr
    in case catMaybes $ map rangeFromInterval [(a3,z3), offset (dst-srce) (a2,z2), (a4,z4)] of
         [] -> [r]
         rs -> rs

mappingR :: [Range] -> Map -> [Range]
mappingR rs (M []) = rs
mappingR rs (M (c:cs)) = foldr convertAllRanges rs (c:cs)
    where
    convertAllRanges c rs = concatMap (\r -> convertR r c) rs

allMappingsR :: [Range] -> [Map] -> [[Range]]
allMappingsR [] _ = []
allMappingsR rs [] = []
allMappingsR rs (m:ms) = result : allMappingsR result ms
    where
        result = mappingR rs m

lowestLocationR :: Almanach -> Int
lowestLocationR sf = minimum $ map start $ last $ allMappingsR (seedRanges sf) (maps sf)


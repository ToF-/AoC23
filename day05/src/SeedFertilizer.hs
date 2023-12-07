module SeedFertilizer
    where

import Data.Maybe
import Data.List

type Range = (Int,Int)

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
                    converters' = converters <> [C { dest = dst, range = (srce, len) }]
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
convert n (C dst (start,len)) =
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

isValidRange :: (Int, Int) -> Bool
isValidRange (a,z) = a < z

validRange :: (Int,Int) -> Maybe Range
validRange (a,z) = if isValidRange (a,z)
                      then Just (a, z - a + 1)
                      else Nothing

offset :: Int -> (Int,Int) -> Range
offset off (a,z) = if isValidRange (a,z) then (a + off, z + off) else (0,0)

convertR :: Range -> Converter -> [Range]
convertR (start,len) (C dst (srce, clen)) =
    let
        (a0,z0) = (start, start + len - 1)
        (a1,z1) = (srce, srce + clen - 1)
        (a2,z2) = (max a0 a1, min z0 z1)
        (a3,z3) = if isValidRange (a2,z2) then (a0,a1-1) else (0,0)
        (a4,z4) = if isValidRange (a2,z2) then (z1+1,z0) else (0,0)
    in case catMaybes $ map validRange [(a3,z3), offset (dst-srce) (a2,z2), (a4,z4)] of
         [] -> [(start,len)]
         rs -> rs

mappingR :: [Range] -> Map -> [Range]
mappingR rs (M []) = rs
mappingR rs (M (c:cs)) = foldr convertAllRanges rs (c:cs)
    where
    convertAllRanges c rs = concatMap (\r -> convertR r c) rs

allMappingsR :: [Range] -> [Map] -> [Range]
allMappingsR rs ms = foldl (\acc m -> mappingR acc m) rs ms

allMappingsRScan :: [Range] -> [Map] -> [[Range]]
allMappingsRScan rs ms = scanl (\acc m -> mappingR acc m) rs ms

lowestLocationR :: Almanach -> Int
lowestLocationR sf = minimum $ map fst $ allMappingsR (seedRanges sf) (maps sf)


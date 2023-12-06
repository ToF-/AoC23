module SeedFertilizer
    where

import Data.Maybe
import Data.List

data Almanach = Almanach {
    seeds :: [Int],
    seedRanges :: [Range],
    mappings :: [[Mapping]]
             }
    deriving (Eq, Show)

type Range = (Int,Int)

data Mapping = Mapping { dest :: Int, range :: Range }
    deriving (Eq, Show)

withLen :: Int -> Int -> Range
withLen src len = (src,src+len-1)



seedToSoil :: Int
seedToSoil = 0

rangeL :: Int -> Int -> Range
rangeL a b = (a,a+b-1)

readAlmanach :: [String] -> Almanach
readAlmanach ss = Almanach { seeds = map read (tail (words (head ss))),
                             seedRanges = readRanges (map read (tail (words (head ss)))),
                             mappings  = readAllMappings [] (tail ss) }
    where
        readRanges :: [Int] -> [Range]
        readRanges [] = []
        readRanges (s:l:xs) = rangeL s l : readRanges xs

        readAllMappings :: [Mapping] -> [String] -> [[Mapping]]
        readAllMappings ms [] = [ms]
        readAllMappings ms (s:ss) | null s = readAllMappings ms ss
        readAllMappings ms (s:ss) | ((words s)!!1) == "map:" =
            if not (null ms)
                then ms : readAllMappings [] ss
                else readAllMappings [] ss
        readAllMappings ms (s:ss) = readAllMappings (ms <> [Mapping dst (src `withLen` len)]) ss
            where dst = head ints
                  src = head (tail ints)
                  len = head (tail (tail ints))
                  ints = map read (words s)


mapping :: Int -> [Mapping] -> Int
mapping n [] = n
mapping n (Mapping dst (start,end):_) | n >= start && n <= end = dst + n - start
mapping n (_:rs) = mapping n rs

rangeMapping :: Range -> Mapping -> (Maybe Range, [Range])
rangeMapping i (Mapping dest r@(mStart, mEnd)) = ((finalRange dest) <$> result, remaining)
    where
        (result, remaining) = i `cross` r
        finalRange dest (start, end) = (start + offset, end + offset)
            where
                offset = dest - mStart

rma :: [Range] -> [Range] -> [Mapping] -> [Range]
rma acc [] _ = acc
rma acc rs [] = acc ++ rs
rma acc (r:rs) (m:ms) = case rangeMapping r m of
                          (Nothing, _) -> rma acc (r:rs) ms 
                          (Just n, ns) -> rma (n:acc) (ns ++ rs) ms

rangeMappings :: [Range] -> [Mapping] -> [Range]
rangeMappings rs ms = rma [] rs ms

expand :: Range -> [Int]
expand (a,b) = [a..b]

mappingsNaive :: [Range] -> [Mapping] -> [Int]
mappingsNaive rs ms = map (\n -> mapping n ms) (concatMap expand rs)

allMappings :: Int -> [[Mapping]] -> Int
allMappings n mms = foldl (\a i -> mapping a (mms !! i)) n [0..6]

allRangeMappings :: [Range] -> [[Mapping]] -> [Range]
allRangeMappings rs mms = foldl (\a i -> rangeMappings a (mms !! i)) rs [0..6]

lowestLocation :: Almanach -> Int
lowestLocation al = minimum (map (\n -> allMappings n (mappings al)) (seeds al))

lowestLocationRangeNaive :: Almanach -> Int
lowestLocationRangeNaive al = minimum (map (\n -> allMappings n (mappings al)) (concatMap expand (seedRanges al)))

lowestLocationRange :: Almanach -> Int
lowestLocationRange al = minimum (map fst (allRangeMappings (seedRanges al) (mappings al)))

validRange :: Int -> Int -> Maybe Range
validRange x y | x <= y = Just (x,y)
               | otherwise = Nothing

inter :: Range -> Range -> Maybe Range
inter (aa,a) (bb,b) = validRange (max aa bb) (min a b)

cross :: Range -> Range -> (Maybe Range, [Range])
cross (aa,a) (bb,b) = (result, remaining result)
    where
        result = inter (aa,a) (bb,b)
        remaining Nothing = [(aa,a)]
        remaining _ = catMaybes [validRange aa (bb-1), validRange (b+1) a]


module SeedFertilizer
    where

data Almanach = Almanach {
    seeds :: [Int],
    maps :: [SfMap]
             }
    deriving (Eq, Show)

type SfMap = [Range]
data Range = Range { dest :: Int,
                     srce :: Int,
                     len  :: Int }
    deriving (Eq, Show)

seedToSoil :: Int
seedToSoil = 0

readAlmanach :: [String] -> Almanach
readAlmanach ss = Almanach { seeds = map read (tail (words (head ss))),
                             maps  = readAllMaps [] (tail ss) }
    where
        readAllMaps :: SfMap -> [String] -> [SfMap] 
        readAllMaps sfMap [] = [sfMap]
        readAllMaps sfMap (s:ss) | null s = readAllMaps sfMap ss
        readAllMaps sfMap (s:ss) | ((words s)!!1) == "map:" = 
            if not (null sfMap) 
                then sfMap : readAllMaps [] ss
                else readAllMaps [] ss
        readAllMaps sfMap (s:ss) = readAllMaps (sfMap <> [Range dst src len]) ss
            where [dst,src,len] = map read (words s)

mapping :: Int -> SfMap -> Int
mapping n [] = n
mapping n (Range dst src len:rs) | n >= src && n < src + len = dst + n - src
mapping n (_:rs) = mapping n rs

allMappings :: Int -> [SfMap] -> Int
allMappings n sfMaps = foldl (\a i -> mapping a (sfMaps !! i)) n [0..6]

lowestLocation :: Almanach -> Int
lowestLocation al = minimum (map (\n -> allMappings n (maps al)) (seeds al))

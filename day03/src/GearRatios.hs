module GearRatios
    where

import Data.Char
import Data.List
import Data.Ord

type Coord = (Int, Int)
data EngineSchematic = EngineSchematic { bounds :: Coord,
                                         elements :: [String] }
engineSchematic :: [String] -> EngineSchematic
engineSchematic ss = EngineSchematic { bounds = (maxRow, maxCol), elements = ss }
    where
        maxRow = length ss
        maxCol = length (head ss)

surrounding :: Coord -> Coord -> [Coord]
surrounding (n,m) (i,j) = filter (\(x,y) -> x >= 0 && x < n && y >= 0 && y < m)
            [(i-1,j-1),(i-1,j),(i-1,j+1),(i,j-1),(i,j+1),(i+1,j-1),(i+1,j),(i+1,j+1)]
connections :: EngineSchematic -> [Coord]
connections sch = (nub . sort . concatMap (surrounding (maxrow,maxcol)). filter symbolAt)
    [(r,c) | r <- [0..maxrow-1], c <- [0..maxcol-1]]
    where
        (maxrow,maxcol) = bounds sch
        symbolAt (i,j) = c /= '.' && not (isDigit c)
            where c = (elements sch) !! i !! j

numbers :: EngineSchematic -> [(Int,[Coord])]
numbers sch = concat $ zipWith  indexOnRow  [0..] (map lineNumbers (elements sch))


indexOnRow :: Int -> [(Int, [Int])] -> [(Int,[Coord])]
indexOnRow r = map (\(n,cds) -> (n, map ((,)r) cds))

lineNumbers :: String -> [(Int, [Int])]
lineNumbers row = 
    (map (\g -> (read (map snd g), map fst g)) .
    filter (\g -> (isDigit . snd . head) g) .
    groupBy (\a b -> (isDigit . snd) a == (isDigit . snd) b) .
    zip [0..]) row

connectedNumbers :: EngineSchematic -> [Int]
connectedNumbers sch = map fst $ filter isConnected $ numbers sch
    where
        isConnected :: (Int,[Coord]) -> Bool
        isConnected (_,cds) = or (map (`elem` cns) cds)
        cns = connections sch

stars :: EngineSchematic -> [Coord]
stars sch = filter (\(r,c) -> (elements sch)!!r!!c == '*')
    [(i,j) | i <- [0..maxrow-1], j <- [0..maxcol-1]]
        where
            (maxrow,maxcol) = bounds sch


starConnectedNumbers :: EngineSchematic -> Coord -> [Int]
starConnectedNumbers sch cd = (sort . map fst . nub . sortBy (comparing snd) . filter (hasConnection cd)) (numbers sch)
    where
        hasConnection :: Coord -> (Int,[Coord]) -> Bool
        hasConnection cd (_,cds) = or (map (`elem` starConnections) cds)
        starConnections = surrounding (bounds sch) cd

allStarConnectedNumbers :: EngineSchematic -> [[Int]]
allStarConnectedNumbers sch = filter (\g-> (length g == 2)) $ map (starConnectedNumbers sch) (stars sch)

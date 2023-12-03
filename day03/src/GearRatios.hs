module GearRatios
    where

import Data.Char 

type Coord = (Int, Int)
data EngineSchematic = EngineSchematic { bounds :: Coord,
                                         elements :: [String] }
engineSchematic :: [String] -> EngineSchematic
engineSchematic ss = EngineSchematic { bounds = (maxRow, maxCol), elements = ss }
    where
        maxRow = length ss
        maxCol = length (head ss)

at :: EngineSchematic -> Coord -> Char
at sch (row,col) = (elements sch) !! row !! col

withinBounds :: Coord -> EngineSchematic -> Bool
withinBounds (r,c) sch = r >=0 && r < (fst (bounds sch)) && c >= 0 && c < (snd (bounds sch))

isASymbol :: Char -> Bool
isASymbol c = c /= '.' && not (isDigit c)

isConnected :: Coord -> EngineSchematic -> Bool
isConnected (row,col) sch = or $ map hasSymbolAt [(row-1,col+1)
                                                 ,(row-1,col)
                                                 ,(row-1,col-1)
                                                 ,(row,col-1)
                                                 ,(row+1,col-1)
                                                 ,(row+1,col)
                                                 ,(row+1,col+1)
                                                 ,(row,col+1)]
    where
        hasSymbolAt coord = coord `withinBounds` sch && isASymbol (sch `at` coord)

connectedNumbers :: EngineSchematic -> [Int]
connectedNumbers sch = cna Nothing False sch coords
    where
    coords = [(row, col) | row <- [0..(fst (bounds sch))-1], col <- [0..(snd (bounds sch))-1]]

    cna :: Maybe Int -> Bool -> EngineSchematic -> [Coord] -> [Int]

    cna Nothing _ _ [] = []
    cna (Just n) False _ [] = []
    cna (Just n) True  _ [] = [n]

    cna Nothing _ sch (coord:coords) | isDigit (sch `at` coord) = cna (Just (digitToInt (sch `at` coord))) (isConnected coord sch) sch coords

    cna Nothing _ sch (coord:coords) = cna Nothing False sch coords

    cna (Just acc) conn sch (coord:coords) | isDigit (sch `at` coord) && (snd coord) == ((snd (bounds sch)) - 1) =
        prefix <> cna Nothing False sch coords
        where
        prefix = if connected then [n] else []
        connected = conn || isConnected coord sch
        n = acc * 10 + digitToInt (sch `at` coord)

    cna (Just acc) conn sch (coord:coords) | isDigit (sch `at` coord) = cna (Just n) connected sch coords
        where
            connected = conn || isConnected coord sch
            n = acc * 10 + digitToInt (sch `at` coord)

    cna (Just acc) conn sch (coord:coords) | not (isDigit (sch `at` coord)) = prefix <> cna Nothing False sch coords
        where
            prefix = if connected then [acc] else []
            connected = conn || isConnected coord sch

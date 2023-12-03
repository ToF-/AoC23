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

        

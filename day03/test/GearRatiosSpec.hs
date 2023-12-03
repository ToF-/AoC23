module GearRatiosSpec
    where

import GearRatios
import Test.Hspec


sample :: [String]
sample = 
  ["467..114.."
  ,"...*......"
  ,"..35..633."
  ,"......#..."
  ,"617*......"
  ,".....+.58."
  ,"..592....."
  ,"......755."
  ,"...$.*...."
  ,".664.598.."]

spec :: SpecWith ()
spec = do
    describe "engine schematic" $ do
        it "can give the locations connected to symbols" $ do
            let sch = engineSchematic ["...."
                                      ,"$..."
                                      ,"...*"]
            connections sch `shouldBe` [(0,0),(0,1),(1,1),(1,2),(1,3),(2,0),(2,1),(2,2)]
            connections (engineSchematic sample) `shouldBe`
                [(0,2),(0,3),(0,4)
                ,(1,2),(1,4),(2,2),(2,3),(2,4),(2,5),(2,6),(2,7)
                ,(3,2),(3,3),(3,4),(3,5),(3,7)
                ,(4,2),(4,4),(4,5),(4,6),(4,7)
                ,(5,2),(5,3),(5,4),(5,6)
                ,(6,4),(6,5),(6,6)
                ,(7,2),(7,3),(7,4),(7,5),(7,6)
                ,(8,2),(8,4),(8,6)
                ,(9,2),(9,3),(9,4),(9,5),(9,6)]

        it "can give the numbers and their locations" $ do
            let sch = engineSchematic [".27"
                                      ,"1.."
                                      ,"498"]
            numbers sch `shouldBe` [(27,[(0,1),(0,2)]),(1,[(1,0)]),(498,[(2,0),(2,1),(2,2)])]
            numbers (engineSchematic sample) `shouldBe`
                [(467,[(0,0),(0,1),(0,2)])
                ,(114,[(0,5),(0,6),(0,7)])
                ,(35,[(2,2),(2,3)])
                ,(633,[(2,6),(2,7),(2,8)])
                ,(617,[(4,0),(4,1),(4,2)])
                ,(58,[(5,7),(5,8)])
                ,(592,[(6,2),(6,3),(6,4)])
                ,(755,[(7,6),(7,7),(7,8)])
                ,(664,[(9,1),(9,2),(9,3)])
                ,(598,[(9,5),(9,6),(9,7)])]

        it "can give the connected numbers" $ do
            let sch = engineSchematic ["*27"
                                      ,"1.."
                                      ,"498"]
            connectedNumbers sch `shouldBe` [27, 1]
            connectedNumbers (engineSchematic sample) `shouldBe` [467,35,633,617,592,755,664,598]
    
        it "can solve the puzzle part one" $ do
            sum (connectedNumbers (engineSchematic sample)) `shouldBe` 4361
            puzzle <- lines <$> readFile "data/puzzle.txt"
            sum (connectedNumbers (engineSchematic puzzle)) `shouldBe` 538046

        it "can give the stars locations" $ do
            stars (engineSchematic sample) `shouldBe` [(1,3),(4,3),(8,5)]


        it "can give the numbers connected to a star location" $ do
            let sch = engineSchematic sample
            starConnectedNumbers sch (1,3) `shouldBe` [35,467]
            starConnectedNumbers sch (4,3) `shouldBe` [617]
            allStarConnectedNumbers sch `shouldBe` [[35,467],[598,755]]

        it "can solve the puzzle part two" $ do
            let sch = engineSchematic sample
            sum (map product (allStarConnectedNumbers sch)) `shouldBe` 467835
            puzzle <- lines <$> readFile "data/puzzle.txt"
            sum (map product (allStarConnectedNumbers (engineSchematic puzzle))) `shouldBe` 81709807


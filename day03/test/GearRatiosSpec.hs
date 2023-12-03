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
        it "tell if a location is connected to a symbol" $ do
            let sch = engineSchematic sample
            isConnected (0,0) sch `shouldBe` False
            isConnected (0,2) sch `shouldBe` True
            isConnected (0,3) sch `shouldBe` True
            isConnected (0,4) sch `shouldBe` True
            isConnected (0,5) sch `shouldBe` False
            isConnected (8,3) sch `shouldBe` False
            isConnected (8,4) sch `shouldBe` True
            isConnected (8,5) sch `shouldBe` False
            isConnected (8,6) sch `shouldBe` True
            isConnected (8,7) sch `shouldBe` False
            isConnected (9,4) sch `shouldBe` True
            isConnected (9,5) sch `shouldBe` True
            isConnected (9,6) sch `shouldBe` True


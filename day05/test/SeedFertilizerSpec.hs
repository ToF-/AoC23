
module SeedFertilizerSpec
    where

import SeedFertilizer
import Test.Hspec


sample :: [String]
sample = ["seeds: 79 14 55 13"
         ,""
         ,"seed-to-soil map:"
         ,"50 98 2"
         ,"52 50 48"
         ,""
         ,"soil-to-fertilizer map:"
         ,"0 15 37"
         ,"37 52 2"
         ,"39 0 15"
         ,""
         ,"fertilizer-to-water map:"
         ,"49 53 8"
         ,"0 11 42"
         ,"42 0 7"
         ,"57 7 4"
         ,""
         ,"water-to-light map:"
         ,"88 18 7"
         ,"18 25 70"
         ,""
         ,"light-to-temperature map:"
         ,"45 77 23"
         ,"81 45 19"
         ,"68 64 13"
         ,""
         ,"temperature-to-humidity map:"
         ,"0 69 1"
         ,"1 0 69"
         ,""
         ,"humidity-to-location map:"
         ,"60 56 37"
         ,"56 93 4"]

spec :: SpecWith ()
spec = do
    let sf = readAlmanach sample 
    describe "seed fertilizer" $ do
        it "can read an almanach" $ do
            seeds sf `shouldBe` [79,14,55,13]
            seedRanges sf `shouldBe` [(79,14),(55,13)]
            (maps sf) !! 0 `shouldBe` Map [Converter {dest = 50, range = (98,2) }
                                          ,Converter {dest = 52, range = (50,48) }]
            (maps sf) !! 6 `shouldBe` Map [Converter {dest = 60, range = (56,37)}
                                          ,Converter {dest = 56, range = (93,4)}]

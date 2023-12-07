
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
            (maps sf) !! 0 `shouldBe` M [C {dest = 50, range = (98,2) }
                                          ,C {dest = 52, range = (50,48) }]
            (maps sf) !! 6 `shouldBe` M [C {dest = 60, range = (56,37)}
                                          ,C {dest = 56, range = (93,4)}]

    describe "converter" $ do
        it "converts an int within a range to a dest" $ do
            let c = C { dest = 50, range = (98,2) }
            convert 98 c `shouldBe` Just 50
            convert 99 c `shouldBe` Just 51
            convert 100 c `shouldBe` Nothing
            convert 97 c `shouldBe` Nothing

        it "converts a range of ints within a range to dest ranges" $ do
            let c = C { dest = 50, range = (98,2) }
            convertR (98,2) c `shouldBe` [(50,2)]
            convertR (98,4) c `shouldBe` [(50,2),(100,2)]
            convertR (96,4) c `shouldBe` [(96,2),(50,2)]
            convertR (96,6) c `shouldBe` [(96,2),(50,2),(100,2)]
            convertR (1,109) c `shouldBe` [(1,97),(50,2),(100,10)]
            convertR (1,10) c `shouldBe` [(1,10)]
            convertR (1,99) c `shouldBe` [(1,97),(50,2)]
            convertR (1,97) (C 52 (50,48)) `shouldBe` [(1,49),(52,48)]
            convertR (50,2) (C 52 (50,48)) `shouldBe` [(52,2)]
            convertR (79,14) (C 52 (50,48)) `shouldBe` [(81,14)]
            convertR (74,14) (C 45 (77,23)) `shouldBe` [(74,3),(45,11)]
            convertR (45,11) (C 81 (45,19)) `shouldBe` [(81,11)]
            convertR (45,11) (C 1 (0,69))`shouldBe` [(46,11)]


    describe "mapping" $ do 
        it "converts an int according to converters or itself if not in range" $ do
            let m = (maps sf)!!0
            mapping 49 m `shouldBe` 49
            mapping 98 m `shouldBe` 50
            mapping 50 m `shouldBe` 52
            mapping 97 m `shouldBe` 99
            mapping 100 m `shouldBe` 100

        it "converts ranges of ints within ranges of converters" $ do
            let m = (maps sf) !! 0
         -- 50 ← 98 2 / (1,99) → (1,97),(50,2)
         -- 52 ← 50 48 / (1,97),(50,2) →  [(1,49),(52,48),(52,2)]
            mappingR [(1,10)] m `shouldBe` [(1,10)]
            mappingR [(1,10),(200,8)] m `shouldBe` [(1,10),(200,8)]
            mappingR [(1,99),(100,199)] m `shouldBe` [(1,49),(52,46),(50,2),(50,2),(100,199)]
            mappingR [(74,14)] (M [C 45 (77,23)])`shouldBe` [(74,3),(45,11)]
            mappingR [(74,3),(45,11)] (M [C 81 (45,19)])`shouldBe` [(74,3),(81,11)]

    describe "all MappingRs" $ do
        it "converts ranges according to a group of mappings" $ do
            allMappingsR [(seedRanges sf)!!0] (maps sf) `shouldBe` [(47,3),(46,11)]
            let result = allMappingsRScan [(seedRanges sf)!!0] (maps sf)
            zip result (maps sf) `shouldBe`
                [([(79,14)],M [C {dest = 50, range = (98,2)},C {dest = 52, range = (50,48)}])
                ,([(81,14)],M [C {dest = 0, range = (15,37)},C {dest = 37, range = (52,2)},C {dest = 39, range = (0,15)}])
                ,([(81,14)],M [C {dest = 49, range = (53,8)},C {dest = 0, range = (11,42)},C {dest = 42, range = (0,7)},C {dest = 57, range = (7,4)}])
                ,([(81,14)],M [C {dest = 88, range = (18,7)},C {dest = 18, range = (25,70)}])
                ,([(74,14)],M [C {dest = 45, range = (77,23)},C {dest = 81, range = (45,19)},C {dest = 68, range = (64,13)}])
                ,([(46,3),(45,11)],M [C {dest = 0, range = (69,1)},C {dest = 1, range = (0,69)}])
                ,([(47,3),(46,11)],M [C {dest = 60, range = (56,37)},C {dest = 56, range = (93,4)}])]

    describe "lowest location" $ do
        it "find the lowest location after all mappings" $ do
            lowestLocation sf `shouldBe` 35 

        it "solve the puzzle part one" $ do
            puzzle <- lines <$> readFile "data/puzzle.txt"
            let al = readAlmanach puzzle
            lowestLocation al `shouldBe` 178159714

    describe "lowest location range" $ do
        it "find the lowest location after all mappings on all ranges" $ do
            lowestLocationR sf `shouldBe` 46

        it "solve the puzzle part two" $ do
            puzzle <- lines <$> readFile "data/puzzle.txt"
            let al = readAlmanach puzzle
            lowestLocationR al `shouldBe` 0

    

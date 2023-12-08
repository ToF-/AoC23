
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
            seedRanges sf `shouldBe` [R 79 14,R 55 13]
            (maps sf) !! 0 `shouldBe` M [C {dest = 50, range = R 98 2 }
                                          ,C {dest = 52, range = R 50 48 }]
            (maps sf) !! 6 `shouldBe` M [C {dest = 60, range = R 56 37}
                                          ,C {dest = 56, range = R 93 4}]
            Prelude.length (maps sf)`shouldBe` 7

    describe "converter" $ do
        it "converts an int within a range to a dest" $ do
            let c = C { dest = 50, range = R 98 2 }
            convert 98 c `shouldBe` Just 50
            convert 99 c `shouldBe` Just 51
            convert 100 c `shouldBe` Nothing
            convert 97 c `shouldBe` Nothing

        it "converts a range of ints within a range to dest ranges" $ do
            let c = C { dest = 50, range = R 98 2 }
            convertR (R 98 2) c `shouldBe` [R 50 2]
            convertR (R 98 4) c `shouldBe` [R 50 2,R 100 2]
            convertR (R 96 4) c `shouldBe` [R 96 2,R 50 2]
            convertR (R 96 6) c `shouldBe` [R 96 2,R 50 2,R 100 2]
            convertR (R 1 109) c `shouldBe` [R 1 97,R 50 2,R 100 10]
            convertR (R 1 10) c `shouldBe` [R 1 10]
            convertR (R 1 99) c `shouldBe` [R 1 97,R 50 2]
            convertR (R 1 97) (C 52 (R 50 48)) `shouldBe` [R 1 49,R 52 48]
            convertR (R 50 2) (C 52 (R 50 48)) `shouldBe` [R 52 2]
            convertR (R 79 14) (C 52 (R 50 48)) `shouldBe` [R 81 14]
            convertR (R 74 14) (C 45 (R 77 23)) `shouldBe` [R 74 3,R 45 11]
            convertR (R 45 11) (C 81 (R 45 19)) `shouldBe` [R 81 11]
            convertR (R 45 11) (C 1 (R 0 69))`shouldBe` [R 46 11]
--
-- convertR R 1 10 (C 50 (98, 2)) =
--         R 1 10 = (1, 1 + 10 - 1)
--         R 98 99 = (98, 98 + 2 - 1)
--         R 98 10 = (max 1 98, min 10 99)
--         R 1 0 = if isValidRange R 98 10 then (1,98-1) else invalid
--         R 1 0 = if isValidRange R 98 10 then (99+1,10) else invalid
--     in case catMaybes $ map validRange [R 1 0, offset (-48) R 98 10, R 1 0] of
--          [] -> [R 1 10]
--          rs -> rs

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
         -- 50 ← 98 2 / R 1 99 → R 1 97,R 50 2
         -- 52 ← 50 48 / R 1 97,R 50 2 →  [R 1 49,R 52 48,R 52 2]
            mappingR [R 1 10] m `shouldBe` [R 1 10]
            mappingR [R 1 10,R 200 8] m `shouldBe` [R 1 10,R 200 8]
            mappingR [R 1 99,R 100 199] m `shouldBe` [R 1 49,R 52 46,R 50 2,R 50 2,R 100 199]
            mappingR [R 74 14] (M [C 45 (R 77 23)])`shouldBe` [R 74 3,R 45 11]
            mappingR [R 74 3,(R 45 11)] (M [C 81 (R 45 19)])`shouldBe` [R 74 3,R 81 11]

    describe "all MappingRs" $ do
        it "converts ranges according to a group of mappings" $ do
            let result = allMappingsR [(seedRanges sf)!!0] (maps sf)
            result `shouldBe`
                 [[R 81 14]
                 ,[R 81 14]
                 ,[R 81 14]
                 ,[R 74 14]
                 ,[R 46 3,R 45 11]
                 ,[R 47 3,R 46 11]
                 ,[R 47 3,R 46 10,R 60 1]]
            (map (sum . (map len)) result) `shouldBe` [14,14,14,14,14,14,14]
            (map (sum . (map len)) (allMappingsR (seedRanges sf) (maps sf))) `shouldBe` [27,27,27,27,27,27,27]
            (length (maps sf)) `shouldBe` 7 
            al <- (readAlmanach . lines) <$> readFile "data/puzzle.txt"
            let result = allMappingsR (seedRanges al) (maps al)
            (map (sum . (map len)) result) `shouldBe`[2014246223,2014246223,2014246223,2014246223,2014246223,2014246223,2014246223] 
            (map (minimum . (map start)) result) `shouldBe` [621215889,54083108,52449107,0,0,15200654,11018487]
            (map (minimum . (map len)) result) `shouldBe` [592198,203262,30075,30075,30075,30075,28522] 
            

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
             lowestLocationR al `shouldBe` 0  -- 11018487 is a wrong answer
    

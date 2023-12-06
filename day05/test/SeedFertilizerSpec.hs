
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
            seedRanges sf `shouldBe` [(79,92),(55,67)]
            (mappings sf) !! 0 `shouldBe` [Mapping 50 (98,99), Mapping 52 (50,97)]
            (mappings sf) !! 6 `shouldBe` [Mapping {dest = 60, range = (56,92)},Mapping {dest = 56, range = (93,96)}]

        describe "has mappings" $ do
            it "that can map a value to another" $ do
                let ms = (mappings sf) !! 0 
                mapping 98 ms `shouldBe` 50
                mapping 51 ms `shouldBe` 53
                mapping 10 ms `shouldBe` 10

            it "that can help find the soil" $ do
                let mms = mappings sf
                allMappings 79 mms `shouldBe` 82
                allMappings 14 mms `shouldBe` 43
                allMappings 55 mms `shouldBe` 86
                allMappings 13 mms `shouldBe` 35

        it "can find lowest location" $ do
            lowestLocation sf `shouldBe` 35

        it "can solve part one of the puzzle" $ do
            puzzle <- lines <$> readFile "data/puzzle.txt"
            let al = readAlmanach puzzle
            lowestLocation al `shouldBe` 178159714

    describe "range mapping" $ do
        it "can map a value to a range" $ do
            rangeMapping (20,29) (Mapping 1000 (20,29)) `shouldBe` (Just (1000,1009), [])
            let m = Mapping 2500 (20,29)
            let range = (10,39)
            -- (10,39) ∩ (20,29) = (20,29)
            -- map 20=2500, 21=2501… offset = 2500 - 20 = 2480
            -- final range = (20+2480,29+2480) 
            rangeMapping range m `shouldBe` (Just (2500,2509),[(10,19),(30,39)])

    describe "range mappings" $ do
        it "can map a value to several ranges" $ do
            let ms = [Mapping {dest = 50, range = (98,99)},Mapping {dest = 52, range = (50,97)}]
            let rs = rangeL 79 14 
            rangeMapping rs (head ms) `shouldBe` (Nothing, [(79,92)])
            -- (79,92) ∩ (50,97) = (79,92)
            -- map 50=52, map 51=52…  offet = 52 - 50 = 2
            -- final range = (79-50+52,92-50+52)
            rangeMapping rs (head (tail ms))`shouldBe` (Just (81,94), [])
            rangeMappings [rs] ms `shouldBe` [(81,94)]


        it "can solve the lowest range location" $ do
            rangeMappings [head (seedRanges sf)] ((mappings sf)!!0)`shouldBe` [(81,94)]
            rangeMappings [(81,94)] ((mappings sf)!!1) `shouldBe` [(81,94)]
            rangeMappings [(81,94)] ((mappings sf)!!2) `shouldBe` [(81,94)]
            rangeMappings [(81,94)] ((mappings sf)!!3) `shouldBe` [(74,87)]
            rangeMappings [(74,87)] ((mappings sf)!!4) `shouldBe` [(45,55),(78,80)]
            rangeMappings [(45,55),(78,80)] ((mappings sf)!!5)`shouldBe` [(46,56),(78,80)]
            rangeMappings [(46,56),(78,80)] ((mappings sf)!!6)`shouldBe`  [(46,55),(60,60),(78,80)]

            rangeMappings [head (tail (seedRanges sf))] ((mappings sf)!!0)`shouldBe` [(57,69)]
            rangeMappings [(57,69)] ((mappings sf)!!1) `shouldBe` [(57,69)]
            rangeMappings [(57,69)] ((mappings sf)!!2)`shouldBe` [(53,56),(61,69)]
            rangeMappings [(53,56),(61,69)] ((mappings sf)!!3)`shouldBe` [(46,49),(61,69)]
            rangeMappings [(46,49),(61,69)] ((mappings sf)!!4)`shouldBe` [(61,63),(68,73),(82,85)]
            rangeMappings [(61,63),(68,73),(82,85)] ((mappings sf)!!5) `shouldBe` [(62,64),(68,73),(82,85)]
            rangeMappings [(62,64),(68,73),(82,85)] ((mappings sf)!!6) `shouldBe` [(66,68),(68,73),(82,85)]


            allRangeMappings [head (seedRanges sf)] (mappings sf) `shouldBe` [(46,55),(60,60),(78,80)]
            lowestLocationRange sf `shouldBe` 46
            lowestLocationRangeNaive sf `shouldBe` 46

        it "can solve part two of the puzzle" $ do
            puzzle <- lines <$> readFile "data/puzzle.txt"
            let al = readAlmanach puzzle
            lowestLocationRange al `shouldBe` 0
            
    describe "mappings" $ do
        it "can be crossed" $ do
            (10,19) `cross` (30,39) `shouldBe`  (Nothing,[(10,19)])
            (10,19) `cross` (0, 5)  `shouldBe`  (Nothing,[(10,19)])
            (10,39) `cross` (20,29) `shouldBe`  (Just (20,29), [(10,19),(30,39)])
            (10,29) `cross` (20,29) `shouldBe`  (Just (20,29), [(10,19)])
            (10,19) `cross` (10,39) `shouldBe`  (Just (10,19), [])
            (10,19) `cross` (10,19) `shouldBe`  (Just (10,19), [])
            (10,29) `cross` (20,39) `shouldBe`  (Just (20,29), [(10,19)])
            (20,29) `cross` (10,39) `shouldBe`  (Just (20,29), [])
            (20,39) `cross` (10,29) `shouldBe`  (Just (20,29), [(30,39)])


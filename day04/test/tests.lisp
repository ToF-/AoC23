(ql:quickload "fiveam")
(in-package :fiveam)
(def-suite* scratchcards)

(require 'cl-ppcre)
(load "./src/scratchcards.lisp")

(defparameter *sample* 
"Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11")

(test read-cards-can-parse-input-lines
      (is (equalp 
            '((1 (41 48 83 86 17) (83 86 6 31 17 9 48 53))
              (2 (13 32 20 16 61) (61 30 68 82 17 32 24 19))
              (3 (1 21 53 59 44) (69 82 63 72 16 21 14 1))
              (4 (41 92 73 84 69) (59 84 76 51 58 5 54 83))
              (5 (87 83 26 28 32) (88 30 70 12 93 22 82 36))
              (6 (31 18 13 56 72) (74 77 10 23 35 67 36 11)))
            (mapcar #'read-card (cl-ppcre:split "\\n" *sample*)))))

(test winnings-counts-winning-numbers
      (is (eql 4 (winnings '(1 (41 48 83 86 17) (83 86 6 31 17 9 48 53))))))

(test points-double-at-every-common-number
      (is (eql 8 (points '(1 (41 48 83 86 17) (83 86 6 31 17 9 48 53)))))
      (is (eql 0 (points '(5 (87 83 26 28 32) (88 30 70 12 93 22 82 36))))))

(test puzzle-part-one
      (is (eql 13 (all-points (mapcar #'read-card (cl-ppcre:split "\\n" *sample*)))))
      (is (eql 33950 (all-points (read-cards-from-file "data/puzzle.txt")))))

(test copies-calculate-number-of-card-copies-won
      (is (eql 30 (copies (mapcar #'winnings (mapcar #'read-card (cl-ppcre:split "\\n" *sample*)))))))

(test puzzle-part-two
      (is (eql 0 (copies (mapcar #'winnings (read-cards-from-file "data/puzzle.txt"))))))

(run!)


(require 'cl-ppcre)
(require 'uiop)

(defun read-card (s)
  (let* ((s1 (cl-ppcre:regex-replace-all "\\|" s ")("))
         (s2 (cl-ppcre:regex-replace-all ":" s1 "("))
         (s3 (cl-ppcre:regex-replace-all "Card " s2 "("))
         (s4 (concatenate 'string s3 ")))")))
    (with-input-from-string (line s4) (read line))))

(defun winnings (card)
  (let ((winning (car (cdr card)))
        (having  (car (cdr (cdr card)))))
    (length (intersection winning having))))

(defun points (card)
  (let ((p (winnings card)))
    (if (> p 0) (expt 2 (- p 1)) 0)))

(defun all-points (cards)
    (apply #'+ (mapcar #'points cards)))

(defun read-cards-from-file (filepath)
  (mapcar #'read-card (uiop:read-file-lines filepath)))

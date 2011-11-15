(define Y
  (lambda (f)
    ((lambda (recur) (f (lambda arg (apply (recur recur) arg))))
     (lambda (recur) (f (lambda arg (apply (recur recur) arg)))))))

(define fact
  (Y (lambda (f)
       (lambda (n)
         (if (<= n 0)
             1
             (* n (f (- n 1))))))))

(print (fact 12))

;;(exit 0)

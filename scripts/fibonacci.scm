(include "./include-test.scm")
(my-print "hey")  
(define (fib n)
  (if (< n 2)
      n
      (+ (fib (- n 1)) (fib (- n 2)))))
(define (infib) 
  (prompt "Enter a number") )
(let 
  ( ( f ( string->number ( infib ) ) ) )
(alert 
  (string-append "Fibonacci(" (number->string f) ") = " (number->string (fib f)))))


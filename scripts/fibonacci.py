def fib(n):
    if n == 0:
        return 0
    elif n == 1:
        return 1
    else:
        return fib(n-1) + fib(n-2)

fin  = int( input( "Enter nth fiboncci number to generate: " ) );
fout = fib( fin ) 
print( "Fibonacci(" + str(fin) + ") = " + str(fout) )

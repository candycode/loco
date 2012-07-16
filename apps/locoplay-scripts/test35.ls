print = Loco.console.println
perror = Loco.console.printerrln
exit = Loco.ctx.exit

try

  ###list
  take(n, [x, ...xs]:list)=
    | n <= 0     => []
    | empty list => []
    | otherwise  => [x] +++ take n - 1,  xs

  print "::take first two elements from list: 1,2,3,4,5"
  print take 2, [ 1 2 3 4 5 ]

  ###currying
  takeThree = take 3
  print "::take three elements with curried function"
  print takeThree [ 1 2 3 4 5 ]

  ###composition
  print "::composed reverse" 
  lastThree = reverse >> takeThree >> reverse
  print lastThree [ 1 to 5 ]
 
  exit 0

catch e

  perror e

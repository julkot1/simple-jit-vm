for i in range(1, 50000000):
    if 50000000 % i == 0:
        print(i)
        
        
1 500000 $range iter i as
    0 i 500000 % = if 
        i print
    end
end


flags
$range
$step
$over
iter {var} as ... end
iter ... end

$iterable 
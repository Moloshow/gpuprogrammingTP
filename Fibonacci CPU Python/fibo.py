#n = int(input("What Fibonacci number would you like to find? "))
n = 10**9

def fib(n):
    i = 0
    nextterm = 1
    present = 1
    previous = 0

    while i < n:
        nextterm = present + previous
        present = previous
        previous = nextterm
        i = i + 1
        #print ("La %s eme valeur est %s")% (i, nextterm)


    return nextterm

print(fib(n))
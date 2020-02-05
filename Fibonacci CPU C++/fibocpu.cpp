#include <iostream>
#include <math.h>
using namespace std;

int fibonacci(int n) {
  unsigned long nextterm = 1;
  unsigned long present = 1;
  unsigned long previous = 0;
  int i;

  for(i = 2; i <= n; i++) {
    nextterm = present + previous;
    present = previous;
    previous = nextterm;
    //cout << nextterm << "\n";
  }
  cout << nextterm << "\n";
  return nextterm;
}

int main(int argc, char const *argv[])
{
	//cout << fibonacci(pow(10,5));
	//cout << fibonacci(100);
	fibonacci(pow(10,9));
}
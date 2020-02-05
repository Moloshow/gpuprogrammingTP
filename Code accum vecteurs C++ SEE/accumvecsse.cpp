#include <immintrin.h>
#include <iostream>

void simd_cumul_vector() {
	__m128 a = _mm_set_ps(1.,2.,3.,4.);
	__m128 b = _mm_set_ps(5.,6.,7.,8.);
	for (int i = 0; i < 1e9; ++i)
	{
		a=_mm_add_ps(a,b);
	}
std::cout << "results:" << a[0] << " " << a[1] << " " << a[2] << " " <<a[3] << std::endl;
}


int main(int argc, char const *argv[])
{
	simd_cumul_vector();
	return 0;
}
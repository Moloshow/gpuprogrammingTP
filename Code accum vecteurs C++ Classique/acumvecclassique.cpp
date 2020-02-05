#include <immintrin.h>
#include <iostream>

void sisd_cumul_vector() {
	float a[4]={1.,2.,3.,4.};
	float b[4]={5.,6.,7.,8.};

	for (int i = 0; i < 1e9; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			a[j]=a[j]+b[j];
		}
	}
std::cout << "results:" << a[0] << " " << a[1] << " " <<a[2] << " " <<a[3] << std::endl;

}

int main(int argc, char const *argv[])
{
	sisd_cumul_vector();
	return 0;
}
#include <iostream>
#include "eigen-3.3.7/Eigen/Dense"
//#include "timer.hpp"

using Eigen::MatrixXcd;

int main() {
    MatrixXcd m,mi;
  {
    //boost::timer::auto_cpu_timer t;
    m = MatrixXcd::Random(1000,1000);
  }
  {
    //boost::timer::auto_cpu_timer t;
    mi = m.inverse();
  }
    std::cout << mi(4,4) << std::endl;
}
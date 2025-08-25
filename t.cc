#include <vector>
#include <exception>
#include "pgplot.hh"

int main(int argc, char** argv) {
  try {

    pgplot::device dev1;
    pgplot::device dev2;
    dev1.env(0,9,0,9,false,pgplot::axis::label);
    dev2.env(10,19,10,19,false,pgplot::axis::label);

    std::vector<long> vx(10);
    std::vector<double> vy(10);
    for (int i=0; i<10; i++) {
      vx[i] = i;
      vy[i] = i;
    }
    dev1.draw_lines< std::vector<long>, std::vector<double> >(vx, vy);
    dev1.label("X axis", "Y axis", "dev1");

    float *ax = new float[10];
    int *ay = new int[10];
    for (int i=0; i<10; i++) {
      ax[i] = i+10;
      ay[i] = i + 10;
    }
    //    dev2.draw_lines<float*, int*>(10, ax, ay);
    dev2.draw_lines(10, ax, ay);

    dev2.label("X axis", "Y axis", "dev2");


  }
  catch (std::exception& e) {
    std::cerr << "exception caught: " << e.what() << std::endl;
    std::exit(1);
  }
}




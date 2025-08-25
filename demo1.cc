#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <valarray>
#include "pgplot.hh"

// ISSUES:
//   examples 9, 10 unimplemented

namespace {

  double myrand(int& seed)
  {
    const int m=2147483647, a=16807, q=127773, r=2836;
    const double am=128.0/m;

    int k = seed / q;
    seed = a * ( seed - k * q) - r * k;
    if (seed < 0)
      seed = seed + m;
    return am * (seed / 128);
  }

  double rnrm(int& seed)
  {
    double r = 2.0;
    double x, y;
    while (r >= 1.0) {
      x = 2.0 * myrand(seed) - 1;
      y = 2.0 * myrand(seed) - 1;
      r = pow(x, 2) + pow(y, 2);
    }
    return x * sqrt(-2.0 * log(r) / r);
  }
}

void example0(const pgplot::device& dev)
{

  const char* list[] = { "version", "state", "user", "now", "device",
			 "file", "type", "dev/type", "hardcopy",
			 "terminal", "cursor"
  };

  for (unsigned int i=0; i<sizeof(list)/sizeof(list[0]); i++)
    std::cout << " " << list[i] << "=" << dev.get_info(list[i]) << std::endl;

  float x1, x2, y1, y2;
  dev.get_view_size(pgplot::unit::inch, x1, x2, y1, y2);
  float x = x2-x1;
  float y = y2-y1;
  std::cout << std::setprecision(2) << std::fixed;
  std::cout << " Plot dimensions (x,y; inches): "
	    << std::setw(9) << x << ", "
	    << std::setw(9) << y << '\n';
  std::cout << "                          (mm): "
	    << std::setw(9) << x*25.4 << ", "
	    << std::setw(9) << y*25.4 << '\n';
}

void example1(const pgplot::device& dev)
{
  dev.env(0, 10, 0, 20, false, pgplot::axis::axis);
  dev.label("(x)", "(y)", "PGPLOT Example 1:  y = x\\u2");

  float xs[] = { 1, 2, 3, 4, 5 };
  int ys[] = { 1, 4, 9, 16, 25 };
  dev.draw_points(sizeof(xs)/sizeof(xs[0]), xs, ys, 9);

  const int npoints = 60;
  std::valarray<float> xr(npoints);
  std::valarray<double> yr(npoints);
  for (int i=0; i<npoints; i++) {
    xr[i] = 0.1 * (i+1);
    yr[i] = pow(xr[i], 2);
  }
  dev.draw_lines(xr,yr);
}

void example2(const pgplot::device& dev)
{
  dev.env(-2, 10, -.4, 1.2, false, pgplot::axis::axis);
  dev.label("(x)", "sin(x)/x", "PGPLOT Example 2:  Sinc Function");

  const int npoints = 100;
  std::valarray<double> xr(npoints);
  std::vector<float> yr(npoints);
  for (int i=0; i<npoints; i++) {
    xr[i] = ((i+1)-20) / 6.0;
    yr[i] = xr[i] != 0.0 ? sin(xr[i])/xr[i] : 1.0;
  }
  dev.draw_lines(xr, yr);
}

void example3(const pgplot::device& dev)
{
  dev.env(0, 720, -2, 2, false, pgplot::axis::none);
  pgplot::save();

  dev.set_color_index(14);
  dev.box("G", 30, 0, "G", .2, 0);

  dev.set_color_index(5);
  dev.box("ABCTSN", 90, 3, "ABCTSNV", 0, 0);

  dev.set_color_index(3);
  dev.label("x (degrees)","f(x)","PGPLOT Example 3");

  float xr[360];
  float yr[360];

  for (int i=0; i<360; i++) {
    xr[i] = 2*(i+1);
    float arg = xr[i] * M_PI / 180.0;
    yr[i] = sin(arg) + 0.5 * cos(2*arg) +
      0.5 * sin(1.5 * arg + M_PI / 3.0);
  }

  dev.set_color_index(6);
  dev.set_line_style(pgplot::linestyle::dash);
  dev.set_line_width(3);
  dev.draw_lines(360, xr, yr);
  pgplot::unsave();
}

void example4(const pgplot::device& dev)
{

  int seed = -5678921;

  std::vector<float> data(1000);
  for (std::vector<float>::iterator it=data.begin(); it<data.end(); it++)
    *it = rnrm(seed);

  pgplot::save();

  dev.hist(data, -3.1, 3.1, 31, 0);

  for (int i=0; i<200; i++)
    data[i] = 1 + 0.5 * rnrm(seed);

  dev.set_color_index(15);
  dev.hist(200, &data[0], -3.1, 3.1, 31, 3);
  dev.set_color_index(0);
  dev.hist(200, &data[0], -3.1, 3.1, 31, 1);
  dev.set_color_index(1);

  dev.box("BST", 0, 0, "", 0, 0);
  dev.label("Variate", "", "PGPLOT Example 4:  Histograms (Gaussian)");

  float x[620], y[620];
  for (int i=0; i<620; i++) {
    x[i] = -3.1 + 0.01*i;
    y[i] = 0.2 * 1000./ sqrt(2.0*M_PI)*exp(-0.5*x[i]*x[i]);
  }
  dev.draw_lines(620, x, y);
  pgplot::unsave();
}

void example5(const pgplot::device& dev)
{
  enum { red=2, green=3, cyan=5 };
  const float freq[] = { 26., 38., 80., 160., 178., 318.,
			 365., 408., 750., 1400., 2695., 2700.,
			 5000., 10695., 14900.
  };
  const float flux[] = { 38.0, 66.4, 89.0, 69.8, 55.9, 37.4,
			 46.8, 42.4, 27.0, 15.8, 9.09, 9.17,
			 5.35, 2.56, 1.73
  };
  const float err[] = { 6.0, 6.0, 13.0, 9.1, 2.9, 1.4,
			2.7, 3.0, 0.34, 0.8, 0.2, 0.46,
			0.15, 0.08, 0.01
  };
  const int n = sizeof(freq) / sizeof(float);

  pgplot::save();

  dev.set_color_index(cyan);
  dev.env(-2, 2, -.5, 2.5, true, pgplot::axis::log);
  dev.label("Frequency, \\gn (GHz)",
	    "Flux Density, S\\d\\gn\\u (Jy)",
	    "PGPLOT Example 5:  Log-Log plot");

  float xp[100];
  float yp[100];
  for (int i=0; i<100; i++)
    {
      float x = 1.3 + (i+1)*.03;
      xp[i] = x - 3;
      yp[i] = 5.18 - 1.15*x -7.72*exp(-x);
    }

  dev.set_color_index(red);
  dev.draw_lines(100, xp, yp);

  for (int i=0; i<n; i++)
    {
      xp[i] = log10(freq[i]) - 3;
      yp[i] = log10(flux[i]);
    }
  dev.set_color_index(green);
  dev.draw_points(n, xp, yp, 17);

  std::vector<float> yhigh(n);
  std::vector<float> ylow(yhigh);
  for (int i=0; i<n; i++)
    {
      yhigh[i] = log10(flux[i] + 2 * err[i]);
      ylow[i] = log10(flux[i] - 2 * err[i]);
    }

  dev.errbary(n, xp, &ylow[0], &yhigh[0], 1);
  pgplot::unsave();
}

void example6(const pgplot::device& dev)
{
  pgplot::begin_batch();
  pgplot::save();

  dev.page();
  dev.set_viewport(0, 1, 0, 1);
  dev.window_adjust(0, 10, 0, 10);

  dev.set_color_index(1);
  dev.text("T", -2, .5, .5, "PGPLOT fill area: routines PGPOLY, PGCIRC, PGRECT");

  const char* labels[] = { "Fill style 1 (solid)", "Fill style 2 (outline)",
		     "Fill style 3 (hatched)", "Fill style 4 (cross-hatched)"
  };
  const pgplot::fillstyle::value fs[] = { pgplot::fillstyle::solid,
					  pgplot::fillstyle::outline,
					  pgplot::fillstyle::hatched,
					  pgplot::fillstyle::crosshatched
  };

  const int n1[] = { 3, 4, 5, 5, 6, 8 };
  const int n2[] = { 1, 1, 1, 2, 1, 3 };
  const int npol = sizeof(n1) / sizeof(int);
  float x[10], y[10];

  for (int k=0; k<4; k++)
    {
      dev.set_color_index(1);
      float y0 = 10 - 2 * (k+1);
      dev.text(.2, y0+.6, labels[k]);
      dev.set_fill_style(fs[k]);

      for (int i=0; i<npol; i++)
	{
	  dev.set_color_index(i+1);
	  for (int j=0; j<n1[i]; j++)
	    {
	      float angle = n2[i] * 2 * M_PI * j / static_cast<float>(n1[i]);
	      x[j] = (i+1) + 0.5 * cos(angle);
	      y[j] = y0 + 0.5 * sin(angle);
	    }
	  dev.draw_poly(n1[i], x, y);
	}
      dev.set_color_index(7);
      dev.draw_circle(7, y0, 0.5);
      dev.set_color_index(8);
      dev.draw_rectangle(7.8, 9.5, y0-.5, y0+.5);
    }

  pgplot::unsave();
  pgplot::end_batch();
}

void example7(const pgplot::device& dev)
{
  pgplot::begin_batch();
  pgplot::save();

  dev.set_color_index(1);
  dev.env(0, 5, -.3, .6, false, pgplot::axis::axis);
  dev.label("\\fix", "\\fiy", "PGPLOT Example 7: scatter plot");

  int seed = -45678921;
  float xs[300], ys[300];

  for (int i=0; i<300; i++) {
    xs[i] = 5 * myrand(seed);
    ys[i] = xs[i] * exp(-xs[i]) + 0.05 * rnrm(seed);
  }
  dev.set_color_index(3);
  dev.draw_points(100, xs, ys, 3);
  dev.draw_points(100, &xs[100], &ys[100], 17);
  dev.draw_points(100, &xs[200], &ys[200], 21);

  float xr[101], yr[101];
  for (int i=0; i<101; i++) {
    xr[i] = .05 * i;
    yr[i] = xr[i] * exp(-xr[i]);
  }
  dev.set_color_index(2);
  dev.draw_lines(101, xr, yr);

  float xp = xs[100];
  float yp = ys[100];
  dev.set_color_index(5);
  dev.set_char_height(3);
  dev.errbar_single(pgplot::err::x, xp, yp, 0.2, 1);
  dev.errbar_single(pgplot::err::y, xp, yp, 0.1, 1);
  dev.draw_marker(xp, yp, 21);

  pgplot::unsave();
  pgplot::end_batch();
}

void example8(const pgplot::device& dev)
{
  enum { black, white, red, green, blue, cyan, magenta, yellow };

  dev.page();

  pgplot::begin_batch();
  pgplot::save();

  dev.set_viewport(.1, .6, .1, .6);
  dev.set_window(0, 630, -2, 2);

  dev.set_color_index(cyan);
  dev.box("ABCTS", 90, 3, "ABCTSV", 0, 0);

  dev.set_color_index(red);
  dev.box("N", 90, 3, "VN", 0, 0);

  float xr[360], yr[360];
  for (int i=0; i<360; i++) {
    xr[i] = 2 * (i+1);
    yr[i] = sin(xr[i] / 57.29577951);
  }

  dev.set_color_index(magenta);
  dev.set_line_style(pgplot::linestyle::dash);
  dev.draw_lines(360, xr, yr);

  dev.set_window(90, 720, -2, 2);
  dev.set_color_index(yellow);
  dev.set_line_style(pgplot::linestyle::dot);
  dev.draw_lines(360, xr, yr);
  dev.set_line_style(pgplot::linestyle::line);

  dev.set_viewport(.45, .85, .45, .85);
  dev.set_window(0, 180, -2, 2);
  dev.set_color_index(0);
  dev.draw_rectangle(0, 180, -2, 2);

  dev.set_color_index(blue);
  dev.box("ABCTSM", 60, 3, "VABCTSM", 1, 2);

  dev.set_color_index(white);
  dev.set_line_style(pgplot::linestyle::dash);
  dev.draw_lines(360, xr, yr);

  pgplot::unsave();
  pgplot::end_batch();
}

void example9(const pgplot::device&) { }
void example10(const pgplot::device&) { }

void example11(const pgplot::device& dev)
{

  const float t = 1.618f;
  const float t1 = 1 + t;
  const float t2 = -t;
  const float t3 = -t1;

  const float vert[20][3] = {
    { t, t, t }, { t, t, t2 },
    { t, t2, t }, { t, t2, t2 },
    { t2, t, t }, { t2, t, t2 },
    { t2, t2, t }, { t2, t2, t2 },
    { t1, 1, 0 }, { t1, -1, 0 },
    { t3, 1, 0 }, { t3, -1, 0 },
    { 0, t1, 1 }, { 0, t1, -1 },
    { 0, t3, 1 }, { 0, t3, -1 },
    { 1, 0, t1 }, { -1, 0, t1 },
    { 1, 0, t3 }, { -1, 0, t3 }
  };

  pgplot::begin_batch();
  pgplot::save();

  dev.env(-4, 4, -4, 4, 1, pgplot::axis::none);
  dev.set_color_index(2);
  dev.set_line_style(pgplot::linestyle::line);
  dev.set_line_width(1);

  dev.label("", "", "PGPLOT Example 11:  Dodecahedron");

  for (int i=0; i<20; i++ )
    dev.draw_marker(vert[i][0] + .2*vert[i][2], vert[i][1] + .3*vert[i][2], 9);

  dev.set_line_width(3);
  float x[2], y[2];
  for (int i=1; i<20; i++) {
    for (int j=0; j<i; j++) {
      float r = 0;
      for (int k=0; k<3; k++)
	r += (pow(vert[i][k] - vert[j][k], 2));
      r = sqrt(r);
      if (fabs(r-2) > .1)
	continue;

      x[0] = vert[i][0] + .2 * vert[i][2];
      y[0] = vert[i][1] + .3 * vert[i][2];

      x[1] = vert[j][0] + .2 * vert[j][2];
      y[1] = vert[j][1] + .3 * vert[j][2];

      dev.draw_lines(2,x,y);
    }
  }

  pgplot::unsave();
  pgplot::end_batch();
}

void example12(const pgplot::device& dev)
{
  pgplot::begin_batch();
  pgplot::save();

  dev.set_char_height(.7);
  dev.set_color_index(2);
  dev.env(-1.05, 1.05, -1.05, 1.05, true, pgplot::axis::box);
  dev.label("", "", "PGPLOT Example 12: PGARRO");
  dev.set_color_index(1);

  pgplot::arrowhead::value ahs=pgplot::arrowhead::filled;
  const int nv = 16;
  const float d = 360./ 57.29577951 / nv ;
  float a = -d;
  for (int i=0; i<nv; i++) {
    a += d;
    float x = cos(a);
    float y = sin(a);
    float xt = .2 * cos(a-d);
    float yt = .2 * sin(a-d);

    dev.set_arrowhead_style(ahs, 80-3*(i+1), .5 * i / static_cast<float>(nv));
    dev.set_char_height(.25 * (i+1));
    dev.draw_arrow(xt, yt, x, y);

    if (ahs == pgplot::arrowhead::outline)
      ahs = pgplot::arrowhead::filled;
    else
      ahs = pgplot::arrowhead::outline;
  }
    
  pgplot::end_batch();
  pgplot::unsave();
}

void example13(const pgplot::device& dev)
{
  dev.page();

  pgplot::save();
  pgplot::begin_batch();

  dev.set_char_height(0.7);

  const int n=10;
  const float x1[n] = { 0,0,0,0, -8000.0, 100.3, 205.3, -45000.0, 0,0 };
  const float x2[n] = { 8000,8000,8000,8000,  8000.0, 101.3, 201.1, -100000,-100000,-100000 };
  const char* xopt[n] = { "BSTN", "BSTNZ", "BSTNZH", "BSTNZD", "BSNTZHFO", 
			  "BSTNZD", "BSTNZHI", "BSTNZHP", "BSTNZDY", "BSNTZHFOY"
  };

  for (int i=0; i<n; i++) {
    dev.set_viewport(0.15, 0.85,
		     (0.7 + (n-(i+1))) / static_cast<float>(n),
		     (0.7 + (n-(i+1)+1)) / static_cast<float>(n)
		     );
    dev.set_window(x1[i], x2[i], 0, 1);
    dev.text_box(xopt[i], 0, 0, "", 0, 0);
    dev.label(std::string("Option = ") + std::string(xopt[i]), "", "");
    if (i == 0)
      dev.text("B", -1, .5, .5, "\\fiAxes drawn with PGTBOX");
  }

  pgplot::end_batch();
  pgplot::unsave();
}

void example14(const pgplot::device& dev)
{

  const int n=33;
  const int m=8;

  float xi[n], yi[n], x0[n], y0[n], xt[3], yt[3];

  const float thinc = 2 * M_PI / static_cast<float>(n);

  for (int i=0; i<n; i++)
    xi[i] = yi[i] = 0;

  pgplot::begin_batch();
  pgplot::save();

  dev.env(-1, 1, -1, 1, true, pgplot::axis::none);
  dev.label("", "", "PGPLOT Example 14: PGPOLY and PGSCR");

  for (int j=0; j<m; j++) {
    float r = 1;
    float g = 1 - (j+1) / static_cast<float>(m);
    float b = g;
    dev.set_color_rep(j+1, r, g, b);
    float theta = -(j+1) * M_PI / static_cast<float>(n);

    r = (j+1) / static_cast<float>(m);

    for (int i=0; i<n; i++) {
      theta += thinc;
      x0[i] = r * cos(theta);
      y0[i] = r * sin(theta);
    }

    for (int i=0; i<n; i++) {
      xt[0] = x0[i];
      yt[0] = y0[i];
      xt[1] = x0[(i+1) % n];
      yt[1] = y0[(i+1) % n];
      xt[2] = xi[i];
      yt[2] = yi[i];

      dev.set_color_index(j+1);
      dev.set_fill_style(pgplot::fillstyle::solid);
      dev.draw_poly(3, xt, yt);
      dev.set_fill_style(pgplot::fillstyle::outline);
      dev.set_color_index(1);
      dev.draw_poly(3, xt, yt);

      xi[i] = x0[i];
      yi[i] = y0[i];
    }
  }

  pgplot::unsave();
  pgplot::end_batch();

}

void example15(const pgplot::device& dev)
{
  pgplot::begin_batch();
  pgplot::save();

  const int n=17;
  float x[n], y[n];

  float d = 360 / n;
  float a = -d;

  for (int i=0; i<n; i++) {
    a += d;
    x[i] = cos(a/57.29577951);
    y[i] = sin(a/57.29577951);
  }

  dev.set_char_height(0.5);
  dev.set_color_index(2);
  dev.env(-1.05, 1.05, -1.05, 1.05, true, pgplot::axis::box);
  dev.label("", "", "PGPLOT Example 15: PGMOVE and PGDRAW");
  dev.set_color_rep(0, .2, .3, .3);
  dev.set_color_rep(1, 1, .5, .2);
  dev.set_color_rep(2, .2, .5, 1);
  dev.set_color_index(1);

  for (int i=0; i<n-1; i++) {
    for (int j=i; j<n; j++) {
      dev.move_pen(x[i], y[i]);
      dev.draw_line(x[j], y[j]);
    }
  }

  pgplot::unsave();
  pgplot::end_batch();
}

int main()
{
  try {
    pgplot::device dev("?");

    example0(dev);
    example1(dev);
    example2(dev);
    example3(dev);
    dev.subdivide(2, 1);
    example4(dev);
    example5(dev);
    dev.subdivide(1,1);
    example6(dev);
    example7(dev);
    example8(dev);
    example9(dev);
    example10(dev);
    example11(dev);
    example12(dev);
    example13(dev);
    example14(dev);
    example15(dev);
  }
  catch (const std::exception& e) {
    std::cerr << "Exception caught, terminating: " << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "Exception caught, terminating: unknown" << std::endl;
  }
}

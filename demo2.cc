#include <iostream>
#include <cstdio>
#include <vector>
#include <valarray>
#include <string>
#include <exception>
#include <algorithm>
#include <cmath>
#include "pgplot.hh"

void example1(const pgplot::device& dev)
{
  pgplot::begin_batch();

  dev.page();
  dev.set_viewport(0, 1, 0, 1);
  dev.window_adjust(0, 1, 0, 1);
  dev.set_color_index(14);
  dev.box("g", 0.02, 1, "g", 0.02, 1);
  dev.set_color_index(15);
  dev.box("g", 0.1, 5, "g", 0.1, 5);
  dev.set_color_index(1);
  dev.box("bc", 0.1, 5, "bc", 0.1, 5);

  for (int i=0; i<16; i++) {
    dev.set_color_index(i);
    float x1 = 0.31 + (i % 4) * 0.1;
    float y1 = 0.61 - (i/4) * 0.1;
    dev.draw_rectangle(x1, x1+0.08, y1, y1+0.08);
  }

  dev.set_color_index(0);
  dev.draw_rectangle(0.31, 1-0.31, 0.85, 0.97);
  dev.set_color_index(1);
  dev.set_fill_style(pgplot::fillstyle::outline);
  dev.draw_rectangle(0.31, 1-.31, 0.85, 0.97);
  dev.text(0.5, 0.91, 0, 0.5, std::string("PGPLOT ")+dev.get_info("version"));
  dev.text(0.5, 0.87, 0, 0.5, std::string("device ")+dev.get_info("type"));

  pgplot::end_batch();
}

void example2(const pgplot::device& dev)
{

  dev.page();
  dev.set_viewport(0, 1, 0, 1);

  float x1, x2, y1, y2;
  dev.get_viewport(pgplot::unit::inch, x1, x2, y1, y2);
  float x = x2-x1;
  float y = y2-y1;

  float xpix1, xpix2, ypix1, ypix2;
  dev.get_viewport(pgplot::unit::pixel, xpix1, xpix2, ypix1, ypix2);
  float res = std::abs((xpix2-xpix1)/x);
  int lw = res > 166 ? 2 : 1;

  int nx, ny;
  if (x>y)
    nx=8, ny=5;
  else
    nx=5, ny=8;

  float dx = std::min(x/nx, 0.95f*y/ny);
  float dy = dx;

  int ix = nx;
  int jy = 1;

  float xoff = x1 + (x-nx*dx)*0.5;
  float yoff = y1 + (0.95*y-ny*dy)*0.5;

  pgplot::begin_batch();

  dev.set_window(-1, 1, -1, 1);

  for (int n=0; n<40; ++n) {

    char buf[3];
    std::sprintf(buf, "%2d", n<=31 ? n : 31-n);

    ++ix;
    if (ix > nx) {
      --jy;
      ix=1;
    }

    if (jy<1) {
      jy=ny;
      if (n) dev.page();
      dev.set_char_height(1.2);
      dev.set_viewport_size(xoff, xoff+nx*dx, yoff, yoff+ny*dy);
      dev.set_line_width(lw);
      dev.text("T", 1.0, 0.5, 0.5, "\\fiPGPLOT \\frMarker Symbols");
    }

    dev.set_viewport_size(xoff+(ix-1)*dx, xoff+ix*dx, yoff+(jy-1)*dy, yoff+jy*dy);
    dev.set_line_width(1);
    dev.box("BC", 10, 0, "BC", 10, 0);
    dev.set_char_height(0.5);
    dev.text("T", -1.5, .05, 0, buf);

    dev.set_line_width(lw);
    dev.set_char_height(1.5);
    dev.draw_marker(0, 0, n<=31 ? n : 31-n);
  }

  pgplot::end_batch();
}

void example3(const pgplot::device& dev)
{

  const char* sample[] = {
    "Normal:  \\fnABCDQ efgh 1234 \\ga\\gb\\gg\\gd \\gL\\gH\\gD\\gW",
    "Roman:  \\frABCDQ efgh 1234 \\ga\\gb\\gg\\gd \\gL\\gH\\gD\\gW",
    "Italic:  \\fiABCDQ efgh 1234 \\ga\\gb\\gg\\gd \\gL\\gH\\gD\\gW",
    "Script:  \\fsABCDQ efgh 1234 \\ga\\gb\\gg\\gd \\gL\\gH\\gD\\gW",
    "\\fif\\fr(\\fix\\fr) = \\fix\\fr\\u2\\dcos(2\\gp\\fix\\fr)e\\u\\fix\\fr\\u2",
    "\\fiH\\d0\\u \\fr= 75 \\(2233) 25 km s\\u-1\\d Mpc\\u-1\\d",
    "\\fsL/L\\d\\(2281)\\u\\fr = 5\\.6 \\x 10\\u6\\d (\\gl1216\\A)",
    "Markers: 3=\\m3, 8=\\m8, 12=\\m12, 28=\\m28.",
    "Cyrillic: \\(2830)\\(2912)\\(2906)\\(2911)\\(2919)\\(2917)\\(2915)."
  };

  const int n = sizeof(sample) / sizeof(sample[0]);

  dev.env(0, 20, n, 0, false, pgplot::axis::none);
  dev.label("", "", "\\fiPGPLOT \\frFonts");

  dev.set_char_height(1.6);
  for (int i=0; i<n; ++i) {
    float x1 = 0;
    float y1 = i+0.5;
    dev.text(x1, y1, sample[i]);
  }
  dev.set_char_height(1);
}

void example4(const pgplot::device& dev)
{

  pgplot::begin_batch();

  dev.env(0, 15, 0, 15, false, pgplot::axis::label);

  dev.label("Line Width", "", "\\fiPGPLOT \\frLine Widths");

  std::vector<int> x(2), y(2);

  for (int iw=1; iw<15; ++iw) {
    x[0] = y[1] = iw;
    x[1] = y[0] = 0;
    dev.set_line_width(iw);
    dev.draw_lines(x, y);
  }

  dev.set_line_style(pgplot::linestyle::dash);
  for (int iw=1; iw<15; ++iw) {
    x[0] = y[1] = iw;
    x[1] = y[0] = 15;
    dev.set_line_width(iw);
    dev.draw_lines(x, y);
  }

  dev.set_line_style(pgplot::linestyle::line);
  dev.set_line_width(1);


  pgplot::end_batch();
}

void example5(const pgplot::device& dev)
{
}

void example6(const pgplot::device&)
{
}

int main()
{
  try {
    pgplot::device dev("?");

    example1(dev);
    example2(dev);
    example3(dev);
    example4(dev);
    example5(dev);
    example6(dev);
  }
  catch (const std::exception& e) {
    std::cerr << "Exception caught, terminating: " << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << "Exception caught, terminating: unkown" << std::endl;
  }
}

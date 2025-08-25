#ifndef PGPLOT_HH
#define PGPLOT_HH

#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <valarray>
#include <algorithm>

extern "C" {
#include <cpgplot.h>
}

// TODO:
//   valarray specializations (address of element doesn't work...)
//   2D data inputs for auto_float
//   data outputs for auto_float

namespace pgplot {

  using std::size_t;

  bool debug = true;

  class auto_float {

    friend class device;

  private:
    bool our_data;
    size_t n;
    float *data;

  public:

    template <typename T>
    auto_float(const std::vector<T>& v)
      : our_data(true), n(v.size()), data(new float[n])
    {
      copy(v.begin(), v.end(), data);
    }

    template <typename T>
    auto_float(size_t n_, const std::vector<T>& v)
      : our_data(true), n(n_), data(new float[n])
    {
      copy(v.begin(), v.begin()+n, data);
    }

    template <typename T>
    auto_float(const std::valarray<T>& v)
      : our_data(true), n(v.size()), data(new float[n])
    {
      for (size_t i=0; i<n; ++i)
	data[i] = v[i];
    }

    template <typename T>
    auto_float(size_t n_, const std::valarray<T>& v)
      : our_data(true), n(n_), data(new float[n])
    {
      for (size_t i=0; i<n; i++)
	data[i] = v[i];
    }

    template <typename T>
    auto_float(size_t n_, const T* data_)
      : our_data(true), n(n_), data(new float[n])
    {
      std::copy(data_, data_+n, data);
    }

    template <typename T>
    auto_float(const T* begin, const T* end)
      : our_data(true), n(end-begin), data(new float[n])
    {
      std::copy(begin, end, data);
    }

    ~auto_float() {
      if (our_data)
	delete [] data;
    }
  };

  //
  // specializations for efficiency when passed float[], vector<float>
  //
  template <> auto_float::auto_float(size_t n_, const float* data_)
    : our_data(false), n(n_), data(const_cast<float*>(data_))
  {
    if (debug)
      std::cerr << "auto_float(size_t, const* float)" << std::endl;
  }

  template <> auto_float::auto_float(const float* begin, const float* end)
    : our_data(false), n(end-begin), data(const_cast<float*>(begin))
  {
    if (debug)
      std::cerr << "auto_float(const* float, const* float)" << std::endl;
  }

  template <> auto_float::auto_float(const std::vector<float>& data_)
    : our_data(false), n(data_.size()), data(const_cast<float*>(&data_[0]))
  {
    if (debug)
      std::cerr << "auto_float(std::vector<float>&)" << std::endl;
  }

  template <> auto_float::auto_float(size_t n_, const std::vector<float>& data_)
    : our_data(false), n(n_), data(const_cast<float*>(&data_[0]))
  {
    if (debug)
      std::cerr << "auto_float(size_t, std::vector<float>&)" << std::endl;
  }

  namespace font {
    enum value { normal=1, roman=2, italic=3, script=4 };
  }
  namespace fillstyle {
    enum value { solid=1, outline=2, hatched=3, crosshatched=4 };
  }
  namespace image_transfer {
    enum value { linear=0, log=1, sqrt=2 };
  }
  namespace linestyle {
    enum value { line=1, dash=2, dotdash=3, dot=4, dashdot=5 };
  }
  namespace arrowhead {
    enum value { filled=1, outline=2 };
  }
  namespace unit {
    enum value { norm=0, inch=1, mm=2, pixel=3, world=4, viewport=5 };
  }
  namespace axis {
    enum value { none=-2, box=-1, label=0, axis=1, grid=2, xlog=10, ylog=20, log=30 };
  }
  namespace err {
    enum value { plusx=1, plusy=2, minusx=3, minusy=4, x=5, y=6 };
  }

  inline void save() { cpgsave(); }

  inline void unsave() { cpgunsa(); }

  inline void begin_batch() { cpgbbuf(); }

  inline void end_batch() { cpgebuf(); }

  // exception thrown when pgopen() fails
  class open_error : public std::runtime_error {
  public:
    open_error(const std::string &m = "failed to open device") : std::runtime_error(m) { }
  };

  class device {
  private:
    int id_;
    std::string devname_;
    // make copy ctor and copy assignment inaccessible
    device(const device&);
    device& operator=(const device&);

  public:

    explicit device(const std::string& devname =
		    std::getenv("PGPLOT_DEV") ? std::getenv("PGPLOT_DEV") : "?"
		    ) throw(open_error) :
      devname_(devname)
    {
      id_ = cpgopen(devname_.c_str());
      if (id_ <= 0)
	throw open_error(std::string("failed to open device '") + devname_ + "'");
    }

    virtual ~device() {
      int current_device;
      cpgqid(&current_device);
      cpgslct(id());
      cpgclos();
      if (current_device != id() && current_device != 0)
	cpgslct(current_device);
    }

    int id() const throw() { return id_; }

    void select() const throw() { cpgslct(id()); }

    void draw_arrow(float x1, float y1, float x2, float y2) const throw()
    {
      select();
      cpgarro(x1, y1, x2, y2);
    }

    void ask(bool flag)
      const throw()
    {
      select();
      cpgask(flag);
    }

    void draw_axis(const std::string& opt,
		   float x1, float y1,
		   float x2, float y2,
		   float v1, float v2,
		   float step, int nsub,
		   float majl, float majr,
		   float min, float disp,
		   float orient)
      const throw()
    {
      select();
      cpgaxis(opt.c_str(), x1, y1, x2, y2, v1, v2, step, nsub,
	      majl, majr, min, disp, orient);
    }

    bool band(int mode, bool posn, float xref, float yref, float& x, float& y, char& ch)
      const throw()
    {
      select();
      return cpgband(mode, posn, xref, yref, &x, &y, &ch);
    }

    template<typename T1, typename T2>
    void hist(T1 v1, T2 v2, bool center) const
    {
      auto_float d1(v1);
      auto_float d2(v2);
      select();
      cpgbin(d1.n, d1.data, d2.data, center);
    }

    template<typename T1, typename T2>
    void hist(size_t n, const T1* p1, const T2* p2, bool center) const
    {
      auto_float d1(n, p1);
      auto_float d2(n, p2);
      select();
      cpgbin(d1.n, d1.data, d2.data, center);
    }

    void box(const std::string& xopt, float xtick, int xsub,
	     const std::string& yopt, float ytick, int ysub)
      const throw()
    {
      select();
      cpgbox(xopt.c_str(), xtick, xsub, yopt.c_str(), ytick, ysub);
    }
	     

    void draw_circle(float x, float y, float r) const throw()
    {
      select();
      cpgcirc(x, y, r);
    }

    // FIXME: PGCONB()
    // FIXME: PGCONF()
    // FIXME: PGCONL()
    // FIXME: PGCONS()
    // FIXME: PGCONT()
    // FIXME: PGCONX()

    template<typename T1, typename T2, typename T3, typename T4>
    void ctab(T1 v1, T2 v2, T3 v3, T4 v4, float contrast, float bright) const
    {
      auto_float l(v1);
      auto_float r(v2);
      auto_float g(v3);
      auto_float b(v4);
      select();
      cpgctab(l.data, r.data, g.data, b.data, l.n, contrast, bright);
    }
    template<typename T1, typename T2, typename T3, typename T4>
    void ctab(const T1* p1, const T2* p2, const T3* p3, const T4* p4, size_t n, float contrast, float bright) const
    {
      auto_float l(n, p1);
      auto_float r(n, p2);
      auto_float g(n, p3);
      auto_float b(n, p4);
      select();
      cpgctab(l.data, r.data, g.data, b.data, n, contrast, bright);
    }

    bool get_cursor_pos(float& x, float& y, char& ch) const throw()
    {
      select();
      return cpgcurs(&x, &y, &ch);
    }

    void draw_line(float x, float y) const throw()
    {
      select();
      cpgdraw(x, y);
    }

    void env(float xmin, float xmax, float ymin, float ymax, bool just, axis::value axis)
      const throw()
    {
      select();
      cpgenv(xmin,xmax,ymin,ymax,just,axis);
    }

    void erase() const throw()
    {
      select();
      cpgeras();
    }

    void errbar_single(err::value dir, float x, float y, float e, float t)
      const throw()
    {
      select();
      cpgerr1(dir, x, y, e, t);
    }

    template<typename T1, typename T2, typename T3>
    void errbar(err::value dir, T1 v1, T2 v2, T3 v3, float t) const
    {
      auto_float x(v1);
      auto_float y(v2);
      auto_float e(v3);
      select();
      cpgerrb(dir, x.n, x.data, y.data, e.data, t);
    }
    template<typename T1, typename T2, typename T3>
    void errbar(err::value dir, size_t n, const T1* p1, const T2* p2, const T3* p3, float t) const
    {
      auto_float x(n, p1);
      auto_float y(n, p2);
      auto_float e(n, p3);
      select();
      cpgerrb(dir, n, x.data, y.data, e.data, t);
    }

    template<typename T1, typename T2, typename T3>
    void errbarx(T1 v1, T2 v2, T3 v3, float t) const
    {
      auto_float x1(v1);
      auto_float x2(v2);
      auto_float y(v3);
      select();
      cpgerrx(x1.n, x1.data, x2.data, y.data, t);
    }
    template<typename T1, typename T2, typename T3>
    void errbarx(size_t n, const T1* p1, const T2* p2, const T3* p3, float t) const
    {
      auto_float x1(n, p1);
      auto_float x2(n, p2);
      auto_float y(n, p3);
      select();
      cpgerrx(n, x1.data, x2.data, y.data, t);
    }

    template<typename T1, typename T2, typename T3>
    void errbary(T1 v1, T2 v2, T3 v3, float t) const
    {
      auto_float x(v1);
      auto_float y1(v2);
      auto_float y2(v3);
      select();
      cpgerry(x.n, x.data, y1.data, y2.data, t);
    }

    template<typename T1, typename T2, typename T3>
    void errbary(size_t n, const T1* p1, const T2* p2, const T3* p3, float t) const
    {
      auto_float x(n, p1);
      auto_float y1(n, p2);
      auto_float y2(n, p3);
      select();
      cpgerry(n, x.data, y1.data, y2.data, t);
    }


    void erase_text() const throw()
    {
      select();
      cpgetxt();
    }

    // FIXME: PGGRAY()
    // FIXME: PGHI2D()

    template<typename T1>
    void hist(T1 v1, float min, float max, int nbin, int flag) const
    {
      auto_float data(v1);
      select();
      cpghist(data.n, data.data, min, max, nbin, flag);
    }
    template<typename T1>
    void hist(size_t n, const T1* p1, float min, float max, int nbin, int flag) const
    {
      auto_float data(n, p1);
      select();
      cpghist(n, data.data, min, max, nbin, flag);
    }

    void identity() const throw()
    {
      select();
      cpgiden();
    }

    // FIXME: PGIMAG()

    void label(const std::string& xlabel,
	       const std::string& ylabel,
	       const std::string& toplabel
	       ) const throw()
    {
      select();
      cpglab(xlabel.c_str(), ylabel.c_str(), toplabel.c_str());
    }

    // FIXME: PGLCUR()

    static void list_devices() throw() { cpgldev(); }

    void text_length(unit::value units, const std::string& text, float& xl, float& yl)
      const throw()
    {
      select();
      cpglen(units, text.c_str(), &xl, &yl);
    }

    template<typename T1, typename T2>
    void draw_lines(T1 v1, T2 v2) const
    {
      auto_float d1(v1);
      auto_float d2(v2);
      select();
      cpgline(d1.n, d1.data, d2.data);
    }

    template<typename T1, typename T2>
    void draw_lines(size_t n, const T1* p1, const T2* p2) const
    {
      auto_float d1(n, p1);
      auto_float d2(n, p2);
      select();
      cpgline(d1.n, d1.data, d2.data);
    }

    void move_pen(float x, float y) const throw()
    {
      select();
      cpgmove(x, y);
    }

    void text(const std::string& side, float disp, float coord, float just,
	      const std::string& text) const throw()
    {
      select();
      cpgmtxt(side.c_str(), disp, coord, just, text.c_str());
    }

    // FIXME: PGNCUR()
    // FIXME: PGOLIN()

    void page() const throw()
    {
      select();
      cpgpage();
    }

    void panel(int x, int y) const throw()
    {
      select();
      cpgpanl(x, y);
    }

    void set_view_size(float width, float aspect) const throw()
    {
      select();
      cpgpap(width, aspect);
    }

    // FIXME: PGPIXL()
    // FIXME: PGPNTS()

    template<typename T1, typename T2>
    void draw_poly(T1 v1, T2 v2) const
    {
      auto_float x(v1);
      auto_float y(v2);
      select();
      cpgpoly(x.n, x.data, y.data);
    }
    template<typename T1, typename T2>
    void draw_poly(size_t n, const T1* p1, const T2* p2) const
    {
      auto_float x(n, p1);
      auto_float y(n, p2);
      select();
      cpgpoly(n, x.data, y.data);
    }

    template<typename T1, typename T2>
    void draw_points(T1 v1, T2 v2, int symbol) const
    {
      auto_float x(v1);
      auto_float y(v2);
      select();
      cpgpt(x.n, x.data, y.data, symbol);
    }
    template<typename T1, typename T2>
    void draw_points(size_t n, const T1* p1, const T2* p2, int symbol) const
    {
      auto_float x(n, p1);
      auto_float y(n, p2);
      select();
      cpgpt(n, x.data, y.data, symbol);
    }

    void draw_marker(float x, float y, int symbol) const throw()
    {
      select();
      cpgpt1(x, y, symbol);
    }

    void text(float x, float y, float angle, float just, const std::string& text)
      const throw()
    {
      select();
      cpgptxt(x, y, angle, just, text.c_str());
    }

    void get_arrowhead_style(arrowhead::value& style, float& angle, float& barb)
      const throw()
    {
      select();
      int style_;
      cpgqah(&style_, &angle, &barb);
      style = arrowhead::value(style_);
    }

    float get_char_height() const throw()
    {
      select();
      float size;
      cpgqch(&size);
      return size;
    }

    int get_color_index() const throw()
    {
      select();
      int ci;
      cpgqci(&ci);
      return ci;
    }

    void get_image_range(int& low, int& high) const throw()
    {
      select();
      cpgqcir(&low, &high);
    }

    bool get_clipping() const throw()
    {
      int status;
      select();
      cpgqclp(&status);
      return status;
    }

    void get_color_range(int& low, int& high) const throw()
    {
      select();
      cpgqcol(&low, &high);
    }

    void get_color_rep(int index, float& r, float& g, float& b) const throw()
    {
      select();
      cpgqcr(index, &r, &g, &b);
    }

    void get_char_height(unit::value units, float& xch, float& ych) const throw()
    {
      select();
      cpgqcs(units, &xch, &ych);
    }

    static void get_nth_dev(int n, std::string& type, std::string& descr, int& inter)
      throw()
    {
      char type_buf[9], descr_buf[65];
      int type_len=9, descr_len=65;
      cpgqdt(n, type_buf, &type_len, descr_buf, &descr_len, &inter);
      type = std::string(type_buf, type_len);
      descr = std::string(descr_buf, descr_len);
    }

    fillstyle::value get_fill_style() const throw()
    {
      int fs;
      select();
      cpgqfs(&fs);
      return fillstyle::value(fs);
    }

    void get_hatch_style(float& angle, float& sep, float& phase) const throw()
    {
      select();
      cpgqhs(&angle, &sep, &phase);
    }

    static int get_current_id() throw()
    {
      int id;
      cpgqid(&id);
      return id;
    }

    static std::string get_info(const std::string& item) throw()
    {
      char buffer[256];
      int length=255;
      cpgqinf(item.c_str(), buffer, &length);
      return std::string(buffer, length);
    }

    image_transfer::value get_image_transfer() const throw()
    {
      int itf;
      select();
      cpgqitf(&itf);
      return image_transfer::value(itf);
    }
	
    linestyle::value get_line_style() const throw()
    {
      int ls;
      select();
      cpgqls(&ls);
      return linestyle::value(ls);
    }

    int get_line_width() const throw() {
      int lw;
      select();
      cpgqlw(&lw);
      return lw;
    }

    static int get_num_devices() throw() {
      int n;
      cpgqndt(&n);
      return n;
    }

    void get_pen_position(float& x, float& y) const throw()
    {
      select();
      cpgqpos(&x, &y);
    }

    int get_text_background() const throw() {
      int retval;
      select();
      cpgqtbg(&retval);
      return retval;
    }

    // FIXME: PGQTXT()

    void get_viewport(unit::value units, float& x1, float& x2, float& y1, float& y2)
      const throw()
    {
      select();
      cpgqvp(units, &x1, &x2, &y1, &y2);
    }

    void get_view_size(unit::value units, float& x1, float& x2, float& y1, float& y2)
      const throw()
    {
      select();
      cpgqvsz(units, &x1, &x2, &y1, &y2);
    }

    void get_window_boundary(float& x1, float& x2, float& y1, float& y2) const throw()
    {
      select();
      cpgqwin(&x1, &x2, &y1, &y2);
    }

    void draw_rectangle(float x1, float x2, float y1, float y2) const throw()
    {
      select();
      cpgrect(x1, x2, y1, y2);
    }

    void range(float x1, float x2, float& xlow, float& xhigh) const throw()
    {
      select();
      cpgrnge(x1, x2, &xlow, &xhigh);
    }

    void set_arrowhead_style(arrowhead::value style, float angle, float barb)
      const throw()
    {
      select();
      cpgsah(style, angle, barb);
    }

    void set_char_font(font::value font) const throw()
    { select(); cpgscf(font); }

    void set_char_height(float size) const throw()
    { select(); cpgsch(size); }

    void set_color_index(int index) const throw()
    { select(); cpgsci(index); }

    void set_color_range(int low, int high) const throw()
    { select(); cpgscir(low,high); }

    void set_clipping(bool state) const throw()
    { select(); cpgsclp(state); }

    void set_color_rep(int index, float r, float g, float b) const throw()
    { select(); cpgscr(index,r,g,b); }

    void scroll_window(float dx, float dy) const throw()
    { select(); cpgscrl(dx,dy); }

    bool set_color_rep_name(int index, const std::string& name)
      const throw()
    {
      select();
      int retval;
      cpgscrn(index, name.c_str(), &retval);
      return retval;
    }

    void set_fill_style(fillstyle::value style) const throw()
    { select(); cpgsfs(style); }

    void set_color_rep_hls(int index, float h, float l, float s)
      const throw()
    { select(); cpgshls(index, h, l, s); }

    void set_hatch_style(float angle, float sep, float phase) const throw()
    { select(); cpgshs(angle, sep, phase); }

    void set_image_transfer(image_transfer::value tf) const throw()
    { select(); cpgsitf(tf); }

    void set_line_style(linestyle::value style) const throw()
    { select(); cpgsls(style); }

    void set_line_width(int width) const throw()
    { select(); cpgslw(width); }

    void set_text_bg_index(int index) const throw()
    { select(); cpgstbg(index); }

    void subdivide(int nx, int ny) const throw()
    { select(); cpgsubp(nx, ny); }

    void set_viewport(float xl, float xr, float yb, float yt)
      const throw()
    { select(); cpgsvp(xl, xr, yb, yt); }

    void set_window(float x1, float x2, float y1, float y2) const throw()
    { select(); cpgswin(x1, x2, y1, y2); }

    void text_box(
		  const std::string& xopt, float xtick, int nx,
		  const std::string& yopt, float ytick, int ny
		  ) const throw()
    {
      select();
      cpgtbox(xopt.c_str(), xtick, nx, yopt.c_str(), ytick, ny);
    }
    void text(float x, float y, const std::string& text) const throw()
    {
      select();
      cpgtext(x, y, text.c_str());
    }
    void tick(float x1, float y1, float x2, float y2, float v,
	      float left, float right, float disp, float orient,
	      const std::string& text
	      ) const throw()
    {
      select();
      cpgtick(x1,y1,x2,y2,v,left,right,disp,orient,text.c_str());
    }
    void update() const throw()
    {
      select();
      cpgupdt();
    }

    // FIXME: PGVECT()

    void set_viewport_size(float xl, float xr, float yb, float yt) const throw()
    {
      select();
      cpgvsiz(xl,xr,yb,yt);
    }

    void set_standard_viewport() const throw()
    {
      select();
      cpgvstd();
    }
    void wedge(const std::string& side, float disp, float width,
	       float fg, float bg, const std::string& label) const throw()
    {
      select();
      cpgwedg(side.c_str(), disp, width, fg, bg, label.c_str());
    }
    void window_adjust(float x1, float x2, float y1, float y2) const throw()
    {
      select();
      cpgwnad(x1, x2, y1, y2);
    }

  };

}

// not planned to be implemented
// PGRND()
// PGFUNT()
// PGFUNX()
// PGFUNY()

#endif // PGPLOT_HH

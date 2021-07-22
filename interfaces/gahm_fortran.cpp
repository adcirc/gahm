
#include <iostream>
#include <string>
#include <vector>

#include "Date.h"
#include "Gahm.h"

extern "C" {
void *gahm_create_ftn(char *filename, long size, double *x, double *y);
void gahm_delete_ftn(void *ptr);

void gahm_get_ftn(void *ptr, int year, int month, int day, int hour, int minute,
                  int second, long size, double *u, double *v, double *p);

long gahm_get_serial_date(int year, int month, int day, int hour, int minute,
                          int second);

void gahm_date_add(int year_in, int month_in, int day_in, int hour_in,
                   int minute_in, int second_in, int add_seconds, int &year_out,
                   int &month_out, int &day_out, int &hour_out, int &minute_out,
                   int &second_out);
}

void *gahm_create_ftn(char *filename, long size, double *x, double *y) {
  const std::string f = std::string(filename);
  std::vector<double> xv;
  std::vector<double> yv;
  xv.reserve(size);
  yv.reserve(size);

  for (auto i = 0; i < size; ++i) {
    xv.push_back(x[i]);
    yv.push_back(y[i]);
  }
  auto *g = new Gahm(f, xv, yv);

  return reinterpret_cast<void *>(g);
}

void gahm_get_ftn(void *ptr, int year, int month, int day, int hour, int minute,
                  int second, long size, double *u, double *v, double *p) {
  auto g = reinterpret_cast<Gahm *>(ptr);
  auto d = Date(year, month, day, hour, minute, second);
  auto result = g->get(d);

  assert(size == result.size());

  auto result_u = result.u();
  auto result_v = result.v();
  auto result_p = result.p();

  std::copy(result_u.begin(), result_u.end(), u);
  std::copy(result_v.begin(), result_v.end(), v);
  std::copy(result_p.begin(), result_p.end(), p);
}

void gahm_delete_ftn(void *ptr) {
  auto g = reinterpret_cast<Gahm *>(ptr);
  delete g;
}

long gahm_get_serial_date(int year, int month, int day, int hour, int minute,
                          int second) {
  auto d = Date(year, month, day, hour, minute, second);
  return d.toSeconds();
}

void gahm_date_add(int year_in, int month_in, int day_in, int hour_in,
                   int minute_in, int second_in, int add_seconds, int &year_out,
                   int &month_out, int &day_out, int &hour_out, int &minute_out,
                   int &second_out) {
  Date date(year_in, month_in, day_in, hour_in, minute_in, second_in);
  date.addSeconds(add_seconds);
  year_out = date.year();
  month_out = date.month();
  day_out = date.day();
  hour_out = date.hour();
  minute_out = date.minute();
  second_out = date.second();
}
// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "atcf/AtcfFile.h"
#include "datatypes/PointCloud.h"
#include "preprocessor/Preprocessor.h"
#include "vortex/Vortex.h"

#ifndef GAHM_FORTRAN_USE_MAP_LOOKUP
#define GAHM_FORTRAN_USE_MAP_LOOKUP 0
#endif

// This is a C++ class that holds the C++ objects that are created by the
// Fortran code. The C++ objects are created by the Fortran code and then
// stored in a map. The map is indexed by the counter variable. The counter
// variable is incremented each time a new instance of the C++ objects is
// created. The counter variable is returned to the Fortran code and is used
// to index the map. The memory is managed using std::unique_ptr. When the
// Fortran code calls the destroy function or when things go out of scope,
// the C++ objects are destroyed and the memory is freed.
class gahm_instance {
 public:
  gahm_instance(const std::string &filename,
                const Gahm::Datatypes::PointCloud &point_cloud, bool quiet) {
    this->m_atcf = std::make_unique<Gahm::Atcf::AtcfFile>(filename, quiet);
    this->m_atcf->read();
    this->m_preprocessor = std::make_unique<Gahm::Preprocessor>(m_atcf.get());
    this->m_preprocessor->solve();
    this->m_vortex = std::make_unique<Gahm::Vortex>(m_atcf.get(), point_cloud);
  }

  ~gahm_instance() = default;

  Gahm::Vortex *vortex() { return m_vortex.get(); }

 private:
  std::unique_ptr<Gahm::Vortex> m_vortex;
  std::unique_ptr<Gahm::Atcf::AtcfFile> m_atcf;
  std::unique_ptr<Gahm::Preprocessor> m_preprocessor;
};

#if GAHM_FORTRAN_USE_MAP_LOOKUP == 1
static std::unordered_map<int, std::unique_ptr<gahm_instance>> g_gahm_instances;
static long g_counter = 0;
#else
static std::vector<std::unique_ptr<gahm_instance>> g_gahm_instances;
#endif

extern "C" {
long gahm_create_ftn(char *filename, long size, double *x, double *y,
                     bool quiet);
void gahm_destroy_ftn(long id);
void gahm_get_ftn(long id, int year, int month, int day, int hour, int minute,
                  int second, long size, double *u, double *v, double *p);
long gahm_get_serial_date_ftn(int year, int month, int day, int hour,
                              int minute, int second);
void gahm_date_add_ftn(int year_in, int month_in, int day_in, int hour_in,
                       int minute_in, int second_in, int add_seconds,
                       int &year_out, int &month_out, int &day_out,
                       int &hour_out, int &minute_out, int &second_out);
}

long gahm_create_ftn(char *filename, long size, double *x, double *y,
                     bool quiet) {
  std::string filename_str(filename);
  std::vector<double> x_pos(x, x + size);
  std::vector<double> y_pos(y, y + size);

  auto point_cloud = Gahm::Datatypes::PointCloud(x_pos, y_pos);
#if GAHM_FORTRAN_USE_MAP_LOOKUP == 1
  g_gahm_instances[g_counter] =
      std::make_unique<gahm_instance>(filename_str, point_cloud, quiet);
  return g_counter;
#else
  g_gahm_instances.push_back(
      std::make_unique<gahm_instance>(filename_str, point_cloud, quiet));
  return g_gahm_instances.size() - 1;
#endif
}

void gahm_destroy_ftn(long id) {
#if GAHM_FORTRAN_USE_MAP_LOOKUP == 1
  g_gahm_instances.erase(id);
#else
  g_gahm_instances[id].reset();
#endif
}

void gahm_get_ftn(long id, int year, int month, int day, int hour, int minute,
                  int second, long size, double *u, double *v, double *p) {
  //...Check that the object exists before solving
  auto instance = g_gahm_instances[id].get();
  if (instance == nullptr) {
    std::cerr << "[GAHM Library ERROR]: The requested vortex object is null."
              << std::endl;
    return;
  }

  auto d = Gahm::Datatypes::Date(year, month, day, hour, minute, second);
  auto sln = instance->vortex()->solve(d);

  assert(sln.size() == size);
  for (size_t i = 0; i < size; ++i) {
    u[i] = sln.u()[i];
    v[i] = sln.v()[i];
    p[i] = sln.p()[i];
  }
}

long gahm_get_serial_date_ftn(int year, int month, int day, int hour,
                              int minute, int second) {
  auto d = Gahm::Datatypes::Date(year, month, day, hour, minute, second);
  return d.toSeconds();
}

void gahm_date_add_ftn(int year_in, int month_in, int day_in, int hour_in,
                       int minute_in, int second_in, int add_seconds,
                       int &year_out, int &month_out, int &day_out,
                       int &hour_out, int &minute_out, int &second_out) {
  auto d = Gahm::Datatypes::Date(year_in, month_in, day_in, hour_in, minute_in,
                                 second_in);
  d.addSeconds(add_seconds);
  year_out = d.year();
  month_out = d.month();
  day_out = d.day();
  hour_out = d.hour();
  minute_out = d.minute();
  second_out = d.second();
}

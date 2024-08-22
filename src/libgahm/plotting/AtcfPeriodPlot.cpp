//
// Created by Zach Cobell on 8/20/24.
//

#include "plotting/AtcfPeriodPlot.h"

#include <algorithm>
#include <ranges>
#include <unordered_map>

#include "output/RadialProfile.h"
#include "physical/Constants.h"
#include "physical/Units.h"
#include "vtkAxis.h"
#include "vtkCamera.h"
#include "vtkChart.h"
#include "vtkChartXY.h"
#include "vtkColorSeries.h"
#include "vtkContextActor.h"
#include "vtkContextView.h"
#include "vtkFloatArray.h"
#include "vtkInteractorStyleRubberBandZoom.h"
#include "vtkNamedColors.h"
#include "vtkNew.h"
#include "vtkPlot.h"
#include "vtkPlotPoints.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTable.h"

namespace {

struct PlotScene {
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkChartXY> chart;
};

auto generate_viewports()
    -> std::tuple<vtkSmartPointer<vtkRenderWindow>,
                  vtkSmartPointer<vtkRenderWindowInteractor>,
                  std::vector<PlotScene>> {
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkRenderWindow> renderWindow;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindow->SetSize(1200, 800);

  // 2x2 grid of plots
  std::array<std::array<double, 4>, 4> viewports = {{
      {0.5, 0.5, 1, 1},
      {0.5, 0, 1, 0.5},
      {0, 0, 0.5, 0.5},
      {0, 0.5, 0.5, 1},
  }};

  std::vector<PlotScene> renderers;
  std::transform(
      viewports.begin(), viewports.end(), std::back_inserter(renderers),
      [&](const auto& viewport) {
        vtkNew<vtkRenderer> renderer;
        vtkNew<vtkChartXY> chart;
        vtkNew<vtkContextScene> scene;
        vtkNew<vtkContextActor> actor;

        // Initialize the renderer
        renderer->SetBackground(colors->GetColor3d("White").GetData());
        renderWindow->AddRenderer(renderer);
        renderer->SetViewport(viewport.data());

        // Enable antialiasing
        renderer->SetUseFXAA(true);

        // Label the chart axes
        chart->GetAxis(vtkAxis::BOTTOM)->SetTitle("Distance (km)");
        chart->GetAxis(vtkAxis::LEFT)->SetTitle("Wind Speed (knots)");

        scene->AddItem(chart);
        actor->SetScene(scene);
        renderer->AddActor(actor);
        scene->SetRenderer(renderer);

        return PlotScene{renderer, chart};
      });

  return {renderWindow, renderWindowInteractor, renderers};
}

auto generate_profile_data_table(
    const Gahm::Atcf::AtcfPeriod& period, const Gahm::Storm::Quadrant& quadrant,
    double distance_start, double distance_end,
    double distance_step) -> vtkSmartPointer<vtkTable> {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);
  constexpr auto ten2one =
      1.0 / Gahm::Physical::Constants::oneMinuteToTenMinuteWind();

  vtkNew<vtkTable> table;

  vtkNew<vtkFloatArray> arr_dis;
  vtkNew<vtkFloatArray> arr_spd;
  arr_dis->SetName("Distance");
  arr_spd->SetName("Profile");
  table->AddColumn(arr_dis);
  table->AddColumn(arr_spd);

  const auto distance = Gahm::Output::RadialProfile::generate_distance_vector(
      distance_start, distance_end, distance_step);

  table->SetNumberOfRows(static_cast<long long>(distance.size()));

  const auto profile =
      Gahm::Output::RadialProfile::get_profile(period, quadrant, distance);

  for (auto pt = profile.data.begin(); pt != profile.data.end(); ++pt) {
    const auto i = std::distance(profile.data.begin(), pt);
    table->SetValue(static_cast<long long>(i), 0,
                    distance[static_cast<size_t>(i)] / 1000.0);
    table->SetValue(
        static_cast<long long>(i), 1,
        profile.data[static_cast<size_t>(i)].wind_vector.magnitude() * ms2kt *
            ten2one);
  }

  return table;
}

auto generate_isotach_marker_table(const Gahm::Storm::Isotach& isotach)
    -> vtkSmartPointer<vtkTable> {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);

  vtkNew<vtkTable> table;
  vtkNew<vtkFloatArray> dis_arr;
  vtkNew<vtkFloatArray> spd_arr;
  dis_arr->SetName("distance");
  spd_arr->SetName("speed");

  table->AddColumn(dis_arr);
  table->AddColumn(spd_arr);

  const auto dis = isotach.is_populated() ? isotach.radius() : 0;
  const auto ws = isotach.is_populated() ? isotach.wind_speed() * ms2kt : 0;

  table->SetNumberOfRows(2);

  table->SetValue(0, 0, dis / 1000.0);
  table->SetValue(1, 0, dis / 1000.0);
  table->SetValue(0, 1, 0);
  table->SetValue(1, 1, ws);

  return table;
}

auto generate_vmax_points(const Gahm::Storm::Isotach& isotach,
                          double v_max) -> vtkSmartPointer<vtkTable> {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);

  vtkNew<vtkTable> table;

  const auto r_max = isotach.gahm_parameters().radius_to_max_winds() / 1000.0;
  std::pair<double, double> vmax_points = {r_max, v_max * ms2kt};

  vtkNew<vtkFloatArray> arr_dis;
  vtkNew<vtkFloatArray> arr_spd;

  arr_dis->SetName("dis");
  arr_spd->SetName("spd");

  table->AddColumn(arr_dis);
  table->AddColumn(arr_spd);

  table->SetNumberOfRows(1);

  table->SetValue(0, 0, vmax_points.first);
  table->SetValue(0, 1, vmax_points.second);

  return table;
}

void add_isotach_markers(const Gahm::Storm::Quadrant& quadrant,
                         vtkChartXY* chart) {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);

  const std::unordered_map<
      int, std::tuple<unsigned char, unsigned char, unsigned char>>
      color_lookup = {{34, {0, 0, 255}}, {50, {255, 0, 0}}, {64, {0, 255, 0}}};

  std::for_each(quadrant.isotachs().begin(), quadrant.isotachs().end(),
                [&](const auto& isotach) {
                  if (isotach.is_populated()) {
                    const auto table = generate_isotach_marker_table(isotach);
                    const auto windspeed_kt = static_cast<int>(
                        std::round(isotach.wind_speed() * ms2kt));
                    const auto label = std::to_string(windspeed_kt) + " kt";
                    const auto [r, g, b] = color_lookup.at(windspeed_kt);
                    vtkPlot* marker = chart->AddPlot(vtkChart::LINE);
                    marker->SetInputData(table, 0, 1);
                    marker->SetColor(r, g, b, 255);
                    marker->SetWidth(2.0);
                    marker->SetLabel(label.c_str());
                  }
                });
}

void add_wind_profile(const Gahm::Atcf::AtcfPeriod& period,
                      const Gahm::Storm::Quadrant& quadrant,
                      vtkChartXY* chart) {
  const auto profile_table =
      generate_profile_data_table(period, quadrant, 0, 500000, 1000);
  vtkPlot* line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(profile_table, 0, 1);
  line->SetColor(0, 0, 0, 255);
  line->SetWidth(3.0);
  line->SetLabel("Wind Profile");
}

void add_vmax_markers(const Gahm::Storm::Quadrant& quadrant, double vmax,
                      vtkChartXY* chart) {
  constexpr auto ms2kt = Gahm::Physical::Units::convert(
      Gahm::Physical::Units::MetersPerSecond, Gahm::Physical::Units::Knot);

  const std::unordered_map<
      int, std::tuple<unsigned char, unsigned char, unsigned char>>
      color_lookup = {{34, {0, 0, 255}}, {50, {255, 0, 0}}, {64, {0, 255, 0}}};

  std::for_each(quadrant.isotachs().begin(), quadrant.isotachs().end(),
                [&](const auto& isotach) {
                  if (isotach.is_populated()) {
                    const auto table = generate_vmax_points(isotach, vmax);
                    const auto wind_speed = static_cast<int>(
                        std::round(isotach.wind_speed() * ms2kt));
                    const auto label = "RMax-" + std::to_string(wind_speed);
                    const auto [r, g, b] = color_lookup.at(wind_speed);
                    vtkPlot* vmax_plt = chart->AddPlot(vtkChart::POINTS);
                    vmax_plt->SetInputData(table, 0, 1);
                    vmax_plt->SetColor(r, g, b, 255);
                    vmax_plt->SetWidth(4.0);
                    vmax_plt->SetLabel(label.c_str());
                    dynamic_cast<vtkPlotPoints*>(vmax_plt)->SetMarkerStyle(
                        vtkPlotPoints::CIRCLE);
                  }
                });
}

auto generate_plot(const Gahm::Atcf::AtcfPeriod& period)
    -> std::tuple<vtkSmartPointer<vtkRenderWindow>,
                  vtkSmartPointer<vtkRenderWindowInteractor>> {
  auto [window, interactor, renderers] = generate_viewports();

  const std::unordered_map<Gahm::Types::QuadCode::QuadrantCode, std::string>
      quadrant_degree_label = {{Gahm::Types::QuadCode::NE, "45"},
                               {Gahm::Types::QuadCode::NW, "135"},
                               {Gahm::Types::QuadCode::SW, "225"},
                               {Gahm::Types::QuadCode::SE, "315"}};

  // Need ranges-v3 to avoid the need for the chart_index variable
  size_t chart_index = 0;
  std::for_each(renderers.begin(), renderers.end(), [&](const auto& renderer) {
    const auto quadrant = period.quadrant(chart_index);
    const auto quadrant_code = quadrant.quadrant_code();
    const auto quadrant_degree = quadrant_degree_label.at(quadrant_code);
    const auto chart_label =
        std::string("Wind Profile, Quadrant ") +
        Gahm::Types::QuadCode::quadrant_code_to_string(quadrant_code) + " (" +
        quadrant_degree + "°)";

    auto chart = renderer.chart;
    chart->SetTitle(chart_label);

    add_isotach_markers(quadrant, chart);

    // Generate the profile data table
    add_wind_profile(period, quadrant, chart);

    // Add the Vmax points as markers (one per isotach)
    add_vmax_markers(quadrant, period.v_max(), chart);

    // Set the range of the chart
    chart->GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);
    chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);
    chart->GetAxis(vtkAxis::BOTTOM)->SetRange(0, 500);
    chart->GetAxis(vtkAxis::LEFT)->SetRange(0, 100);

    chart->SetShowLegend(true);

    chart_index++;
  });

  return {window, interactor};
}

}  // namespace

Gahm::Plotting::AtcfPeriodPlot::AtcfPeriodPlot(const Atcf::AtcfPeriod& period)
    : m_period(period) {}

/**
 * Function to plot a radial profile using Vtk and display it to the user
 */
void Gahm::Plotting::AtcfPeriodPlot::show() const {
  auto [window, interactor] = generate_plot(m_period);
  window->Render();
  window->SetWindowName("GAHM: ATCF Period Plot");
  interactor->Initialize();
  interactor->Start();
}

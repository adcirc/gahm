#!/usr/bin/env python3

from datetime import datetime
from typing import Tuple
import numpy as np
from pygahm import pygahm
import xarray as xr
import matplotlib.pyplot as plt


def generate_contour_range(plot_type: str) -> Tuple[np.ndarray, str]:
    if plot_type == "pressure":
        contour_range = np.arange(900, 1020, 5)
        contour_label = "Pressure (mb)"
    elif plot_type == "quadrant":
        contour_range = np.arange(0, 4, 0.5)
        contour_label = "Quadrant"
    elif plot_type == "quadrant_weight":
        contour_range = np.arange(0, 1.1, 0.1)
        contour_label = "Quadrant Weight"
    elif plot_type == "isotach":
        contour_range = np.arange(0, 4, 0.5)
        contour_label = "Isotach"
    elif plot_type == "isotach_weight":
        contour_range = np.arange(0, 100, 1)
        contour_label = "Isotach Weight"
    else:
        contour_range = np.arange(0, 90, 1)
        contour_label = "Wind Speed (m/s)"

    return contour_range, contour_label


def sln_to_xarray(sln) -> xr.Dataset:

    arr = pygahm.PointSolutionArray(sln)

    # ...Create the xarray dataset
    return xr.Dataset(
        {
            "pressure": ("points", arr.pressure),
            "u_vector": ("points", arr.wind_u),
            "v_vector": ("points", arr.wind_v),
            "wind_speed": ("points", arr.wind_speed),
        }
    )



def get(gahm_vortex, time: datetime, wind_grid, plot_type="pressure"):
    """
    Get the pressure or wind speed at a given time and
    return as a numpy array
    """
    time_c = pygahm.Datetime(time.year, time.month, time.day, time.hour)
    solution = pygahm.get_points(gahm_vortex, time_c, wind_grid.points())
    return sln_to_xarray(solution)

def main():
    import argparse
    from datetime import timedelta
    from mpl_toolkits.basemap import Basemap
    from matplotlib.widgets import Slider

    parser = argparse.ArgumentParser(
        description="Generate an interactive map of a GAHM model"
    )
    parser.add_argument(
        "--type",
        type=str,
        help="Type of plot to generate (pressure, wind)",
        default="pressure",
    )
    parser.add_argument("--interactive", action="store_true", help="Interactive mode")
    parser.add_argument("--output", type=str, help="Base name for output images")
    parser.add_argument("--time", type=datetime.fromisoformat, help="Time to plot")
    parser.add_argument("file", type=str, help="ATCF file to plot")
    args = parser.parse_args()

    # ....Generate the wind grid
    wind_grid = pygahm.Grid.fromCorners(-100.0, 22.0, -60.0, 40.0, 0.1, 0.1)
    x_points = np.array(wind_grid.x_grid())
    y_points = np.array(wind_grid.y_grid())

    atcf_track = pygahm.AtcfIO(args.file)
    sln = atcf_track.read()

    # ...Generate the start and end date
    start_date = datetime(2005, 8, 24, 0)
    end_date = datetime(2005, 8, 30, 12)

    def interactive():
        # ...Create a numpy array of dates in 1 hour increments
        dates = np.arange(start_date, end_date, timedelta(hours=1)).astype(datetime)

        # ...Create a basemap figure
        fig = plt.figure(figsize=(12, 8))

        # ...Make the map
        ax = fig.add_subplot(111)

        # ...Make the slider
        slider_ax = fig.add_axes([0.25, 0.05, 0.65, 0.03])
        slider = Slider(
            ax=slider_ax,
            label="Time (hours)",
            valmin=0,
            valmax=len(dates) - 1,
            valinit=0,
            valstep=1,
        )

        s = {}

        def plot(time: datetime):

            # ...Clear the plot if it's been used previously
            ax.clear()

            ax.set_title(
                "Hurricane Katrina (2005) - {:s}".format(
                    time.strftime("%Y-%m-%d %H:%M:%S")
                )
            )

            m = Basemap(
                projection="merc",
                llcrnrlon=-100.0,
                llcrnrlat=22.0,
                urcrnrlon=-60.0,
                urcrnrlat=40.0,
                resolution="i",
                ax=ax,
            )

            data = get(sln, time, wind_grid, plot_type=args.type)
            arr = data[args.type].values
            u_vec = data["u_vector"].values
            v_vec = data["v_vector"].values

            arr = arr.reshape((x_points.shape[0], x_points.shape[1]))
            u_vec = u_vec.reshape((x_points.shape[0], x_points.shape[1]))
            v_vec = v_vec.reshape((x_points.shape[0], x_points.shape[1]))

            contour_range, contour_label = generate_contour_range(args.type)

            cs = m.contourf(
                x_points,
                y_points,
                arr,
                cmap="jet",
                levels=contour_range,
                latlon=True,
            )

            # ...Plot using quivers, but coarsen the grid
            coarsen_factor = 3
            scale_factor = 2000
            x_p = x_points[::coarsen_factor, ::coarsen_factor]
            y_p = y_points[::coarsen_factor, ::coarsen_factor]
            u_vec = u_vec[::coarsen_factor, ::coarsen_factor]
            v_vec = v_vec[::coarsen_factor, ::coarsen_factor]
            m.quiver(
                x_p, y_p, u_vec, v_vec, latlon=True, scale=scale_factor, width=0.001
            )

            s["plot"] = cs

            m.drawcoastlines()
            m.drawparallels(np.arange(10, 50, 5), labels=[1, 0, 0, 0])
            m.drawmeridians(np.arange(-120, -40, 5), labels=[0, 0, 0, 1])
            fig.subplots_adjust(hspace=0.1)
            fig.subplots_adjust(wspace=0.1)

        def update(val):
            """
            Update the contour plot
            """
            plot(dates[int(val)])
            fig.canvas.draw_idle()

        slider.on_changed(update)
        plot(start_date)

        # ...Make the colorbar
        cbar = fig.colorbar(s["plot"], ax=ax, orientation="horizontal")
        _, contour_label = generate_contour_range(args.type)
        cbar.set_label(contour_label)

        plt.show()

    def static_graphics(time: datetime):
        fig = plt.figure(figsize=(12, 8))
        ax = fig.add_subplot(111)
        ax.set_title(
            "Hurricane Katrina (2005) - {:s}".format(time.strftime("%Y-%m-%d %H:%M:%S"))
        )

        m = Basemap(
            projection="merc",
            llcrnrlon=-100.0,
            llcrnrlat=22.0,
            urcrnrlon=-60.0,
            urcrnrlat=40.0,
            resolution="i",
            ax=ax,
        )
        data = get(sln, time, wind_grid, plot_type=args.type)
        arr = data[args.type].values
        u_vec = data["u_vector"].values
        v_vec = data["v_vector"].values

        arr = arr.reshape((x_points.shape[0], x_points.shape[1]))
        u_vec = u_vec.reshape((x_points.shape[0], x_points.shape[1]))
        v_vec = v_vec.reshape((x_points.shape[0], x_points.shape[1]))

        contour_range, contour_label = generate_contour_range(args.type)

        cs = m.contourf(
            x_points,
            y_points,
            arr,
            cmap="jet",
            levels=contour_range,
            latlon=True,
        )
        m.drawcoastlines()

        # ...Plot using quivers, but coarsen the grid
        coarsen_factor = 3
        scale_factor = 2000
        x_p = x_points[::coarsen_factor, ::coarsen_factor]
        y_p = y_points[::coarsen_factor, ::coarsen_factor]
        u_vec = u_vec[::coarsen_factor, ::coarsen_factor]
        v_vec = v_vec[::coarsen_factor, ::coarsen_factor]
        m.quiver(x_p, y_p, u_vec, v_vec, latlon=True, scale=scale_factor, width=0.001)

        m.drawparallels(np.arange(10, 50, 5), labels=[1, 0, 0, 0])
        m.drawmeridians(np.arange(-120, -40, 5), labels=[0, 0, 0, 1])

        cbar = fig.colorbar(cs, ax=ax, orientation="horizontal")
        cbar.set_label(contour_label)
        plt.savefig(args.output, dpi=300, bbox_inches="tight")

    if args.interactive:
        interactive()
    else:
        if not args.time:
            raise ValueError("Must specify a time for static graphics")
        static_graphics(args.time)


if __name__ == "__main__":
    main()
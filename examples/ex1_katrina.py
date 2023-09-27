#!/usr/bin/env python3

from datetime import datetime
import numpy as np
import pygahm


def get(gahm_vortex, time: datetime, plot_type="pressure"):
    """
    Get the pressure or wind speed at a given time and
    return as a numpy array
    """
    time_c = pygahm.Date(time.year, time.month, time.day, time.hour)
    solution = gahm_vortex.solve(time_c)

    if plot_type == "pressure":
        return np.array(solution.p())
    elif plot_type == "wind":
        u = np.array(solution.u())
        v = np.array(solution.v())

        return np.sqrt(u**2 + v**2)


def main():
    import argparse
    from datetime import timedelta
    import matplotlib.pyplot as plt
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
    wind_grid = pygahm.WindGrid.fromCorners(-100.0, 22.0, -60.0, 40.0, 0.1, 0.1)
    x_points = np.array(wind_grid.x_grid())
    y_points = np.array(wind_grid.y_grid())

    atcf = pygahm.AtcfFile(args.file, True)
    atcf.read()

    # ...Run the preprocessor
    prep = pygahm.Preprocessor(atcf)
    prep.prepareAtcfData()
    prep.solve()

    # ...Generate the vortex
    vortex = pygahm.Vortex(atcf, wind_grid.points())

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

            # ...Clear the plot if its been used previously
            ax.clear()

            ax.set_title(
                "Hurricane Katrina (2005) - {:s}".format(
                    time.strftime("%Y-%m-%d %H:%M:%S")
                )
            )

            ax.set_xlabel("Longitude")
            ax.set_ylabel("Latitude")

            m = Basemap(
                projection="merc",
                llcrnrlon=-100.0,
                llcrnrlat=22.0,
                urcrnrlon=-60.0,
                urcrnrlat=40.0,
                resolution="i",
                ax=ax,
            )

            arr = get(vortex, time, plot_type=args.type)
            arr = arr.reshape((x_points.shape[0], x_points.shape[1]))

            if args.type == "pressure":
                contour_range = np.arange(900, 1020, 5)
            else:
                contour_range = np.arange(0, 80, 5)

            cs = m.contourf(
                x_points,
                y_points,
                arr,
                cmap="jet",
                levels=contour_range,
                latlon=True,
            )
            s["plot"] = cs
            m.drawcoastlines()

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
        if args.type == "pressure":
            cbar.set_label("Pressure (mb)")
        else:
            cbar.set_label("Wind Speed (m/s)")

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
        arr = get(vortex, time, plot_type=args.type)
        arr = arr.reshape((x_points.shape[0], x_points.shape[1]))
        if args.type == "pressure":
            contour_range = np.arange(900, 1020, 5)
        else:
            contour_range = np.arange(0, 80, 5)
        cs = m.contourf(
            x_points,
            y_points,
            arr,
            cmap="jet",
            levels=contour_range,
            latlon=True,
        )
        m.drawcoastlines()
        m.drawparallels(np.arange(10, 50, 5), labels=[1, 0, 0, 0])
        m.drawmeridians(np.arange(-120, -40, 5), labels=[0, 0, 0, 1])

        cbar = fig.colorbar(cs, ax=ax, orientation="horizontal")
        if args.type == "pressure":
            cbar.set_label("Pressure (mb)")
        else:
            cbar.set_label("Wind Speed (m/s)")
        plt.savefig(args.output, dpi=300, bbox_inches="tight")

    if args.interactive:
        interactive()
    else:
        if not args.time:
            raise ValueError("Must specify a time for static graphics")
        static_graphics(args.time)


if __name__ == "__main__":
    main()

#!/usr/bin/env python3

from datetime import datetime
from typing import Tuple
import numpy as np
from matplotlib import pyplot as plt
from pygahm import pygahm
import haversine

# ...Convert M/S to KNOT
ms_to_kt = 1.94384

# ...Convert M to NM
m_to_nm = 0.000539957

# ...Move from boundary layer to 10m
topToTen = 0.9

# ... Convert 1 minute wind to 10 minute wind
min10_to_min1 = 1.12

atcf_file = "../tests/test_files/bal122005.dat"
atcf = pygahm.AtcfFile(atcf_file, True)
atcf.read()

prep = pygahm.Preprocessor(atcf)
prep.prepareAtcfData()
prep.solve()

snap = 24

# ...Get the time of the storm
storm_time = atcf.data()[snap].date()
print("Storm time: {:s}".format(storm_time.toString()))

isotachs = atcf.data()[snap].getIsotachs()
isotach_data = []
for iso in isotachs:
    # ...Get the wind speed and convert to knots
    isotach_data.append(
        {
            "wind_speed": iso.getWindSpeed() * ms_to_kt / min10_to_min1,
            "distance": [
                iso.getQuadrant(0).getIsotachRadius() * m_to_nm,
                iso.getQuadrant(1).getIsotachRadius() * m_to_nm,
                iso.getQuadrant(2).getIsotachRadius() * m_to_nm,
                iso.getQuadrant(3).getIsotachRadius() * m_to_nm,
            ],
        }
    )

# ...Create a line of points along center of each storm quadrant from the strom center
storm_center_x = atcf.data()[snap].position().x()
storm_center_y = atcf.data()[snap].position().y()
travel_direction = (
    atcf.data()[snap].translation().translationDirection() * 180.0 / np.pi
)
print("Travel direction: ", travel_direction)
vmax = atcf.data()[snap].vmax() * ms_to_kt

# ...Create a numpy array of x, y points along the center of each quadrant
quad = [{}, {}, {}, {}]
quad[0]["angle"] = 45.0
quad[1]["angle"] = -45.0
quad[2]["angle"] = -135.0
quad[3]["angle"] = -225.0

point_count = 1000
dx = 5.0

for i in range(4):
    quad[i]["x_bounds"] = np.array(
        [storm_center_x, storm_center_x + dx * np.cos(np.deg2rad(quad[i]["angle"]))]
    )
    quad[i]["y_bounds"] = np.array(
        [storm_center_y, storm_center_y + dx * np.sin(np.deg2rad(quad[i]["angle"]))]
    )
    quad[i]["x_pts"] = np.linspace(
        quad[i]["x_bounds"][0], quad[i]["x_bounds"][1], point_count
    )
    quad[i]["y_pts"] = np.linspace(
        quad[i]["y_bounds"][0], quad[i]["y_bounds"][1], point_count
    )


# ...Concatenate the x and y points into a single array
x_pts = np.concatenate(
    (quad[0]["x_pts"], quad[1]["x_pts"], quad[2]["x_pts"], quad[3]["x_pts"])
)
y_pts = np.concatenate(
    (quad[0]["y_pts"], quad[1]["y_pts"], quad[2]["y_pts"], quad[3]["y_pts"])
)
point_cloud = pygahm.PointCloud(x_pts, y_pts)

vortex = pygahm.Vortex(atcf, point_cloud)
sln = vortex.solve(storm_time)
u = np.array(sln.u()) * ms_to_kt
v = np.array(sln.v()) * ms_to_kt
iso = np.array(sln.isotach())
# quadrant = np.array(sln.quadrant())
# iso_weight = np.array(sln.isotach_weight())
# quad_weight = np.array(sln.quadrant_weight())
# isotach_speed = np.array(sln.isotach_speed()) * ms_to_kt
# gahm_dis = np.array(sln.distance()) * m_to_nm
mag = np.sqrt(u**2 + v**2)

# ...Split the wind speed into quadrants
quad[0]["q_in"] = np.full(point_count, 0)
quad[1]["q_in"] = np.full(point_count, 1)
quad[2]["q_in"] = np.full(point_count, 2)
quad[3]["q_in"] = np.full(point_count, 3)
q_in = np.concatenate(
    (quad[0]["q_in"], quad[1]["q_in"], quad[2]["q_in"], quad[3]["q_in"])
)

quad[0]["wind"] = mag[0:point_count]
quad[1]["wind"] = mag[point_count : 2 * point_count]
quad[2]["wind"] = mag[2 * point_count : 3 * point_count]
quad[3]["wind"] = mag[3 * point_count : 4 * point_count]

quad[0]["iso"] = iso[0:point_count]
quad[1]["iso"] = iso[point_count : 2 * point_count]
quad[2]["iso"] = iso[2 * point_count : 3 * point_count]
quad[3]["iso"] = iso[3 * point_count : 4 * point_count]

# ...Compute the distance to the storm center using x_pts, y_pts and haversine for each quad
quad[0]["distance"] = np.full(point_count, 0, dtype=float)
quad[1]["distance"] = np.full(point_count, 0, dtype=float)
quad[2]["distance"] = np.full(point_count, 0, dtype=float)
quad[3]["distance"] = np.full(point_count, 0, dtype=float)

for i in range(4):
    for j in range(point_count):
        quad[i]["distance"][j] = haversine.haversine(
            (quad[i]["y_pts"][j], quad[i]["x_pts"][j]),
            (storm_center_y, storm_center_x),
            unit=haversine.Unit.NAUTICAL_MILES,
        )

# quad[0]["iso_weight"] = iso_weight[0:point_count]
# quad[1]["iso_weight"] = iso_weight[point_count : 2 * point_count]
# quad[2]["iso_weight"] = iso_weight[2 * point_count : 3 * point_count]
# quad[3]["iso_weight"] = iso_weight[3 * point_count : 4 * point_count]
#
# quad[0]["quad"] = quadrant[0:point_count]
# quad[1]["quad"] = quadrant[point_count : 2 * point_count]
# quad[2]["quad"] = quadrant[2 * point_count : 3 * point_count]
# quad[3]["quad"] = quadrant[3 * point_count : 4 * point_count]
#
# quad[0]["quad_weight"] = quad_weight[0:point_count]
# quad[1]["quad_weight"] = quad_weight[point_count : 2 * point_count]
# quad[2]["quad_weight"] = quad_weight[2 * point_count : 3 * point_count]
# quad[3]["quad_weight"] = quad_weight[3 * point_count : 4 * point_count]
#
# quad[0]["iso_speed"] = isotach_speed[0:point_count]
# quad[1]["iso_speed"] = isotach_speed[point_count : 2 * point_count]
# quad[2]["iso_speed"] = isotach_speed[2 * point_count : 3 * point_count]
# quad[3]["iso_speed"] = isotach_speed[3 * point_count : 4 * point_count]
#
# quad[0]["dis_gahm"] = gahm_dis[0:point_count]
# quad[1]["dis_gahm"] = gahm_dis[point_count : 2 * point_count]
# quad[2]["dis_gahm"] = gahm_dis[2 * point_count : 3 * point_count]
# quad[3]["dis_gahm"] = gahm_dis[3 * point_count : 4 * point_count]

plot_scatter = False

if plot_scatter:
    # ...Plot within the range 0,4
    plt.figure(figsize=(10, 10))
    plt.scatter(x_pts, y_pts, c=mag, cmap="jet", vmin=0, vmax=3)
    plt.colorbar(label="Wind Speed (Knots)", orientation="horizontal")
    plt.gca().set_aspect("equal", adjustable="box")
    plt.show()
else:

    fig, axs = plt.subplots(4, 1, figsize=(10, 10))

    color_list = ["red", "blue", "green", "orange"]

    # ...Plot the isotachs
    for iso in isotach_data:
        for i in range(4):
            axs[i].plot(
                [iso["distance"][i], iso["distance"][i]],
                [0, iso["wind_speed"]],
                color=color_list[i],
                linewidth=2,
                label="Isotach",
            )

    for i in range(4):

        axs[i].plot(
            quad[i]["distance"],
            quad[i]["wind"],
            color=color_list[i],
            linewidth=2,
            label="Wind Profile",
#            marker=".",
        )

        # axs[i].plot(
        #    quad[i]["dis_gahm"],
        #    quad[i]["iso_weight"]*100,
        #    color="black",
        #    linewidth=2,
        #    label="Isotach",
        # )

        axs[i].plot(
            [0, 200],
            [vmax, vmax],
            color="black",
            linewidth=2,
            linestyle="dashed",
        )

        axs[i].set_title("Quadrant {:d}".format(i + 1))
        axs[i].grid(True)
        axs[i].set_ylabel("Wind Speed (Knots)")
        axs[i].set_xlim([0, 300])
        axs[i].set_ylim([0, 400])

    axs[3].set_xlabel("Distance (NM)")

    plt.tight_layout()

    plt.savefig("wind_profile.png")
    plt.show()

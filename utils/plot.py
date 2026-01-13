# nbody plot script
# plots 3d trajectories of bodies from csv data file

import argparse
import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser(description="Visualize n-body simulation data.")
parser.add_argument("data_file", type=str, help="Path to the CSV data file.")
args = parser.parse_args()

data = np.loadtxt(args.data_file, delimiter=",", skiprows=1)

# get all bodies using unique body ids
body_ids = np.unique(data[:, 1])

fig = plt.figure()
ax = fig.add_subplot(111, projection="3d")

for body_id in body_ids:
    body_data = data[data[:, 1] == body_id]
    mass = body_data[0, 2]
    pos = body_data[:, 3:6]
    ax.plot(
        pos[:, 0],
        pos[:, 1],
        pos[:, 2],
        label=f"Body {int(body_id)}",
        linewidth=max(0.5, min(5.0, mass * 10)),
    )

# Set labels and title
ax.set_xlabel("X Position")
ax.set_ylabel("Y Position")
ax.set_zlabel("Z Position")
ax.set_title("N-Body Simulation Visualization")
ax.legend()
# set limits
mn = np.min(data[:, 3:6])
mx = np.max(data[:, 3:6])
ax.set_xlim(mn, mx)
ax.set_ylim(mn, mx)
ax.set_zlim(mn, mx)
# set equal aspect ratio
ax.set_box_aspect([ub - lb for lb, ub in (getattr(ax, f"get_{a}lim")() for a in "xyz")])

plt.show()

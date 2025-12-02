# nbody visualizer script
# a crude way to visualize nbody simulation data stored in CSV files
# until we have a proper GUI

import argparse
import time
import numpy as np

try:
    import cv2
except ImportError:
    print("OpenCV (cv2) module not found.")
    print("Video saving functionality will be disabled.")
    cv2 = None

try:
    from drawing3d import Window, DrawList, EventList
except ImportError:
    print("drawing3d module not found")
    print("Please install it from with for Linux and MacOS (not Windows support):")
    print("pip install drawing3d")
    exit(1)

parser = argparse.ArgumentParser(description="Visualize n-body simulation data.")
parser.add_argument("data_file", type=str, help="Path to the CSV data file.")
parser.add_argument(
    "--speed",
    type=float,
    default=0.5,
    help="Playback speed multiplier",
)
parser.add_argument(
    "--save",
    type=str,
    default=None,
    help="Path to save the visualization as a mkv video file",
)
args = parser.parse_args()

if args.save is not None and cv2 is None:
    print("Cannot save video without OpenCV installed.")
    exit(1)


# i am sorry for this function
# i cannot bother with an external dependency
def hsv_to_rgb(h, s, v):
    if s == 0.0:
        return v, v, v
    i = int(h * 6.0)
    f = (h * 6.0) - i
    p = v * (1.0 - s)
    q = v * (1.0 - f * s)
    t = v * (1.0 - (1.0 - f) * s)
    i = i % 6
    if i == 0:
        return v, t, p
    if i == 1:
        return q, v, p
    if i == 2:
        return p, v, t
    if i == 3:
        return p, q, v
    if i == 4:
        return t, p, v
    if i == 5:
        return v, p, q


# load data
data = np.loadtxt(args.data_file, delimiter=",", skiprows=1)

# get all bodies using unique body ids
body_ids = np.unique(data[:, 1])

# collect masses and trajectories
masses = []
traj = []
vels = []
ts = []

# for each body, extract mass and trajectory
for body_id in body_ids:
    body_data = data[data[:, 1] == body_id]
    mass = body_data[0, 2]
    pos = body_data[:, 3:6]
    vel = body_data[:, 6:9]
    t = body_data[:, 0]

    ts.append(t)
    masses.append(mass)
    traj.append(pos)
    vels.append(vel)

# find min and max vel for color mapping
all_vels = np.abs(data[:, 6:9])
vmin = 0.0  # np.min(all_vels)
vmax = 5.18354  # np.max(all_vels)
print("Velocity range: ", vmin, vmax)

start_hue = 0.25
stop_hue = -0.85

# create drawing window
window = Window(1280, 720, b"N-Body Simulation Visualizer")

# create draw list
draw_list = DrawList()
draw_list_bg = DrawList()

# draw static elements in background draw list
draw_list_bg.style2(0.0, 0.0, 0.0, 0.02, 1.0)
draw_list_bg.clear()

# set up camera
camera = window.get_camera()
camera.set_perspective(45.0, 45.0)
camera.rotation = (0.0, -0.47, 1.57)
camera.distance = 27.5

# create event list
event_list = EventList()

stime = time.time()
prev_t = 0.0
quiting = False

# set up video writer if needed
if args.save is not None:
    # make it non-compressed
    fourcc = cv2.VideoWriter_fourcc(*"MJPG")
    # 60 FPS, window size
    video_writer = cv2.VideoWriter(args.save, fourcc, 60.0, (1280, 720))

while not event_list.poll():
    curr_t = time.time() - stime
    dt = curr_t - prev_t
    prev_t = curr_t

    # find the part of the trajectory to draw based on time and trail
    sim_time = curr_t * args.speed

    # clear the dynamic draw list
    draw_list.empty()

    # draw all bodies
    for i, body_id in enumerate(body_ids):
        mass = masses[i]
        traj_i = traj[i]
        t_i = ts[i]
        vel_i = vels[i]

        # find which index where are in
        idx = np.searchsorted(t_i, sim_time, "left")

        # now get two points to interpolate between
        if idx == 0:
            pos = traj_i[0]
        elif idx >= len(t_i):
            pos = traj_i[-1]
        else:
            t0 = t_i[idx - 1]
            t1 = t_i[idx]
            p0 = traj_i[idx - 1]
            p1 = traj_i[idx]

            # linear interpolation
            alpha = (sim_time - t0) / (t1 - t0)
            pos = (1 - alpha) * p0 + alpha * p1

        # calculate size based on mass
        size = mass ** (1.0 / 3.0)
        size = max(2.0, min(32.0, 100 * size))

        # calculate hue based on velocity magnitude
        vel = vel_i[min(idx, len(vel_i) - 1)]
        vel_mag = np.min([vmax, np.linalg.norm(vel)])
        vel_norm = (vel_mag - vmin) / (vmax - vmin + 1e-10)
        h = (start_hue + (stop_hue - start_hue) * vel_norm) % 1.0

        # use saturation with mass
        s = 1 - 0.5 * mass / max(masses)

        r, g, b = hsv_to_rgb(h, s, 1.0)

        # draw the point
        draw_list.style2(r, g, b, 1.0, size)
        draw_list.point(pos[0], pos[1], pos[2])

    # render the draw lists on the windows
    # and handle events
    window.render(draw_list_bg)
    window.render(draw_list)
    window.render_end()

    window.save_png(b"/tmp/frame.png")
    if args.save is not None:
        # read the saved png and write to video
        frame = cv2.imread("/tmp/frame.png")
        video_writer.write(frame)

    # handle events
    if window.handle_events(event_list):
        quiting = True

    # if saving and last index is reached, quit
    if args.save is not None:
        if idx >= len(t_i):
            quiting = True

    if quiting:
        break

    # clear the event list
    event_list.reset()


# clean up
window.destroy()
draw_list.destroy()
event_list.destroy()

if args.save is not None:
    video_writer.release()
    print(f"Saved video to {args.save}")

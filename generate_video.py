#!/usr/bin/env python3
import subprocess
import logging
import sys
from PIL import Image, ImageDraw, ImageFont, ImageColor
import argparse
import json
import numpy as np
import cv2

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

algo_names = [
    "Seq",
    "Par-Ex",
    "Par-P",
]

# Text fonts
text_font = ImageFont.truetype("resources/LiberationSans-Regular.ttf", size=16)

# Solution colors
sol_colors = ['yellow','darkturquoise','forestgreen','royalblue','springgreen','coral']


def maze_to_image(maze, piece_size):

    # Calculate board width and size
    cols = len(maze[0])
    rows = len(maze)
    h = rows*piece_size
    w = cols*piece_size

    # Draw the numberlink board
    image = Image.new("RGB", (w, h), "white")
    draw = ImageDraw.Draw(image)

    y = 0
    for line in maze:

        x = 0
        for ch in line:

            # draw the grid
            draw.rectangle((x, y, x+piece_size, y+piece_size),
                           None, (0, 0, 0), 1)

            # Dots are blank spaces
            if ch == ".":
                x += piece_size
                continue

            # Get color from map
            wall_color = (0, 0, 0)
            path_color = (255, 128, 0)

            # Fill according to type
            if ch == "X":
                # Wall
                draw.rectangle((x, y, x+piece_size, y+piece_size),
                               wall_color, wall_color, 1)
            else:
                # Path
                draw.rectangle((x, y, x+piece_size, y+piece_size),
                               path_color, (0, 0, 0), 1)

            x += piece_size
        y += piece_size

    # our start/goal is always in this position
    start = [cols+1, 0]
    goal = [cols-1, rows]

    return image, start, goal


def run_executable(executable, arguments, runs):
    """
    TODO
    """
    best_execution = None
    run = 0
    cmd_str = f"{executable} {str(' ').join(arguments)}"
    while run < runs:
        try:
            logger.debug(
                "(%d/%d) A correr: %s", run+1, runs, cmd_str)
            output = subprocess.check_output(
                [executable] + arguments, universal_newlines=True)
            logger.debug(
                "(%d/%d) A processar resultado", run+1, runs)

            # Try to parse video data
            data = json.loads(output.strip())

            logger.debug(
                "(%d/%d) Resultado processado", run+1, runs)

            # first execution
            if best_execution is None:
                run += 1
                best_execution = data
                continue

            # We get the best execution for this algorithm
            if data["execution_time"] < best_execution["execution_time"]:
                best_execution = data
            run += 1
        except subprocess.CalledProcessError:
            logger.error(
                "Execução com error: %s", cmd_str)
            continue
        except json.JSONDecodeError:
            logger.error(
                "Execução com resultado inválido: %s", cmd_str)
            continue

    return best_execution


def run_measurement(problem, instance, num_runs, thread_num=0,
                    first_solution=False):
    """
    TODO
    """
    # Execution arguments
    exec_cmd = f"./{problem}/bin/{problem}"
    # No flags required in this mode
    exec_args = []

    # Average result row
    if thread_num > 0:
        if first_solution:
            exec_args.append("-p")

        # Update execution arguments
        exec_args.append("-n")
        exec_args.append(str(thread_num))

    # Add instance to run
    exec_args.append(f"./instances/maze_{instance}")

    # Run execution and return json stats data
    return run_executable(exec_cmd, exec_args, num_runs)


def create_video(problem, instance, stats_data, measurements, speed=1.0, output_name=None):
    """
    TODO
    """
    # generation parameters
    fps = 30
    spacing = 20
    piece_size = 10
    time_scale = 100000
    end_delay = 2

    instance_file = f"./instances/maze_{instance}"

    # Read the maze and draw it
    with open(instance_file, encoding="utf-8") as fd:
        maze = fd.read().strip().split("\n")
        board, _, goal = maze_to_image(maze, piece_size)

    # Some vars to be able to draw the search process
    radius = int((piece_size//2) * 0.9)
    off_w = off_h = piece_size // 2
    time_modifier = time_scale * 1 / speed

    # video filename
    video_filename = f"report/{problem}-{instance}.mp4"
    if output_name:
        video_filename = output_name
    else:
        video_filename = f"report/{problem}-{instance}.mp4"

    algos = []

    # Get max execution time to calculate video length
    video_time = 0
    entries = []
    i = 0
    base_time = 0
    for stats in stats_data:
        if stats is None:
            i += 1
            continue

        # Get execution time and get scaler to match measurement
        # value
        exec_time = float(stats["execution_time"])
        # We want to match our measurements, so get the scaler
        try:
            if i == 0:
                scaler = float(measurements[i][-1])/exec_time
            else:
                scaler = float(measurements[i][-2])/exec_time
        except IndexError:
            # Defaults to no scale
            scaler = 1

        # Scale it to fit measurement and set video_time to max(exec_time)
        exec_time *= scaler
        if video_time < exec_time:
            video_time = exec_time

        # Set header with name, time and speed-up if parallel
        header = f"{algo_names[i]} E:{exec_time:.6f}"

        # Parallel with seq, set speed-up
        if base_time > 0:
            speed_up = base_time / exec_time
            header += f" SUp:{speed_up:.3f}"

        # Seq exists set base time so we can calculate speed-up for
        # parallel
        if i == 0:
            base_time = exec_time

        # Add to our data
        algos.append({"header": header, "raw_data": stats, "scaler": scaler})
        entries.append(sorted(stats["entries"], key=lambda x: x["frametime"]))
        i += 1

    available_algos = len(algos)
    original_size = (board.width*available_algos+spacing *
                     (available_algos+1), board.height+spacing*2)

    if original_size[0] > 1280 or original_size[1] > 720:
        video_size = (1280, 720)
    else:
        video_size = original_size

    # Get ratio in case we have scale image
    w_ratio =  video_size[0] / original_size[0]
    h_ratio =  video_size[1] / original_size[1]

    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video = cv2.VideoWriter(
        video_filename, fourcc, fps, video_size)

    # Draw a base frame which is the algo bords side by side
    base_image = Image.new('RGB', original_size, color='white')
    base_image_draw = ImageDraw.Draw(base_image)
    x = y = spacing
    middle = board.width // 2
    for data in algos:
        text_middle = text_font.getlength(data["header"]) // 2
        base_image_draw.text([x+middle-text_middle, 0],
                             data["header"], fill=(0, 0, 0), font=text_font)
        base_image.paste(board, (x, y))
        x += board.width + spacing

    # the initial frame is the base image (boards)
    frame = base_image.resize(video_size,Image.Resampling.LANCZOS)
    search_data = Image.new('RGBA', video_size)

    # Video generation counters
    event_counter = [0, 0, 0]
    finished_drawing = [False] * available_algos
    current_time = 0
    time_step = 1 / fps

    # Calculate video time with time modifier applied
    end_time = video_time * time_modifier + end_delay

    logger.debug(
        "Video: d: %s, fps: %d, time: %4f, step: %4f, entries: %s",
        str(video_size), fps, end_time, time_step,
        str([len(entries[i]) for i in range(available_algos)]))

    while current_time < end_time:
        logger.debug("A processar frame: %.4f, event counter: %s",
                     current_time, str(event_counter))

        # Path mask for this frame
        path_mask = Image.new('RGBA', video_size)

        for a in range(available_algos):

            if finished_drawing[a]:
                continue

            while True:
                # Get current entry+
                entry_idx = event_counter[a]
                entry = entries[a][entry_idx]
                scaler = algos[a]["scaler"]

                # calculate entry relative time to initial frametime
                current_entry_time = entry["frametime"] * \
                    time_modifier * scaler

                # check if we need to move to the next entry depending on the
                # entry relative time
                if current_entry_time > current_time:
                    draw_event(spacing, piece_size, board, radius, off_h,
                               off_w, a, search_data, path_mask, entry, w_ratio, h_ratio)
                    break

                # Draw current frame
                if not draw_event(spacing, piece_size, board, radius,
                                  off_h, off_w, a, search_data, path_mask,
                                  entry, w_ratio, h_ratio):
                    draw_solutions(spacing, piece_size, board, radius,
                                   off_h, off_w, a, search_data, entries[a], goal, w_ratio, h_ratio)
                    finished_drawing[a] = True
                    break

                # Move to the next event
                event_counter[a] += 1
                if event_counter[a] == len(entries[a]):
                    finished_drawing[a] = True
                    break

        # paste the received frame to the frame
        frame.paste(search_data, (0, 0), mask=search_data)
        frame.paste(path_mask, (0, 0), mask=path_mask)

        # Write frame and move to next frame
        video.write(cv2.cvtColor(np.array(frame), cv2.COLOR_RGB2BGR))
        current_time += time_step

    video.release()


def draw_event(spacing, piece_size, board, radius, off_h, off_w, algo,
               search_data, path_mask, entry, w_ratio, h_ratio):
    """
    TODO
    """
    # Draw according to the type of event
    event_type = entry["action"]

    if event_type == "end":
        return False

    if event_type == "goal":
        return True

    # So we can draw in the base image
    search_data_draw = ImageDraw.Draw(search_data)

    # calculate the initial position where to draw
    x = y = spacing
    x += (board.width + spacing) * algo

    # get the event position and calculate offset to draw
    position = entry["data"]["position"]
    row = position[1]
    col = position[0]
    off_x = x+(col * piece_size) + off_w
    off_y = y+(row * piece_size) + off_h

    # the color is event type dependent
    if event_type == "visited":
        color = "red"
    elif event_type == "successor":
        color = "blue"
    elif event_type == "goal":
        color = "yellow"

    # update the base board with the new visited position
    search_data_draw.rectangle((
        int((off_x - radius) * w_ratio), int((off_y - radius) * h_ratio),
        int((off_x + radius) * w_ratio), int((off_y + radius) * h_ratio)
    ), outline=color, fill=color)

    # Draw current path in frame only
    path = entry["data"].get("path", None)
    if path is None:
        return True

    # draw path
    path_mask_draw = ImageDraw.Draw(path_mask)
    for position in path:
        row = position[1]
        col = position[0]
        off_x = x+(col * piece_size) + off_w
        off_y = y+(row * piece_size) + off_h
        path_mask_draw.rectangle((
            int((off_x - radius) * w_ratio), int((off_y - radius) * h_ratio),
            int((off_x + radius) * w_ratio), int((off_y + radius) * h_ratio)
        ), outline=sol_colors[0], fill=sol_colors[0])

    return True


def draw_solutions(spacing, piece_size, board, radius, off_h, off_w, algo,
                   frame, entries, goal, w_ratio, h_ratio):
    """
    TODO
    """
    c_idx = 0
    for entry in entries:
        # Draw according to the type of event
        event_type = entry["action"]

        if event_type != "goal":
            continue

        position = entry["data"]["position"]
        if position[0] == goal[0] and position[1] == goal[1]:
            c_idx += 1

        # So we can draw in the base image
        frame_draw = ImageDraw.Draw(frame)

        # calculate the initial position where to draw
        x = y = spacing
        x += (board.width + spacing) * algo

        # get the event position and calculate offset to draw
        row = position[1]
        col = position[0]
        off_x = x+(col * piece_size) + off_w
        off_y = y+(row * piece_size) + off_h

        # update the base board with the new visited position
        frame_draw.rectangle((
            int((off_x - radius) * w_ratio), int((off_y - radius)*h_ratio),
            int((off_x + radius) * w_ratio), int((off_y + radius)*h_ratio)
        ), outline=sol_colors[c_idx], fill=sol_colors[c_idx])


def load_measurements(problem, instance, threads, in_file):

    measurements = []
    try:
        with open(in_file) as fd:
            for line in fd.readlines():
                cells = [str(value).strip().replace("\"", "").replace(",", ".")
                        for value in line.split(";")]
                # Must be our instance and if not sequencial the threads
                # number must match
                if f"{problem}-{instance}" not in cells:
                    continue
                if cells[1] != "sequencial" and int(cells[2]) != threads:
                    continue

                # Measurement found
                measurements.append(cells)
    except FileNotFoundError:
        logger.debug("Measurements file not found!")
        pass

    return measurements


def make_videos(problem, instance, num_runs, threads, algo, speed, output, in_file):
    """
    TODO
    """
    # To store measurements
    # 0-> sequential
    # 1-> parallel (first solution)
    # 2-> parallel(exhaustive search))
    stats_data = [None, None, None]
    measurements = load_measurements(problem, instance, threads, in_file)

    if algo == "seq":
        stats_data[0] = run_measurement(problem, instance, num_runs)
    elif algo == "par-ex":
        stats_data[1] = run_measurement(problem, instance, num_runs, threads)
    elif algo == "par-p":
        stats_data[2] = run_measurement(
            problem, instance, num_runs, threads, True)
    elif algo == "all":
        # Sequential, Parallel first and exhaustive
        stats_data[0] = run_measurement(problem, instance, num_runs)
        stats_data[1] = run_measurement(problem, instance, num_runs, threads)
        stats_data[2] = run_measurement(
            problem, instance, num_runs, threads, True)

    create_video(problem, instance, stats_data, measurements, speed, output)

# Custom function to convert a comma-separated string to a list of strings


def parse_str_list(arg):
    """
    TODO
    """
    try:
        # Split the string by commas and convert each part to an integer
        values = [str(x) for x in arg.split(',')]
        return values
    except ValueError as exc:
        raise argparse.ArgumentTypeError(
            "Invalid format. Should be comma-separated string.") from exc


if __name__ == '__main__':
    # Create the parser
    parser = argparse.ArgumentParser(
        description='Medidor de performance algoritmo A*')

    # Add command-line arguments
    parser.add_argument('-t', '--threads',
                        help='Número de trabalhadores', default=8)
    parser.add_argument('-s', '--speed',
                        help='Velocidade do video', default=1.0)
    parser.add_argument('-o', '--output',
                        help='Ficheiro de saida', default=None)
    parser.add_argument('-a', '--algo',
                        choices=['seq', 'par-p', 'par-ex', 'all'],
                        help='Algoritmo a usar', default='all')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')
    parser.add_argument('-r', '--runs', default=1,
                        help='Número de execuções')
    parser.add_argument('-m', '--measurements', default="report/measurements.csv",
                        help='Measurements file')
    parser.add_argument('problem', type=str, help='problema a utilizar')
    parser.add_argument('instance', type=str, help='instância a utilizar')

    # Parse the command-line arguments
    args = parser.parse_args()

    if args.debug:
        logger.setLevel(logging.DEBUG)

    # Make videos
    make_videos(args.problem, args.instance, int(args.runs), int(args.threads),
                args.algo, float(args.speed), args.output, args.measurements)

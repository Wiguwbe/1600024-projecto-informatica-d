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
text_font = ImageFont.truetype("resources/LiberationSans-Regular.ttf", size=24)

# Solution colors
sol_colors = ['yellow', 'darkturquoise',
              'forestgreen', 'royalblue', 'springgreen', 'coral']


def load_maze(file):
    # Read the maze and draw it
    with open(file, encoding="utf-8") as fd:
        maze = fd.read().strip().split("\n")

    cols = len(maze[0])
    rows = len(maze)
    return maze, cols, rows


def draw_board(image, maze, size, offset):

    # Calculate board width and size
    cols = len(maze[0])
    rows = len(maze)

    # Draw the board
    draw = ImageDraw.Draw(image)

    y = 0
    for line in maze:
        x = offset
        for ch in line:

            # draw the grid
            draw.rectangle((x, y, x+size[0], y+size[1]),
                           None, (0, 0, 0), 1)

            # Dots are blank spaces
            if ch == ".":
                x += size[0]
                continue

            # Get color from map
            wall_color = (0, 0, 0)
            path_color = (255, 128, 0)

            # Fill according to type
            if ch == "X":
                # Wall
                draw.rectangle((x, y, x+size[0], y+size[1]),
                               wall_color, wall_color, 1)
            else:
                # Path
                draw.rectangle((x, y, x+size[0], y+size[1]),
                               path_color, (0, 0, 0), 1)

            x += size[0]
        y += size[1]

    return image


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


def create_video(problem, instance, stats_data, measurements, speed, output_name, piece_size):
    """
    TODO
    """
    # generation parameters
    fps = 30
    time_scale = 100000
    end_delay = 2
    spacing = 20

    # Some vars to be able to draw the search process
    time_modifier = time_scale * 1 / speed

    # video filename
    video_filename = f"report/{problem}-{instance}.mp4"
    if output_name:
        video_filename = output_name
    else:
        video_filename = f"report/{problem}-{instance}.mp4"

    # Get max execution time to calculate video length
    video_time = 0
    entries = []
    i = 0
    base_time = 0
    algos = []
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

    num_algos = len(algos)

    # We always write a 720p video
    video_size = (1280, 720)
    boards_area = (1240, 600)

    # Read data from the board, goal is always in the same position
    instance_file = f"./instances/maze_{instance}"
    maze, cols, rows = load_maze(instance_file)
    original_board_size = (cols*piece_size, rows*piece_size)
    goal = [cols-1, rows]

    # Calculate expected board_size and scale factor
    e_board_x = (boards_area[0] // num_algos) - ((spacing // 2)*(num_algos-1))
    e_board_y = boards_area[1]
    scale_w = e_board_x / original_board_size[0]
    scale_h = e_board_y / original_board_size[1]

    # calculate the new board size with the scale applied
    scaled_piece_size = (piece_size * scale_w, piece_size * scale_h)
    n_board_size = (
        int(original_board_size[0] * scale_w),
        int(original_board_size[1] * scale_h)
    )
    boards_size = (
        num_algos*(n_board_size[0]+spacing)-spacing,
        n_board_size[1]
    )

    # create image for the boards and draw it
    boards_image = Image.new("RGB", boards_size, "white")
    off_x = 0
    for i in range(num_algos):
        draw_board(boards_image, maze, scaled_piece_size, off_x)
        off_x = (i + 1) * (n_board_size[0]+spacing)

    # the initial frame is the base image (boards)
    frame = Image.new('RGB', video_size, "white")
    frame.paste(boards_image, (spacing, 100))
    frame_draw = ImageDraw.Draw(frame)

    # Draw titles in the frame
    off_x = spacing
    middle = n_board_size[0] // 2
    for i in range(num_algos):
        data = algos[i]
        text = data["header"]
        text_middle = text_font.getlength(text) // 2
        pos = [off_x+middle-text_middle, 50]
        frame_draw.text(pos, text, fill=(0, 0, 0), font=text_font)
        off_x = (i + 1) * (n_board_size[0]+spacing)

    # Search data
    search_data = Image.new('RGBA', boards_size)

    # Video generation counters
    event_counter = [0, 0, 0]
    finished_drawing = [False] * num_algos
    current_time = 0
    time_step = 1 / fps

    # Calculate video time with time modifier applied
    end_time = video_time * time_modifier + end_delay

    logger.debug(
        "Video: d: %s, fps: %d, time: %4f, step: %4f, entries: %s",
        str(video_size), fps, end_time, time_step,
        str([len(entries[i]) for i in range(num_algos)]))

    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video = cv2.VideoWriter(
        video_filename, fourcc, fps, video_size)

    while current_time < end_time:
        logger.debug("A processar frame: %.4f, event counter: %s",
                     current_time, str(event_counter))

        # Path mask for this frame
        path_mask = Image.new('RGBA', boards_size)

        for i in range(num_algos):

            if finished_drawing[i]:
                continue

            off_x = i * (n_board_size[0]+spacing)
            while True:
                # Get current entry+
                entry_idx = event_counter[i]
                entry = entries[i][entry_idx]
                scaler = algos[i]["scaler"]

                # calculate entry relative time to initial frametime
                current_entry_time = entry["frametime"] * \
                    time_modifier * scaler

                # check if we need to move to the next entry depending on the
                # entry relative time
                if current_entry_time > current_time:
                    draw_event(scaled_piece_size, off_x,
                               search_data, path_mask, entry)
                    break

                # Draw current frame
                if not draw_event(scaled_piece_size, off_x, search_data, path_mask, entry):
                    draw_solutions(scaled_piece_size, off_x,
                                   search_data, entries[i], goal)
                    finished_drawing[i] = True
                    break

                # Move to the next event
                event_counter[i] += 1
                if event_counter[i] == len(entries[i]):
                    finished_drawing[i] = True
                    break

        # paste the received frame to the frame
        frame.paste(search_data, (spacing, 100), mask=search_data)
        frame.paste(path_mask, (spacing, 100), mask=path_mask)

        # Write frame and move to next frame
        video.write(cv2.cvtColor(np.array(frame), cv2.COLOR_RGB2BGR))
        current_time += time_step

    video.release()


def draw_event(piece_size, off_x, search_data, path_mask, entry):
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

    # get the event position and calculate offset to draw
    position = entry["data"]["position"]
    row = position[1]
    col = position[0]
    pos_x = off_x + col * piece_size[0]
    pos_y = row * piece_size[1]

    # the color is event type dependent
    if event_type == "visited":
        color = "red"
    elif event_type == "successor":
        color = "blue"
    elif event_type == "goal":
        color = "yellow"

    # update the base board with the new visited position
    search_data_draw.rectangle((
        pos_x, pos_y,
        pos_x+piece_size[0], pos_y+piece_size[1]
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
        pos_x = off_x + col * piece_size[0]
        pos_y = row * piece_size[1]
        path_mask_draw.rectangle((
            pos_x, pos_y,
            pos_x+piece_size[0], pos_y+piece_size[1]
        ), outline=sol_colors[0], fill=sol_colors[0])

    return True


def draw_solutions(piece_size, off_x, frame, entries, goal):
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

        # get the event position and calculate offset to draw
        row = position[1]
        col = position[0]
        pos_x = off_x + col * piece_size[0]
        pos_y = row * piece_size[1]

        # update the base board with the new visited position
        frame_draw.rectangle((
            pos_x, pos_y,
            pos_x+piece_size[0], pos_y+piece_size[1]
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


def generate_video(problem, instance, num_runs, threads, algo, speed, output, in_file, piece_size):
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

    create_video(problem, instance, stats_data,
                 measurements, speed, output, piece_size)

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
    parser.add_argument('-p', '--piece_size', default=10,
                        help='piece_size')
    parser.add_argument('-m', '--measurements', default="report/measurements.csv",
                        help='Measurements file')
    parser.add_argument('problem', type=str, help='problema a utilizar')
    parser.add_argument('instance', type=str, help='instância a utilizar')

    # Parse the command-line arguments
    args = parser.parse_args()

    if args.debug:
        logger.setLevel(logging.DEBUG)

    # Make videos
    generate_video(args.problem, args.instance, int(args.runs), int(args.threads),
                   args.algo, float(args.speed), args.output,
                   args.measurements, int(args.piece_size))

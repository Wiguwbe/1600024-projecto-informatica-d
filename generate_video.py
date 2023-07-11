#!/usr/bin/env python3
import subprocess
import logging
import sys
from PIL import Image, ImageDraw, ImageFont
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
text_font = ImageFont.truetype("LiberationSans-Regular.ttf", size=16)


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
        for i in range(len(line)):

            # draw the grid
            draw.rectangle((x, y, x+piece_size, y+piece_size),
                           None, (0, 0, 0), 1)

            ch = line[i]

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

    return image


def run_executable(executable, arguments, num_runs):
    best_execution = None
    run = 0
    cmd_str = f"{executable} {str(' ').join(arguments)}"
    while run < num_runs:
        logger.debug(
            f"({run}/{num_runs}) A correr: {cmd_str}")
        output = subprocess.check_output(
            [executable] + arguments, universal_newlines=True)
        try:
            # Try to parse video data
            data = json.loads(output.strip())

            # first execution
            if best_execution is None:
                run += 1
                best_execution = data
                continue

            # We get the best execution for this algorithm
            if data["execution_time"] < best_execution["execution_time"]:
                best_execution = data
            run += 1

        except json.JSONDecodeError:
            logger.error(
                f"Execução com resultado inválido: {executable} {arguments}")
            continue

    return best_execution


def run_measurement(problem, instance, num_runs, thread_num=0,
                    first_solution=False):
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


def create_video(problem, instance, stats_data, speed=1.0, output_name=None):

    # generation parameters
    fps = 30
    spacing = 20
    piece_size = 10
    time_scale = 100000
    end_delay = 2

    instance_file = f"./instances/maze_{instance}"

    # Read the maze and draw it
    with open(instance_file) as fd:
        maze = fd.read().strip().split("\n")
        board = maze_to_image(maze, piece_size)

    # Some vars to be able to draw the search process
    radius = int((piece_size//2) * 0.7)
    off_w = off_h = piece_size // 2
    time_modifier = time_scale * 1 / speed
    video_filename = f"reports/{problem}-{instance}.mp4" if not output_name else output_name
    algos = []

    # Get max execution time to calculate video length
    video_time = 0
    entries = []
    i = 0
    for stats in stats_data:
        if stats is None:
            i += 1
            continue
        algos.append({"name": algo_names[i], "raw_data": stats})
        entries.append(sorted(stats["entries"], key=lambda x: x["frametime"]))
        exec_time = float(stats["execution_time"]) * time_modifier
        logger.debug(
            f"A processar maior tempo -> {exec_time} ")
        if video_time < exec_time:
            video_time = exec_time
        i += 1

    available_algos = len(algos)
    video_size = (board.width*available_algos+spacing *
                  (available_algos+1), board.height+spacing*2)
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video = cv2.VideoWriter(
        video_filename, fourcc, fps, video_size)

    # Draw a base frame which is the algo bords side by side
    base_image = Image.new('RGB', video_size, color='white')
    base_image_draw = ImageDraw.Draw(base_image)
    x = y = spacing
    middle = board.width // 2
    for data in algos:
        text_middle = text_font.getlength(data["name"]) // 2
        base_image_draw.text([x+middle-text_middle, 0],
                             data["name"], fill=(0, 0, 0), font=text_font)
        base_image.paste(board, (x, y))
        x += board.width + spacing

    # the initial frame is the base image (boards)
    frame = base_image.copy()
    search_data = base_image.copy()

    # Video generation counters
    counter = [0, 0, 0]
    finished_drawing = [False, False, False]
    current_frametime = 0
    time_step = 1 / fps

    logger.debug(
        f"A gerar video: dimensão: {video_size}, fps: {fps}, video_time: {video_time}, time_step: {time_step}, entries: {[len(entries[i]) for i in range(available_algos)]} ")

    while current_frametime < video_time + end_delay:
        logger.debug(
            f"A processar frame -> counter: {counter}, current_frametime: {current_frametime} ")

        # Path mask for this frame
        path_mask = Image.new('RGBA', video_size)

        for algo in range(available_algos):

            if finished_drawing[algo]:
                continue

            while True:
                # Get current entry
                entry_idx = counter[algo]
                entry = entries[algo][entry_idx]

                # calculate entry relative time to initial frametime
                current_entry_time = entry["frametime"] * time_modifier

                # check if we need to move to the next entry depending on the entry relative time
                if current_entry_time > current_frametime:
                    draw_event(spacing, piece_size, board, radius, off_h,
                               off_w, algo, search_data, path_mask, entry)
                    break

                # Draw current frame
                if not draw_event(spacing, piece_size, board, radius, off_h, off_w, algo, search_data, path_mask, entry):
                    finished_drawing[algo] = True
                    break

                # Move to the next event
                counter[algo] += 1
                if counter[algo] == len(entries[algo]):
                    finished_drawing[algo] = True
                    break

        # paste the received frame to the frame
        frame.paste(search_data, (0, 0))
        frame.paste(path_mask, (0, 0), mask=path_mask)

        # Write frame and move to next frame
        video.write(cv2.cvtColor(np.array(frame), cv2.COLOR_RGB2BGR))
        current_frametime += time_step

    video.release()


def draw_event(spacing, piece_size, board, radius, off_h, off_w, algo, search_data, path_mask, entry):

    # Draw according to the type of event
    event_type = entry["type"]

    if event_type == "end":
        return False

    # So we can draw in the base image
    search_data_draw = ImageDraw.Draw(search_data)

    # calculate the initial position where to draw
    x = y = spacing
    x += (board.width + spacing) * algo

    # get the event position and calculate offset to draw
    position = entry["position"]
    row = position[1]
    col = position[0]
    off_x = x+(col * piece_size) + off_w
    off_y = y+(row * piece_size) + off_h

    # the color is event type dependent
    if event_type == "visited":
        color = "red"
    elif event_type == "sucessor":
        color = "blue"
    elif event_type == "goal":
        color = "yellow"

    # update the base board with the new visited position
    search_data_draw.ellipse((off_x - radius, off_y - radius, off_x +
                              radius, off_y + radius), outline='black', fill=color)

    # Draw current path in frame only
    path = entry.get("path", None)
    if path is None:
        return True

    # draw path
    path_mask_draw = ImageDraw.Draw(path_mask)
    for position in path:
        row = position[1]
        col = position[0]
        off_x = x+(col * piece_size) + off_w
        off_y = y+(row * piece_size) + off_h
        path_mask_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                radius, off_y + radius), outline='black', fill="lightgreen")

    return True


def make_videos(problem, instance, num_runs, threads, algo, speed, output_name):

    # To store measurements
    # 0-> sequential
    # 1-> parallel (first solution)
    # 2-> parallel(exhaustive search))
    stats_data = [None, None, None]

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

    create_video(problem, instance, stats_data, speed, output_name)

# Custom function to convert a comma-separated string to a list of strings


def parse_str_list(arg):
    try:
        # Split the string by commas and convert each part to an integer
        values = [str(x) for x in arg.split(',')]
        return values
    except ValueError:
        raise argparse.ArgumentTypeError(
            "Invalid string list format. Should be comma-separated string.")


if __name__ == '__main__':
    # Create the parser
    parser = argparse.ArgumentParser(
        description='Medidor de performance algoritmo A*')

    # Add command-line arguments
    parser.add_argument('-t', '--threads',
                        help='Número de trabalhadores', default=6)
    parser.add_argument('-s', '--speed',
                        help='Número de trabalhadores', default=1.0)
    parser.add_argument('-o', '--output',
                        help='Ficheiro de saida', default=None)
    parser.add_argument(
        '-a', '--algo', choices=['seq', 'par-p', 'par-ex', 'all'], help='Algoritmo a usar', default='all')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')
    parser.add_argument('-r', '--runs', default=10,
                        help='mumber of runs to get the best execution')
    parser.add_argument('problem', type=str, help='problema a utilizar')
    parser.add_argument('instance', type=str, help='instancia a utilizar')

    # Parse the command-line arguments
    args = parser.parse_args()

    # Access the parsed arguments
    threads = int(args.threads)
    debug = args.debug
    speed = float(args.speed)
    problem = args.problem
    instance = args.instance
    output = args.output
    algo = args.algo
    num_runs = int(args.runs)

    if debug:
        logger.setLevel(logging.DEBUG)

    # Run measurements
    make_videos(problem, instance, num_runs, threads, algo, speed, output)

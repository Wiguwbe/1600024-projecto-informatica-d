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
text_font = ImageFont.truetype("LiberationSans-Regular", size=16)


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
    for _ in range(num_runs): 
        logger.debug(f"A correr: {executable} {str(' ').join(arguments)}")
        output = subprocess.check_output(
            [executable] + arguments, universal_newlines=True)
        try:
            data = json.loads(output.strip())
            
            # first execution
            if best_execution is None:
                best_execution = data
                continue

            # We get the best execution for this algorithim
            if data["execution_time"] < best_execution["execution_time"]:
                best_execution = data

        except json.JSONDecodeError as e:
            logger.error(f"Execução com resultado inválido: {executable} {arguments}")
            continue
        
    return best_execution


def run_measurement(problem, instance, num_runs, thread_num=0, first_solution=False):
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
    video_filename = f"reports/{problem}-{instance}.mp4" if output_name is None else output_name
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
    board_image = Image.new('RGB', video_size, color='white')
    draw = ImageDraw.Draw(board_image)

    x = y = spacing
    middle = board.width // 2
    for data in algos:
        text_middle = text_font.getlength(data["name"]) // 2
        draw.text([x+middle-text_middle, 0],
                  data["name"], fill=(0, 0, 0), font=text_font)
        board_image.paste(board, (x, y))
        x += board.width + spacing

    counter = [0, 0, 0]
    current_entry = [entries[i][0] for i in range(available_algos)]
    finished_drawing = [False, False, False]
    current_frametime = 0
    time_step = 1 / fps
    logger.debug(
        f"A gerar video: dimensão: {video_size}, fps: {fps}, video_time: {video_time}, time_step: {time_step}, entries: {[len(entries[i]) for i in range(available_algos)]} ")
    frame_image = board_image.copy()
    frame_draw = ImageDraw.Draw(frame_image)
    while current_frametime < video_time + end_delay:
        # draw frame specific stuff here.
        logger.debug(
            f"A processar frame -> counter: {counter}, current_frametime: {current_frametime} ")

        move_to_next_frame = True

        # Copy board to current frame
        frame_image.paste(board_image, (0, 0))

        for i in range(available_algos):

            if finished_drawing[i]:
                continue

            entry = current_entry[i]
            entry_type = entry["type"]

            if entry_type == "visited":
                logger.debug(
                    f"A gerar frame visit -> frame time: {current_frametime},  entry frametime: {entry['frametime'] * time_modifier }, algo: {algo_names[i]}, ")
                x = y = spacing
                # Position x in the board position
                x += (board.width + spacing) * i

                # Draw a filled circle in the visited
                position = entry["position"]
                row = position[1]
                col = position[0]

                off_x = x+(col * piece_size) + off_w
                off_y = y+(row * piece_size) + off_h

                # Draw in both search space and frame
                draw.ellipse((off_x - radius, off_y - radius, off_x +
                              radius, off_y + radius), outline='black', fill="red")
                frame_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                    radius, off_y + radius), outline='black', fill="red")

                # Draw current path in frame only
                if "path" in entry:
                    # draw_path
                    for position in entry["path"]:
                        row = position[1]
                        col = position[0]
                        off_x = x+(col * piece_size) + off_w
                        off_y = y+(row * piece_size) + off_h
                        frame_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                            radius, off_y + radius), outline='black', fill="green")

            elif entry_type == "sucessor":
                logger.debug(
                    f"A gerar frame sucessor -> frame time: {current_frametime},  entry frametime: {entry['frametime'] * time_modifier }, algo: {algo_names[i]}, ")
                # Draw a blue circle in the sucessors
                x = y = spacing
                # Position x in the board position
                x += (board.width + spacing) * i

                # Draw a filled circle in the visited
                position = entry["position"]
                row = position[1]
                col = position[0]

                off_x = x+(col * piece_size) + off_w
                off_y = y+(row * piece_size) + off_h

                # draw in both search space and frame
                draw.ellipse((off_x - radius, off_y - radius, off_x +
                              radius, off_y + radius), outline='black', fill="blue")

                frame_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                    radius, off_y + radius), outline='black', fill="blue")

                # Draw current path on frame only
                if "path" in entry:
                    # draw_path
                    for position in entry["path"]:
                        row = position[1]
                        col = position[0]
                        off_x = x+(col * piece_size) + off_w
                        off_y = y+(row * piece_size) + off_h
                        frame_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                            radius, off_y + radius), outline='black', fill="lightgreen")

            elif entry_type == "goal":
                logger.debug(
                    f"A gerar frame goal -> frame time: {current_frametime}, algo: {algo_names[i]}")
                # We are now drawing the return
                x = y = spacing
                # Position x in the board position
                x += (board.width + spacing) * i

                # Draw a filled circle in the visited
                position = entry["position"]
                off_x = x+(position[0] * piece_size) + off_w
                off_y = y+(position[1] * piece_size) + off_h

                # draw in both search space and frame
                draw.ellipse((off_x - radius, off_y - radius, off_x +
                              radius, off_y + radius), outline='black', fill="yellow")
                frame_draw.ellipse((off_x - radius, off_y - radius, off_x +
                                    radius, off_y + radius), outline='black', fill="yellow")

            elif entry_type == "end":
                finished_drawing[i] = True

            # calculate entry relative time to initial frametime
            current_entry_time = current_entry[i]["frametime"] * time_modifier

            logger.debug(
                f"A processar tempo de frame -> frame time: {current_frametime}, current entry time: {current_entry_time} , algo: {algo_names[i]}")

            # check if we need to move to the next entry depending on the entry relative time
            if current_entry_time < current_frametime and not finished_drawing[i]:
                counter[i] += 1
                entry = current_entry[i] = entries[i][counter[i]]

                # calculate entry relative time to initial frametime
                next_entry_time = current_entry[i]["frametime"] * time_modifier
                move_to_next_frame = move_to_next_frame and next_entry_time > current_frametime

        # Write frame and move to next frame
        if move_to_next_frame:
            video.write(cv2.cvtColor(np.array(frame_image), cv2.COLOR_RGB2BGR))
            current_frametime += time_step

    video.release()


def run_measurements(problem, instance, num_runs, threads, algo, speed, output_name):

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
        stats_data[2] = run_measurement(problem, instance, num_runs, threads, True)
    elif algo == "all":
        # Sequential, Parallel first and exhaustive
        stats_data[0] = run_measurement(problem, instance, num_runs)
        stats_data[1] = run_measurement(problem, instance, num_runs, threads)
        stats_data[2] = run_measurement(problem, instance, num_runs, threads, True)

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

    # Run measurments
    run_measurements(problem, instance, num_runs, threads, algo, speed, output)

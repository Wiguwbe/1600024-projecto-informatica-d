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

problems = {
    "maze": [1, 2, 3, 4, 5, 6, 7, 8, 9]
}

excluded_instances = {
    "maze": []
}

row_par_first_excluded = {
    "maze": []
}

row_par_exhaustive_excluded = {
    "maze": []
}

# Text variables
measurement_header = "Procura"

algo_names = [
    "sequencial",
    "paralelo - procura exaustiva",
    "paralelo - primeira solução",
]

# Text fonts
text_font = ImageFont.truetype("LiberationSans-Regular", size=16)


def maze_to_image(maze, piece_size):

    # Calculate board width and size
    cols = len(maze[0])
    rows = len(maze)
    h = cols*piece_size
    w = rows*piece_size

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


# Map of each draw function to each problem
create_board_func = {
    "maze": maze_to_image
}


def run_executable(executable, arguments):
    logger.debug(f"A correr: {executable} {str(' ').join(arguments)}")
    output = subprocess.check_output(
        [executable] + arguments, universal_newlines=True)
    return output.strip()


def run_measurement(problem, instance, thread_num=0, first_solution=False):
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
    exec_args.append(f"./instances/{problem}_{instance}")

    # Run execution and return json stats data
    try:
        return json.loads(run_executable(exec_cmd, exec_args))
    except json.JSONDecodeError as e:
        logger.error("Error parsing JSON:", e)
        return None


def create_video(problem, instance, stats_data, speed=1.0):

    # generation parameters
    fps = 30
    spacing = 20
    piece_size = 25
    time_scale = 100000
    end_delay = 2

    instance_file = f"./instances/{problem}_{instance}"

    # Read the maze and draw it
    with open(instance_file) as fd:
        maze = fd.read().strip().split("\n")
        board = create_board_func[problem](maze, piece_size)

    # Some vars to be able to draw the search process
    radius = int((piece_size//2) * 0.7)
    off_w = off_h = piece_size // 2
    time_modifier = time_scale * 1 / speed

    # Get max execution time to calculate video length
    video_time = 0
    for stats in stats_data:
        exec_time = float(stats["execution_time"]) * time_modifier
        logger.debug(
            f"A processar maior tempo -> {exec_time} ")
        if video_time < exec_time:
            video_time = exec_time

    # # Sort search data by frametime
    entries = [
        sorted(stats_data[0]["entries"], key=lambda x: x["frametime"]),
        sorted(stats_data[1]["entries"], key=lambda x: x["frametime"]),
        sorted(stats_data[2]["entries"], key=lambda x: x["frametime"])
    ]

    # # Sort search data by frametime
    # entries = [
    #     stats_data[0]["entries"],
    #     stats_data[1]["entries"],
    #     stats_data[2]["entries"],
    # ]

    video_size = (board.width*3+spacing*4, board.height+spacing*2)
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video = cv2.VideoWriter(
        f"reports/{problem}-{instance}.mp4", fourcc, fps, video_size)
    frame_image = Image.new('RGB', video_size, color='white')
    draw = ImageDraw.Draw(frame_image)

    # Paste the 3 boards into the frame
    x = y = spacing
    middle = board.width // 2
    for i in range(3):
        text_middle = text_font.getlength(algo_names[i]) // 2
        draw.text([x+middle-text_middle, 0],
                  algo_names[i], fill=(0, 0, 0), font=text_font)
        frame_image.paste(board, (x, y))
        x += board.width + spacing

    counter = [0, 0, 0]
    current_entry = [ entries[i][0] for i in range(3) ]
    finished_drawing = [False, False, False]
    current_frametime = 0
    time_step = 1 / fps
    logger.debug(
        f"A gerar video: dimensão: {video_size}, fps: {fps}, video_time: {video_time}, time_step: {time_step}, entries: {[len(entries[i]) for i in range(3)]} ")
    while current_frametime < video_time + end_delay:
        # draw frame specific stuff here.
        logger.debug(
            f"A processar frame -> counter: {counter}, current_frametime: {current_frametime} ")

        move_to_next_frame = True

        for i in range(3):

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

                off_x = x+( col * piece_size) + off_w
                off_y = y+( row * piece_size) + off_h
                draw.ellipse((off_x - radius, off_y - radius, off_x +
                                radius, off_y + radius), outline='black', fill="red")

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

                off_x = x+( col * piece_size) + off_w
                off_y = y+( row * piece_size) + off_h
                draw.ellipse((off_x - radius, off_y - radius, off_x +
                                radius, off_y + radius), outline='black', fill="blue")


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
                draw.ellipse((off_x - radius, off_y - radius, off_x +
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


def run_measurements(problems, threads, excluded_problems, speed):

    # run all measurements
    problems_measurements = {}
    for problem, instances in problems.items():

        # problem excluded? ignore
        if problem in excluded_problems:
            continue

        # To store measurements
        # 0-> sequential
        # 1-> parallel (first solution)
        # 2-> parallel(exhaustive search))
        measurements = [[], [], []]
        for instance in instances:

            stats_data = [None, None, None]

            # Excluded instance? ignore
            if instance in excluded_instances[problem]:
                continue

            # Sequential, Parallel first and exhaustive
            stats_data[0] = run_measurement(problem, instance)
            stats_data[1] = run_measurement(problem, instance, threads)
            stats_data[2] = run_measurement(problem, instance, threads, True)

            create_video(problem, instance, stats_data, speed)

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
    parser.add_argument('-x', '--excluded', type=parse_str_list,
                        help='Problemas excluidos', default=[])
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')

    # Parse the command-line arguments
    args = parser.parse_args()

    # Access the parsed arguments
    threads = args.threads
    excluded_problems = args.excluded
    debug = args.debug
    speed = float(args.speed)

    if debug:
        logger.setLevel(logging.DEBUG)

    # Run measurments
    run_measurements(problems, threads, excluded_problems, speed)

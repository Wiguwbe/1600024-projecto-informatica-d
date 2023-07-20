#!/usr/bin/env python3
import subprocess
import logging
import sys
from PIL import Image, ImageDraw, ImageFont
import argparse

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()


# Text variables
solutions_header = "Soluções encontradas"

algo_names = [
    "sequencial",
    "paralelo - procura exaustiva",
    "paralelo - primeira solução",
]

# Colors and drawing
numberlink_bg_color_map = {
    "A": (255, 128, 0),
    "B": (128, 255, 0),
    "C": (255, 255, 0),
    "D": (0, 255, 255),
    "E": (0, 0, 255),
    "F": (127, 0, 255),
    "G": (255, 0, 255),
    "H": (255, 0, 127),
    "I": (0, 102, 0),
    "J": (0, 102, 102),
    "K": (153, 76, 0),
    "L": (255, 0, 0),
}

numberlink_fg_color_map = {
    "A": (0, 0, 0),
    "B": (0, 0, 0),
    "C": (0, 0, 0),
    "D": (0, 0, 0),
    "E": (255, 255, 255),
    "F": (255, 255, 255),
    "G": (0, 0, 0),
    "H": (255, 255, 255),
    "I": (255, 255, 255),
    "J": (255, 255, 255),
    "K": (255, 255, 255),
    "L": (255, 255, 255),
}

# Text fonts
solution_font = ImageFont.truetype(
    "resources/LiberationSans-Regular.ttf", size=10)
header_font = ImageFont.truetype(
    "resources/LiberationSans-Regular.ttf", size=24)


def puzzle8_to_image(solution: str):

    # Puzzle parameters
    border_width = 5
    piece_size = 30
    border_color = (0, 0, 0)

    # Width and height on puzzle 8 are always the same
    w = h = 3*piece_size+border_width*2

    # Draw the puzzle to a PNG image
    image = Image.new("RGB", (w, h), "white")
    draw = ImageDraw.Draw(image)
    draw.rectangle((0, 0, w, h), None, border_color, border_width)

    y = border_width
    for line in solution:

        x = border_width
        for i in range(len(line)):

            # Draw the grid
            draw.rectangle((x, y, x+piece_size, y+piece_size),
                           None, (0, 0, 0), 1)

            ch = line[i]

            if ch == "-":
                x += piece_size
                continue

            l = solution_font.getlength(ch)
            draw.text([x+(piece_size // 2)-(l // 2), y+(piece_size //
                      2)-(l // 2)], ch, fill=(0, 0, 0), font=solution_font)
            x += piece_size

        y += piece_size

    return image


def numberlink_to_image(solution):

    # Numberlink board parameters
    border_width = 5
    border_color = (0, 0, 0)
    piece_size = 30

    # Calculate board width and size
    w = len(solution[0])*piece_size+border_width*2
    h = len(solution)*piece_size+border_width*2

    # Draw the numberlink board
    image = Image.new("RGB", (w, h), "white")
    draw = ImageDraw.Draw(image)
    draw.rectangle((0, 0, w, h), None, border_color, border_width)

    y = border_width
    for line in solution:

        x = border_width
        for i in range(len(line)):

            # draw the grid
            draw.rectangle((x, y, x+piece_size, y+piece_size),
                           None, (0, 0, 0), 1)

            ch = line[i]

            # Dots are blank spaces
            if ch == ".":
                x += piece_size
                continue

            l = solution_font.getlength(ch)

            # Get color from map
            bg_color = numberlink_bg_color_map[ch.upper()]
            fg_color = numberlink_fg_color_map[ch.upper()]

            # Fill according to pair color
            draw.rectangle((x, y, x+piece_size, y+piece_size),
                           bg_color, (0, 0, 0), 1)

            if ch.isupper():
                draw.text([x+(piece_size // 2)-(l // 2), y+(piece_size //
                          2)-(l // 2)], ch, fill=fg_color, font=solution_font)

            x += piece_size
        y += piece_size

    return image


def maze_to_image(solution):

    # maze board parameters
    piece_size = 20

    # Calculate board width and size
    w = len(solution[0])*piece_size
    h = len(solution)*piece_size

    # Draw the numberlink board
    image = Image.new("RGB", (w, h), "white")
    draw = ImageDraw.Draw(image)

    y = 0
    for line in solution:

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
            wall_color = "black"
            path_color = "orange"

            # Fill according to type
            if ch == "X":
                # Wall
                draw.rectangle((x, y, x+piece_size, y+piece_size),
                               wall_color, wall_color, 1)
            else:
                # Path
                draw.rectangle((x, y, x+piece_size, y+piece_size),
                               path_color, "black", 1)

            x += piece_size
        y += piece_size

    return image


# Map of each draw function to each problem
draw_solution_func = {
    "8puzzle": puzzle8_to_image,
    "numberlink": numberlink_to_image,
    "maze": maze_to_image,
}


def run_executable(executable, arguments, runs):
    solutions = set()
    run = 0
    cmd_str = f"{executable} {str(' ').join(arguments)}"
    while run < runs:
        try:
            logger.debug(
                "(%d/%d) A correr: %s", run+1, runs, cmd_str)
            output = subprocess.check_output(
                [executable] + arguments, universal_newlines=True)
            output = output.strip().split("\n")
            # Split solution from measurement
            solution = "\n".join(output[:-1])
            # Store solution if available
            if not solution:
                logger.debug(
                    "(%d/%d) Execução finalizada sem solução!", run+1, runs)
                run += 1
                continue

            logger.debug(
                "(%d/%d) Execução finalizada a guardar solução.", run+1, runs)
            solutions.add(solution)

            run += 1
        except subprocess.CalledProcessError:
            logger.error(
                "Execução com error: %s", cmd_str)
            continue

    return solutions


def run_measurement(problem, instance,
                    num_runs, thread_num=0,
                    first_solution=False):
    # Execution arguments
    exec_cmd = f"./{problem}/bin/{problem}"
    # -r flag means we want in CSV format
    # -s flag means we want the solution
    exec_args = ["-r", "-s"]

    # Average result row
    if thread_num > 0:
        if first_solution:
            exec_args.append("-p")

        # Update execution arguments
        exec_args.append("-n")
        exec_args.append(str(thread_num))

    # Add instance to run
    if "maze" in problem:
        exec_args.append(f"./instances/maze_{instance}")
    else:
        exec_args.append(f"./instances/{problem}_{instance}")

    # Run execution and get average results
    return run_executable(exec_cmd, exec_args, num_runs)


def draw_solutions(draw_function, solutions):

    # draw parameters
    spacing = 5

    # local vars
    images = []
    w = (len(solutions)+1)*spacing
    h = 0

    # Generate a image per solution and store so we can draw
    for solution in solutions:
        image = draw_function(solution.split("\n"))
        w += image.width
        if image.height > h:
            h = image.height
        images.append(image)

    h += spacing * 2

    # Draw the numberlink board
    image = Image.new("RGB", (w, h), "white")

    x = spacing
    y = spacing
    for solution in images:
        image.paste(solution, (x, y))
        x += solution.width + spacing

    return image


def save_solutions_image(problem, instance, solutions, output, threads):

    # draw parameters
    spacing = 30
    half_spacing = spacing // 2

    # Prepare main algorithm header
    main_header = f"{solutions_header} - {problem} - {instance}"
    main_header_len = header_font.getlength(main_header)
    widths = [main_header_len]
    h = spacing * 5

    # Prepare headers for algorithms names
    algo_headers = []
    algo_headers_len = []
    for algo in range(3):

        if not solutions[algo]:
            continue

        if algo == 0:
            # Sequential
            algo_header = f"Algoritmo A* - {algo_names[algo]}"
            algo_header_len = header_font.getlength(algo_header)
            widths.append(solutions[algo].width)
            h += solutions[algo].height + half_spacing
        else:
            # Parallel
            algo_header = f"Algoritmo A* - {algo_names[algo]} - {','.join(map(str, threads))} threads"
            algo_header_len = header_font.getlength(algo_header)
            for solution in solutions[algo]:
                widths.append(solution.width)
                h += solution.height + half_spacing

        algo_headers.append(algo_header)
        algo_headers_len.append(algo_header_len)
        widths.append(algo_header_len)

    # Sort in descending order, so we set the image size to the max width
    widths.sort(reverse=True)

    # Image properties
    w = int(widths[0] + (spacing * 2))
    h = int(h)
    half_w = w // 2

    # Draw the numberlink board
    image = Image.new("RGB", (w, h), "white")
    draw = ImageDraw.Draw(image)

    x = y = spacing
    middle = half_w
    text_middle = main_header_len // 2
    draw.text([middle-text_middle, y],
              main_header, fill=(0, 0, 0), font=header_font)

    y += spacing
    if solutions[0] != None:
        text_middle = algo_headers_len[0] // 2
        draw.text([middle-text_middle, y],
                  algo_headers[0], fill=(0, 0, 0), font=header_font)
        y += spacing
        image.paste(solutions[0], (middle - solutions[0].width // 2, y))
        y += solutions[0].height + spacing // 2

    if solutions[1]:
        text_middle = algo_headers_len[1] // 2
        draw.text([middle-text_middle, y],
                  algo_headers[1], fill=(0, 0, 0), font=header_font)
        y += spacing
        for solution in solutions[1]:
            image.paste(solution, (middle - solution.width // 2, y))
            y += solution.height + spacing // 2

    if solutions[2]:
        text_middle = algo_headers_len[2] // 2
        draw.text([middle-text_middle, y],
                  algo_headers[2], fill=(0, 0, 0), font=header_font)
        y += spacing
        for solution in solutions[2]:
            image.paste(solution, (middle - solution.width // 2, y))
            y += solution.height + spacing // 2

    # Save image in location
    image.save(f"{output}/{problem}-{instance}.png", "PNG")


def generate_solution(problem, instance, threads, num_runs, output_image):

    draw_function = draw_solution_func[problem]

    # 0-> sequential
    # 1-> parallel(exhaustive search))
    # 2-> parallel (first solution)
    solution_images = [None, [], []]

    # Sequential
    solutions = run_measurement(
        problem, instance, num_runs)

    # Draw solutions
    if len(solutions):
        solution_images[0] = draw_solutions(
            draw_function, solutions)

    # Parallel per thread count
    for thread_num in threads:
        # First solution average
        solutions = run_measurement(problem, instance,
                                    num_runs, thread_num)
        # Draw solutions
        if len(solutions):
            solution_images[1].append(
                draw_solutions(draw_function, solutions))

        # First solution average
        solutions = run_measurement(problem, instance,
                                    num_runs, thread_num,
                                    True)
        # Store row and draw solutions
        if len(solutions):
            solution_images[2].append(
                draw_solutions(draw_function, solutions))

    # We must have at least a solution in one of the algorithms
    if solution_images[0] or solution_images[1] or solution_images[2]:
        save_solutions_image(
            problem, instance, solution_images, output_image, threads)

# Custom function to convert a comma-separated string to a list of integers


def parse_int_list(arg):
    try:
        # Split the string by commas and convert each part to an integer
        values = [int(x) for x in arg.split(',')]
        return values
    except ValueError:
        raise argparse.ArgumentTypeError(
            "Invalid integer list format. Should be comma-separated integers.")

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
        description='Gerador de soluções algoritmo A*')

    # Add command-line arguments
    parser.add_argument(
        '-r', '--runs', help='Número de execuções', default=10)
    parser.add_argument('-t', '--threads', type=parse_int_list,
                        help='Número de trabalhadores', default=[2, 4, 6, 8])
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')
    parser.add_argument('-o', '--output',
                        help='Localização para gravar imagens', default="report/solutions")
    parser.add_argument('problem', type=str, help='problema a utilizar')
    parser.add_argument('instance', type=str, help='instância a utilizar')

    # Parse the command-line arguments
    args = parser.parse_args()

    if args.debug:
        logger.setLevel(logging.DEBUG)

    # Run measurements
    generate_solution(args.problem, args.instance, args.threads,
                      int(args.runs), args.output)

#!/usr/bin/env python3
import subprocess
import logging
import sys
from PIL import Image, ImageDraw, ImageFont
import argparse

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

problems = {
    "8puzzle": ["easy_1", "easy_2", "easy_3",
                "hard_1", "hard_2",
                "impossible_1", "impossible_2"],
    "numberlink": [1, 2, 3, 4, 5, 6],
    "maze_bad": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
    "maze_good": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
}

excluded_instances = {
    "8puzzle": [],
    "numberlink": [6],
    "maze_bad": [8,9,10],
    "maze_good": []
}

row_par_first_excluded = {
    "8puzzle": [],
    "numberlink": [],
    "maze_bad": [],
    "maze_good": []
}

row_par_exhaustive_excluded = {
    "8puzzle": [],
    "numberlink": [6],
    "maze_bad": [],
    "maze_good": []
}

# Text variables
measurement_header = "Medições"
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
solution_font = ImageFont.truetype("LiberationSans-Regular.ttf", size=10)
header_font = ImageFont.truetype("LiberationSans-Regular.ttf", size=24)


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
draw_solution_func = {
    "8puzzle": puzzle8_to_image,
    "numberlink": numberlink_to_image,
    "maze_bad": maze_to_image,
    "maze_good": maze_to_image
}


def run_executable(executable, arguments, num_runs):
    solutions = set()
    rows = []
    for _ in range(num_runs):
        logger.debug(f"A correr: {executable} {str(' ').join(arguments)}")
        output = subprocess.check_output(
            [executable] + arguments, universal_newlines=True)
        output = output.strip().split("\n")
        solution = "\n".join(output[:-1])
        row = output[-1]
        if (solution):
            solutions.add(solution)
        logger.debug(f"Resultado: {row}")
        rows.append(row.split(';'))

    return rows, solutions


def calculate_average(rows):
    num_rows = len(rows)
    average_row = ['', '', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0]

    min_cost = sys.maxsize
    max_cost = -sys.maxsize
    solution_found = set()
    for row in rows:
        # Solution found
        solution_found.add(row[0])
        # Solution Cost (Min-Max)
        cost = int(row[1])
        if (cost < min_cost):
            min_cost = cost
        if (cost > max_cost):
            max_cost = cost
        # States Generated
        average_row[2] += int(row[2])
        # States explored
        average_row[3] += int(row[3])
        # Max Heap Size
        average_row[4] += int(row[4])
        # New Nodes
        average_row[5] += int(row[5])
        # Nodes reinserted
        average_row[6] += int(row[6])
        # Paths worst or Equals
        average_row[7] += int(row[7])
        # Paths better
        average_row[8] += int(row[8])
        # Paths better
        average_row[9] += int(row[9])
        # Paths better
        average_row[10] += int(row[10])
        # Paths better
        average_row[11] += int(row[11])
        # Execution time
        average_row[12] += float(row[12])

    # Solution found must be the same always
    if len(solution_found) > 1:
        average_row[0] = "\"Sim/Não\""
    else:
        average_row[0] = solution_found.pop()

    # Set the averages and min-max for cost
    if min_cost == max_cost:
        average_row[1] = min_cost
    else:
        average_row[1] = f"\"{min_cost}-{max_cost}\""

    # Set the average for the rest
    average_row[2] = average_row[2] // num_rows
    average_row[3] = average_row[3] // num_rows
    average_row[4] = average_row[4] // num_rows
    average_row[5] = average_row[5] // num_rows
    average_row[6] = average_row[6] // num_rows
    average_row[7] = average_row[7] // num_rows
    average_row[8] = average_row[8] // num_rows
    average_row[9] = average_row[9] // num_rows
    average_row[10] = average_row[10] // num_rows
    average_row[11] = average_row[11] // num_rows
    average_row[12] = round(average_row[12] / num_rows, 6)

    return average_row


def row_to_str(row: list):
    row_str = str(';'.join(map(str, row)))
    return row_str.replace('.', ',')


def run_measurement(problem, instance,
                    num_runs, thread_num=0,
                    first_solution=False):
    # Execution arguments
    exec_cmd = f"./{problem}/bin/{problem}"
    # -r flag means we want in CSV format
    # -s flag means we want the solution
    exec_args = ["-r", "-s"]

    # Average result row
    average_row = [f"\"{problem}-{instance}\""]
    if thread_num > 0:
        if first_solution:
            exec_args.append("-p")
            average_row.append("paralelo - primeira solução")
        else:
            average_row.append("paralelo - procura exaustiva")

        # Update execution arguments
        exec_args.append("-n")
        exec_args.append(str(thread_num))

        # Update return row
        average_row.append(thread_num)

    else:
        average_row.append("\"sequencial\"")

    # Add instance to run
    if "maze" in problem:
        exec_args.append(f"./instances/maze_{instance}")
    else:
        exec_args.append(f"./instances/{problem}_{instance}")

    # Run execution and get average results
    rows, solutions = run_executable(exec_cmd, exec_args, num_runs)
    average_row.extend(calculate_average(rows))

    # Print debug information ()
    logger.debug(f"Média: {row_to_str(average_row)}")

    return average_row, solutions


def draw_solutions(draw_function, solutions, statistic):

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


def save_solutions_image(problem, instance, solutions):

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

    image.save(f"reports/{problem}-{instance}.png", "PNG")


def run_measurements(problems, threads, num_runs, excluded_problems,
                     csv_output=True, save_solutions=False):

    # run all measurements
    problems_measurements = {}
    for problem, instances in problems.items():

        # problem excluded? ignore
        if problem in excluded_problems:
            continue

        draw_function = draw_solution_func[problem]

        # To store measurements
        # 0-> sequential
        # 1-> parallel (first solution)
        # 2-> parallel(exhaustive search))
        measurements = [[], [], []]
        for instance in instances:

            solution_images = [None, [], []]

            # Excluded instance? ignore
            if instance in excluded_instances[problem]:
                continue

            # Sequential
            row, solutions = run_measurement(
                problem, instance, num_runs)
            # Base time for calculating speed-up
            base_exec_time = row[-1]
            # Store row and draw solutions
            if len(solutions) and save_solutions:
                solution_images[0] = draw_solutions(
                    draw_function, solutions, row)
            measurements[0].append(row)

            # Parallel
            for thread_num in threads:
                if instance not in row_par_first_excluded[problem]:
                    # First solution average
                    row, solutions = run_measurement(problem, instance,
                                                     num_runs, thread_num,
                                                     True)
                    # Calculate speed-up and append to row
                    speed_up = round(base_exec_time/row[-1], 3)
                    row.append(speed_up)
                    # Store row and draw solutions
                    if len(solutions) and save_solutions:
                        solution_images[1].append(
                            draw_solutions(draw_function, solutions, row))
                    measurements[1].append(row)
                if instance not in row_par_exhaustive_excluded[problem]:
                    # First solution average
                    row, solutions = run_measurement(problem, instance,
                                                     num_runs, thread_num)
                    # Calculate speed-up and append to row
                    speed_up = round(base_exec_time/row[-1], 3)
                    row.append(speed_up)
                    # Store row and draw solutions
                    if len(solutions) and save_solutions:
                        solution_images[2].append(
                            draw_solutions(draw_function, solutions, row))
                    measurements[2].append(row)

            # We must have at least a solution in one of the algorithms
            if solution_images[0] or solution_images[1] or solution_images[2]:
                save_solutions_image(problem, instance, solution_images)

            # Store measurements for later save
            problems_measurements[problem] = measurements

    if csv_output:
        # Write results to CSV file
        with open(f"measurements/measurements.csv", "wt") as f:
            for problem, _ in problems.items():

                if problem not in problems_measurements:
                    continue

                measurements = problems_measurements[problem]
                f.write(f"{measurement_header} - {problem}\n")
                for algo in range(3):
                    f.write(f"Algoritmo A* - {algo_names[algo]}\n")
                    for instance in measurements[algo]:
                        f.write(row_to_str(instance)+"\n")
    else:
        # Print to screen
        for problem, _ in problems.items():
            if problem not in problems_measurements:
                continue

            measurements = problems_measurements[problem]
            print(f"{measurement_header} - {problem}")
            for algo in range(3):
                print(f"Algoritmo A* - {algo_names[algo]}")
                for instance in measurements[algo]:
                    print(row_to_str(instance))

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
        description='Medidor de performance algoritmo A*')

    # Add command-line arguments
    parser.add_argument('-r', '--runs', help='Número de execuções', default=100)
    parser.add_argument('-t', '--threads', type=parse_int_list,
                        help='Número de trabalhadores', default=[2, 4, 6, 8])
    parser.add_argument('-x', '--excluded', type=parse_str_list,
                        help='Problemas excluidos', default=[])
    parser.add_argument('-c', '--csv', help='Saida CSV', action='store_false')
    parser.add_argument('-i', '--images', action='store_true',
                        help='Guarda imagem das soluções')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')

    # Parse the command-line arguments
    args = parser.parse_args()

    # Access the parsed arguments
    num_runs = int(args.runs)
    threads = args.threads
    excluded_problems = args.excluded
    csv_output = args.csv
    print(csv_output)
    save_solutions = args.images
    debug = args.debug

    if debug:
        logger.setLevel(logging.DEBUG)

    # Run measurments
    run_measurements(problems, threads, num_runs,
                     excluded_problems, csv_output, save_solutions)

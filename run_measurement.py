#!/usr/bin/env python3
import subprocess
import logging
import sys
import argparse

logging.basicConfig(stream=sys.stdout, level=logging.INFO)
logger = logging.getLogger()

algo_names = [
    "sequencial",
    "paralelo - procura exaustiva",
    "paralelo - primeira solução",
]


def run_executable(executable, arguments, runs):
    rows = []
    run = 0
    cmd_str = f"{executable} {str(' ').join(arguments)}"
    while run < runs:
        try:
            logger.debug(
                "(%d/%d) A correr: %s", run+1, runs, cmd_str)
            output = subprocess.check_output(
                [executable] + arguments, universal_newlines=True)
            row = output.strip()
            logger.debug(
                "(%d/%d) Resultado: %s", run+1, runs, row)

            rows.append(row.split(';'))
            run += 1
        except subprocess.CalledProcessError:
            logger.error(
                "Execução com error: %s", cmd_str)
            continue

    return rows


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
    exec_args = ["-r"]

    # Average result row
    average_row = [f"\"{problem}-{instance}\""]
    if thread_num > 0:
        if first_solution:
            exec_args.append("-p")
            average_row.append("\"paralelo - primeira solução\"")
        else:
            average_row.append("\"paralelo - procura exaustiva\"")

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
    rows = run_executable(exec_cmd, exec_args, num_runs)
    average_row.extend(calculate_average(rows))

    # Print debug information ()
    logger.debug(f"Média: {row_to_str(average_row)}")

    return average_row


def run_measurements(problem, instance, threads, num_runs,
                     save_csv, output, truncate):

    # To store measurements
    # 0-> sequential
    # 1-> parallel(exhaustive search))
    # 2-> parallel (first solution)
    measurements = [[], [], []]

    # Sequential
    row = run_measurement(
        problem, instance, num_runs)
    # Base time for calculating speed-up
    base_exec_time = row[-1]
    # Store row
    measurements[0].append(row)

    # Parallel per thread count
    for thread_num in threads:
        # First solution average
        row = run_measurement(problem, instance,
                              num_runs, thread_num)
        # Calculate speed-up and append to row
        speed_up = round(base_exec_time/row[-1], 3)
        row.append(speed_up)
        # Store row
        measurements[1].append(row)

        # First solution average
        row = run_measurement(problem, instance,
                              num_runs, thread_num,
                              True)
        # Calculate speed-up and append to row
        speed_up = round(base_exec_time/row[-1], 3)
        row.append(speed_up)
        # Store row
        measurements[2].append(row)

    if save_csv:
        # Write results to CSV file
        mode = "wt" if truncate else "a"
        with open(output, mode) as f:
            for algo in range(3):
                for instance in measurements[algo]:
                    f.write(row_to_str(instance)+"\n")
    else:
        for algo in range(3):
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
    parser.add_argument(
        '-r', '--runs', help='Número de execuções', default=10)
    parser.add_argument('-t', '--threads', type=parse_int_list,
                        help='Número de trabalhadores', default=[2, 4, 6, 8])
    parser.add_argument('-c', '--csv', help='Saida CSV', action='store_true')
    parser.add_argument('-d', '--debug', action='store_true',
                        help='Ativa mensagens de debug')
    parser.add_argument('-n', '--truncate', action='store_true',
                        help='Trunca ficheiro CSV')
    parser.add_argument('-o', '--output',
                        help='Nome do ficheiro CSV', default="report/measurements.csv")
    parser.add_argument('problem', type=str, help='problema a utilizar')
    parser.add_argument('instance', type=str, help='instância a utilizar')

    # Parse the command-line arguments
    args = parser.parse_args()

    if args.debug:
        logger.setLevel(logging.DEBUG)

    # Run measurements
    run_measurements(args.problem, args.instance, args.threads, int(args.runs),
                     args.csv, args.output, args.truncate)

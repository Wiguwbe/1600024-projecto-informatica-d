import subprocess
import logging
import sys

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
logger = logging.getLogger()

problems = {
    "8puzzle": ["easy_1", "easy_2", "easy_3", "hard_1", "hard_2", "impossible_1", "impossible_2"],
    "numberlink": [1, 2, 3, 4, 5, 6]
}

num_runs = 10

threads = [2, 4, 6]

def run_executable(executable, arguments, executions=20):
    rows = []
    for _ in range(executions):
        logger.debug(f"A correr: {executable} {str(' ').join(arguments)}")
        output = subprocess.check_output(
            [executable] + arguments, universal_newlines=True)
        logger.debug(f"Resultado: {output.strip()}")
        rows.append(output.strip().split(';'))
    return rows


def calculate_average(rows):
    num_rows = len(rows)
    average_row = ['sim', 0, 0, 0, 0, 0, 0, 0, 0, 0.0]

    for row in rows:
        average_row[1] += int(row[1])
        average_row[2] += int(row[2])
        average_row[3] += int(row[3])
        average_row[4] += int(row[4])
        average_row[5] += int(row[5])
        average_row[6] += int(row[6])
        average_row[7] += int(row[7])
        average_row[8] += int(row[8])
        average_row[9] += float(row[9])

    average_row[0] = row[0]
    average_row[1] = average_row[1] // num_rows
    average_row[2] = average_row[2] // num_rows
    average_row[3] = average_row[3] // num_rows
    average_row[4] = average_row[4] // num_rows
    average_row[5] = average_row[5] // num_rows
    average_row[6] = average_row[6] // num_rows
    average_row[7] = average_row[7] // num_rows
    average_row[8] = average_row[8] // num_rows
    average_row[9] = round(average_row[9] / num_rows, 6)

    return average_row


def measure(problems, threads, executions, output="measurements.csv"):

    with open(f"measurements/{output}", "wt") as f:
        for problem, instances in problems.items():
            problem_measurements = {
                "sequencial": [],
                "paralelo - procura exaustiva": [],
                "paralelo - primeira solução": [],
            }
            for instance in instances:
                # Sequencial
                row_seq = [f"\"{problem}-{instance}\"", "\"sequencial\""]
                results_seq = run_executable(
                    f"./{problem}/bin/{problem}", ["-r", f"./instances/{problem}_{instance}"], executions)
                row_seq.extend(calculate_average(results_seq))
                problem_measurements["sequencial"].append(row_seq)
                logger.debug(f"Média Sequencial: {str(';'.join(map(str, row_seq))).replace('.', ',')}")

                # for calculating speed-up
                seq_exec_time = row_seq[-1]

                # Parallel
                for thread_num in threads:

                    # Exhaustive search
                    row_par_exhaust = [
                        f"\"{problem}-{instance}\"", "\"paralelo - procura exaustiva\"", thread_num]
                    results_par_exhaust = run_executable(f"./{problem}/bin/{problem}", [
                                                         "-r", "-n", str(thread_num), f"./instances/{problem}_{instance}"], executions)
                    row_par_exhaust.extend(
                        calculate_average(results_par_exhaust))
                    logger.debug(f"Média Paralelo (procura exaustiva): {str(';'.join(map(str, row_par_exhaust))).replace('.', ',')}")
                    # Calculate speed-up
                    par_exec_time = row_par_exhaust[-1]
                    row_par_exhaust.append(
                        round(seq_exec_time/par_exec_time, 3))
                    problem_measurements["paralelo - procura exaustiva"].append(
                        row_par_exhaust)

                    # First solution
                    row_par_first = [f"\"{problem}-{instance}\"",
                                     "\"paralelo - primeira solução\"", thread_num]
                    results_par_first = run_executable(f"./{problem}/bin/{problem}", ["-r", "-n", str(
                        thread_num), "-p", f"./instances/{problem}_{instance}"], executions)
                    row_par_first.extend(calculate_average(results_par_first))
                    logger.debug(f"Média Paralelo (primeira solução): {str(';'.join(map(str, row_par_first))).replace('.', ',')}")
                    # Calculate speed-up
                    par_exec_time = row_par_first[-1]
                    row_par_first.append(round(seq_exec_time/par_exec_time, 3))

                    problem_measurements["paralelo - primeira solução"].append(
                        row_par_first)

            f.write(f"Medições - {problem}\n")
            for algo, measurement in problem_measurements.items():
                f.write(f"Algoritmo A* - {algo}\n")
                for instance in measurement:
                    f.write(str(';'.join(map(str, instance))
                                ).replace('.', ',')+"\n")


if __name__ == '__main__':
    measure(problems, threads, num_runs)

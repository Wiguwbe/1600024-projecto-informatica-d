import random
import sys

def wilsons_maze(width, height):
    # Make dimensions odd
    width -= width % 2
    width += 1
    height -= height % 2
    height += 1

    maze = [[1] * width for _ in range(height)]

    s = rand_coord(width, height)
    maze[s[0]][s[1]] = 0

    while not complete(maze):
        c = None
        while not c or maze[c[0]][c[1]] != 1:
            c = rand_coord(width, height)

        maze[c[0]][c[1]] = 2

        path = [c]
        while maze[path[-1][0]][path[-1][1]] != 0:
            last = path[-1]
            n = neighbors_AB(maze, last[0], last[1])
            nb = random.choice(n)
            path.append(nb)

            maze[(nb[0] + last[0]) // 2][(nb[1] + last[1]) // 2] = 2
            if maze[nb[0]][nb[1]] == 0:
                for i in range(height):
                    for j in range(width):
                        if maze[i][j] == 2:
                            maze[i][j] = 0
            else:
                maze[nb[0]][nb[1]] = 2
                loc = index_of_coord(path, nb)
                if loc != len(path) - 1:
                    removed = path[loc + 1:]
                    maze[(nb[0] + last[0]) // 2][(nb[1] + last[1]) // 2] = 1
                    last = path[-1]
                    for k in range(len(removed) - 1, -1, -1):
                        on = removed[k]
                        next_ = removed[k - 1] if k else last
                        if k != len(removed) - 1:
                            maze[on[0]][on[1]] = 1
                        maze[(on[0] + next_[0]) // 2][(on[1] + next_[1]) // 2] = 1
    maze[0][1] = 0
    maze[height - 1][width - 2] = 0
    return maze


def neighbors_AB(maze, ic, jc):
    final = []
    for i in range(4):
        n = [ic, jc]
        n[i % 2] += ((i // 2 * 2) or -2)
        if 0 < n[0] < len(maze) and 0 < n[1] < len(maze[0]):
            final.append(n)
    return final


def index_of_coord(s, c):
    for i, coord in enumerate(s):
        if coord[0] == c[0] and coord[1] == c[1]:
            return i
    return -1


def complete(maze):
    for i in range(1, len(maze), 2):
        for j in range(1, len(maze[0]), 2):
            if maze[i][j] != 0:
                return False
    return True


def rand_coord(width, height):
    c = []
    c.append((random.randint(0, height // 2 - 1) * 2) + 1)
    c.append((random.randint(0, width // 2 - 1) * 2) + 1)
    return c


def save_maze(maze, file_name):
    with open(file_name, 'w') as file:
        for row in maze:
            line = ""
            for i in row:
                if i:
                    line += 'X'
                else:
                    line += '.'
            file.write(line + '\n')


if __name__ == "__main__":
    # Check if all required script arguments are provided
    if len(sys.argv) < 4:
        print("Usage: python script.py <width> <height> <file_name>")
        sys.exit(1)

    # Parse script arguments
    width = int(sys.argv[1])
    height = int(sys.argv[2])
    file_name = sys.argv[3]

    maze = wilsons_maze(width, height)
    save_maze(maze, file_name)

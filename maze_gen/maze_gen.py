'''
Python version of https://keesiemeijer.github.io/maze-generator/ by 
'''
import random
import sys
import random
import argparse

def replace_at(s, index, new_char):
    return s[:index] + new_char + s[index + 1:]


class Maze:
    def __init__(self,
                 width=20,
                 height=20,
                 entry_type="diagonal",
                 bias=None,
                 remove_walls=0,
                 max_walls_remove=300):

        self.matrix = []
        self.walls_removed = 0
        self.width = width
        self.height = height
        self.remove_walls = remove_walls
        self.entry_nodes = self.get_entry_nodes(entry_type)
        self.bias = bias
        self.max_walls_remove = int(max_walls_remove)

    def generate(self):
        nodes = self.generate_nodes()
        nodes = self.parse_maze(nodes)
        self.get_matrix(nodes)
        self.remove_maze_walls()

    def generate_nodes(self):
        count = self.width * self.height
        nodes = ["01111" for _ in range(count)]
        return nodes

    def parse_maze(self, nodes):
        maze_size = len(nodes)
        position_index = {'n': 1, 's': 2, 'w': 3, 'e': 4}
        opposite_index = {'n': 2, 's': 1, 'w': 4, 'e': 3}

        if not maze_size:
            return

        max_steps = 0
        move_nodes = []
        visited = 0
        position = random.randint(0, len(nodes) - 1)
        bias_count = 0
        bias_factor = 3

        if self.bias == 'horizontal':
            bias_factor = self.width // 100 + 2 if self.width >= 100 else 3
        elif self.bias == 'vertical':
            bias_factor = self.height // 100 + 2 if self.height >= 100 else 3

        nodes[position] = replace_at(nodes[position], 0, '1')

        while visited < (maze_size - 1):
            bias_count += 1
            max_steps += 1

            next_positions = self.get_neighbours(position)
            directions = [key for key, val in next_positions.items(
            ) if val != -1 and not int(nodes[val][0])]

            if self.bias and bias_count != bias_factor:
                directions = self.bias_directions(directions)
            else:
                bias_count = 0

            if directions:
                visited += 1

                if len(directions) > 1:
                    move_nodes.append(position)

                direction = random.choice(directions)

                nodes[position] = replace_at(
                    nodes[position], position_index[direction], '0')
                position = next_positions[direction]
                nodes[position] = replace_at(
                    nodes[position], opposite_index[direction], '0')
                nodes[position] = replace_at(nodes[position], 0, '1')
            else:
                if not move_nodes:
                    break

                position = move_nodes.pop()

        return nodes

    def get_matrix(self, nodes):
        maze_size = self.width * self.height
        row1 = ''
        row2 = ''

        if len(nodes) != maze_size:
            return

        for i in range(maze_size):
            row1 += '1' if not row1 else ''
            row2 += '1' if not row2 else ''

            if int(nodes[i][1]):
                row1 += '11'
                if int(nodes[i][4]):
                    row2 += '01'
                else:
                    row2 += '00'
            else:
                has_above = i - \
                    self.width >= 0 and int(nodes[i - self.width][4])
                has_next = i + 1 < maze_size and int(nodes[i + 1][1])

                if int(nodes[i][4]):
                    row1 += '01'
                    row2 += '01'
                elif has_next or has_above:
                    row1 += '01'
                    row2 += '00'
                else:
                    row1 += '00'
                    row2 += '00'

            if (i + 1) % self.width == 0:
                self.matrix.append(row1)
                self.matrix.append(row2)
                row1 = ''
                row2 = ''

        self.matrix.append('1' * (self.width * 2 + 1))

    def get_entry_nodes(self, access):
        y = (self.height * 2 + 1) - 2
        x = (self.width * 2 + 1) - 2
        entry_nodes = {}

        if access == 'diagonal':
            entry_nodes['start'] = {'x': 1, 'y': 1, 'gate': {'x': 1, 'y': 0}}
            entry_nodes['end'] = {'x': x, 'y': y, 'gate': {'x': x, 'y': y+1}}

        if access == 'horizontal' or access == 'vertical':
            xy = y if access == 'horizontal' else x
            xy = (xy - 1) // 2
            even = xy % 2 == 0
            xy = xy + 1 if even else xy

            start_x = 1 if access == 'horizontal' else xy
            start_y = xy if access == 'horizontal' else 1
            end_x = x if access == 'horizontal' else (
                start_x if even else start_x + 2)
            end_y = y if access == 'horizontal' else (
                start_y + 2 if even else start_y)
            start_gate = {'x': 0, 'y': start_y} if access == 'horizontal' else {
                'x': start_x, 'y': 0}
            end_gate = {
                'x': x + 1, 'y': end_y} if access == 'horizontal' else {'x': end_x, 'y': y + 1}

            entry_nodes['start'] = {'x': start_x,
                                    'y': start_y, 'gate': start_gate}
            entry_nodes['end'] = {'x': end_x, 'y': end_y, 'gate': end_gate}

        return entry_nodes

    def bias_directions(self, directions):
        horizontal = 'w' in directions or 'e' in directions
        vertical = 'n' in directions or 's' in directions

        if self.bias == 'horizontal' and horizontal:
            directions = [key for key in directions if key in {'w', 'e'}]
        elif self.bias == 'vertical' and vertical:
            directions = [key for key in directions if key in {'n', 's'}]

        return directions

    def get_neighbours(self, pos):
        return {
            'n': pos - self.width if pos - self.width >= 0 else -1,
            's': pos + self.width if pos + self.width < self.width * self.height else -1,
            'w': pos - 1 if pos % self.width != 0 else -1,
            'e': pos + 1 if (pos + 1) % self.width != 0 else -1,
        }

    def remove_wall(self, row, index):
        even_row = row % 2 == 0
        even_index = index % 2 == 0
        wall = int(self.matrix[row][index])

        if not wall:
            return False

        if not even_row and even_index:
            has_top = row - 2 > 0 and int(self.matrix[row - 2][index])
            has_bottom = row + \
                2 < len(self.matrix) and int(self.matrix[row + 2][index])

            if has_top and has_bottom:
                self.matrix[row] = replace_at(self.matrix[row], index, '0')
                return True
            elif not has_top and has_bottom:
                left = int(self.matrix[row - 1][index - 1])
                right = int(self.matrix[row - 1][index + 1])
                if left or right:
                    self.matrix[row] = replace_at(self.matrix[row], index, '0')
                    return True
            elif not has_bottom and has_top:
                left = int(self.matrix[row + 1][index - 1])
                right = int(self.matrix[row + 1][index + 1])
                if left or right:
                    self.matrix[row] = replace_at(self.matrix[row], index, '0')
                    return True

        elif even_row and not even_index:
            has_left = int(self.matrix[row][index - 2])
            has_right = int(self.matrix[row][index + 2])

            if has_left and has_right:
                self.matrix[row] = replace_at(self.matrix[row], index, '0')
                return True
            elif not has_left and has_right:
                top = int(self.matrix[row - 1][index - 1])
                bottom = int(self.matrix[row + 1][index - 1])
                if top or bottom:
                    self.matrix[row] = replace_at(self.matrix[row], index, '0')
                    return True
            elif not has_right and has_left:
                top = int(self.matrix[row - 1][index + 1])
                bottom = int(self.matrix[row + 1][index + 1])
                if top or bottom:
                    self.matrix[row] = replace_at(self.matrix[row], index, '0')
                    return True

        return False

    def remove_maze_walls(self):
        if not self.remove_walls or not self.matrix:
            return

        min_index = 1
        max_index = len(self.matrix) - 1
        max_tries = self.max_walls_remove
        tries = 0

        while tries < max_tries:
            tries += 1

            if self.walls_removed >= self.remove_walls:
                break

            y = random.randint(min_index, max_index - 1)
            walls = [i for i, c in enumerate(self.matrix[y]) if i != 0 and i != len(
                self.matrix[y]) - 1 and int(c)]

            random.shuffle(walls)

            for wall_index in walls:
                if self.remove_wall(y, wall_index):
                    self.walls_removed += 1
                    break

    def __str__(self):
        rows = len(self.matrix)
        cols = len(self.matrix[0])
        gate_entry = self.entry_nodes['start']['gate']
        gate_exit = self.entry_nodes['end']['gate']
        s = ""
        for y in range(rows):
            if y > 0:
                s += "\n"

            for x in range(cols):
                if x == gate_entry['x'] and y == gate_entry['y']:
                    s += "."
                    continue
                if x == gate_exit['x'] and y == gate_exit['y']:
                    s += "."
                    continue
            
                s += "X" if self.matrix[y][x] == '1' else '.'
        return s



    def save(self, output):
        with open(output, 'w') as file:
            file.write(str(self))


if __name__ == "__main__":
    # Create the parser
    parser = argparse.ArgumentParser(
        description='Medidor de performance algoritmo A*')

    # Add command-line arguments
    parser.add_argument('-r', '--remove_walls', default=0,
                        help='Parede a remover')
    parser.add_argument('-m', '--max_remove_walls', default=300,
                        help='Máximo de paredes a remover')
    parser.add_argument('-b', '--bias', choices=['horizontal', 'vertical', 'none'],
                        help='Tipo de entrada', default='none')
    parser.add_argument('-t', '--entry_type', choices=['diagonal', 'horizontal'],
                        help='Tipo de entrada', default='diagonal')
    parser.add_argument('-o', '--output',
                        help='Ficheiro de saida', default=None)
    parser.add_argument('-l', '--width', type=int,
                        help='Largura do labirinto', default=20)
    parser.add_argument('-a', '--height', type=int,
                        help='Altura do labirinto', default=20)

    # Parse the command-line arguments
    args = parser.parse_args()

    maze = Maze(args.width//2, args.height//2, args.entry_type,
                args.bias, args.remove_walls, args.max_remove_walls)

    maze.generate()

    if args.output:
        print(f'A gravar ficheiro: {args.output}')
        maze.save(args.output)
        sys.exit(0)

    else:
        print("Resultado:")
        print(str(maze))

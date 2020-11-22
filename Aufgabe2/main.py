#    0
#   1 2
#  3 4 5
# 6 7 8 9


class Puzzle:
    def __init__(self, types, piece_nr):
        self.types = types
        self.pieceNr = piece_nr


class Piece:
    def __init__(self, new_a, new_b, new_c, new_x, new_r):
        self.a = new_a  # right / upside-down left
        self.b = new_b  # left / upside-down right
        self.c = new_c  # bottom / upside-down top
        self.x = new_x  # piece_number
        self.r = new_r  # rotation counter-clockwise

    def rotate(self):  # counter-clockwise rotation
        n_a = self.c
        n_b = self.a
        n_c = self.b
        n_r = self.r + 1
        return Piece(n_a, n_b, n_c, self.x, n_r)


def r_reset_rec(m_a, m_b, m_c, m_x, m_r):  # reset rotation
    if m_r == 0:
        return Piece(m_a, m_b, m_c, m_x, m_r)
    r_reset(m_b, m_c, m_a, m_x, m_r - 1)


def r_reset(m_a, m_b, m_c, m_x, m_r):  # reset rotation
    while m_r > 0:
        tmp = m_a
        m_a = m_b
        m_b = m_c
        m_c = tmp
        m_r = m_r - 1
    return Piece(m_a, m_b, m_c, m_x, m_r)


def index_exists(a_list, index):
    if index < len(a_list):
        return True
    return False


def fit(pieces, piece_origin, i):  # check 1 piece
    piece_main = r_reset(piece_origin.a, piece_origin.b, piece_origin.c, piece_origin.x, piece_origin.r)
    y = 0  # 0
    if y == len(pieces):
        return True

    if i == y:
        return True

    y += 1  # 1
    if y == len(pieces):
        return False

    if i == y:
        return True

    y += 1  # 2
    if y == len(pieces):
        return False

    if i == y and piece_origin.a + pieces[1].a == 0 \
            and piece_main.c + pieces[0].c == 0:
        return True

    y += 1  # 3
    if y == len(pieces):
        return False

    if i == y and piece_origin.b + pieces[2].b == 0:
        return True

    y += 1  # 4
    if y == len(pieces):
        return False

    if i == y:
        return True

    y += 1  # 5
    if y == len(pieces):
        return False

    if i == y and piece_origin.a + pieces[4].a == 0 \
            and piece_main.c + pieces[1].c == 0:
        return True

    y += 1  # 6
    if y == len(pieces):
        return False

    if i == y and piece_origin.b + pieces[5].b == 0:
        return True

    y += 1  # 7
    if y == len(pieces):
        return False

    if i == y and piece_origin.a + pieces[6].a == 0:
        if piece_main.c + pieces[3].c == 0:
            return True

    y += 1  # 8
    if y == len(pieces):
        return False

    if i == y and piece_origin.b + pieces[7].b == 0:
        return True

    return False


def piece_not_puzzle(puzzle, piece):
    for i in range(0, len(puzzle)):
        if puzzle[i].x == piece.x:
            return False
    return True


def recursion(i, sol, puzzle_piece):
    if i == 9:
        return sol

    for j in range(0, 9):
        current = puzzle_piece[j]
        if piece_not_puzzle(sol, current):
            if fit(sol, puzzle_piece[j], j):
                sol.append(puzzle_piece[j])
                if not recursion(i + 1, sol, puzzle_piece) == []:
                    return sol
                sol.pop()

            if fit(sol, current.rotate(), j):
                sol.append(current.rotate())
                if not recursion(i + 1, sol, puzzle_piece) == []:
                    return sol
                sol.pop()

            if fit(sol, current.rotate().rotate(), j):
                sol.append(current.rotate().rotate())
                if not recursion(i + 1, sol, puzzle_piece) == []:
                    return sol
                sol.pop()

    return []


def output(final):
    if not final:
        print("no solution")
    else:
        print("\nSolution available:")
        for z in range(0, 9):
            print("Position ", z + 1, ", Piece", final[z].x, ", rotated counter-clockwise", final[z].r, "times.")


# driver
# file read
def main():
    print("File path and name: ")
    x = input()
    file = open(x, "r")
    count_types = int(file.readline())
    count_pieces = int(file.readline())
    puzzle = Puzzle(count_types, count_pieces)
    piece = []
    for x in range(puzzle.pieceNr):
        cur_line = file.readline()
        tokens = cur_line.split()
        a = int(tokens[0])
        b = int(tokens[1])
        c = int(tokens[2])
        piece_n = Piece(a, b, c, x+1, 0)
        piece.append(piece_n)
    file.close()
    # output
    solution = recursion(0, [], piece)
    output(solution)
    # driver end


if __name__ == '__main__':
    main()

#include<stdint.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<queue>
#include<limits>
#include<algorithm>

using std::cout;
using std::endl;

struct Cell
{
    uint32_t f, g;
    uint32_t row, col;
    char value;
    bool open, closed;
    Cell * parent;

    Cell(void)
    {
        row = 0;
        col = 0;
        f = std::numeric_limits<uint32_t>::max();
        g = std::numeric_limits<uint32_t>::max();
        value = 'X';
        open = false;
        closed = false;
        parent = 0;
    }

    bool operator==(Cell const & rhs)
    {
        return row == rhs.row && col == rhs.col;
    }
};

struct CellCompare
{
    bool operator()(Cell const & lhs, Cell const & rhs) const
    {
        return lhs.f < rhs.f;
    }
};

uint32_t calculateHeuristic(uint32_t row, uint32_t col, uint32_t end_row, uint32_t end_col)
{
    return abs(end_row - row) + abs(end_col - col);
}

Cell * buildMaze(std::ifstream & file, uint32_t & start_row, uint32_t & start_col, uint32_t & end_row, uint32_t & end_col, uint32_t & width, uint32_t & height)
{
    std::vector<std::string> file_buffer;
    bool valid_file = false;
    while(true) {
        std::string line;
        std::getline(file, line);

        if(file.eof()) {
            break;
        }

        file_buffer.push_back(line);
        valid_file = true;
    }

    if(! valid_file) {
        cout << "ERROR: invalid maze" << endl;
        exit(3);
    }

    width = file_buffer[0].size(), height = file_buffer.size();
    Cell * maze = new Cell[width * height];

    bool found_start = false, found_end = false;
    for(uint32_t row = 0; row < height; row += 1) {
        for(uint32_t col = 0; col < width; col += 1) {
            if(file_buffer[row][col] == 'S') {
                start_row = row;
                start_col = col;
                found_start = true;
            } else if(file_buffer[row][col] == 'E') {
                end_row = row;
                end_col = col;
                found_end = true;
            }

            Cell & current = maze[row * width + col];
            current.row = row;
            current.col = col;
            current.value = file_buffer[row][col];
        }
    }

    if(! found_start || ! found_end) {
        cout << "ERROR: malformed maze (no start and/or end)" << endl;
        exit(4);
    }

    return maze;
}

void solveMaze(Cell * maze, uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, uint32_t width, uint32_t height)
{
    std::priority_queue<Cell, std::vector<Cell>, CellCompare> open;
    Cell & start = maze[start_row * width + start_col];
    start.g = 0;
    start.f = calculateHeuristic(start_row, start_col, end_row, end_col);
    open.push(start);

    while(open.size() > 0) {
        Cell current_top = open.top();
        Cell & current = maze[current_top.row * width + current_top.col];

        if(current.row == end_row && current.col == end_col) {
            Cell * current_ptr = current.parent;
            if(current_ptr != 0) {
                while(current_ptr->parent != 0) {
                    current_ptr->value = '#';
                    current_ptr = current_ptr->parent;
                }
            }
        }

        current.open = false;
        current.closed = true;
        open.pop();

        for(int32_t i = ((int32_t) current.row) - 1; i <= ((int32_t) current.row) + 1; i += 1) {
            for(int32_t j = ((int32_t) current.col) - 1; j <= ((int32_t) current.col) + 1; j += 1) {
                if((i == current.row && j != current.col) || (i != current.row && j == current.col)) {
                    if(i >= 0 && i < height && j >= 0 && j < width) {
                        Cell & neighbor = maze[i * width + j];
                        if(neighbor.value == ' ' || neighbor.value == 'E') {
                            if(neighbor.closed == false) {
                                uint32_t tentative_g = current.g + 1;
                                if(tentative_g < neighbor.g) {
                                    neighbor.parent = &current;
                                    neighbor.g = tentative_g;
                                    neighbor.f = tentative_g + calculateHeuristic(neighbor.row, neighbor.col, end_row, end_col);

                                    if(neighbor.open == false) {
                                        neighbor.open = true;
                                        open.push(neighbor);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char ** argv)
{
    if(argc != 2) {
        cout << "usage: ./" << argv[0] << " mazefile" << endl;
        exit(1);
    }

    std::ifstream file(argv[1]);

    if(! file) {
        cout << "ERROR: could not find " << argv[1] << endl;
        exit(2);
    }

    uint32_t start_row, start_col;
    uint32_t end_row, end_col;
    uint32_t width, height;
    Cell * maze = buildMaze(file, start_row, start_col, end_row, end_col, width, height);
    solveMaze(maze, start_row, start_col, end_row, end_col, width, height);

    return 0;
}

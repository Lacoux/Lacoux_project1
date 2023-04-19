#include <fstream>

#include "page.h"

using std::endl;
using std::ofstream;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, int content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

    private:
        int num_jobs, width, height;
        int width_border, height_border;
        ofstream& output; 
        char** board;
        int** idlist;
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char*[width*height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = new char[2*num_jobs];
            for(int j = 0; j < num_jobs; j++) {
                board[h*width + w][j] = ' ';
            }
        }
    }

    idlist = new int*[num_jobs];
    
    for(int h = 0; h < num_jobs; h++) {
        idlist[h] = new int[5];
        for(int w = 0; w < 5; w++) {
            idlist[h, w] = 0;
        }
    }

}

Board::~Board() {
    delete [] board;
    delete [] idlist;
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w][0] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {

        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int x, int y, int page_width, int page_height, int id, int content) {
    for(int h = y; h <= y + page_height; h++) {
        for(int w = x; w <= x + page_width; w++) { 
            for(int i = 0; i < num_jobs-1; i++) {
                board[h*width + w][i+1] = board[h*width + w][i];
                board[h*width + w][i+1+num_jobs] = board[h*width + w][i + num_jobs];
            }
            board[h*width + w][0] = content;
            char char_id = id;
            board[h*width + w][num_jobs] = char_id;
        }
    }
    int i = 0;
    while(idlist[i][0] != 0) {
        i = i + 1;
    }
    idlist[i][0] = id;
    idlist[i][1] = y;
    idlist[i][2] = page_height;
    idlist[i][3] = x;
    idlist[i][4] = page_width;
    print_board();
}

void Board::delete_page(int id) {

}

void Board::modify_content(int id, char content) {
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int y = idlist[i][1];
    int page_height = idlist[i][2];
    int x = idlist[i][3];
    int page_width = idlist[1][4];
    char char_id = id;

    for(int h = y; h <= y + page_height; h++) {
        for(int w = x; w <= x + page_width; w++) { 
            int i = 0;
            while(board[h*width + w][i+num_jobs] != char_id) {
                i = i + 1;
            }
            board[h*width + w][i] = content;
        }
    }
    print_board();
}

void Board::modify_position(int id, int x, int y) {
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int prev_y = idlist[i][1];
    int page_height = idlist[i][2];
    int prev_x = idlist[i][3];
    int page_width = idlist[1][4];
    char char_id = id;
    int i = 0;
    while(board[prev_y*width + prev_x][i+num_jobs] != char_id) {
        i = i + 1;
    }
    char content = board[prev_y*width + prev_x][i];

    for(int h = prev_y; h <= prev_y + page_height; h++) {
        for(int w = prev_x; w <= prev_x + page_width; w++) { 
            int i = 0;
            while(board[h*width + w][i+num_jobs] != char_id) {
                i = i + 1;
            }
            if(i == num_jobs - 1) {
                board[h*width + w][i] = ' ';
                board[h*width + w][i+num_jobs] = ' ';
            }
            else {
                for(int j = i; j < num_jobs-1; j++) {
                    board[h*width + w][j] = board[h*width + w][j+1];
                    board[h*width + w][j+num_jobs] = board[h*width + w][j+1+num_jobs];
                }
            }
        }
    }
    for(int h = y; h <= y + page_height; h++) {
        for(int w = x; w <= x + page_width; w++) { 
            for(int i = 0; i < num_jobs-1; i++) {
                board[h*width + w][i+1] = board[h*width + w][i];
                board[h*width + w][i+1+num_jobs] = board[h*width + w][i + num_jobs];
            }
            board[h*width + w][0] = content;
            board[h*width + w][num_jobs] = char_id;
        }
    }
}

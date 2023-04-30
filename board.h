#include <fstream>
#include <vector>
#include <string>
#include "page.h"

using std::endl;
using std::ofstream;
using std::vector;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void insert_page_light(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void delete_page_light(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

    private:
        int num_jobs, width, height;
        ofstream& output; 
        vector<vector<char>> board;
        vector<vector<int>> board_id;
        vector<vector<int>> idlist;
        vector<int> memory_id;
        vector<char> memory_content;
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    //Define Array Using Vectors
    board= vector<vector<char>>(width*height, vector<char>(num_jobs, ' '));
    board_id = vector<vector<int>>(width*height, vector<int>(num_jobs, 0));
    idlist = vector<vector<int>>(num_jobs, vector<int>(5, 0));
    memory_id = vector<int>(num_jobs, 0);
    memory_content = vector<char>(num_jobs, ' ');
}

Board::~Board() {
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w][0] << " "; //board[x][0] is printed
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


void Board::insert_page(int x, int y, int page_width, int page_height, int id, char content) {
    for(int h = y; h < y+ page_height; h++) {
        for(int w = x; w < x + page_width; w++) { 
            for(int i = num_jobs - 2; i >= 0; i--) {
                board[h*width + w][i+1] = board[h*width + w][i];
                board_id[h*width + w][i+1] = board_id[h*width + w][i];
            }
            board[h*width + w][0] = content;
            board_id[h*width + w][0] = id; //Push Arrays and insert content
        }
    }
    int j = 0;
    while(idlist[j][0] != 0) {
        j = j + 1;
    }
    //saving id and id's info to idlist
    idlist[j][0] = id;
    idlist[j][1] = y;
    idlist[j][2] = page_height;
    idlist[j][3] = x;
    idlist[j][4] = page_width;
    print_board();
}

// this is page insertion, but it doesn't save idlist
void Board::insert_page_light(int x, int y, int page_width, int page_height, int id, char content) {
    for(int h = y; h < y+ page_height; h++) {
        for(int w = x; w < x + page_width; w++) { 
            for(int i = num_jobs - 2; i >= 0; i--) {
                board[h*width + w][i+1] = board[h*width + w][i];
                board_id[h*width + w][i+1] = board_id[h*width + w][i];
            }
            board[h*width + w][0] = content;
            board_id[h*width + w][0] = id;
        }
    }
    print_board();
}

void Board::delete_page(int id) {
    //Get information using id and idlist
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int prev_y = idlist[i][1];
    int page_height = idlist[i][2];
    int prev_x = idlist[i][3];
    int page_width = idlist[i][4];
    
    //Is all content cor to id is all displayed(board[][0])?
    loop:
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            i = 0;
            while(board_id[h*width + w][i] != id) {
                i = i + 1;
                if(i == num_jobs - 1) {
                    break;
                }
            }
            if (i != 0) {
                int new_id = board_id[h*width + w][i-1];
                delete_page_light(new_id); //it's not, so let's look at the above content
                goto loop;
            }
        }
    }

    // delete content and below content will be displayed
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            for(int j = 0; j < num_jobs-1; j++) {
                board[h*width + w][j] = board[h*width + w][j+1];
                board_id[h*width + w][j] = board_id[h*width + w][j+1];
            }
            board[h*width + w][num_jobs - 1] = ' ';
            board_id[h*width + w][num_jobs - 1] = 0;
        }
    }
    print_board();
    
    // retrieve pages using memory_id and memory_content, and insert_page_light
    i = 0;
    int read_id;
    char read_content;
    int c = 0;

    while(memory_id[i] != 0) {
        read_id = memory_id[i];
        read_content = memory_content[i];
        c = 0;
        while(idlist[c][0] != read_id) {
            c = c + 1;
            if(c == num_jobs - 1) {
                break;
            }
        }
        prev_y = idlist[c][1];
        page_height = idlist[c][2];
        prev_x = idlist[c][3];
        page_width = idlist[c][4];
        insert_page_light(prev_x, prev_y, page_width, page_height, read_id, read_content);
        i = i + 1;
    }

    //we used memory well, so let's reset it.
    for(int i = 0; i < num_jobs; i++) {
        memory_id[i] = 0;
        memory_content[i] = ' ';
    }
}

//this is recursion function and at last this function will delete content which is all displayed
void Board::delete_page_light(int id) {
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int prev_y = idlist[i][1];
    int page_height = idlist[i][2];
    int prev_x = idlist[i][3];
    int page_width = idlist[i][4];
    
    // So is it the last?
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            i = 0;
            while(board_id[h*width + w][i] != id) {
                i = i + 1;
                if(i == num_jobs - 1) {
                    break;
                }
            }
            if (i != 0) {
                int new_id = board_id[h*width + w][i-1];
                delete_page_light(new_id); // if not, let's do it again
            }
        }
    }

    // it's the last, so we came here. And let's memorize this id and content
    if(memory_id[0] != 0) {
        for(int j = num_jobs - 2; j >= 0; j--) {
            memory_id[j+1] = memory_id[j];
            memory_content[j+1] = memory_content[j];
        }
    }
    memory_id[0] = id;
    memory_content[0] = board[prev_y*width + prev_x][0];
    // last memorized one is first retrieved
    
    // delete it and display below content
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            for(int j = 0; j < num_jobs-1; j++) {
                board[h*width + w][j] = board[h*width + w][j+1];
                board_id[h*width + w][j] = board_id[h*width + w][j+1];
            }
            board[h*width + w][num_jobs - 1] = ' ';
            board_id[h*width + w][num_jobs - 1] = 0;
        }
    }
    print_board();
}

// almost similar with delete code, but there is a code added
void Board::modify_content(int id, char content) {
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int prev_y = idlist[i][1];
    int page_height = idlist[i][2];
    int prev_x = idlist[i][3];
    int page_width = idlist[i][4];
    
    loop:
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            i = 0;
            while(board_id[h*width + w][i] != id) {
                i = i + 1;
                if(i == num_jobs - 1) {
                    break;
                }
            }
            if (i != 0) {
                int new_id = board_id[h*width + w][i-1];
                delete_page_light(new_id);
                goto loop;
            }
        }
    }

    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            for(int j = 0; j < num_jobs-1; j++) {
                board[h*width + w][j] = board[h*width + w][j+1];
                board_id[h*width + w][j] = board_id[h*width + w][j+1];
            }
            board[h*width + w][num_jobs - 1] = ' ';
            board_id[h*width + w][num_jobs - 1] = 0;
        }
    }
    print_board();

    // let's modify content
    insert_page_light(prev_x, prev_y, page_width, page_height, id, content);

    i = 0;
    int read_id;
    char read_content;
    int c = 0;

    while(memory_id[i] != 0) {
        read_id = memory_id[i];
        read_content = memory_content[i];
        c = 0;
        while(idlist[c][0] != read_id) {
            c = c + 1;
            if(c == num_jobs - 1) {
                break;
            }
        }
        prev_y = idlist[c][1];
        page_height = idlist[c][2];
        prev_x = idlist[c][3];
        page_width = idlist[c][4];
        insert_page_light(prev_x, prev_y, page_width, page_height, read_id, read_content);
        i = i + 1;
    }

    for(int i = 0; i < num_jobs; i++) {
        memory_id[i] = 0;
        memory_content[i] = ' ';
    }
}

// it is also almost similar with delete, but there is added parts.
void Board::modify_position(int id, int x, int y) {
    int i = 0;
    while(idlist[i][0] != id) {
        i = i + 1;
    }
    int prev_y = idlist[i][1];
    int page_height = idlist[i][2];
    int prev_x = idlist[i][3];
    int page_width = idlist[i][4];   

    int j = 0;
    while(board_id[prev_y*width + prev_x][j] != id) {
        j = j + 1;
    }
    //let's memorize this content, it will be needed...
    char content = board[prev_y*width + prev_x][j];

    loop:
    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            i = 0;
            while(board_id[h*width + w][i] != id) {
                i = i + 1;
                if(i == num_jobs - 1) {
                    break;
                }
            }
            if (i != 0) {
                int new_id = board_id[h*width + w][i-1];
                delete_page_light(new_id);
                goto loop;
            }
        }
    }

    for(int h = prev_y; h < prev_y + page_height; h++) {
        for(int w = prev_x; w < prev_x + page_width; w++) { 
            for(int j = 0; j < num_jobs-1; j++) {
                board[h*width + w][j] = board[h*width + w][j+1];
                board_id[h*width + w][j] = board_id[h*width + w][j+1];
            }
            board[h*width + w][num_jobs - 1] = ' ';
            board_id[h*width + w][num_jobs - 1] = 0;
        }
    }
    print_board();

    // let's insert position and let idlist know changes
    insert_page_light(x, y, page_width, page_height, id, content);
    j = 0;
    while(idlist[j][0] != id) {
        j = j + 1;
    }
    idlist[j][1] = y;
    idlist[j][2] = page_height;
    idlist[j][3] = x;
    idlist[j][4] = page_width;
    
    i = 0;
    int read_id;
    char read_content;
    int c = 0;

    while(memory_id[i] != 0) {
        read_id = memory_id[i];
        read_content = memory_content[i];
        c = 0;
        while(idlist[c][0] != read_id) {
            c = c + 1;
            if(c == num_jobs - 1) {
                break;
            }
        }
        prev_y = idlist[c][1];
        page_height = idlist[c][2];
        prev_x = idlist[c][3];
        page_width = idlist[c][4];
        insert_page_light(prev_x, prev_y, page_width, page_height, read_id, read_content);
        i = i + 1;
    }

    for(int i = 0; i < num_jobs; i++) {
        memory_id[i] = 0;
        memory_content[i] = ' ';
    }
}
 

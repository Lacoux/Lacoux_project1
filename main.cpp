#include <iostream>
#include <fstream>
#include <string>

#include "board.h"

using namespace std;

void manage_board(string input_file, string output_file) {
    
    int width, height;
    int num_jobs; 
    char job_type;
    ifstream input;
    ofstream output;

    input.open(input_file);

    if (!(input.is_open())) {
        cerr << "Input missing" << endl;
        return;
    }    
    
    input >> width >> height >> num_jobs;

    output.open(output_file, ofstream::trunc);

    if (!(output.is_open())) {
        cerr << "Output missing" << endl;
        return;
    }


    Board board = Board(num_jobs, width, height, output);


    int page_x, page_y, page_width, page_height, page_id;
    char page_content;
    
    for (int job_idx = 1; job_idx <= num_jobs; job_idx++) {
        input >> job_type;

        switch (job_type) {
            case 'i': 
                //insert page
                input >> page_id >> page_x >> page_y >> page_width >> page_height >> page_content;
                board.print_job(job_idx, job_type, page_id);
                board.insert_page(page_x, page_y, page_width, page_height, page_id, page_content);      
                break;
            case 'd':
                //delete page
                input >> page_id;
                board.print_job(job_idx, job_type, page_id);
                board.delete_page(page_id);
                break;
            case 'm':
                //modify page
                int modify_content;
                input >> modify_content;

                if (modify_content) {
                    //modify content 
                    input >> page_id >> page_content;
                    board.print_job(job_idx, job_type, page_id);
                    board.modify_content(page_id, page_content);
                } else {
                    //modify position
                    input >> page_id >> page_x >> page_y;
                    board.print_job(job_idx, job_type, page_id);
                    board.modify_position(page_id, page_x, page_y);
                }
                break;
            default:
                cerr << "Wrong job type" << endl;
                return;
        }

        cerr << "Job " << job_idx << " Complete!" << endl;

    } 
    

    input.close();
    output.close();
    
}


int main(int argc, char *argv[]) {

    if (argc==3) manage_board(string(argv[1]), string(argv[2]));
    else cerr << "Wrong arguments" << endl;
    

    return 0; 

}
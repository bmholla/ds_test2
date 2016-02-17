// Declare type_info to avoid compile error with C++11 and GCC 4.4.
namespace std {
class type_info;
}

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>


typedef struct exec_time_node {
    double t;
    struct exec_time_node *link;
    } ExecTimeNode;


void check_results (void);
void init_timer (void);
double snapshot_timer (void);
void append_an_exec_time (double t);
void setup (int argc, char *argv[]);
bool is_a_hex_string (const std::string &s);

// MAP results file
std::ofstream fp;
// CPU results (correct output) file
std::ofstream fpr;

ExecTimeNode *times_head = NULL;
ExecTimeNode *times_tail = NULL;
std::string res_filename = "res";
std::string res_correct_filename = "res_correct";
std::string report_filename = "report.xml";
bool use_seq_data = false;

int num_map_calls = 0;


void check_results_1 (void) {
    std::ifstream fp;
    std::ifstream fpr;
    std::ofstream report;
    ExecTimeNode *p;
    unsigned match_count=0, error_count=0;
    uint64_t mask, bit_difference_vector=0;
    bool value_count_matches=true;
    int i;

    report.open(report_filename.c_str());
    if (!report.is_open()) {
        std::cerr << "couldn't open file '" << report_filename << "'" << std::endl;
        exit (1);
    }

    report << "  <num_map_calls>" << num_map_calls << "</num_map_calls>\n";

    fp.open(res_filename.c_str());
    if (!fp.is_open()) {
        report << "  <no_result_file>yes</no_result_file>\n";
        report << "</test>\n";
        exit (1);
    }

    fpr.open(res_correct_filename.c_str());
    if (!fpr.is_open()) {
        report << "  <no_result_file>yes</no_result_file>\n";
        report << "</test>\n";
        exit (1);
    }

    while (1) {
        std::string line_fp;
        std::string line_fpr;

        getline (fp, line_fp);
        getline (fpr, line_fpr);

        if (((line_fp == "") && (line_fpr != "")) ||
            ((line_fp != "") && (line_fpr == ""))) {
            value_count_matches = false;
            break;
        }

        if (line_fp == "") {
            break;
        }

        if (line_fp == line_fpr) {
            match_count++;
        } else {
            error_count++;

            const bool st_fp = is_a_hex_string (line_fp);
            const bool st_fpr = is_a_hex_string (line_fpr);

            if (st_fp != st_fpr) {
                // the format of the files became inconsistent...
                value_count_matches = false;
                break;
            }

            if (st_fp == false) {
                // even if this isn't hex more values may be worth examining
                continue;
            }

            const uint64_t val_fp  = strtoull (line_fp.c_str(), NULL, 16);
            const uint64_t val_fpr = strtoull (line_fpr.c_str(), NULL, 16);

            if (val_fp == val_fpr)
                // not in a format that enables detecting what the error is
                ;
            else {
                mask = 1;

                for (i=0; i<8; i++) {
                    uint64_t tmp0, tmp1;
                    tmp0 = val_fp & mask;
                    tmp1 = val_fpr & mask;

                    if (tmp0 != tmp1)
                        bit_difference_vector |= mask;

                    mask <<= 1;
                }
            }
        }
    }

    report << "  <right_number_of_vals>" << (value_count_matches?"yes":"no") << "</right_number_of_vals>\n";
    report << "  <num_correct_vals>" << match_count << "</num_correct_vals>\n";
    report << "  <num_incorrect_vals>" << error_count << "</num_incorrect_vals>\n";
    report << "  <error_vector>0x" << std::hex << bit_difference_vector << "</error_vector>\n";

    report << "  <exec_times>\n";
    for (p=times_head; p!=NULL; p=p->link)
        report << "    <time>" << p->t << "</time>\n";
    report << "  </exec_times>\n";

    report << "  <code_completed>yes</code_completed>\n";

    fp.close();
    fpr.close();
    report.close();

    if (!value_count_matches || (error_count > 0))
        exit (2);
}


bool is_a_hex_string (const std::string &s) {
    if (s.length() < 3)
        return false;
    if (s.substr(0, 2) != "0x")
        return false;
    return isxdigit(s[2]);
}


double snapshot_timer (void) {
    unsigned int tm_sec, tm_usec;
    double tm;
    struct itimerval itv0;

    getitimer (ITIMER_REAL, &itv0);
    tm_sec = itv0.it_value.tv_sec;
    tm_usec = itv0.it_value.tv_usec;
    tm = tm_sec + (double)tm_usec/1000000;

    return tm;
}


void init_timer (void) {
    struct itimerval itv0, itv1;

    itv0.it_interval.tv_sec = 10000000;
    itv0.it_interval.tv_usec = 0;
    itv0.it_value.tv_sec = 10000000;
    itv0.it_value.tv_usec = 0;
    setitimer (ITIMER_REAL, &itv0, &itv1);
}


void append_an_exec_time (double t) {
    ExecTimeNode *tmp;

    tmp = (ExecTimeNode*) malloc (sizeof (ExecTimeNode));
    tmp->t = t;
    tmp->link = NULL;

    if (times_head == NULL) {
        times_head = tmp;
        times_tail = tmp;
    } else {
        times_tail->link = tmp;
        times_tail = tmp;
    }
}


void setup (int argc, char *argv[]) {
    for (unsigned i=1; i<argc; i++)
        if (strncmp (argv[i], "--resdir=", strlen ("--resdir=")) == 0) {
            std::string path = std::string(argv[i] + strlen ("--resdir="));
            res_filename = path + "/res";
            res_correct_filename = path + "/res_correct";
            report_filename = path + "report.xml";
        }
        else if (strncmp (argv[i], "--use_seq_data", strlen ("--use_seq_data")) == 0) {
            use_seq_data = true;
        }

    fp.open(res_filename.c_str());
    if (!fp.is_open()) {
        std::cerr << "couldn't open file '" << res_filename << "'" << std::endl;
        exit (1);
    }

    fpr.open(res_correct_filename.c_str());
    if (!fpr.is_open()) {
        std::cerr << "couldn't open file '" << res_correct_filename << "'" << std::endl;
        exit (1);
    }

    srandom (42);
    init_timer ();
}


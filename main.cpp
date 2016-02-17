// M10100 main.cpp

#include <carte/host>
#include <test_suite_routines.h>
using namespace carte;

const unsigned SIZE = 128;


void subr(cpu_ptr<long> A, cpu_ptr<long> B, long value, int m, int mapnum);
void run_a_test (int64_t value);

int64_t *A, *B;

int main (int argc, char *argv[]) {
    setup (argc, argv);

    A = (int64_t*) Cache_Aligned_Allocate (SIZE * sizeof (int64_t));
    B = (int64_t*) Cache_Aligned_Allocate (SIZE * sizeof (int64_t));

    map_allocate (1);

    run_a_test (0x1234567887654321L);
    run_a_test (0x8765432112345678L);

    map_free (1);

    fp.close();
    fpr.close();

    check_results_1 ();

    return 0;
}

void run_a_test (int64_t value) {
    double t0, t1;
    int mapnum = 0;

    for (unsigned i=0; i<SIZE; i++) {
        A[i] = (random() < 0x40000000) ? 1 : 0;
        if (use_seq_data)
            B[i] = i;
        else
            B[i] = random();
        fpr << "0x" << std::hex << (A[i] ? value : B[i]) << std::endl;
    }

    t0 = snapshot_timer();
    subr (make_cpu(A), make_cpu(B), value, SIZE, mapnum);
    t1 = snapshot_timer();

    for (unsigned i=0; i<SIZE; i++) {
        fp << "0x" << std::hex << B[i] << std::endl;
    }

    append_an_exec_time (t0 - t1);
    num_map_calls++;
}

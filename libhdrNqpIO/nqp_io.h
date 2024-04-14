#pragma once

typedef struct nqp_writer nqp_writer;
typedef struct nqp_init_args nqp_init_args;
typedef struct nqp_start_args nqp_start_args;

nqp_writer * nqp_write_init(nqp_init_args * args);

int nqp_write_start(nqp_start_args * start_args);
void nqp_write_notify_computation_complete(unsigned long long s_count);
void nqp_write_wait();
void nqp_write_solution(int dim, int * solution, nqp_writer * writer);
void nqp_write_end();
void nqp_write_close(nqp_writer * writer);

#ifndef PCG_BASIC_H_INCLUDED
#define PCG_BASIC_H_INCLUDED 1
#include <inttypes.h>
#if __cplusplus
extern "C" {
#endif
struct pcg_state_setseq_64 {
    uint64_t state;
    uint64_t inc;
};
typedef struct pcg_state_setseq_64 pcg32_random_t;
#define PCG32_INITIALIZER   { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }
void pcg32_srandom(uint64_t initstate, uint64_t initseq);
void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate,
                     uint64_t initseq);
uint32_t pcg32_random(void);
uint32_t pcg32_random_r(pcg32_random_t* rng);
uint32_t pcg32_boundedrand(uint32_t bound);
uint32_t pcg32_boundedrand_r(pcg32_random_t* rng, uint32_t bound);
#if __cplusplus
}
#endif
#endif
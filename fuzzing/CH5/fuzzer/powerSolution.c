#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>
#include<time.h>
#include<setjmp.h>
#include<string.h>
#include<ctype.h>
#include<fcntl.h>
#include<unistd.h>

#include"runner.h"
#include"fuzzer.h"
#include"coverage.h"
#include"powerSolution.h"

Population assignEnergy_PowerSchedule(PowerSchedule ps, Population pop){

	for(int i = 0; i < pop.pop_size; i++)
		pop.se[i].energy = 1;
	return pop;

}

float* normalizedEnergy_PowerSchedule(PowerSchedule ps, Population pop){

	float sum = 0;
        float* norm_energy = malloc(sizeof(float)*pop.pop_size);

        for(int i = 0; i < pop.pop_size; i++)
                sum += pop.se[i].energy;

        for(int i = 0; i < pop.pop_size; i++)
                norm_energy[i] = pop.se[i].energy / sum;
        return norm_energy;

}

SeedEnergy choose_PowerSchedule(PowerSchedule ps, Population pop){

	struct timeval now;

        gettimeofday(&now, NULL);
        srand((unsigned int)now.tv_usec);
        pop = ps.assignEnergy(ps, pop);
        float* norm_energy = ps.normalizedEnergy(ps, pop);
        SeedEnergy se;

        int p = rand()%100 + 1;

        int min_idx = 0;

        float sum_norm = 0.0;

        int id = 0;
        for(int i = 0; i< pop.pop_size; i++){
                if((norm_energy[i] * 100.0 + sum_norm ) > p ){
                        min_idx = i;
                        break;

                }
                sum_norm += norm_energy[i] * 100.0;
        }


        se.seed = pop.se[min_idx].seed;
        se.energy = norm_energy[min_idx];
        free(norm_energy);
        return se;
}

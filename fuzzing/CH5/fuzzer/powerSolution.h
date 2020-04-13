#ifndef __POWERSOLUTION_H__
#define __POWERSOLUTION_H__

typedef struct _PowerSchedule{

        MutationFuzzer mf;

        Population (*assignEnergy)(struct _PowerSchedule, Population);

        float* (*normalizedEnergy)(struct _PowerSchedule, Population);

        SeedEnergy (*choose)(struct _PowerSchedule, Population);

}PowerSchedule;

Population assignEnergy_PowerSchedule(PowerSchedule ps, Population pop);
float* normalizedEnergy_PowerSchedule(PowerSchedule ps, Population pop);
SeedEnergy choose_PowerSchedule(PowerSchedule ps, Population pop);

#endif

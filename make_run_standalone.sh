#!/bin/bash
g++ -lm -Wall -O -g ./src/main_soil_moisture.cxx ./src/bmi_soil_moisture_profile.cxx ./src/soil_moisture_profile.cxx -o run_bmi_smp
./run_bmi_smp config/config_conceptual.txt
rm -f run_bmi_smp
#${CXX} -lm -Wall -O -g ./src/bmi_main.cxx ./src/bmi_coupler.cxx ./src/smc_profile.cxx -o run_bmi_coupler

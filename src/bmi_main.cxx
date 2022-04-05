#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../bmi/bmi.hxx"
#include "../include/bmi_coupler.hxx"
#include "../include/smc_profile.hxx"

#define SUCCESS 0
int main(int argc, char *argv[])
{
  BmiCoupler model;
  
  if (argc != 2) {
    printf("Usage: run_bmifrozensoilcxx CONFIGURATION_FILE\n\n");
    printf("Run the frozensoilcxx model through its BMI with a configuration file.\n");
    printf("Output is written to the file `bmifrozensoilcxx.out`.\n");
    return SUCCESS;
  }

  FILE *fp = fopen("bmi_file.out", "w");
  fprintf(fp, "Configuration file = %s\n", argv[1]);
  fprintf(fp, "Initializing... ");
  
  model.Initialize(argv[1]);
  
  fprintf(fp, "done\n");

  {
    std::string model_name;
    model_name = model.GetComponentName();
    fprintf(fp, "%s\n", model_name.c_str());
  }

  {
    std::string var_name_s = "soil_storage";
    std::string var_name_sc = "soil_storage_change";
    std::string var_name_wt = "soil_water_table";
    std::string var_name_smc = "soil_moisture_profile";
    std::string var_name_smcl = "soil_moisture_layered";
    std::string var_name_smc_bmi = "soil_moisture_profile_option_bmi";
    
    int grid, rank, *shape;
    double *var_s = NULL;
    double *var_sc = NULL;
    double *var_wt = NULL;

    fprintf(fp, "variable = %s\n", var_name_s.c_str());
    fprintf(fp, "variable = %s\n", var_name_sc.c_str());
    fprintf(fp, "variable = %s\n", var_name_wt.c_str());
    fprintf(fp, "variable = %s\n", var_name_smc.c_str());
    fprintf(fp, "variable = %s\n", var_name_smcl.c_str());
    fprintf(fp, "variable = %s\n", var_name_smc_bmi.c_str());
    
    grid = model.GetVarGrid(var_name_smc);

    rank = model.GetGridRank(grid);
    fprintf(fp, "rank = %d\n", rank);
    shape = new int[rank];
    model.GetGridShape(grid, shape);

    fprintf(fp, "shape = %d x %d x %d\n", shape[0],1,1);

    // Set values
    double storage_m = 0.526328;
    double storage_change_m = -0.000472;
    double *storage_m_ptr = &storage_m;
    double *storage_change_m_ptr = &storage_change_m;
    double smc_layers[] = {0.25, 0.15, 0.1, 0.12};

    int soil_moisture_profile_option;

    model.GetValue(var_name_smc_bmi,&soil_moisture_profile_option);
	
    model.SetValue(var_name_s,storage_m_ptr);

    model.SetValue(var_name_sc,storage_change_m_ptr);

    model.SetValue(var_name_smcl,&smc_layers[0]);
    
    var_s = (double *)model.GetValuePtr(var_name_s);
    var_sc = (double *)model.GetValuePtr(var_name_sc);

    std::cout<<"soil_moisture_profile_option: "<<soil_moisture_profile_option<<"\n";
    std::cout<<"storage: "<<*var_s<<"\n";
    std::cout<<"storage change: "<<*var_sc<<"\n";

    var_wt = (double *)model.GetValuePtr(var_name_wt);
    
    std::cout<<"water table: "<<*var_wt<<"\n";

    model.Update();
    
    // unit test
    //double SMCT[] ={0.322036, 0.33341, 0.367307, 0.439};
    double SMCT[] ={0.32207, 0.333438, 0.367336, 0.439};
   
    // Get values
    double *var_smc = new double[4];
    
    model.GetValue(var_name_smc,&var_smc[0]);
    
    enum option { Conceptual = 1, Layered = 2};
      
    if (soil_moisture_profile_option == Conceptual)
      for (int i=0; i < shape[0]; i++) {
	std::cout<<"Main: "<<var_smc[i]<<" "<<SMCT[i]<<" "<<abs(var_smc[i] - SMCT[i])<<"\n";
	assert (abs(var_smc[i] - SMCT[i]) < 1.E-6);     
	fprintf(fp, "%6.4e", var_smc[i]);
	fprintf(fp, "\n");
      }
    
  }
  
  fprintf(fp, "Finalizing... ");

  model.Finalize();
  fprintf(fp, "done\n");
  fclose(fp);
  return SUCCESS;
}

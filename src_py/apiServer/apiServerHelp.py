
API_SERVER_HELP_STR = """
__________NERLNET CHECKLIST__________
Nerlnet configuration files are located at config directory.
Make sure data and jsons in correct folder, and jsons include the correct paths
* Data includes: a single csv that includes all the data for the experiment (training and prediction phases)
* Jsons include: - distributed configuration (dc_<name>.json)
                 - connection map (conn_<name>.json)
                 - experiment flow (exp_<name>.json)
* Jsons directory: can be defined by changing the config file: config/jsonsDir.nerlconfig

____________API COMMANDS_____________
==========Setting experiment========

-showJsons():                                           lists available json files in jsons directory (dc, conn, exp) to be used with setJsons and getUserJsons
-list_datasets():                                       reads `hf_repo_ids.json` and list of datasets and files of Nerlnet organizaion on https://huggingface.co/Nerlnet
-download_dataset(idx, dir):                            downloads dataset files from Huggingface to the specified directory (default is /tmp/nerlnet/data/NerlnetData-master/nerlnet)
-add_repo_to_datasets_list(repo, name , description):   adds a repository to the datasets list in `hf_repo_ids.json`
-printArchParams(Num)                                   print description of selected arch file

-selectJsons():                                         get input from user for arch / conn / exp selection
-setJsons(arch, conn, exp):                             set selected jsons to get their path by getUserJsons
-getUserJsons():                                        return a tuple of 3 paths to dc, conn, exp jsons that is used for initialization

-initialization(experiment_name, dc, conn, exp_flow, custom_csv_path):  
                                                        setting up the api-server to communicate with main-server of Nerlnet cluster
                                                        dc - path to distributed configuration file (can be generated by Nerlplanner)
                                                        conn - path to connection map file, graph of connections between entities
                                                        exp - path to experiment flow file, defines the flow of the experiment demonstrated as experiment phases of training and prediction
                                                        custom_csv_path - optional, path to custom csv file for the experiment, overrides the one in experiment flow file
                                                        
-send_jsons_to_devices():                               send each NerlNet device the dc and conn jsons to init entities on it
-sendDataToSources(phase(,split)):                      phase := "training" | "prediction". split := 1 default (split) | 2 (whole file). send the experiment data to sources (currently happens in beggining of train/predict)

======== Running experiment ==========
-experiment_phase_is_valid()        returns True if there are more experiment phases to run
-run_current_experiment_phase()     runs the current experiment phase
-next_experiment_phase()            moves to the next experiment phase

======== Retrieving statistics ======
-get_experiment_flow(experiment_name).generate_stats()   returns statistics object (E.g., assigned to StatsInst) class for the current experiment phase
-StatsInst.get_communication_stats_workers()         returns communication statistics for workers
-StatsInst.get_communication_stats_sources()         returns communication statistics for sources
-StatsInst.get_communication_stats_clients()         returns communication statistics for clients
-StatsInst.get_communication_stats_routers()         returns communication statistics for routers
-StatsInst.get_communication_stats_main_server()     returns communication statistics for main server
-StatsInst.get_loss_ts()                             returns the loss over time
-StatsInst.get_min_loss()                            returns the minimum loss
-StatsInst.get_missed_batches()                      returns the missed batches

======== Workers Model Metrics and Performance ========
-StatsInst.get_confusion_matrices()                  returns tuple of two types of confusion matrices ordered by sources and ordered by workers
-StatsInst.get_model_performence_stats(confusion_matrix_worker_dict, saveToFile) returns the model performance statistics for the workers
"""
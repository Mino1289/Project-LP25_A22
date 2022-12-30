#include <stdio.h>
#include <stdint.h>

#include "global_defs.h"
#include "configuration.h"
#include "fifo_processes.h"
#include "mq_processes.h"
#include "direct_fork.h"
#include "reducers.h"
#include "utility.h"
#include "analysis.h"

#include <sys/msg.h>
#include <sys/select.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>


#include <dirent.h>

// Choose a method below by uncommenting ONLY one of the following 3 lines:
#if (defined(MQ))
#define METHOD_MQ
#elif (defined(DIRECT))
#define METHOD_DIRECT
#elif (defined(FIFO))
#define METHOD_FIFO
#else
#error "No method defined, please define one of the following: MQ, DIRECT, FIFO (compile with MQ=1, DIRECT=1 or FIFO=1)"
#endif

#ifdef METHOD_MQ
#if (defined(METHOD_DIRECT) || defined(METHOD_FIFO))
#error "Only one method may be defined (METHOD_MQ already defined)"
#endif
#endif

#ifdef METHOD_DIRECT
#ifdef METHOD_FIFO
#error "Only one method may be defined (METHOD_DIRECT already defined)"
#endif
#endif

int main(int argc, char *argv[]) {

    configuration_t config = {
            .data_path = "",
            .temporary_directory = "",
            .output_file = "",
            .is_verbose = false,
            .cpu_core_multiplier = 2,
    };
    
    make_configuration(&config, argv, argc);
    
    if (!is_configuration_valid(&config)) {
        printf("\nUsage: %s -d <data_path> -t <temporary_directory> -o <output_file> [-v] [-n <cpu_core_multiplier>] -f <config-file>\n", argv[0]);
        display_configuration(&config);
        printf("\nExiting\n");
        return -1;
    }

    config.process_count = 1;//get_nprocs() * config.cpu_core_multiplier;
    printf("Running analysis on configuration:\n");
    display_configuration(&config);
    print_msg(config, "\nPlease wait, it can take a while\n\n");

    system("rm -rf temp/*");
    FILE *f = fopen(config.output_file, "w");
    fclose(f);
    // Running the analysis, based on defined method:

#ifdef METHOD_MQ
    print_msg(config, "Running analysis using message queues\n");
    // Initialization
    int mq = make_message_queue();
    if (mq == -1) {
        printf("Could not create MQ, exiting\n");
        return -1;
    }
    pid_t *my_children = mq_make_processes(&config, mq);
	
    // Execution
    print_msg(config, "Processing directory\n");
    mq_process_directory(&config, mq, my_children);
    sync_temporary_files(config.temporary_directory);
    
    char temp_result_name[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step1_output", temp_result_name);
    
    print_msg(config, "Reducing files list\n");
    files_list_reducer(config.data_path, config.temporary_directory, temp_result_name);
    
    char step2_file[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step2_output", step2_file);

    print_msg(config, "Processing files\n");
    mq_process_files(temp_result_name, step2_file, config.process_count, mq, my_children);
    sync_temporary_files(config.temporary_directory);
    
    print_msg(config, "Reducing files\n");
    files_reducer(step2_file, config.output_file); // error here
        
    print_msg(config, "Cleaning up\n");
    print_msg(config, "Closing processes\n");
    close_processes(&config, mq, my_children);
    free(my_children);
    print_msg(config, "Closing message queue\n");
    close_message_queue(mq);

#endif

#ifdef METHOD_FIFO
    print_msg(config, "Running analysis using FIFOs\n");
    make_fifos(config.process_count, "fifo-in-%d");
    make_fifos(config.process_count, "fifo-out-%d");
    pid_t *children = make_processes(config.process_count);
    int *command_fifos = open_fifos(config.process_count, "fifo-in-%d", O_WRONLY);
    int *notify_fifos = open_fifos(config.process_count, "fifo-out-%d", O_RDONLY);
    fifo_process_directory(config.data_path, config.temporary_directory, notify_fifos, command_fifos, config.process_count);
    sync_temporary_files(config.temporary_directory);
    char fifo_temp_result_name[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step1_output", fifo_temp_result_name);
    files_list_reducer(config.data_path, config.temporary_directory, fifo_temp_result_name);
    fifo_process_files(config.data_path, config.temporary_directory, notify_fifos, command_fifos, config.process_count);
    sync_temporary_files(config.temporary_directory);
    char fifo_step2_file[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step2_output", fifo_step2_file);
    files_reducer(fifo_step2_file, config.output_file);
    shutdown_processes(config.process_count, command_fifos);
    close_fifos(config.process_count, command_fifos);
    close_fifos(config.process_count, notify_fifos);
    erase_fifos(config.process_count, "fifo-in-%d");
    erase_fifos(config.process_count, "fifo-out-%d");
#endif

#ifdef METHOD_DIRECT
    print_msg(config, "Running analysis using direct fork\n");
    
    print_msg(config, "Forking directories\n");
    direct_fork_directories(config.data_path, config.temporary_directory, config.process_count);
    
    print_msg(config, "Syncing temporary files\n");
    sync_temporary_files(config.temporary_directory);
    char direct_temp_result_name[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step1_output", direct_temp_result_name);
    
    print_msg(config, "Reducing files list\n");
    files_list_reducer(config.data_path, config.temporary_directory, direct_temp_result_name);
    
    char direct_step2_file[STR_MAX_LEN];
    concat_path(config.temporary_directory, "step2_output", direct_step2_file);

    print_msg(config, "Forking files\n");
    direct_fork_files(direct_temp_result_name, direct_step2_file, config.process_count);
   
    print_msg(config, "Syncing temporary files\n");
    sync_temporary_files(config.temporary_directory);
    
    print_msg(config, "Reducing files\n");
    files_reducer(direct_step2_file, config.output_file);
    
#endif

    print_msg(config, "Analysis finished\n");
    return 0;
}

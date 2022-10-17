#include <string.h>
#include <stdio.h>
#include <unistd.h>

void log_to_file(int time, int no_of_alerts_detected, int* no_of_msg_per_node) {
  /**
   * @brief      Output the result into result-tasknumber.txt file
   *
   * @details    Saves the wait, turnaround, deadline met informations.
   *
   * @param      time - The current time
   *
   * @param      current - the current index
   *
   * @param      deadline_met - boolean indicating whether deadline is met.
   *
   * @param      task_number - string task number result is from.
   */

  char *file_name = "logs.txt";

  int total_no_of_messages = 0;
  for (int i = 0; i < 3; i++) {
    total_no_of_messages += no_of_msg_per_node[i];
  }

  /***************************************************************************/
  /*                          Append to output file */
  /***************************************************************************/
  FILE *file = fopen(file_name, "a+");
  fprintf(file, "Simulation Time: %i seconds\n", time);
  fprintf(file, "Number of Alerts Detected: %i\n", no_of_alerts_detected);
  for (int i = 0; i < 3; i++)
    fprintf(file, "Number of Messages from P rank %i with neighbor: %i\n", i, no_of_msg_per_node[i]);
  fprintf(file, "Total Number of Messages: %i\n", total_no_of_messages);
  fclose(file);
}

/* int main(int argc, char *argv[]) */
/* { */
/*   int time = 1001; */
/*   int no_of_alerts_detected = 2312; */
/*   int no_of_msg_per_node[3] = {12, 12, 11}; */
/*   int total_no_of_messages = 15; */
/*   output_result(time, no_of_alerts_detected, no_of_msg_per_node, total_no_of_messages); */
/*   return 0; */
/* } */
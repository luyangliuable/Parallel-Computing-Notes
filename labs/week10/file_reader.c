#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int open_file(char *filename);

int *init_process_instruction_pointers(char *filename, int number_of_rows) {
  char buffer[1024];
  int current_position = 0;
  int file = open_file(filename);
  int *positions = malloc(sizeof(int) * number_of_rows);
  int n;
  int current_row = 0;

  /***************************************************************************/
  /*  Save all the locations of start lseek positions of every process data */
  /***************************************************************************/
  while ((n = read(file, buffer, sizeof(buffer))) > 0) {
    char *each_word = strtok(buffer, " ");

    /*************************************************************************/
    /*                     Save the start of the process */
    /*************************************************************************/

    int word_count = 1;

    current_position += strlen(each_word);

    while (each_word != NULL) {
      each_word = strtok(NULL, " \n");

      if (each_word == NULL) {
        break;
      }

      int which_value = word_count % 3;

      if (which_value == 0) {
        positions[current_row] = current_position;
        current_row++;
      };
      current_position += strlen(each_word) + 1;
      word_count++;
    };
  }

  return positions;
};

int open_file(char *filename) {
  /**
   * @brief      Open a file
   *
   * @details    If fail to open the fail output error and write to stderr.
   *
   * @param      filename The file's name
   *
   * @return     returns the value pointing to the opened file
   */
  int file;
  if ((file = open(filename, O_RDONLY)) < 0) {
    char err_msg[] = "Error opening file\n";
    write(2, err_msg, strlen(err_msg));
    exit(2);
  }

  return file;
}

float *fetch_next_instruction(int next_row, char *file_name) {
  float *result = malloc(sizeof(float) * 2);
  int n;
  int file = open_file(file_name);
  char buffer[1024];

  int next_line = next_row;
  lseek(file, next_line, SEEK_SET);

  while ((n = read(file, buffer, sizeof(buffer))) > 0) {
    char *each_word = strtok(buffer, " ");
    result[0] = atoi(each_word);
    int word_count = 1;

    while (each_word != NULL) {
      each_word = strtok(NULL, " \n");

      if (each_word == NULL) {
        break;
      }

      int which_value = word_count % 3;

      if (which_value == 1) {
        result[1] = atoi(each_word);
      } else if (which_value == 2) {
        result[2] = atoi(each_word);
        return result;
      }

      word_count++;
    }
  }

  return result;
}

int fetch_number_of_rows(char *file_name) {
  int file = open_file(file_name);
  char buff[1024];
  read(file, buff, sizeof(buff));
  return atoi(buff);
}

void write_number_of_rows_to_output(int num_of_rows) {
  char *file_name = "roots.txt";

  FILE *file = fopen(file_name, "w");

  fprintf(file, "%i\n", num_of_rows);

  fclose(file);
}

void output_result(float *roots) {
  /***************************************************************************/
  /*                             Create file name */
  /***************************************************************************/

  char *file_name = "roots.txt";

  FILE *file = fopen(file_name, "a+");
  fprintf(file, "%s    ", "x1");
  fprintf(file, "%s", "x2");
  if (roots[1] == 0 && roots[3] == 0) {
    /*************************************************************************/
    /*                            Non-complex root                           */
    /*************************************************************************/
    fprintf(file, "\n%.2f   ", roots[0]);
    fprintf(file, "%.2f\n", roots[2]);
  } else {
    fprintf(file, "    %s    ", "x1_real");
    fprintf(file, "%s    ", "x1_img");
    fprintf(file, "%s    ", "x2_real");
    fprintf(file, "%s\n", "x2_img");
    fprintf(file, "            %.2f       ", roots[0]);
    fprintf(file, "%.2f     ", roots[1]);
    fprintf(file, "%.2f     ", roots[2]);
    fprintf(file, "%.2f\n", roots[3]);
  }
  fclose(file);
}

/* int main(int argc, char *argv[]) { */
/*   float roots[4] = {1.2, 0, 2.2, 0}; */
/*   float roots2[4] = {1.2, 1, 2.2, -1}; */
/*   output_result(roots); */
/*   output_result(roots2); */
/*   return 0; */
/* } */

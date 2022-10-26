typedef struct {
  int year;                // A string that identifies the process
  int month;               // A string that identifies the process
  int day;                 // A string that identifies the process
  int hour;                // A string that identifies the process
  int minute;              // A string that identifies the process
  int second;              // A string that identifies the process
  double latitude;         // The time process entered system
  double longitude;        // The time process entered system
  double magnitude;        // The total CPU time required by the process int
  double depth;            // Remaining service time until completion
  int source_rank;            // The source ground node IP address.
  char *source;            // The source ground node IP address.
} seismic_reading;

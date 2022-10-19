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
  int no_of_messages[100]; // The number of messages in the node, max 100
                           // processes
  char *source;            // The source ground node IP address.
} seismic_reading;

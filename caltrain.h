#include <pthread.h>

struct station {
   int noOfEmptySeats;
   int noOfPassengersWaitingAtStation;
   int noOfPassengersInsideTrain;
    
    pthread_mutex_t mutex;     // Mutex for protecting the shared variables
    pthread_cond_t train_arrived;  // Signal passengers that train has arrived
    pthread_cond_t all_boarded;    // Signal train that all boarding is complete
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
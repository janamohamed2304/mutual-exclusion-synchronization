#include <pthread.h>
#include "caltrain.h"

void
station_init(struct station *station)
{
    station->noOfEmptySeats = 0;
    station->noOfPassengersWaitingAtStation = 0;
    station->noOfPassengersInsideTrain = 0;
    
    pthread_mutex_init(&station->mutex, NULL);
    pthread_cond_init(&station->train_arrived, NULL);
    pthread_cond_init(&station->all_boarded, NULL);
}

void
station_load_train(struct station *station, int count)
{
    pthread_mutex_lock(&station->mutex);
    
    // If no seats available or no waiting passengers, return immediately
    if (count == 0 || station->noOfPassengersWaitingAtStation == 0) {
        pthread_mutex_unlock(&station->mutex);
        return;
    }
    
    // Set available seats and notify waiting passengers
    station->noOfEmptySeats = count;
    pthread_cond_broadcast(&station->train_arrived);
    
    // Wait until either the train is full or all waiting passengers have boarded,
    // AND all passengers who started boarding have completed boarding
    while (station->noOfPassengersInsideTrain > 0 || 
           (station->noOfPassengersWaitingAtStation > 0 && station->noOfEmptySeats > 0)) {
        pthread_cond_wait(&station->all_boarded, &station->mutex);
    }
    
    // Train is leaving, reset seat count
    station->noOfEmptySeats = 0;
    
    pthread_mutex_unlock(&station->mutex);
}

void
station_wait_for_train(struct station *station)
{
    pthread_mutex_lock(&station->mutex);
    
    // Increment waiting passengers count
    station->noOfPassengersWaitingAtStation++;
    
    // Wait until a train arrives with free seats
    while (station->noOfEmptySeats == 0) {
        pthread_cond_wait(&station->train_arrived, &station->mutex);
    }
    
    // Train is here, passenger begins boarding
    station->noOfPassengersWaitingAtStation--;
    station->noOfPassengersInsideTrain++;
    station->noOfEmptySeats--;
    
    pthread_mutex_unlock(&station->mutex);
}

void
station_on_board(struct station *station)
{
    pthread_mutex_lock(&station->mutex);
    
    // Passenger has boarded and is seated
    station->noOfPassengersInsideTrain--;
    
    // Signal train to leave if appropriate conditions are met
    if (station->noOfPassengersInsideTrain == 0 && 
        (station->noOfEmptySeats == 0 || station->noOfPassengersWaitingAtStation == 0)) {
        pthread_cond_signal(&station->all_boarded);
    }
    
    pthread_mutex_unlock(&station->mutex);
}
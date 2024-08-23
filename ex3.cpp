#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

/**
 * Gengeric Unbounded queue, using mutex.
 * @tparam T
 */


// Unbounded queue
class UBQ : public std::queue<string>
{
private:
    sem_t mutex;
    sem_t full;

public:

    UBQ() {
        int x = sem_init(&mutex, 0, 1);
        if (x != 0) {
            perror("sem init failed!!");
        }
        x = sem_init(&full, 0, 0);
        if (x != 0) {
            perror("sem init failed!!");
        }
    }

    ~UBQ() {
        sem_destroy(&full);
        sem_destroy(&mutex);
    }

    void enqueue(string str)
    {
        sem_wait(&mutex);
        queue<string>::push(str);
        sem_post(&mutex);
        sem_post(&full);
    }

    string dequeue() {
        sem_wait(&full);
        sem_wait(&mutex);
        string last_str = this->front();
        queue<string>::pop();
        sem_post(&mutex);
        return last_str;
    }

    int isEmpty()
    {
        if(this->size() == 0) {
            return 1;
        }
        return 0;
    }
};

// Bounded queue

class BQ : public UBQ
{
private:
    sem_t empty;
public:
    BQ(int size) : UBQ() {
        int x = sem_init(&empty, 0, size);
        if (x != 0) {
            perror("sem init failed!!");
        }
    }

    ~BQ() {
        sem_destroy(&empty);
    }


    string dequeue()
    {
        string last_str = UBQ::dequeue();
        sem_post(&empty);
        return last_str;
    }
};

string category(int i){
    switch (i) {
        case 0:
            return "news";
        case 1:
            return "weather";
        case 2:
            return "sports";
    }
}



/// holding the data from config.txt
typedef struct {
    int producerId;
    unsigned int numOfProducts;
    int queueSize;
} Data;

//// global vars.
static vector<BQ*> proVec;
UBQ* sportsQ;
UBQ* weatherQ;
UBQ* newsQ;
BQ* mainQ;
vector<UBQ*> ubqVec;
vector<Data*> dataVec;
int PRODNUM;



/**
 * produce articles randomly.
 * @param data
 */
void *producerFunc(void* arg) {


    Data* data = (Data*) arg;
    BQ* boundedQ = proVec[data->producerId - 1];
    unsigned int articleNumber = 1, i = 0, articles = data->numOfProducts;
    while (i < articles) {
        boundedQ->enqueue("producer " + (to_string(data->producerId) + " " + category(rand()%3) + " " +to_string(articleNumber)));
        i++;
        articleNumber++;
        sleep(1);
    }
    boundedQ->enqueue("X");
    delete data;
}

/**
 * parse the config.txt by format.
 * @param configPath path of config.txt
 * @param CEQsize - pointer to int, holding the co editor queue size.
 * @param dataVec vector of data struct
 */
 void parseConfig(char * configPath, int *CEQsize, vector<Data*>& dataVec) {


     char buf[10];
     char buf2[10];
     char buf3[10];
     char dummy[10];
     ifstream config;
     config.open(configPath);
     if (!config.is_open()) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
     }
     Data *d = new Data;
     while (!config.eof()) {
         d = new Data;
         config.getline(buf, 10);
           if (!config.eof()) {
             config.getline(buf2, 10);
             if (buf2[0] == NULL){
                 break;
             }
             config.getline(buf3, 10);
             d->producerId = atoi(buf);
             d->numOfProducts = atoi(buf2);
             d->queueSize = atoi(buf3);
             dataVec.push_back(d);
             config.getline(dummy, 10);
           }

     }
     *CEQsize = (atoi(buf));
     config.close();
    }

/**
 * sort the producer products into relvant Q.
 */

void *dispatcherFunc(void* arg) {

    int i = 0;
    int j;
    string s;
    while (1) {
        for (j = 0; j < PRODNUM; j++) {
            if (proVec[j] == NULL) continue;
            s = proVec[j]->dequeue();
            if (s.find("X") != std::string::npos) {
                delete (proVec[j]);
                proVec[j] = NULL;
                i++;

            } else { break; }
        }
        if (i == PRODNUM) {
            break;
        }
        if (s.find("sports") != std::string::npos) {
            sportsQ->enqueue(s);
        } else if (s.find("weather") != std::string::npos) {
            weatherQ->enqueue(s);
        } else if (s.find("news") != std::string::npos) {
            newsQ->enqueue(s);
        }
    }
    sportsQ->enqueue("X");
    weatherQ->enqueue("X");
    newsQ->enqueue("X");
}



/**
 * print into screen
 */

void* screenManagerFunc(void* arg){
int count = 0;
while(1){
    if (count == 3){
        cout << "DONE" << endl;
        delete mainQ;
        break;
    }
    string s = mainQ->dequeue();
    if (s.find("DONE") != string::npos){
        count++;
        continue;
    }
    else if (s.find("DONE") == string::npos) {
        cout << s << endl;

    }
}
}


/**
 * push from sportsQ, newsQ, weatherQ to the mainQ.
 * @param num - kind of Q.
 */
void *coEditorFunc(void *num){
    long id;
    id = (long)num;
    while(1) {
        UBQ* ptr = ubqVec[id];
        string s = ptr->dequeue();
        if (s.find("X") != std::string::npos){
            delete ptr;
            ubqVec[id] = NULL;
            mainQ->enqueue("DONE");
            break;
        }
        mainQ->enqueue(s);
    }
}



int main(int argc, char *argv[]) {
    int x, k;
    if (argc != 2) {
        perror("error with args");
        return -1;
    }
    int CoEditorQueueSize = 0;
    parseConfig(argv[1], &CoEditorQueueSize, dataVec);
    unsigned size = dataVec.size();
    PRODNUM = size;
    for (k=0; k<size ; k++){
        proVec.push_back(new BQ(dataVec[k]->queueSize)); // free at dispatcher
    }

    mainQ = new BQ(CoEditorQueueSize); // free at screen
    sportsQ = new UBQ();// free at coeditor
    weatherQ = new UBQ();// free at coeditor
    newsQ = new UBQ(); // free at coeditor
    ubqVec.push_back(sportsQ);
    ubqVec.push_back(weatherQ);
    ubqVec.push_back(newsQ);

    /// producers.
    pthread_t thread[size + 5];
    for (k = 0; k < size; k++) {
        x = pthread_create(&thread[k], NULL, producerFunc, dataVec[k]);
        if (x < 0) {
            perror("thread create failed\n");
        }
    }

    /// dispatcher.
    x = pthread_create(&thread[size], NULL, dispatcherFunc, NULL);
    if (x < 0) {
        perror("thread create failed\n");
        return -1;
    }

    /// co-editor
    x = pthread_create(&thread[size + 1], NULL, coEditorFunc, (void *) 0);
    if (x < 0) {
        perror("thread create failed\n");
        return -1;
    }
    x = pthread_create(&thread[size + 2], NULL, coEditorFunc, (void *) 1);
    if (x < 0) {
        perror("thread create failed\n");
        return -1;
    }
    pthread_join(thread[size + 2], NULL);
    x = pthread_create(&thread[size + 3], NULL, coEditorFunc, (void *) 2);
    if (x < 0) {
        perror("thread create failed\n");
        return -1;
    }

    /// screen manager
    x = pthread_create(&thread[size + 4], NULL, screenManagerFunc, (void *) 0);
    if (x < 0) {
        perror("thread create failed\n");
    }

    // Join threads
    for (k = 0 ; k<size+5 ;k++){
        pthread_join(thread[k], NULL);
    }

    // Free allocated memory
    for (k=0; k<size ; k++){
        delete proVec[k];
    }
    delete mainQ;
    delete sportsQ;
    delete weatherQ;
    delete newsQ;

    return 0;
}
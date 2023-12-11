/*
Meri Khurshudyan
CSCI 340 222
Project 2
*/

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

using namespace std;
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int threadsCompleted = 0;
pthread_mutex_t arrayMutex;
int parts[4][5];
int parts2[2][10];
int MasterArr[] = {1, 20, 5, 13, 22, 10, 27, 9, 24, 98, 7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
// reuse same class from project to parition array based on lower and upper bounds
class MyParam
{
public:
    MyParam(int l, int u, int i) : lb(l), ub(u), lid(i){};
    int getLower() { return lb; }
    int getUpper() { return ub; }
    int getLid() { return lid; }

private:
    int lb, ub, lid;
};

///////////////////////////////////////////////////////HELPER FUNCTIONS///////////////////////////////////////////////////////////////////////////
// simple bubblesort
void bubbleSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] < arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
// Merge based on highest value first order
int *merge(int arr1[], int arr2[], int n1, int n2)
{
    int *arrs = new int[n1 + n2];
    int pos1 = 0;
    int pos2 = 0;
    for (int i = 0; i < (n1 + n2); i++)
    {
        if (pos1 == n1)
        {
            arrs[i] = arr2[pos2];
            pos2++;
        }
        else if (pos2 == n2)
        {
            arrs[i] = arr1[pos1];
            pos1++;
        }
        else if (arr1[pos1] > arr2[pos2])
        {
            arrs[i] = arr1[pos1];
            pos1++;
        }
        else
        {
            arrs[i] = arr2[pos2];
            pos2++;
        }
    }

    return arrs;
}

///////////////////////////////////////////////////////RUNNER(S)///////////////////////////////////////////////////////////////////
void *myrunner(void *param) // first runner to sort the arrays
{
    MyParam *p = (MyParam *)param;
    int lower = p->getLower();
    int upper = p->getUpper();
    int indx = p->getLid();
    int *arr = new int[5];
    int i = 0;
    while (lower <= upper) // separate the array into 4 parts
    {
        arr[i] = MasterArr[lower];
        lower++;
        i++;
    }

    bubbleSort(arr, i); // sort

    for (int j = 0; j < 5; j++) // push into global scope
    {
        parts[indx][j] = arr[j];
    }
    string ms = ""; // simple print out message
    for (int i = 0; i < 5; i++)
    {
        ms += to_string(parts[indx][i]) + " ";
    }
    string msg = "Located in thread: " + std::to_string(indx) + "\n" + "Array is: " + ms + "\n";
    write(STDOUT_FILENO, msg.c_str(), 45);
    delete p; // delete instance of myParam
    pthread_exit(0);
}

void *myrunner2(void *param) // runner function to merge the 4 sorted arrays
{
    MyParam *m = (MyParam *)param; // keep
    int indx = m->getLid();
    int i1 = m->getLower(); // here lower boundary represents the array of array that will be merged
    int i2 = m->getUpper(); // with this other array of array Array[mainArray][subarray]

    if (indx == 6) // last thread is saved to merge the final two arrays based on index
    {
        pthread_mutex_lock(&mute);
        while (threadsCompleted < 2)
        {
            pthread_cond_wait(&condition, &mute);
        }
        pthread_mutex_unlock(&mute); // when condition has met allow index 6 to proceed
        int *arr = new int[20];      // final final array is not stored just printed
        arr = merge(parts2[0], parts2[1], 10, 10);
        string msg = "";
        for (int i = 0; i < 20; i++)
        {
            msg += to_string(arr[i]) + " ";
        }
        string ms = "Located in thread: " + to_string(6) + "\n" + "Array is: " + msg + "\n";
        write(STDOUT_FILENO, ms.c_str(), 83);
    }
    else // first two merge threads go here
    {
        int *arr = new int[10];
        arr = merge(parts[i1], parts[i2], 5, 5); // parts is a global array containing the 4 sorted arrays
        int k;
        if (indx == 4) // if we are in the 4th thread then we want to place the resulting merged array into position 0
        {
            k = 0;
        }
        else
        {
            k = 1;
        }
        pthread_mutex_lock(&mute); // lock shared array
        for (int i = 0; i < 10; i++)
        {
            parts2[k][i] = arr[i]; // another global array to store the 2 final arrays
        }
        string msg = "";
        for (int i = 0; i < 10; i++)
        {
            msg += to_string(parts2[k][i]) + " ";
        }
        string ms = "Located in thread: " + to_string(indx) + "\n" + "Array is: " + msg + "\n";
        write(STDOUT_FILENO, ms.c_str(), 59);
        threadsCompleted++;
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mute); // count to 2 and unlock
    }
    delete (m);
    pthread_exit(0);
}
///////////////////////////////////////////////////////////MAIN////////////////////////////////////////////////////////////

int main()
{
    MyParam *p[4]; // the first set of threads
    p[0] = new MyParam(0, 4, 0);
    p[1] = new MyParam(5, 9, 1);
    p[2] = new MyParam(10, 14, 2);
    p[3] = new MyParam(15, 19, 3);

    // Create the pthreads
    pthread_t tid[4];
    pthread_attr_t attr[4];

    for (int i = 0; i < 4; ++i)
    {
        pthread_attr_init(&(attr[i]));
        pthread_create(&(tid[i]), &(attr[i]), myrunner, p[i]);
    }

    // wait for pthreads to finish
    for (int i = 0; i < 4; ++i)
    {
        pthread_join(tid[i], NULL);
    }

    // ***************************Start next set of threads to merge the created global arrays****************************************

    MyParam *m[3]; // second set of threads

    m[0] = new MyParam(0, 1, 4); // reuse the same class structure
    m[1] = new MyParam(2, 3, 5);
    m[2] = new MyParam(0, 1, 6);

    pthread_t tid1[3];
    pthread_attr_t attr1[3];

    for (int i = 0; i < 3; ++i)
    {
        pthread_attr_init(&(attr1[i]));
        pthread_create(&(tid1[i]), &(attr1[i]), myrunner2, m[i]);
    }
    // wait for pthreads to finish
    for (int i = 0; i < 3; ++i)
    {
        pthread_join(tid1[i], NULL);
    }
}
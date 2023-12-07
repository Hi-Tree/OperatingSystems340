/*
Meri Khurshudyan
CSCI 340 222
Project 2
*/

#include<pthread.h>
#include<iostream>

using namespace std;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int parts[2][10];
int MasterArr[]={1,20,5,13,22,10,27,9,24,98,7,12,19,3,18,4,2,6,15,8};
//reuse same class from project to parition array based on lower and upper bounds
class MyParam {

public:
  MyParam(int l, int u, int i, int t) :   lb(l), ub(u), lid(i), trd(t) { };
  int getLower() { return lb;}
  int getUpper() { return ub;}
  int getLid()   { return lid;}
  int getThrd()  { return trd;}

private:
  int lb, ub, lid, trd;
};

///////////////////////////////////////////////////////HELPER FUNCTIONS///////////////////////////////////////////////////////////////////////////

void bubbleSort(int arr[], int n){
    for(int i=0; i<n-1; i++){
        for(int j=0; j<n-i-1; j++){
          if(arr[j]<arr[j+1]){
            int temp = arr[j];
            arr[j] = arr[j+1];
            arr[j+1]=temp;
          }
        }
    }
}

int *merge(int arr1[], int arr2[], int n1, int n2){
    int *arr=new int[n1+n2];
    int pos1 = 0;
    int pos2 = 0;
    for(int i =0;i<(n1+n2); i++){
        if(pos1==n1){
            arr[i]=arr2[pos2];
            pos2++;
        }
        else if(pos2==n2){
            arr[i]=arr1[pos1];
            pos1++;
        }
        else if(arr1[pos1]>arr2[pos2]){
            arr[i]=arr1[pos1];
            pos1++;
        }
        else{
            arr[i]=arr2[pos2];
            pos2++;
        }
    }
    return arr;
}

///////////////////////////////////////////////////////RUNNER///////////////////////////////////////////////////////////////////
void *myrunner(void *param){
    MyParam *p = (MyParam *) param;
    int lower = p->getLower();
    int upper = p->getUpper();
    int indx = p->getLid();
    int thread = p->getThrd();
    int *arrz = new int[10];
    int *arr=new int[upper-lower];
    int i =0;
    while(lower<=upper){
        arr[i]=MasterArr[lower];
        lower++;
        i++;
    }
    bubbleSort(arr,i);
    for(int j = 0; j<10; j++){
        parts[indx][j]=arr[j];
    }
    if(thread==0){
        pthread_cond_signal(&cond);
    }
    else if(thread==1){
        pthread_cond_wait(&cond, &mutex);
        arrz=merge(parts[0],parts[1],10,10);
    }
    for(int i=0; i<20; i++){
        cout<<arrz[i];
    }
    // cout<<"end1"<<endl;
    pthread_exit(0);
}

///////////////////////////////////////////////////////////MAIN////////////////////////////////////////////////////////////

int main(){
    //i
    // int arr1[]={50,4,3,3};
    // int arr2[]={10,6,5,0};

    // int n1 = sizeof(arr1)/sizeof(arr1[0]);
    // int n2 = sizeof(arr2)/sizeof(arr2[0]);
    // int *arr = merge(arr1,arr2,n1,n2);
    // for(int i=0; i<n1+n2; i++){
    //     cout<<arr[i];
    // }
    MyParam *p[4];
    p[0] = new MyParam(0,9,0,0);
    p[1] = new MyParam(10,19,1,1);

    // Create the pthreads
    pthread_t tid[2];
    pthread_attr_t attr[2];

    for (int i = 0; i < 2; ++i) {
    pthread_attr_init(&(attr[i]));
    pthread_create(&(tid[i]), &(attr[i]), myrunner, p[i]);
    cout << "Created thread\n";
    }

    // wait for pthreads to finish 
    for (int i = 0; i < 2; ++i) {
    pthread_join(tid[i], NULL);
    cout << "Joined thread\n"; 
    }

    for(int i=0; i<10; i++){
        cout<<parts[0][i]<<",";
    }
}
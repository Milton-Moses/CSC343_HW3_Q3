#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

int main()
{
    const int SIZE = 20;
    int arr[SIZE];
    int fd[2]; // Pipe file descriptors [cite: 949]
    // Initialize random seed and fill array
    srand(time(0));
    cout << "Array: ";
    for (int i = 0; i < SIZE; i++)
    {
        arr[i] = rand() % 100;
        cout << arr[i] << " ";
    }
    cout << endl;

    if (pipe(fd) == -1)
        return 1; // Create pipe

    pid_t id = fork(); // Create child process [

    if (id < 0)
        return 2;

    if (id == 0)
    {
        // Child Process: Second half (indices 10-19)
        close(fd[0]);
        int childMin = arr[10];
        for (int i = 11; i < SIZE; i++)
        {
            if (arr[i] < childMin)
                childMin = arr[i];
        }
        cout << "Child (PID: " << getpid() << ") found min in 2nd half: " << childMin << endl;
        write(fd[1], &childMin, sizeof(childMin)); // Send to parent
        close(fd[1]);
        exit(0);
    }
    else
    {
        // Parent Process: First half (indices 0-9)
        close(fd[1]);
        int parentMin = arr[0];
        for (int i = 1; i < 10; i++)
        {
            if (arr[i] < parentMin)
                parentMin = arr[i];
        }
        cout << "Parent (PID: " << getpid() << ") found min in 1st half: " << parentMin << endl;

        int receivedMin;
        read(fd[0], &receivedMin, sizeof(receivedMin)); // Read from child
        close(fd[0]);
        wait(NULL); // Wait for child

        int finalMin = (parentMin < receivedMin) ? parentMin : receivedMin;
        cout << "Final minimum of the array: " << finalMin << endl;
    }
    return 0;
}
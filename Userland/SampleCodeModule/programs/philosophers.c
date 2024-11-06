#include <syscall.h>
#include <libc.h>
#include <processes.h>

#define MAX_PHYLOS 15
#define MIN_PHYLOS 5
#define PHYLO_BUFFER_SIZE 3

#define MUTEX_ID 16
#define SEM_ID 17

#define LEFT(i) ((phyloId + phylosCount - 1) % phylosCount)
#define RIGHT(i) ((phyloId + 1) % phylosCount)

typedef enum{
    THINKING,
    HUNGRY,
    EATING
} state_t;

int phylosCount = 0;

state_t state[MAX_PHYLOS] = {THINKING};
int mutex;                   
int s[MAX_PHYLOS];           
int philosopherPids[MAX_PHYLOS] = {0};

void takeForks(int phyloId);
void putForks(int phyloId);
void test(int phyloId);
void think();
void eat();
void render();

int philosopher(int argc, char *argv[]);
void addPhilosopher();
void removePhilosopher();

void startDining() {
    char c;

    printf("Welcome to the Dining Philosophers\n");
    printf("Commands: (A)dd, (R)emove, (Q)uit\n");

    while ((c = readchar()) != 'Q') {
        if(c == 'A'){
            printf("Adding philosopher\n");
            addPhilosopher();
        }
        if(c == 'R'){
            printf("Removing philosopher\n");
            removePhilosopher();
        }
        
    }

    for (int i = phylosCount - 1; i >= 0; i--) {
        syskillProcess(philosopherPids[i]);
    }
    syssemClose(mutex);
    return;
}

void addPhilosopher() {
    if (phylosCount >= MAX_PHYLOS) {
        printf("Maximum philosophers reached\n");
        return;
    }

    syswait(mutex);
    state[phylosCount] = THINKING;
    char phyloBuff[PHYLO_BUFFER_SIZE] = {0};

    s[phylosCount] = syssemInit(phylosCount, 0);  

    itoa(phylosCount, phyloBuff, 10); 
    char *params[] = {"philosopher", phyloBuff};
    int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};

    philosopherPids[phylosCount] = syscreateProcess((uint64_t)philosopher, params, 2, 1, fileDescriptors, 1);
    if(philosopherPids[phylosCount] < 0){
        printf("Error creating philosopher\n");
        sysexit();
        return;
    }
    if(sysunblockProcess(philosopherPids[phylosCount]) == -1){
        sysexit();
        return;
    }

    phylosCount++;
    //render();
    syspost(mutex);
}

void removePhilosopher() {
    if (phylosCount <= MIN_PHYLOS) {
        printf("Minimum philosophers reached\n");
        return;
    }

    syswait(mutex);
    phylosCount--;
    syskillProcess(philosopherPids[phylosCount]);
    syssemClose(s[phylosCount]);

    render();
    syspost(mutex);
}

int philosopher(int argc, char *argv[]) {
    int i = atoi(argv[1]);
    while (1) {
        think();
        takeForks(i);
        eat();
        putForks(i);
    }
    return 0;
}

void takeForks(int phyloId) {
    syswait(mutex);
    state[phyloId] = HUNGRY;
    test(phyloId);
    syspost(mutex);
    syswait(s[phyloId]);  
}

void putForks(int phyloId) {
    syswait(mutex);
    state[phyloId] = THINKING;
    test(LEFT(phyloId));
    test(RIGHT(phyloId));
    syspost(mutex);
}

void test(int phyloId) {
    if (state[phyloId] == HUNGRY && state[LEFT(phyloId)] != EATING && state[RIGHT(phyloId)] != EATING) {
        state[phyloId] = EATING;
        syspost(s[phyloId]);
    }
    render();
}

void render() {
    for (int i = 0; i < phylosCount; i++) {
        printf(state[i] == EATING ? "E " : ". ");
    }
    putchar('\n');
}

void think() {
    for(int i = 0; i < 5000; i++);
}

void eat() {
    for(int i = 0; i < 5000; i++);
}

void phylo(int argc, char *argv[]) {
    if(argc != 2){
        printf("You must insert ONE parameter indicating the amount of philosophers you desire to start with\n");
        sysexit();
        return;
    }
    int aux = atoi(argv[1]);
    if(aux < MIN_PHYLOS || aux > MAX_PHYLOS){
        printf("The amount of philosophers must be between %d and %d\n", MIN_PHYLOS, MAX_PHYLOS);
        sysexit();
        return;
    }
    if((mutex = syssemInit(MUTEX_ID, 1)) == -1){
        printf("Couldn't create semaphore mutex\n");
        sysexit();
        return;
    }

    for (int i = 0; i < aux; i++) {
        addPhilosopher();
    }

    startDining();
    sysexit();
    return;
}

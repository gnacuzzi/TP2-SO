#include <syscall.h>
#include <libc.h>
#include <processes.h>

#define MAX_PHYLOS 15
#define MIN_PHYLOS 5
#define PHYLO_BUFFER_SIZE 3

#define MUTEX_ID 16
#define PRINT_ID 17

#define LEFT(phyloId) ((phyloId + phylosCount - 1) % phylosCount)
#define RIGHT(phyloId) ((phyloId + 1) % phylosCount)

typedef enum{
    THINKING,
    HUNGRY,
    EATING
} state_t;

int phylosCount = 0;

state_t state[MAX_PHYLOS] = {THINKING};       
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
            syswait(PRINT_ID);
            printf("Adding philosopher\n");
            syspost(PRINT_ID);
            addPhilosopher();
        }
        if(c == 'R'){
            syswait(PRINT_ID);
            printf("Removing philosopher\n");
            syspost(PRINT_ID);
            removePhilosopher();
        }
    }

    for (int i = 0; i < phylosCount; i++) {
        if(syskillProcess(philosopherPids[i]) == -1){
            printf("Error killing philosopher %d\n", i);
            sysexit();
            return;
        }
        if(syssemClose(i) == -1){
            printf("Error closing semaphore %d\n", i);
            sysexit();
            return;
        }    
    }
    if(syssemClose(MUTEX_ID) == -1){
        printf("Error closing semaphpore mutex\n");
        sysexit();
        return;
    }
    if(syssemClose(PRINT_ID) == -1){
        printf("Error closing semaphpore print\n");
        sysexit();
        return;
    }
    return;
}

void addPhilosopher() {
    if (phylosCount >= MAX_PHYLOS) {
        printf("Maximum philosophers reached\n");
        return;
    }

    syswait(MUTEX_ID);
    state[phylosCount] = THINKING;
    char phyloBuff[PHYLO_BUFFER_SIZE] = {0};

    if(syssemInit(phylosCount, 0) == -1){
        printf("Error creating semaphore %d\n", phylosCount);
        sysexit();
        return;
    }

    itoa(phylosCount, phyloBuff, 10); 
    char *params[] = {"philosopher", phyloBuff};
    int16_t fileDescriptors[] = {STDIN, STDOUT, STDERR};

    philosopherPids[phylosCount] = syscreateProcess((uint64_t)philosopher, params, 2, 1, fileDescriptors, 1);
    if(philosopherPids[phylosCount] < 0){
        printf("Error creating philosopher %d\n", phylosCount);
        sysexit();
        return;
    }
    if(sysunblockProcess(philosopherPids[phylosCount]) == -1){
        sysexit();
        return;
    }

    phylosCount++;
    syspost(MUTEX_ID);
}

void removePhilosopher() {
    if (phylosCount <= MIN_PHYLOS) {
        printf("Minimum philosophers reached\n");
        return;
    }

    syswait(MUTEX_ID);
    syskillProcess(philosopherPids[phylosCount]);
    syssemClose(phylosCount);
    phylosCount--;
    syspost(MUTEX_ID);
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
    syswait(MUTEX_ID);
    state[phyloId] = HUNGRY;
    test(phyloId);
    syspost(MUTEX_ID);
    syswait(phyloId);  
}

void putForks(int phyloId) {
    syswait(MUTEX_ID);
    state[phyloId] = THINKING;
    test(LEFT(phyloId));
    test(RIGHT(phyloId));
    syspost(MUTEX_ID);
}

void test(int phyloId) {
    if (state[phyloId] == HUNGRY && state[LEFT(phyloId)] != EATING && state[RIGHT(phyloId)] != EATING) {
        state[phyloId] = EATING;
        syspost(phyloId);
    }
    render();
}

void render() {
    syswait(PRINT_ID);
    for (int i = 0; i < phylosCount; i++) {
        printf(state[i] == EATING ? "E " : ". ");
    }
    putchar('\n');
    syspost(PRINT_ID);
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
    if(syssemInit(MUTEX_ID, 1) == -1){
        printf("Error creating semaphore mutex\n");
        sysexit();
        return;
    }
    if(syssemInit(PRINT_ID, 1) == -1){
        printf("Error creating semaphore print\n");
        sysexit();
        return;
    }

    for (int i = 0; i < aux; i++) {
        addPhilosopher();
    }

    startDining();
    phylosCount = 0;
    sysexit();
    return;
}

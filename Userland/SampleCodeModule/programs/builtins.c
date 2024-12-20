// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <builtins.h>
#include <syscall.h>
#include <libc.h>

void memState(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Mem doesn't need parameters\n");
		return;
	}

	mem_info memInfo;

	if (sysgetMemInfo(&memInfo) == -1) {
		printf("Couldn't retrive memory information\n");
		return;
	}

	printf("Used memory: %d bytes\n", (int) memInfo.used);
	printf("Free memory: %d bytes\n", (int) memInfo.free);
	printf("Total memory: %d bytes\n", (int) memInfo.total);

	return;
}

/* Process Managing */

void killProcess(int argc, char *argv[]) {
	if (argc != 1) {
		printf("You must insert ONE parameter indicating the PID of the process you desire to kill\n");
		return;
	}

	int pid = atoi(argv[0]);

	if (pid <= 1) {
		printf("PID must be a number greater than 1\n");
		return;
	}

	int out = syskillProcess(pid);
	printf("Process %d %s\n", pid, out == 0 ? "killed" : "couldn't be killed");
	return;
}

void changePriority(int argc, char *argv[]) {
	if (argc != 2) {
		printf("You must insert TWO parameters indicating the PID of the process you desire to change the priority and "
			   "the new priority\n");
		return;
	}

	int pid = atoi(argv[0]);
	int priority = atoi(argv[1]);

	if(pid == 1  || pid == 0){
		printf("You can't change the priority of the idle process or the shell\n");
		return;
	}

	if (pid < 0) {
		printf("PID must be a number greater than 0\n");
		return;
	}

	if (priority < 1 || priority > 5) {
		printf("Priority must be a number between 0 and 5\n");
		return;
	}

	int out = syschangePriority(pid, priority);

	if (out == -1) {
		printf("Process %d not found\n", pid);
		return;
	}

	printf("Priority of process %d %s\n", pid, out >= 0 ? "changed" : "not changed");

	return;
}

void blockProcess(int argc, char *argv[]) {
	if (argc != 1) {
		printf("You must insert ONE parameter indicating the PID of the process you desire to block\n");
		return;
	}

	int pid = atoi(argv[0]);

	if (pid < 0) {
		printf("PID must be a number greater than 0\n");
		return;
	}

	int out = sysblockProcess(pid);

	printf("Process %d %s\n", pid, out == 0 ? "blocked" : "couldn't be blocked");

	return;
}

void unblockProcess(int argc, char *argv[]) {
	if (argc != 1) {
		printf("You must insert ONE parameter indicating the PID of the process you desire to unblock\n");
		return;
	}

	int pid = atoi(argv[0]);

	if (pid <= 1) {
		printf("PID must be a number greater than 1\n");
		return;
	}

	int out = sysunblockProcess(pid);

	printf("Process %d %s\n", pid, out == 0 ? "unblocked" : "couldn't be unblocked");

	return;
}

static void printProcInfo(PSinfo proc) {
	putchar('\n');
	printf("NAME: %s\n", proc.name == NULL ? "unnamed" : proc.name);

	printf("PID: %d\n", (int) proc.pid);

	char *status = NULL;
	if (proc.status == 0)
		status = "BLOCKED";
	else if (proc.status == 1)
		status = "READY";
	else if (proc.status == 2)
		status = "RUNNING";
	else if (proc.status == 3)
		status = "KILLED";
	else
		status = "UNKNOWN";

	printf("Priority: %d | Stack base: 0x%d | Stack pointer: 0x%d | Ground: %s\n | Status: %s\n", (int) proc.priority,
		   (int) proc.stackBase, (int) proc.stackPos, proc.ground == 0 ? "foreground" : "background", status);
}

void listProcesses(int argc, char *argv[]) {
	if (argc != 0) {
		printf("Ps doesn't need parameters\n");
		return;
	}

	uint16_t procAmount;

	PSinfo *processes = sysps(&procAmount);

	if (procAmount == 0 || processes == NULL) {
		printf("No processes found\n");
		return;
	}

	printf("There are %d processes:\n", procAmount);

	for (int i = 0; i < procAmount; i++) {
		printProcInfo(processes[i]);
	}

	for (int i = 0; i < procAmount; i++) {
		sysfree(processes[i].name);
	}

	sysfree(processes);
}

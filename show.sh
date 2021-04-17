echo "Memory"
ipcs -tm | grep "$(whoami)"

echo "Semaphors"
ipcs -ts | grep "$(whoami)"

echo "Files of semaphors"
find /dev/shm -user "$(whoami)"

echo "Processes"
ps ux
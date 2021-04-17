echo "Memory:"
ipcs -tm | grep "$(whoami)"

echo
echo "Semaphors:"
ipcs -ts | grep "$(whoami)"

echo
echo "Files of semaphors:"
find /dev/shm -user "$(whoami)"

echo
echo "Processes:"
ps ux
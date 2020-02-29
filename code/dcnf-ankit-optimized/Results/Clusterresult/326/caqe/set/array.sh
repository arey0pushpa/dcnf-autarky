#!/bin/sh
#SBATCH -J ca16
#SBATCH -c 2
#SBATCH -a 1-326
#SBATCH -o /dev/null
. ./config.sh
if [ x"$SLURM_ARRAY_TASK_ID" = x ]
then
  path=`head -1 benchmarks|awk '{print $2}'`
  name=`head -1 benchmarks|awk '{print $3}'`
  SLURM_ARRAY_TASK_ID=0
else
  path="`awk '$1 == "'$SLURM_ARRAY_TASK_ID'"{print $2}' benchmarks`"
  name="`awk '$1 == "'$SLURM_ARRAY_TASK_ID'"{print $3}' benchmarks`"
  binary=./binary
  exec 1>"$name".log 2>"$name".err
fi
echo "c array.sh: path:  $path"
echo "c array.sh: name:  $name"
echo "c array.sh: task:  $SLURM_ARRAY_TASK_ID"
echo "c array.sh: host:  `hostname`"
echo "c array.sh: start: `date`"
/home/biere/bin/runlim \
  --time-limit="$time" \
  --real-time-limit="$time" \
  --space-limit="$space" \
  "$binary" \
  $options \
  "$path"
echo "c array.sh: end `date`"

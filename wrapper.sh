#!/bin/bash

available_mutations=(
ArgErrorNullBufferMutation
ArgErrorCommunicator1Mutation
ArgErrorCommunicator2Mutation
ArgMismatchCommunicatorMutation
ArgErrorCount1Mutation
ArgErrorCount2Mutation
ArgErrorRank1Mutation
ArgErrorRank2Mutation
ArgErrorNullRequestMutation
ArgErrorTag1Mutation
ArgErrorTag2Mutation
ArgMismatchTagMutation
ArgErrorType1Mutation
ArgErrorType2Mutation
ArgMismatchTypeMutation
Deadlock1Mutation
MpiCallBeforeInitMutation
ModifyBufferDuringIsendMutation
MissingCallMpiFinalizeMutation
MissingCallMpiRecvMutation
MissingCallMpiSendMutation
MissingCallMpiWaitMutation
MissingCallCollectiveMutation
ArgErrorRoot1Mutation
ArgErrorRoot2Mutation
ArgErrorOp1Mutation
ArgErrorOp2Mutation
ArgMismatchRootMutation
ArgMismatchCountMutation
ArgMismatchOpMutation
MisplacedCallBarrierMutation
MisplacedCallBarrier2Mutation
ArgMismatchIrecvBufferOverlapMutation
)

only_list=false
max=100
compilation_database=false
src_dir=false
tool_path='./build/MPIMutationTool'
output_dir='./patches'
with_headers=false

while getopts "lm:c:s:t:o:h" arg; do
  case $arg in
    l)
      only_list=true
      ;;
    m)
      max="$OPTARG"
      ;;
    c)
      compilation_database="$OPTARG"
      ;;
    s)
      src_dir="$OPTARG"
      ;;
    t)
      tool_path="$OPTARG"
      ;;
    o)
      output_dir="$OPTARG"
      ;;
    h)
      with_headers=true
      ;;
  esac
done


shift "$((OPTIND - 1))"
DIR="$1"

tool_path=$(realpath $tool_path)
output_dir=$(realpath $output_dir)

if [ "$src_dir" = "false" ]
then
  src_dir=./
else
  src_dir=$(realpath $src_dir)
fi

echo $src_dir

if [ "$compilation_database" = "false" ]
then
  compilation_database=./
else
  compilation_database=$(realpath $compilation_database)
fi

cd "$DIR"

declare -A stats

if $with_headers
then
  files=`find "$src_dir" -iname '*.c' -o -iname '*.cpp' -o -iname '*.cc' -o -iname '*.cxx' -o -iname '*.h' -o -iname '*.hpp'`
else
  files=`find "$src_dir" -iname '*.c' -o -iname '*.cpp' -o -iname '*.cc' -o -iname '*.cxx'`
fi

for mutation in "${available_mutations[@]}"
do
  stats[$mutation]=`"$tool_path" -p "$compilation_database" -c -m $mutation $files`
done

for mutation in "${available_mutations[@]}"
do
  echo $mutation: ${stats[$mutation]}
done > /tmp/mutation_stats.$$

if $only_list
then
  cat /tmp/mutation_stats.$$
  exit 0
fi

mv /tmp/mutation_stats.$$ $output_dir

git init || exit
git add .
git commit -am "Initial Commit"

for mutation in "${available_mutations[@]}"
do
  for ((i=1; i<=max && i<=stats[$mutation]; i++))
  do
    #clang-format-15 -i $files
    #git commit -am "Code formatted"
    "$tool_path" -p "$compilation_database" -l $i -m $mutation $files
    #clang-format-15 -i $files
    git diff > $output_dir/$mutation.$i.patch
    #git reset HEAD~
    git checkout -- .
  done
done

rm -rf .git

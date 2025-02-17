#include "Mutations.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Execution.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/WithColor.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <iostream>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using namespace mpi_mutation_tool;

// Set up the command line options
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::OptionCategory MPIMutationToolCategory("tool-template options");

static cl::list<std::string> mutations("m", cl::desc("Mutation to execute"),
                                      cl::value_desc("mutation"),
                                      cl::cat(MPIMutationToolCategory));

static cl::opt<bool> onlycount("c", cl::desc("Only count how often we could apply the mutation"),
                                      cl::init(false),
                                      cl::cat(MPIMutationToolCategory));

static cl::list<int> locations("l", cl::desc("Locations where to apply mutation to"),
                                      cl::value_desc("location"),
                                      cl::cat(MPIMutationToolCategory));

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  llvm::Expected<CommonOptionsParser> OptionsParser =
      CommonOptionsParser::create(argc, argv, MPIMutationToolCategory,
                                  llvm::cl::OneOrMore);

  if (!OptionsParser) {
    llvm::WithColor::error() << llvm::toString(OptionsParser.takeError());
    return 1;
  }


  auto Executor = clang::tooling::createExecutorFromCommandLineArgs(
      argc, argv, MPIMutationToolCategory);

  if (!Executor) {
    llvm::errs() << llvm::toString(Executor.takeError()) << "\n";
    return 1;
  }

  ast_matchers::MatchFinder Finder;
  auto context = Executor->get()->getExecutionContext();

  bool only_count = false;
  if (onlycount)
    only_count = true;

  //sorry for naming. i cant be bothered
  std::set<int> my_locs;
  if (!locations.empty()) {
    for (auto &location : locations) {
      my_locs.insert(location);
    }
  }

  //Couldn't get my Registry to compile so you have to deal with this abomination
  MutationCallback *callback = NULL;

  int possible_location_count = 0;
  int other_count = 0;

  std::vector<Mutation*> mutation_objects;

  if (!mutations.empty()) {
    for (auto &mutation_str : mutations) {
      if(mutation_str == "ArgErrorNullBufferMutation"){
        mutation_objects.push_back(new ArgErrorNullBufferArg1Mutation());
        mutation_objects.push_back(new ArgErrorNullBufferArg2Mutation());
        mutation_objects.push_back(new ArgErrorNullBufferArg3Mutation());
        mutation_objects.push_back(new ArgErrorNullBufferArg4Mutation());
      }
      else if(mutation_str == "ArgErrorCommunicator1Mutation"){
        mutation_objects.push_back(new ArgErrorCommunicator1Arg6Mutation());
        mutation_objects.push_back(new ArgErrorCommunicator1Arg7Mutation());
        mutation_objects.push_back(new ArgErrorCommunicator1Arg8Mutation());
      }
      else if(mutation_str == "ArgErrorCommunicator2Mutation"){
        mutation_objects.push_back(new ArgErrorCommunicator2Arg6Mutation());
        mutation_objects.push_back(new ArgErrorCommunicator2Arg7Mutation());
        mutation_objects.push_back(new ArgErrorCommunicator2Arg8Mutation());
      }
      else if(mutation_str == "ArgMismatchCommunicatorMutation"){
        mutation_objects.push_back(new ArgMismatchCommunicatorMutation());
      }
      else if(mutation_str == "ArgErrorCount1Mutation"){
        mutation_objects.push_back(new ArgErrorCount1Arg2Mutation());
        mutation_objects.push_back(new ArgErrorCount1Arg3Mutation());
        mutation_objects.push_back(new ArgErrorCount1Arg5Mutation());
      }
      else if(mutation_str == "ArgErrorCount2Mutation"){
        mutation_objects.push_back(new ArgErrorCount2Arg2Mutation());
        mutation_objects.push_back(new ArgErrorCount2Arg3Mutation());
        mutation_objects.push_back(new ArgErrorCount2Arg5Mutation());
      }
      else if(mutation_str == "ArgErrorRank1Mutation"){
        mutation_objects.push_back(new ArgErrorRank1Mutation());
      }
      else if(mutation_str == "ArgErrorRank2Mutation"){
        mutation_objects.push_back(new ArgErrorRank2Mutation());
      }
      else if(mutation_str == "ArgErrorNullRequestMutation"){
        mutation_objects.push_back(new ArgErrorNullRequestMutation());
      }
      else if(mutation_str == "ArgErrorTag1Mutation"){
        mutation_objects.push_back(new ArgErrorTag1Mutation());
      }
      else if(mutation_str == "ArgErrorTag2Mutation"){
        mutation_objects.push_back(new ArgErrorTag2Mutation());
      }
      else if(mutation_str == "ArgMismatchTagMutation"){
        mutation_objects.push_back(new ArgMismatchTagMutation());
      }
      else if(mutation_str == "ArgErrorType1Mutation"){
        mutation_objects.push_back(new ArgErrorType1Arg3Mutation());
        mutation_objects.push_back(new ArgErrorType1Arg4Mutation());
        mutation_objects.push_back(new ArgErrorType1Arg6Mutation());
      }
      else if(mutation_str == "ArgErrorType2Mutation"){
        mutation_objects.push_back(new ArgErrorType2Arg3Mutation());
        mutation_objects.push_back(new ArgErrorType2Arg4Mutation());
        mutation_objects.push_back(new ArgErrorType2Arg6Mutation());
      }
      else if(mutation_str == "ArgMismatchTypeMutation"){
        mutation_objects.push_back(new ArgMismatchTypeMutation());
      }
      else if(mutation_str == "Deadlock1Mutation"){
        mutation_objects.push_back(new Deadlock1Mutation());
      }
      else if(mutation_str == "MpiCallBeforeInitMutation"){
        mutation_objects.push_back(new MpiCallBeforeInitMutation());
      }
      else if(mutation_str == "ModifyBufferDuringIsendMutation"){
        mutation_objects.push_back(new ModifyBufferDuringIsendMutation());
      }
      else if(mutation_str == "MissingCallMpiFinalizeMutation"){
        mutation_objects.push_back(new MissingCallMpiFinalizeMutation());
      }
      else if(mutation_str == "MissingCallMpiRecvMutation"){
        mutation_objects.push_back(new MissingCallMpiRecvMutation());
      }
      else if(mutation_str == "MissingCallMpiSendMutation"){
        mutation_objects.push_back(new MissingCallMpiSendMutation());
      }
      else if(mutation_str == "MissingCallMpiWaitMutation"){
        mutation_objects.push_back(new MissingCallMpiWaitMutation());
      }
      else if(mutation_str == "MissingCallCollectiveMutation"){
        mutation_objects.push_back(new MissingCallCollectiveMutation());
      }
      else if(mutation_str == "ArgErrorRoot1Mutation"){
        mutation_objects.push_back(new ArgErrorRoot1Mutation());
        mutation_objects.push_back(new ArgErrorRoot1Arg6Mutation());
      }
      else if(mutation_str == "ArgErrorRoot2Mutation"){
        mutation_objects.push_back(new ArgErrorRoot2Mutation());
        mutation_objects.push_back(new ArgErrorRoot2Arg6Mutation());
      }
      else if(mutation_str == "ArgErrorOp1Mutation"){
        mutation_objects.push_back(new ArgErrorOp1Mutation());
      }
      else if(mutation_str == "ArgErrorOp2Mutation"){
        mutation_objects.push_back(new ArgErrorOp2Mutation());
      }
      else if(mutation_str == "ArgMismatchRootMutation"){
        mutation_objects.push_back(new ArgMismatchRootMutation());
      }
      else if(mutation_str == "ArgMismatchCountMutation"){
        mutation_objects.push_back(new ArgMismatchCountMutation());
      }
      else if(mutation_str == "ArgMismatchOpMutation"){
        mutation_objects.push_back(new ArgMismatchOpMutation());
      }
      else if(mutation_str == "MisplacedCallBarrierMutation"){
        mutation_objects.push_back(new MisplacedCallBarrierMutation());
      }
      else if(mutation_str == "MisplacedCallBarrier2Mutation"){
        mutation_objects.push_back(new MisplacedCallBarrier2Mutation());
      }
      else if(mutation_str == "ArgMismatchIrecvBufferOverlapMutation"){
        mutation_objects.push_back(new ArgMismatchIrecvBufferOverlapMutation());
      }

      for(auto mutation : mutation_objects) {
        callback = new MutationCallback(mutation, only_count, my_locs, &possible_location_count);
        Finder.addDynamicMatcher(mutation->getMatcher(), callback);
      } 
    }
  }

  if(mutation_objects.empty()) {
    llvm::errs() << "Error: No valid mutation passed\n";
  }

  auto Err = Executor->get()->execute(newFrontendActionFactory(&Finder));
  if (Err) {
    llvm::errs() << llvm::toString(std::move(Err)) << "\n";
  }

  if(callback && only_count)
    printf("%d\n", possible_location_count);

  Executor->get()->getToolResults()->forEachResult(
      [](llvm::StringRef key, llvm::StringRef value) {
        llvm::errs() << "----" << key.str() << "\n" << value.str() << "\n";
      });
}


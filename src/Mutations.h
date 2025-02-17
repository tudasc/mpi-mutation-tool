#ifndef MPI_MUTATION_TOOL_MUTATIONS
#define MPI_MUTATION_TOOL_MUTATIONS

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/Tooling/Execution.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

namespace mpi_mutation_tool {

class Mutation {
  public:
    virtual ast_matchers::internal::DynTypedMatcher getMatcher() = 0;
    virtual void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) = 0;
};

class MutationCallback : public MatchFinder::MatchCallback {
  public:
    MutationCallback(Mutation *mutation, bool count, std::set<int> locations, int *possible_location_count);
    ~MutationCallback();
    void run(const MatchFinder::MatchResult &result);
    void onEndOfTranslationUnit();

    int *possible_location_count;

  private:
    Mutation *mutation;
    bool count;
    std::set<int> locations;
    bool locations_empty_from_beginning;
};

/*
 * Base class for mutations that replace arguments of MPI calls with NULL pointers
 */
class SetArgNullMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that replace the Communicator of MPI calls with MPI_COMM_NULL
 */
class UseMpiCommNullMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that replace the Communicator of MPI calls with NULL pointers
 */
class InsertNullCommunicatorMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that replace the Communicator of MPI calls with one generated
 * by a SplitCommunicatorMutation
 */
class SplitCommunicatorMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that double the count of MPI calls
 *
 * This does not automatically produce an error, but we assume that is does most of the
 * time because programmers try to write memory-efficient programs and the allocated buffer
 * will usually be just big enough to fit the number of specified objects
 *
 */
class DoubleCountMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that make the rank of MPI calls too large
 *
 */
class TooLargeRankMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that make the rank of MPI calls negative
 *
 */
class NegativeArgumentMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that make the tag of MPI calls too large
 *
 */
class TooLargeTagMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that make increment an argument of MPI call
 *
 */
class IncrementArgumentMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that make the type in MPI call wrong
 *
 */
class WrongTypeMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that set the type in MPI call to NULL
 *
 */
class NullTypeMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that replace Recv calls with Send calls
 *
 */
class RecvToSendMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that insert a call to MPI_Comm_rank at a match location
 *
 */
class InsertMpiCallMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that edit the send buffer directly after a call
 *
 */
class ModifyBufferMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that remove a specific MPI call
 *
 */
class RemoveCallMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that set root to -1
 *
 */
class NegativeRootMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that set root to a huge value
 *
 */
class TooLargeRootMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that set op to a NULL value
 *
 */
class NullOpMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that set op to MPI_REPLACE
 *
 */
class SetOpToReplaceMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that switch op
 *
 */
class SwitchOpMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that remove call to collective function in one process
 *
 */
class RemoveCollectiveMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Meta-Mutation that splits a collective call with an if/else statement based on rank
 *
 */
class SplitCollectiveCallMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Base class for mutations that place misplaced Barrier calls around an MPI call
 *
 */
class PlaceMismatchingBarrierMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that place additional Barrier calls before an MPI Call
 *
 */
class PlaceAdditionalBarrierMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Base class for mutations that insert an overlapping irecv
 *
 */
class InsertOverlappingIrecvMutation: public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
};

/*
 * Replaces first argument of MPI call with NULL pointer
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Buffer-1
 * - pt2pt/ArgError-MPIISend-Buffer
 * - pt2pt/ArgError-MPIRecv-Buffer
 * - pt2pt/ArgError-MPISend-Buffer
 *
 * - coll/ArgError-MPIAllgather-SendBuffer
 * - coll/ArgError-MPIGather-SendBuffer
 * - coll/ArgError-MPIReduce-SendBuffer
 * - coll/ArgError-MPIScatter-SendBuffer
 *
 */
class ArgErrorNullBufferArg1Mutation : public SetArgNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces second argument of MPI call with NULL pointer
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-RecvBuffer.c
 * - pt2pt/ArgError-MPITest-Flag-duplicate.c
 * - pt2pt/ArgError-MPITest-Flag.c
 *
 */
class ArgErrorNullBufferArg2Mutation : public SetArgNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces third argument of MPI call with NULL pointer
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPITest-Status.c
 *
 */
class ArgErrorNullBufferArg3Mutation : public SetArgNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces fourth argument of MPI call with NULL pointer
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-RecvBuffer-2.c
 * - coll/ArgError-MPIGather-RecvBuffer-2.c
 * - coll/ArgError-MPIScatter-RecvBuffer.c
 */
class ArgErrorNullBufferArg4Mutation : public SetArgNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces Communicator of MPI call with MPI_COMM_NULL
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Communicator-1.c
 * - pt2pt/ArgError-MPIISend-Communicator-2.c
 * - pt2pt/ArgError-MPIRecv-Communicator-2.c
 * - pt2pt/ArgError-MPISend-Communicator-1.c
 *
 */
class ArgErrorCommunicator1Arg6Mutation : public UseMpiCommNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces Communicator of MPI call with MPI_COMM_NULL
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-Communicator-1.c
 * - coll/ArgError-MPIReduce-Communicator-2.c
 *
 */
class ArgErrorCommunicator1Arg7Mutation : public UseMpiCommNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};


/*
 * Replaces Communicator of MPI call with MPI_COMM_NULL
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIGather-Communicator-1.c
 * - coll/ArgError-MPIScatter-Communicator-1.c
 *
 */
class ArgErrorCommunicator1Arg8Mutation : public UseMpiCommNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Inserts NULL pointer as communicator in MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Communicator-2.c
 * - pt2pt/ArgError-MPIISend-Communicator-1.c
 * - pt2pt/ArgError-MPIRecv-Communicator-1.c
 * - pt2pt/ArgError-MPISend-Communicator-2.c
 *
 */
class ArgErrorCommunicator2Arg6Mutation : public InsertNullCommunicatorMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Inserts NULL pointer as communicator in MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-Communicator-2.c
 * - coll/ArgError-MPIReduce-Communicator-1.c
 *
 */
class ArgErrorCommunicator2Arg7Mutation : public InsertNullCommunicatorMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};


/*
 * Inserts NULL pointer as communicator in MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIGather-Communicator-2.c
 * - coll/ArgError-MPIScatter-Communicator-2.c
 *
 */
class ArgErrorCommunicator2Arg8Mutation : public InsertNullCommunicatorMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Splits communicator of MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgMismatch-MPIISend-Communicator-3.c
 * - pt2pt/ArgMismatch-MPISend-Communicator-1.c
 * - pt2pt/ArgMismatch-MPISend-Communicator-2.c
 *
 */
class ArgMismatchCommunicatorMutation : public SplitCommunicatorMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Doubles count in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Count-1.c
 * - pt2pt/ArgError-MPIISend-Count-2.c (kind of)
 * - pt2pt/ArgError-MPIRecv-Count-2.c
 * - pt2pt/ArgError-MPISend-Count-1.c
 * - pt2pt/ArgError-MPISend-Count-3.c (kind of)
 *
 * - coll/ArgError-MPIAllgather-Count-1.c
 * - coll/ArgError-MPIGather-Count-2.c
 * - coll/ArgError-MPIScatter-Count-1.c
 *
 */
class ArgErrorCount1Arg2Mutation : public DoubleCountMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Makes count negative in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Count-2.c
 * - pt2pt/ArgError-MPIISend-Count-1.c
 * - pt2pt/ArgError-MPIRecv-Count-1.c
 * - pt2pt/ArgError-MPISend-Count-2.c
 *
 * - coll/ArgError-MPIAllgather-Count-3.c
 * - coll/ArgError-MPIGather-Count-3.c
 * - coll/ArgError-MPIScatter-Count-3.c
 *
 */
class ArgErrorCount2Arg2Mutation : public NegativeArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Doubles count in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Count-2.c
 * - coll/ArgError-MPIReduce-Count-3.c
 *
 */
class ArgErrorCount1Arg3Mutation : public DoubleCountMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Makes count negative in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Count-1.c
 *
 */
class ArgErrorCount2Arg3Mutation : public NegativeArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Doubles count in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-Count-2.c
 * - coll/ArgError-MPIGather-Count-1.c
 * - coll/ArgError-MPIScatter-Count-2.c
 *
 */
class ArgErrorCount1Arg5Mutation : public DoubleCountMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Makes count negative in MPI calls
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-Count-4.c
 * - coll/ArgError-MPIScatter-Count-4.c
 *
 */
class ArgErrorCount2Arg5Mutation : public NegativeArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Produces argument mismatch of argument count
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgMismatch-MPIReduce-count.c
 */
class ArgMismatchCountMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces rank in MPI calls with one that is too large
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Rank-1.c
 * - pt2pt/ArgError-MPIISend-Rank-2.c
 * - pt2pt/ArgError-MPIRecv-Rank-2.c
 * - pt2pt/ArgError-MPISend-Rank-1.c
 *
 *
 */
class ArgErrorRank1Mutation : public TooLargeRankMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Negates the specified rank in an MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Rank-2.c
 * - pt2pt/ArgError-MPIISend-Rank-1.c
 * - pt2pt/ArgError-MPIRecv-Rank-1.c
 * - pt2pt/ArgError-MPISend-Rank-2.c
 *
 */
class ArgErrorRank2Mutation : public NegativeArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces request argument of nonblocking MPI call with NULL pointer
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Request.c
 * - pt2pt/ArgError-MPIISend-Request-1.c
 *
 */
class ArgErrorNullRequestMutation : public SetArgNullMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Makes tag argument of MPI call negative
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Tag.c
 * - pt2pt/ArgError-MPIISend-Tag-1.c
 * - pt2pt/ArgError-MPIRecv-Tag.c
 * - pt2pt/ArgError-MPISend-Tag-1.c
 *
 */
class ArgErrorTag1Mutation : public NegativeArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Makes tag argument of MPI call too large
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIISend-Tag-2.c
 * - pt2pt/ArgError-MPISend-Tag-2.c
 *
 */
class ArgErrorTag2Mutation : public TooLargeTagMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Increments tag argument of MPI call to produce tag mismatch
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgMismatch-MPIIRecv-Tag-1.c (not really)
 * - pt2pt/ArgMismatch-MPIIRecv-Tag-2.c
 * - pt2pt/ArgMismatch-MPIRecv-Tag-1.c
 * - pt2pt/ArgMismatch-MPIRecv-Tag-2.c (not really)
 * - pt2pt/ArgMismatch-MPIRecv-Tag-3.c
 *
 */
class ArgMismatchTagMutation : public IncrementArgumentMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with wrong type
 *
 * every type is set to MPI_LONG_DOUBLE, except when the buffer is 
 * actually long double, then it is set to MPI_CHAR
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Type-1.c
 * - pt2pt/ArgError-MPIIRecv-Type-3.c
 * - pt2pt/ArgError-MPIISend-Type-1.c
 * - pt2pt/ArgError-MPIISend-Type-3.c
 * - pt2pt/ArgError-MPIRecv-Type-2.c
 * - pt2pt/ArgError-MPIRecv-Type-3.c
 * - pt2pt/ArgError-MPISend-Type-3.c
 *
 * - coll/ArgError-MPIAllgather-Type-1.c
 * - coll/ArgError-MPIAllgather-Type-4.c
 * - coll/ArgError-MPIGather-Type-1.c
 * - coll/ArgError-MPIGather-Type-4.c
 * - coll/ArgError-MPIScatter-Type-1.c
 * - coll/ArgError-MPIScatter-Type-3.c
 * - coll/ArgMismatch-MPIGather-Type-1.c
 *
 */
class ArgErrorType1Arg3Mutation : public WrongTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with NULL
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgError-MPIIRecv-Type-2.c
 * - pt2pt/ArgError-MPIISend-Type-2.c
 * - pt2pt/ArgMismatch-MPIISend-Type.c (equivalent with above)
 * - pt2pt/ArgError-MPIRecv-Type-1.c
 * - pt2pt/ArgMismatch-MPIRecv-Type-1.c (equivalent with above)
 * - pt2pt/ArgError-MPISend-Type-2.c
 */
class ArgErrorType2Arg3Mutation : public NullTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with wrong type
 *
 * every type is set to MPI_LONG_DOUBLE, except when the buffer is 
 * actually long double, then it is set to MPI_CHAR
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Type-1.c
 * - coll/ArgError-MPIReduce-Type-3.c
 *
 */
class ArgErrorType1Arg4Mutation : public WrongTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with NULL
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Type-2.c
 */
class ArgErrorType2Arg4Mutation : public NullTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with wrong type
 *
 * every type is set to MPI_LONG_DOUBLE, except when the buffer is 
 * actually long double, then it is set to MPI_CHAR
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIAllgather-Type-2.c
 * - coll/ArgError-MPIGather-Type-2.c
 * - coll/ArgError-MPIScatter-Type-2.c
 * - coll/ArgMismatch-MPIGather-Type-2.c
 *
 */
class ArgErrorType1Arg6Mutation : public WrongTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with NULL
 *
 * No CorrBench Errors did this but we are doing it anyway.
 *
 */
class ArgErrorType2Arg6Mutation : public NullTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces type in MPI call with wrong type
 *
 * This is esentially ArgErrorType1Arg3Mutation but only applied to MPI_Recv so the
 * types beween send and receive calls do not match either
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgMismatch-MPIRecv-Type-2.c
 * - pt2pt/ArgMismatch-MPIRecv-Type-7.c (actually equivalent)
 *
 */
class ArgMismatchTypeMutation : public WrongTypeMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Produces deadlock by replacing Recv call with Send call
 *
 * Introduces the following CorrBench errors:
 *
 * This does not quite implement the exact CorrBench errors, as this is not
 * really feasible. The second one ist actually a tag mismatch and therefore
 * already covered by another mutation. The other ones require knowledge about
 * the actual program flow which we do not have.
 * 
 * - pt2pt/MisplacedCall-MPIRecv-Deadlock-1.c
 * - pt2pt/MisplacedCall-MPIRecv-Deadlock-2.c
 * - pt2pt/MisplacedCall-MPIRecv-Deadlock-4.c
 *
 */
class Deadlock1Mutation : public RecvToSendMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Inserts disallowed call to MPI_Comm_rank before MPI_Init 
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MisplacedCall-MPISend.c
 *
 * Actually uses a different MPI call than the CorrBench error but this should
 * be equivalent and does not alter program flow
 *
 */
class MpiCallBeforeInitMutation : public InsertMpiCallMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Modifies send buffer between an Isend call and a potential wait
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MisplacedCall-MPIWait.c
 *
 */
class ModifyBufferDuringIsendMutation : public ModifyBufferMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Removes call to MPI_Finalize
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MissingCall-MPIFinalize.c
 *
 */
class MissingCallMpiFinalizeMutation : public RemoveCallMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Removes call to MPI_Recv
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MissingCall-MPIRecv.c
 *
 */
class MissingCallMpiRecvMutation : public RemoveCallMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Removes call to MPI_Send
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MissingCall-MPISend-Deadlock.c
 *
 */
class MissingCallMpiSendMutation : public RemoveCallMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Removes call to MPI_Wait
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/MissingCall-MPIWait.c
 * - coll/MissingCall-MPIIBcast.c
 *
 */
class MissingCallMpiWaitMutation : public RemoveCallMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces root with negative value
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIGather-Dest-1.c
 * - coll/ArgError-MPIScatter-Rank.c
 *
 */
class ArgErrorRoot1Mutation : public NegativeRootMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces root with too large value
 *
 * This is not necessarily an error but it most likely is
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIGather-Dest-2.c
 * 
 * Also introduces this error for MPI_Scatter
 *
 */
class ArgErrorRoot2Mutation : public TooLargeRootMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces root with negative value
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Root.c
 *
 */
class ArgErrorRoot1Arg6Mutation : public NegativeRootMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces root with too large value
 *
 * This is not necessarily an error but it most likely is
 *
 * Introduces the following CorrBench errors:
 *
 * None but we are doing it anyway
 *
 *
 */
class ArgErrorRoot2Arg6Mutation : public TooLargeRootMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Produces argument mismatch of argument root
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgMismatch-MPIReduce-root.c
 */
class ArgMismatchRootMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces op with NULL operation
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Op-1.c
 *
 */
class ArgErrorOp1Mutation : public NullOpMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces op with illegal operation
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgError-MPIReduce-Op-2.c
 *
 */
class ArgErrorOp2Mutation : public SetOpToReplaceMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Replaces op with a different legal one
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/ArgMismatch-MPIReduce-Op.c
 *
 */
class ArgMismatchOpMutation : public Mutation {
  public:
    void mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter);
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Removes call to collective function in one process
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/MissingCall-MPIGather-Deadlock.c
 * - coll/MissingCall-MPIReduce-Deadlock.c
 *
 */
class MissingCallCollectiveMutation : public RemoveCollectiveMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Places mismatching Barriers around an MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/MisplacedCall-MPIBarrier-Deadlock-1.c
 *
 */
class MisplacedCallBarrierMutation : public PlaceMismatchingBarrierMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Places additional Barrier around an MPI call
 *
 * Introduces the following CorrBench errors:
 *
 * - coll/MisplacedCall-MPIBarrier-Deadlock-2.c
 *
 */
class MisplacedCallBarrier2Mutation : public PlaceAdditionalBarrierMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

/*
 * Inserts Irecv with overlapping buffer after Irecv
 *
 * Introduces the following CorrBench errors:
 *
 * - pt2pt/ArgMismatch-MPIIrecv-buffer-overlap.c
 *
 */
class ArgMismatchIrecvBufferOverlapMutation : public InsertOverlappingIrecvMutation {
  public:
    ast_matchers::internal::DynTypedMatcher getMatcher();
};

}
#endif // MPI_MUTATION_TOOL_MUTATIONS

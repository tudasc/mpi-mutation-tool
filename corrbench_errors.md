# CorrBench Errors

## Implemented

### ArgErrorNullBufferArg1Mutation

- ArgError-MPISend-Buffer.c
- ArgError-MPIIRecv-Buffer-1.c
- ArgError-MPIISend-Buffer.c
- ArgError-MPIRecv-Buffer.c

- ArgError-MPIAllgather-SendBuffer.c
- ArgError-MPIGather-SendBuffer.c
- ArgError-MPIReduce-SendBuffer.c
- ArgError-MPIScatter-SendBuffer.c

### ArgErrorNullBufferArg2Mutation

- ArgError-MPIReduce-RecvBuffer.c
- ArgError-MPITest-Flag-duplicate.c
- ArgError-MPITest-Flag.c

### ArgErrorNullBufferArg3Mutation

- ArgError-MPITest-Status.c

### ArgErrorNullBufferArg4Mutation

- ArgError-MPIAllgather-RecvBuffer-2.c
- ArgError-MPIGather-RecvBuffer-2.c
- ArgError-MPIScatter-RecvBuffer.c

### ArgErrorCommunicator1Arg6Mutation

- ArgError-MPIIRecv-Communicator-1.c
- ArgError-MPIISend-Communicator-2.c
- ArgError-MPIRecv-Communicator-2.c
- ArgError-MPISend-Communicator-1.c

### ArgErrorCommunicator1Arg7Mutation

- ArgError-MPIAllgather-Communicator-1.c
- ArgError-MPIReduce-Communicator-2.c

### ArgErrorCommunicator1Arg8Mutation

- ArgError-MPIGather-Communicator-1.c
- ArgError-MPIScatter-Communicator-1.c

### ArgErrorCommunicator2Arg6Mutation

- ArgError-MPIIRecv-Communicator-2.c
- ArgError-MPIISend-Communicator-1.c
- ArgError-MPIRecv-Communicator-1.c
- ArgError-MPISend-Communicator-2.c

### ArgErrorCommunicator2Arg7Mutation

- ArgError-MPIAllgather-Communicator-2.c
- ArgError-MPIReduce-Communicator-1.c

### ArgErrorCommunicator2Arg8Mutation

- ArgError-MPIGather-Communicator-2.c
- ArgError-MPIScatter-Communicator-2.c

### ArgErrorCount1Arg2Mutation

We might think about an optimal value to increase the specified count.
Currently we are only doubling to make sure to produce an error, but we could
also just increment the count by one to try to produce off-by-one errors, but
less reliably.

- ArgError-MPIIRecv-Count-1.c
- ArgError-MPIISend-Count-2.c (kind of)
- ArgError-MPIRecv-Count-2.c
- ArgError-MPISend-Count-1.c
- ArgError-MPISend-Count-3.c (kind of)

- ArgError-MPIAllgather-Count-1.c
- ArgError-MPIGather-Count-2.c
- ArgError-MPIScatter-Count-1.c

### ArgErrorCount2Arg2Mutation

- ArgError-MPIIRecv-Count-2.c
- ArgError-MPIISend-Count-1.c
- ArgError-MPIRecv-Count-1.c
- ArgError-MPISend-Count-2.c

- ArgError-MPIAllgather-Count-3.c
- ArgError-MPIGather-Count-3.c
- ArgError-MPIScatter-Count-3.c

### ArgErrorCount1Arg3Mutation

- ArgError-MPIReduce-Count-2.c
- ArgError-MPIReduce-Count-3.c

### ArgErrorCount2Arg3Mutation

- ArgError-MPIReduce-Count-1.c

### ArgErrorCount1Arg5Mutation

We might think about a compound mutations that performs both Arg2 and Arg5
mutations at the same time

- ArgError-MPIAllgather-Count-2.c
- ArgError-MPIGather-Count-1.c
- ArgError-MPIScatter-Count-2.c

The following two are kinda the same in the sense that the actual error that
happens with the count mutations is that the buffer runs over (most of the
time)

- ArgError-MPIAllgather-RecvBuffer-1.c
- ArgError-MPIGather-RecvBuffer-1.c


### ArgErrorCount2Arg5Mutation

- ArgError-MPIAllgather-Count-4.c
- ArgError-MPIScatter-Count-4.c

### ArgErrorRank1Mutation

- ArgError-MPIIRecv-Rank-1.c
- ArgError-MPIISend-Rank-2.c
- ArgError-MPIRecv-Rank-2.c
- ArgError-MPISend-Rank-1.c

### ArgErrorRank2Mutation

- ArgError-MPIIRecv-Rank-2.c
- ArgError-MPIISend-Rank-1.c
- ArgError-MPIRecv-Rank-1.c
- ArgError-MPISend-Rank-2.c

### ArgErrorNullRequestMutation

- ArgError-MPIIRecv-Request.c
- ArgError-MPIISend-Request-1.c

### ArgErrorTag1Mutation (negative tag)

- ArgError-MPIIRecv-Tag.c
- ArgError-MPIISend-Tag-1.c
- ArgError-MPIRecv-Tag.c
- ArgError-MPISend-Tag-1.c

### ArgErrorTag2Mutation (too large tag)

- ArgError-MPIISend-Tag-2.c
- ArgError-MPISend-Tag-2.c

### ArgErrorType1Arg3Mutation (wrong type)

- ArgError-MPIIRecv-Type-1.c
- ArgError-MPIIRecv-Type-3.c
- ArgError-MPIISend-Type-1.c
- ArgError-MPIISend-Type-3.c
- ArgError-MPIRecv-Type-2.c
- ArgError-MPIRecv-Type-3.c
- ArgError-MPISend-Type-3.c

- ArgError-MPIAllgather-Type-1.c
- ArgError-MPIAllgather-Type-4.c
- ArgError-MPIGather-Type-1.c
- ArgError-MPIGather-Type-4.c
- ArgError-MPIScatter-Type-1.c
- ArgError-MPIScatter-Type-3.c
- ArgMismatch-MPIGather-Type-1.c

### ArgErrorType2Arg3Mutation (NULL type)

- ArgError-MPIIRecv-Type-2.c
- ArgError-MPIISend-Type-2.c
- ArgMismatch-MPIISend-Type.c (equivalent with above)
- ArgError-MPIRecv-Type-1.c
- ArgMismatch-MPIRecv-Type-1.c (equivalent with above)
- ArgError-MPISend-Type-2.c

### ArgErrorType1Arg4Mutation (wrong type)

- ArgError-MPIReduce-Type-1.c
- ArgError-MPIReduce-Type-3.c

### ArgErrorType2Arg4Mutation (NULL type)

- ArgError-MPIReduce-Type-2.c

### ArgErrorType1Arg6Mutation (wrong type)

- ArgError-MPIAllgather-Type-2.c
- ArgError-MPIGather-Type-2.c
- ArgError-MPIScatter-Type-2.c
- ArgMismatch-MPIGather-Type-2.c

### ArgErrorType2Arg6Mutation (NULL type)

No CorrBench Errors did this but we are doing it anyway.

### Compound TODOs

There might be others we have skipped but really should put here

- ArgError-MPIAllgather-Type-3.c
- ArgError-MPIGather-Type-3.c

### ArgMismatchTagMutation

We cannot really do the first one, because as we do this in a static way we
would need to add additional program logic to build this in, which is
not really in the spirit of mutation testing. We might think about doing it
anyway maybe.

- ArgMismatch-MPIIRecv-Tag-1.c (not really)
- ArgMismatch-MPIIRecv-Tag-2.c
- ArgMismatch-MPIRecv-Tag-1.c
- ArgMismatch-MPIRecv-Tag-2.c (not really)
- ArgMismatch-MPIRecv-Tag-3.c

### ArgMismatchCommunicatorMutation

- ArgMismatch-MPIISend-Communicator-3.c
- ArgMismatch-MPISend-Communicator-1.c
- ArgMismatch-MPISend-Communicator-2.c

### ArgMismatchTypeMutation

- ArgMismatch-MPIRecv-Type-2.c
- ArgMismatch-MPIRecv-Type-7.c

### Deadlock1Mutation

This does not quite implement the exact CorrBench errors, as this is not
really feasible. The second one ist actually a tag mismatch and therefore
already covered by another mutation. The other ones require knowledge about
the actual program flow which we do not have.

- MisplacedCall-MPIRecv-Deadlock-1.c
- MisplacedCall-MPIRecv-Deadlock-2.c
- MisplacedCall-MPIRecv-Deadlock-4.c

### MpiCallBeforeInitMutation

- MisplacedCall-MPISend.c

### ModifyBufferDuringIsendMutation

- MisplacedCall-MPIWait.c

### MissingCallMpiFinalizeMutation

- MissingCall-MPIFinalize.c

### MissingCallMpiRecvMutation

- MissingCall-MPIRecv.c

### MissingCallMpiSendMutation

- MissingCall-MPISend-Deadlock.c

### MissingCallMpiWaitMutation

- MissingCall-MPIWait.c
- MissingCall-MPIIBcast.c

### ArgErrorRoot1Mutation

- ArgError-MPIGather-Dest-1.c
- ArgError-MPIScatter-Rank.c

### ArgErrorRoot2Mutation

- ArgError-MPIGather-Dest-2.c

### ArgErrorRoot1Arg6Mutation

- ArgError-MPIReduce-Root.c

### ArgErrorRoot2Arg6Mutation

No CorrBench errors exist for this one

### ArgErrorOp1Mutation

- ArgError-MPIReduce-Op-1.c

### ArgErrorOp2Mutation

- ArgError-MPIReduce-Op-2.c

### MissingCallCollectiveMutation

- MissingCall-MPIGather-Deadlock.c
- MissingCall-MPIReduce-Deadlock.c

### ArgMismatchRootMutation

- ArgMismatch-MPIReduce-root.c

### ArgMismatchCountMutation

- ArgMismatch-MPIReduce-Count.c

### ArgMismatchOpMutation

- ArgMismatch-MPIReduce-Op.c

### MisplacedCallBarrierMutation

- MisplacedCall-MPIBarrier-Deadlock-1.c

### MisplacedCallBarrier2Mutation

- MisplacedCall-MPIBarrier-Deadlock-2.c

## ArgMismatchIrecvBufferOverlapMutation

- ArgMismatch-MPIIrecv-buffer-overlap.c

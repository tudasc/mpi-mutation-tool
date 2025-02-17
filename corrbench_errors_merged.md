# CorrBench Errors

## Implemented

### ArgErrorNullBufferMutation

- ArgError-MPISend-Buffer.c
- ArgError-MPIIRecv-Buffer-1.c
- ArgError-MPIISend-Buffer.c
- ArgError-MPIRecv-Buffer.c
- ArgError-MPIAllgather-SendBuffer.c
- ArgError-MPIGather-SendBuffer.c
- ArgError-MPIReduce-SendBuffer.c
- ArgError-MPIScatter-SendBuffer.c
- ArgError-MPIReduce-RecvBuffer.c
- ArgError-MPITest-Flag-duplicate.c
- ArgError-MPITest-Flag.c
- ArgError-MPITest-Status.c
- ArgError-MPIAllgather-RecvBuffer-2.c
- ArgError-MPIGather-RecvBuffer-2.c
- ArgError-MPIScatter-RecvBuffer.c

### ArgErrorCommunicator1Mutation

- ArgError-MPIIRecv-Communicator-1.c
- ArgError-MPIISend-Communicator-2.c
- ArgError-MPIRecv-Communicator-2.c
- ArgError-MPISend-Communicator-1.c
- ArgError-MPIAllgather-Communicator-1.c
- ArgError-MPIReduce-Communicator-2.c
- ArgError-MPIGather-Communicator-1.c
- ArgError-MPIScatter-Communicator-1.c

### ArgErrorCommunicator2Mutation

- ArgError-MPIIRecv-Communicator-2.c
- ArgError-MPIISend-Communicator-1.c
- ArgError-MPIRecv-Communicator-1.c
- ArgError-MPISend-Communicator-2.c
- ArgError-MPIAllgather-Communicator-2.c
- ArgError-MPIReduce-Communicator-1.c
- ArgError-MPIGather-Communicator-2.c
- ArgError-MPIScatter-Communicator-2.c

### ArgErrorCount1Mutation

- ArgError-MPIIRecv-Count-1.c
- ArgError-MPIISend-Count-2.c
- ArgError-MPIRecv-Count-2.c
- ArgError-MPISend-Count-1.c
- ArgError-MPISend-Count-3.c
- ArgError-MPIAllgather-Count-1.c
- ArgError-MPIGather-Count-2.c
- ArgError-MPIScatter-Count-1.c
- ArgError-MPIReduce-Count-2.c
- ArgError-MPIReduce-Count-3.c
- ArgError-MPIAllgather-Count-2.c
- ArgError-MPIGather-Count-1.c
- ArgError-MPIScatter-Count-2.c
- ArgError-MPIAllgather-RecvBuffer-1.c
- ArgError-MPIGather-RecvBuffer-1.c

### ArgErrorCount2Mutation

- ArgError-MPIIRecv-Count-2.c
- ArgError-MPIISend-Count-1.c
- ArgError-MPIRecv-Count-1.c
- ArgError-MPISend-Count-2.c
- ArgError-MPIAllgather-Count-3.c
- ArgError-MPIGather-Count-3.c
- ArgError-MPIScatter-Count-3.c
- ArgError-MPIReduce-Count-1.c
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

### ArgErrorTag1Mutation

- ArgError-MPIIRecv-Tag.c
- ArgError-MPIISend-Tag-1.c
- ArgError-MPIRecv-Tag.c
- ArgError-MPISend-Tag-1.c

### ArgErrorTag2Mutation

- ArgError-MPIISend-Tag-2.c
- ArgError-MPISend-Tag-2.c

### ArgErrorType1Mutation

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
- ArgError-MPIReduce-Type-1.c
- ArgError-MPIReduce-Type-3.c
- ArgError-MPIAllgather-Type-2.c
- ArgError-MPIGather-Type-2.c
- ArgError-MPIScatter-Type-2.c
- ArgMismatch-MPIGather-Type-2.c

### ArgErrorType2Mutation

- ArgError-MPIIRecv-Type-2.c
- ArgError-MPIISend-Type-2.c
- ArgMismatch-MPIISend-Type.c
- ArgError-MPIRecv-Type-1.c
- ArgMismatch-MPIRecv-Type-1.c
- ArgError-MPISend-Type-2.c

### ArgMismatchTagMutation

- ArgMismatch-MPIIRecv-Tag-1.c
- ArgMismatch-MPIIRecv-Tag-2.c
- ArgMismatch-MPIRecv-Tag-1.c
- ArgMismatch-MPIRecv-Tag-2.c
- ArgMismatch-MPIRecv-Tag-3.c

### ArgMismatchCommunicatorMutation

- ArgMismatch-MPIISend-Communicator-3.c
- ArgMismatch-MPISend-Communicator-1.c
- ArgMismatch-MPISend-Communicator-2.c

### ArgMismatchTypeMutation

- ArgMismatch-MPIRecv-Type-2.c
- ArgMismatch-MPIRecv-Type-7.c

### Deadlock1Mutation

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
- ArgError-MPIReduce-Root.c

### ArgErrorRoot2Mutation

- ArgError-MPIGather-Dest-2.c

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

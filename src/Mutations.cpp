#include "Mutations.h"

#include "clang/Rewrite/Core/Rewriter.h"

#include <iostream>

// I am not going to split this into multiple files so here are some markers to jump to
// BUFFERMUTATIONS
// COMMUNICATORMUTATIONS
// COUNTMUTATIONS
// RANKMUTATIONS
// REQUESTMUTATIONS
// TAGMUTATIONS
// TYPEMUTATIONS
// DEADLOCKMUTATIONS
// OTHERMUTATIONS
// MISSINGCALLMUTATIONS
// ROOTMUTATIONS
// OPMUTATIONS
// SPLITMUTATIONS
// BARRIERMUTATIONS

namespace mpi_mutation_tool {

  static Rewriter *rewriter = NULL;
  static std::string unique = "0";

  MutationCallback::MutationCallback(Mutation *mutation, bool count, std::set<int> locations, int *possible_location_count) :
    mutation(mutation), count(count), locations(locations), possible_location_count(possible_location_count) {

      if(locations.empty())
        locations_empty_from_beginning = true;
    
    }

  MutationCallback::~MutationCallback() {
    if(rewriter != NULL)
      delete rewriter;
  }

  void MutationCallback::onEndOfTranslationUnit() {
    if(rewriter)
      rewriter->overwriteChangedFiles();
  }

  void MutationCallback::run(const MatchFinder::MatchResult &result){
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    assert(mpi_call_expr);

    if (!result.SourceManager->isWrittenInMainFile(mpi_call_expr->getBeginLoc()))
      return;

    (*possible_location_count)++;
    unique = std::to_string(*possible_location_count);

    if(count)
      return;

    if(locations_empty_from_beginning || locations.erase(*possible_location_count) > 0) {
      if(rewriter == NULL) {
        rewriter = new Rewriter(*result.SourceManager, result.Context->getLangOpts());
      }
      mutation->mutationCallback(result, rewriter);
      if(!locations_empty_from_beginning && locations.empty()) {
        onEndOfTranslationUnit();
        exit(0);
      }
    }
  }


// BUFFERMUTATIONS


  void SetArgNullMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *buffer_expr = result.Nodes.getNodeAs<Expr>("buffer_expr");
    auto *buffer_decl_ref_expr = result.Nodes.getNodeAs<DeclRefExpr>("buffer_decl_ref_expr");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(buffer_expr);

    bool is_array = false;
    if (buffer_decl_ref_expr) {
      const Type *buffer_type = buffer_decl_ref_expr->getDecl()->getType().getTypePtrOrNull();
      if(buffer_type && buffer_type->isArrayType()) {
        is_array = true;
      }
    }

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    if(buffer_decl_ref_expr && !is_array) {
      auto name = buffer_decl_ref_expr->getNameInfo().getAsString();

      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, name + " = NULL;");

      if(!parent_compound_stmt){
        rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
        rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
      }
    } else {
      CharSourceRange csr = result.SourceManager->getExpansionRange(
          SourceRange(buffer_expr->getBeginLoc(), buffer_expr->getEndLoc()));

      rewriter->ReplaceText(csr, "NULL");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorNullBufferArg1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send"),

                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Reduce"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(0,
              expr(
                expr().bind("buffer_expr"),
                optionally(declRefExpr().bind("buffer_decl_ref_expr"))
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorNullBufferArg2Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Reduce"),
                  hasName("MPI_Test")
                  )
                )
              ),
            hasArgument(1,
              expr(
                expr().bind("buffer_expr"),
                optionally(declRefExpr().bind("buffer_decl_ref_expr"))
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorNullBufferArg3Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Test")
                )
              ),
            hasArgument(2,
              expr(
                expr().bind("buffer_expr"),
                optionally(declRefExpr().bind("buffer_decl_ref_expr"))
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorNullBufferArg4Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(3,
              expr(
                expr().bind("buffer_expr"),
                optionally(declRefExpr().bind("buffer_decl_ref_expr"))
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }


// COMMUNICATORMUTATIONS


  void UseMpiCommNullMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *communicator_arg = result.Nodes.getNodeAs<Expr>("communicator_arg");

    assert(mpi_call_expr);
    assert(communicator_arg);

    SourceRange sr(result.SourceManager->getFileLoc(communicator_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(communicator_arg->getEndLoc()));

    rewriter->ReplaceText(sr, "MPI_COMM_NULL");
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator1Arg6Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send")
                  )
                )
              ),
            hasArgument(5, expr().bind("communicator_arg"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator1Arg7Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Reduce")
                  )
                )
              ),
            hasArgument(6, expr().bind("communicator_arg"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator1Arg8Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(7, expr().bind("communicator_arg"))
            ));

    return matcher;
  }


  void InsertNullCommunicatorMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *communicator_arg = result.Nodes.getNodeAs<Expr>("communicator_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(communicator_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange sr(result.SourceManager->getFileLoc(communicator_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(communicator_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "MPI_Comm null_comm_abc" + unique + " = NULL;");
    rewriter->ReplaceText(sr, "null_comm_abc" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator2Arg6Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send")
                  )
                )
              ),
            hasArgument(5, expr().bind("communicator_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator2Arg7Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Reduce")
                  )
                )
              ),
            hasArgument(6, expr().bind("communicator_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCommunicator2Arg8Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(7, expr().bind("communicator_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }


  void SplitCommunicatorMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *communicator_arg = result.Nodes.getNodeAs<Expr>("communicator_arg");

    assert(mpi_call_expr);
    assert(communicator_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());

    std::string code = 
      "int rank_abc" + unique + ";"
      "MPI_Comm_rank(MPI_COMM_WORLD, &rank_abc" + unique + ");"
      "int color_abc" + unique + " = rank_abc" + unique + " / 1;"
      "MPI_Comm new_wrong_communicator_abc" + unique + ";"
      "MPI_Comm_split(MPI_COMM_WORLD, color_abc" + unique + ", rank_abc" + unique + ", &new_wrong_communicator_abc" + unique + ");";

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, code);

    SourceRange sr(result.SourceManager->getFileLoc(communicator_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(communicator_arg->getEndLoc()));

    rewriter->ReplaceText(sr, "new_wrong_communicator_abc" + unique);
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchCommunicatorMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Isend"),
                  hasName("MPI_Send")
                  )
                )
              ),
            hasArgument(5, expr().bind("communicator_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }


// COUNTMUTATIONS


  void DoubleCountMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");

    assert(mpi_call_expr);
    assert(arg);

    rewriter->InsertTextBefore(result.SourceManager->getFileLoc(arg->getBeginLoc()), "2 * ");
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCount1Arg2Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send"),
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(1, expr().bind("arg"))
            ));

    return matcher;
  }

  void NegativeArgumentMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");

    assert(mpi_call_expr);
    assert(arg);

    rewriter->InsertTextBefore(result.SourceManager->getFileLoc(arg->getBeginLoc()), "-2-");
  }

  //I am so sorry for this
  ast_matchers::internal::DynTypedMatcher ArgErrorCount2Arg2Mutation::getMatcher(){
    ArgErrorCount1Arg2Mutation m;
    return m.getMatcher();
  }


  ast_matchers::internal::DynTypedMatcher ArgErrorCount1Arg3Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Reduce"),
                  hasName("MPI_Reduce")
                  )
                )
              ),
            hasArgument(2, expr().bind("arg"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCount2Arg3Mutation::getMatcher(){
    ArgErrorCount1Arg3Mutation m;
    return m.getMatcher();
  }


  ast_matchers::internal::DynTypedMatcher ArgErrorCount1Arg5Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(4, expr().bind("arg"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorCount2Arg5Mutation::getMatcher(){
    ArgErrorCount1Arg5Mutation m;
    return m.getMatcher();
  }


  void ArgMismatchCountMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(arg);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange call_expr_sr(result.SourceManager->getFileLoc(mpi_call_expr->getBeginLoc()),
        result.SourceManager->getFileLoc(mpi_call_expr->getEndLoc()).getLocWithOffset(1));

    std::string call_str = rewriter->getRewrittenText(call_expr_sr);

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_abc" + unique + "; MPI_Comm_rank(" + rewriter->getRewrittenText(comm_arg_sr) + ", &rank_abc" + unique + "); if(rank_abc" + unique + " == 0) {");

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(1), "} else {" + call_str + "}");


    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }

    rewriter->InsertTextBefore(result.SourceManager->getFileLoc(arg->getBeginLoc()), "1+");
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchCountMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(2, expr().bind("arg")),
            hasArgument(6, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  
// RANKMUTATIONS
    
  void TooLargeRankMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *communicator_arg = result.Nodes.getNodeAs<Expr>("communicator_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(arg);
    assert(communicator_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    CharSourceRange sr = result.SourceManager->getExpansionRange(
        SourceRange(arg->getBeginLoc(), arg->getEndLoc()));

    CharSourceRange comm_sr = result.SourceManager->getExpansionRange(
        SourceRange(communicator_arg->getBeginLoc(), communicator_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int illegal_rank_abc" + unique + "; MPI_Comm_size(" + rewriter->getRewrittenText(comm_sr) + ", &illegal_rank_abc" + unique + ");");
    rewriter->ReplaceText(sr, "illegal_rank_abc" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorRank1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send")
                  )
                )
              ),
            hasArgument(3, expr().bind("arg")),
            hasArgument(5, expr().bind("communicator_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  //I am so sorry for this
  ast_matchers::internal::DynTypedMatcher ArgErrorRank2Mutation::getMatcher(){
    ArgErrorRank1Mutation m;
    return m.getMatcher();
  }

// REQUESTMUTATIONS

  ast_matchers::internal::DynTypedMatcher ArgErrorNullRequestMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Isend"),
                  hasName("MPI_Irecv")
                  )
                )
              ),
            hasArgument(6,
              expr(
                expr().bind("buffer_expr"),
                optionally(declRefExpr().bind("buffer_decl_ref_expr"))
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }


// TAGMUTATIONS
    
  void TooLargeTagMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange sr(result.SourceManager->getFileLoc(arg->getBeginLoc()),
        result.SourceManager->getFileLoc(arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "long long illegal_tag_abc" + unique + " = MPI_TAG_UB + 1;");
    rewriter->ReplaceText(sr, "illegal_tag_abc" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorTag1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send")
                  )
                )
              ),
            hasArgument(4, expr().bind("arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  //I am so sorry for this
  ast_matchers::internal::DynTypedMatcher ArgErrorTag2Mutation::getMatcher(){
    ArgErrorTag1Mutation m;
    return m.getMatcher();
  }

  void IncrementArgumentMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");

    assert(mpi_call_expr);
    assert(arg);

    rewriter->InsertTextBefore(result.SourceManager->getFileLoc(arg->getBeginLoc()), "1+");
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchTagMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Recv")
                  )
                )
              ),
            hasArgument(4, expr().bind("arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

// TYPEMUTATIONS
    
  void WrongTypeMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *buffer_expr = result.Nodes.getNodeAs<Expr>("buffer_expr");

    assert(mpi_call_expr);
    assert(arg);
    assert(buffer_expr);

    QualType buffer_type = buffer_expr->getType();

    std::string new_wrong_type = "MPI_LONG_DOUBLE";
    if(buffer_type.getAsString().find("long double") != std::string::npos)
        new_wrong_type = "MPI_CHAR";
        
    CharSourceRange sr(result.SourceManager->getExpansionRange(SourceRange(
          arg->getBeginLoc(), arg->getEndLoc())));

    rewriter->ReplaceText(sr, new_wrong_type);

  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType1Arg3Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Isend"),
                  hasName("MPI_Recv"),
                  hasName("MPI_Send"),
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(2, expr().bind("arg")),
            hasArgument(0, expr().bind("buffer_expr"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType1Arg4Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(3, expr().bind("arg")),
            hasArgument(0, expr().bind("buffer_expr"))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType1Arg6Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(5, expr().bind("arg")),
            hasArgument(0, expr().bind("buffer_expr"))
            ));

    return matcher;
  }


  void NullTypeMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");

    assert(mpi_call_expr);
    assert(arg);

    CharSourceRange sr(result.SourceManager->getExpansionRange(SourceRange(
          arg->getBeginLoc(), arg->getEndLoc())));

    rewriter->ReplaceText(sr, "NULL");
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType2Arg3Mutation::getMatcher(){
    ArgErrorType1Arg3Mutation m;
    return m.getMatcher();
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType2Arg4Mutation::getMatcher(){
    ArgErrorType1Arg4Mutation m;
    return m.getMatcher();
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorType2Arg6Mutation::getMatcher(){
    ArgErrorType1Arg6Mutation m;
    return m.getMatcher();
  }



  ast_matchers::internal::DynTypedMatcher ArgMismatchTypeMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Irecv"),
                  hasName("MPI_Recv")
                  )
                )
              ),
            hasArgument(2, expr().bind("arg")),
            hasArgument(0, expr().bind("buffer_expr"))
            ));

    return matcher;
  }

// DEADLOCKMUTATIONS

  void RecvToSendMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg5 = result.Nodes.getNodeAs<Expr>("arg5");
    auto *arg6 = result.Nodes.getNodeAs<Expr>("arg6");

    assert(mpi_call_expr);
    assert(arg5);
    assert(arg6);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    rewriter->ReplaceText(mpi_call_expr_begin_loc, 8, "MPI_Send");

    CharSourceRange comm_sr = result.SourceManager->getExpansionRange(SourceRange(
          arg5->getBeginLoc(), arg5->getEndLoc()));


    SourceRange sr(result.SourceManager->getFileLoc(arg5->getBeginLoc()), mpi_call_expr_end_loc);

    rewriter->ReplaceText(sr, rewriter->getRewrittenText(comm_sr) + ")");
  }

  ast_matchers::internal::DynTypedMatcher Deadlock1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Recv")
                )
              ),
            hasArgument(5, expr().bind("arg5")),
            hasArgument(6, expr().bind("arg6"))
            ));

    return matcher;
  }

// OTHERMUTATIONS

  void InsertMpiCallMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");

    assert(mpi_call_expr);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "int dummy_rank_123" + unique + "; MPI_Comm_rank(MPI_COMM_WORLD, &dummy_rank_123" + unique + ");");
  }

  ast_matchers::internal::DynTypedMatcher MpiCallBeforeInitMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Init")
                )
              )
            ));

    return matcher;
  }

  void ModifyBufferMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *buffer_arg = result.Nodes.getNodeAs<Expr>("buffer_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(buffer_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    CharSourceRange sr(result.SourceManager->getExpansionRange(SourceRange(
            buffer_arg->getBeginLoc(), buffer_arg->getEndLoc())));

    std::string code = "*(" + rewriter->getRewrittenText(sr) + ") = 9736282;";

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), code);

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ModifyBufferDuringIsendMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Isend")
                )
              ),
            hasArgument(0, expr(
                expr().bind("buffer_arg")
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            )
          );

    return matcher;
  }

// MISSINGCALLMUTATIONS

  void RemoveCallMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{}");
    }

    SourceRange sr(mpi_call_expr_begin_loc, mpi_call_expr_end_loc.getLocWithOffset(2));
    rewriter->RemoveText(sr);
  }

  ast_matchers::internal::DynTypedMatcher MissingCallMpiFinalizeMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Finalize")
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher MissingCallMpiRecvMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Recv")
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher MissingCallMpiSendMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Send")
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher MissingCallMpiWaitMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Wait")
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  void RemoveCollectiveMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_123" + unique + "; MPI_Comm_rank(MPI_COMM_WORLD, &rank_123" + unique + "); if(rank_123" + unique + " != 0) ");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher MissingCallCollectiveMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Allgather"),
                  hasName("MPI_Gather"),
                  hasName("MPI_Reduce"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

// ROOTMUTATIONS

  void NegativeRootMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *dest_arg = result.Nodes.getNodeAs<Expr>("dest_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(dest_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange sr(result.SourceManager->getFileLoc(dest_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(dest_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "int negative_dest_123" + unique + " = -1;");
    rewriter->ReplaceText(sr, "negative_dest_123" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorRoot1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                anyOf(
                  hasName("MPI_Gather"),
                  hasName("MPI_Scatter")
                  )
                )
              ),
            hasArgument(6, expr().bind("dest_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorRoot1Arg6Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(5, expr().bind("dest_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  void TooLargeRootMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *dest_arg = result.Nodes.getNodeAs<Expr>("dest_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(dest_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange sr(result.SourceManager->getFileLoc(dest_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(dest_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "unsigned int too_large_dest_123" + unique + " = -1;");
    rewriter->ReplaceText(sr, "too_large_dest_123" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorRoot2Mutation::getMatcher(){
    ArgErrorRoot1Mutation m;
    return m.getMatcher();
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorRoot2Arg6Mutation::getMatcher(){
    ArgErrorRoot1Arg6Mutation m;
    return m.getMatcher();
  }
  

  void ArgMismatchRootMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(arg);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange call_expr_sr(result.SourceManager->getFileLoc(mpi_call_expr->getBeginLoc()),
        result.SourceManager->getFileLoc(mpi_call_expr->getEndLoc()).getLocWithOffset(1));

    std::string call_str = rewriter->getRewrittenText(call_expr_sr);

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_abc" + unique + "; MPI_Comm_rank(" + rewriter->getRewrittenText(comm_arg_sr) + ", &rank_abc" + unique + "); if(rank_abc" + unique + " == 0) {");

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(1), "} else {" + call_str + "}");


    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }

    rewriter->InsertTextBefore(result.SourceManager->getFileLoc(arg->getBeginLoc()), "1+");
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchRootMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(5, expr().bind("arg")),
            hasArgument(6, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

// OPMUTATIONS

  void NullOpMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *op_arg = result.Nodes.getNodeAs<Expr>("op_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(op_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange sr(result.SourceManager->getFileLoc(op_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(op_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "MPI_Op null_op_123" + unique + " = NULL;");
    rewriter->ReplaceText(sr, "null_op_123" + unique + "");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorOp1Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(4, expr().bind("op_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  void SetOpToReplaceMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *op_arg = result.Nodes.getNodeAs<Expr>("op_arg");

    assert(mpi_call_expr);
    assert(op_arg);

    SourceRange sr(result.SourceManager->getFileLoc(op_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(op_arg->getEndLoc()));

    rewriter->ReplaceText(sr, "MPI_REPLACE");
  }

  ast_matchers::internal::DynTypedMatcher ArgErrorOp2Mutation::getMatcher(){
    ArgErrorOp1Mutation m;
    return m.getMatcher();
  }


  void SwitchOpMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *op_arg = result.Nodes.getNodeAs<Expr>("op_arg");

    assert(mpi_call_expr);
    assert(op_arg);

    SourceRange sr(result.SourceManager->getFileLoc(op_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(op_arg->getEndLoc()));

    std::string new_op = "MPI_MAX";
    if (rewriter->getRewrittenText(sr) == "MPI_MAX")
      new_op = "MPI_SUM";

    rewriter->ReplaceText(sr, new_op);
  }

  void ArgMismatchOpMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *arg = result.Nodes.getNodeAs<Expr>("arg");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(arg);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange call_expr_sr(result.SourceManager->getFileLoc(mpi_call_expr->getBeginLoc()),
        result.SourceManager->getFileLoc(mpi_call_expr->getEndLoc()).getLocWithOffset(1));

    std::string call_str = rewriter->getRewrittenText(call_expr_sr);

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_abc" + unique + "; MPI_Comm_rank(" + rewriter->getRewrittenText(comm_arg_sr) + ", &rank_abc" + unique + "); if(rank_abc" + unique + " == 0) {");

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(1), "} else {" + call_str + "}");


    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }

    CharSourceRange sr = result.SourceManager->getExpansionRange(SourceRange(
          arg->getBeginLoc(), arg->getEndLoc()));

    std::string new_op = "MPI_MAX";
    if (rewriter->getRewrittenText(sr) == "MPI_MAX")
      new_op = "MPI_SUM";

    rewriter->ReplaceText(sr, new_op);
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchOpMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(4, expr().bind("arg")),
            hasArgument(6, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }


// SPLITMUTATIONS

  void SplitCollectiveCallMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange call_expr_sr(result.SourceManager->getFileLoc(mpi_call_expr->getBeginLoc()),
        result.SourceManager->getFileLoc(mpi_call_expr->getEndLoc()).getLocWithOffset(1));

    std::string call_str = rewriter->getRewrittenText(call_expr_sr);

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_abc" + unique + "; MPI_Comm_rank(" + rewriter->getRewrittenText(comm_arg_sr) + ", &rank_abc" + unique + "); if(rank_abc" + unique + " == 0) {");

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(1), "} else {" + call_str + "}");


    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher SplitCollectiveCallMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Reduce")
                )
              ),
            hasArgument(6, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

// BARRIERMUTATIONS

  void PlaceMismatchingBarrierMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    std::string comm_str = rewriter->getRewrittenText(comm_arg_sr);

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc,
        "int rank_abc" + unique + "; MPI_Comm_rank(" + comm_str + ", &rank_abc" + unique + "); if(rank_abc" + unique + " == 0) { MPI_Barrier(" + comm_str + ");}");

    rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(1),
        "if(rank_abc" + unique + " != 0) { MPI_Barrier(" + comm_str + ");}");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher MisplacedCallBarrierMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Bcast")
                )
              ),
            hasArgument(4, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  void PlaceAdditionalBarrierMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        result.SourceManager->getFileLoc(comm_arg->getEndLoc()));

    std::string comm_str = rewriter->getRewrittenText(comm_arg_sr);

    rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "MPI_Barrier(" + comm_str + ");");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher MisplacedCallBarrier2Mutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Recv")
                )
              ),
            hasArgument(5, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

  void InsertOverlappingIrecvMutation::mutationCallback(const MatchFinder::MatchResult &result, Rewriter *rewriter) {
    auto *mpi_call_expr = result.Nodes.getNodeAs<CallExpr>("mpi_call_expr");
    auto *comm_arg = result.Nodes.getNodeAs<Expr>("comm_arg");
    auto *parent_compound_stmt = result.Nodes.getNodeAs<CompoundStmt>("parent_compound_stmt");

    assert(mpi_call_expr);
    assert(comm_arg);

    SourceLocation mpi_call_expr_begin_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getBeginLoc());
    SourceLocation mpi_call_expr_end_loc = result.SourceManager->getSpellingLoc(mpi_call_expr->getEndLoc());

    SourceLocation end_of_comm_arg = result.SourceManager->getFileLoc(
        comm_arg->getEndLoc());
    SourceRange comm_arg_sr(result.SourceManager->getFileLoc(comm_arg->getBeginLoc()),
        end_of_comm_arg);
    std::string comm_str = rewriter->getRewrittenText(comm_arg_sr);

    SourceRange call_until_comm_arg_sr(
        result.SourceManager->getFileLoc(mpi_call_expr->getBeginLoc()), end_of_comm_arg);
    std::string call_expr_str = rewriter->getRewrittenText(call_until_comm_arg_sr);

    rewriter->InsertTextAfter(mpi_call_expr_end_loc.getLocWithOffset(2),
        "MPI_Request dummy_request_abc" + unique + ";" + call_expr_str + ", &dummy_request_abc" + unique + "); MPI_Request_free(&dummy_request_abc" + unique + ");");

    if(!parent_compound_stmt){
      rewriter->InsertTextBefore(mpi_call_expr_begin_loc, "{");
      rewriter->InsertTextAfterToken(mpi_call_expr_end_loc.getLocWithOffset(2), "}");
    }
  }

  ast_matchers::internal::DynTypedMatcher ArgMismatchIrecvBufferOverlapMutation::getMatcher(){
    ast_matchers::internal::DynTypedMatcher matcher =

      traverse(TK_IgnoreUnlessSpelledInSource,
          callExpr(
            callExpr().bind("mpi_call_expr"),
            hasDeclaration(
              functionDecl(
                hasName("MPI_Irecv")
                )
              ),
            hasArgument(5, expr().bind("comm_arg")),
            optionally(hasParent(compoundStmt().bind("parent_compound_stmt")))
            ));

    return matcher;
  }

}

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"


using namespace clang;

// Visitor to check increment operations inside the for loop body
class ForBodyVisitor : public RecursiveASTVisitor<ForBodyVisitor> {
public:
  explicit ForBodyVisitor(const VarDecl *IteratorVar) : IteratorVar(IteratorVar), FoundIncrement(false) {}

  bool VisitUnaryOperator(UnaryOperator *UO) {
    // ++iterator or iterator++
    if ((UO->getOpcode() == UO_PreInc || UO->getOpcode() == UO_PostInc)) {
      if (const DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(UO->getSubExpr())) {
        if (DRE->getDecl() == IteratorVar) {
          llvm::outs() << "Iterator increment found in loop body.\n";
          FoundIncrement = true;
        }
      }
    }
    return true;
  }

  bool VisitBinaryOperator(BinaryOperator *BOP) {
    // iterator += ...
    if (BOP->getOpcode() == BO_AddAssign) {
      if (const DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(BOP->getLHS())) {
        if (DRE->getDecl() == IteratorVar) {
          llvm::outs() << "Iterator += found in loop body.\n";
          FoundIncrement = true;
        }
      }
    }
    // iterator = iterator + ...
    else if (BOP->getOpcode() == BO_Assign) {
      if (const DeclRefExpr *LHS = dyn_cast<DeclRefExpr>(BOP->getLHS())) {
        if (LHS->getDecl() == IteratorVar) {
          if (const BinaryOperator *RHSBOP = dyn_cast<BinaryOperator>(BOP->getRHS())) {
            if (RHSBOP->getOpcode() == BO_Add) {
              if (const DeclRefExpr *RHSLHS = dyn_cast<DeclRefExpr>(RHSBOP->getLHS())) {
                if (RHSLHS->getDecl() == IteratorVar) {
                  llvm::outs() << "Iterator = iterator + ... found in loop body.\n";
                  FoundIncrement = true;
                }
              }
            }
          }
        }
      }
    }
    return true;
  }

  bool foundIncrement() const { return FoundIncrement; }

private:
  const VarDecl *IteratorVar;
  bool FoundIncrement;
};

class ForLoopVisitor : public RecursiveASTVisitor<ForLoopVisitor> {
public:
  explicit ForLoopVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitForStmt(ForStmt *forStmt) {
    // Check if the init is a variable declaration
    if (auto *init = dyn_cast_or_null<DeclStmt>(forStmt->getInit())) {
      if (init->isSingleDecl()) {
        if (auto *var = dyn_cast<VarDecl>(init->getSingleDecl())) {
          std::string iteratorName = var->getNameAsString();
          llvm::outs() << "Iterator variable: " << iteratorName << "\n";

          // Check the increment statement of the for loop
          bool incStmtIncrementsIterator = false;
          if (const Expr *inc = forStmt->getInc()) {
            // ++i or i++
            if (const UnaryOperator *uop = dyn_cast<UnaryOperator>(inc)) {
              if ((uop->getOpcode() == UO_PreInc || uop->getOpcode() == UO_PostInc)) {
                if (const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(uop->getSubExpr())) {
                  if (dre->getDecl() == var) {
                    incStmtIncrementsIterator = true;
                  }
                }
              }
            }
            // i += ... or i = i + ...
            else if (const BinaryOperator *bop = dyn_cast<BinaryOperator>(inc)) {
              // i += ...
              if (bop->getOpcode() == BO_AddAssign) {
                if (const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(bop->getLHS())) {
                  if (dre->getDecl() == var) {
                    incStmtIncrementsIterator = true;
                  }
                }
              }
              // i = i + ...
              else if (bop->getOpcode() == BO_Assign) {
                if (const DeclRefExpr *lhs = dyn_cast<DeclRefExpr>(bop->getLHS())) {
                  if (lhs->getDecl() == var) {
                    if (const BinaryOperator *rhsBop = dyn_cast<BinaryOperator>(bop->getRHS())) {
                      if (rhsBop->getOpcode() == BO_Add) {
                        if (const DeclRefExpr *rhsLHS = dyn_cast<DeclRefExpr>(rhsBop->getLHS())) {
                          if (rhsLHS->getDecl() == var) {
                            incStmtIncrementsIterator = true;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          if (incStmtIncrementsIterator) {
            llvm::outs() << "Increment statement increments the iterator.\n";
          } else {
            llvm::outs() << "Warning: Increment statement does NOT increment the iterator!\n";
          }

          // Use a separate visitor to check for increment in the loop body
          ForBodyVisitor bodyVisitor(var);
          bodyVisitor.TraverseStmt(forStmt->getBody());
          if (bodyVisitor.foundIncrement()) {
            llvm::outs() << "Increment operation found in loop body.\n";
          } else {
            llvm::outs() << "Warning: No increment operation found for iterator in loop body!\n";
          }
        }
      }
    }
    return true;
  }

private:
  ASTContext *Context;
};

class ForLoopConsumer : public ASTConsumer {
public:
  explicit ForLoopConsumer(ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  ForLoopVisitor Visitor;
};

class ForLoopAction : public ASTFrontendAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
    return std::make_unique<ForLoopConsumer>(&CI.getASTContext());
  }
};

// To run: clang-tooling infrastructure, e.g. with clang::tooling::runToolOnCode
int main(int argc, const char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(std::make_unique<ForLoopAction>(), argv[1]);
  }
}

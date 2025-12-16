#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

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

          // Now check the increment statement
          const Expr *inc = forStmt->getInc();
          bool incrementsIterator = false;
          if (inc) {
            // ++i or i++
            if (const UnaryOperator *uop = dyn_cast<UnaryOperator>(inc)) {
              if ((uop->getOpcode() == UO_PreInc || uop->getOpcode() == UO_PostInc)) {
                if (const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(uop->getSubExpr())) {
                  if (dre->getDecl() == var) {
                    incrementsIterator = true;
                  }
                }
              }
            }
            // i += 1 or i = i + 1
            else if (const BinaryOperator *bop = dyn_cast<BinaryOperator>(inc)) {
              // i += ...
              if (bop->getOpcode() == BO_AddAssign) {
                if (const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(bop->getLHS())) {
                  if (dre->getDecl() == var) {
                    incrementsIterator = true;
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
                            incrementsIterator = true;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
          if (incrementsIterator) {
            llvm::outs() << "Increment statement increments the iterator.\n";
          } else {
            llvm::outs() << "Warning: Increment statement does NOT increment the iterator!\n";
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

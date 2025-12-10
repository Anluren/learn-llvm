#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/Expr.h"
// #include "clang/AST/ExprConstant.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::tooling;

// Visitor that finds and evaluates constant expressions
class EvalVisitor : public RecursiveASTVisitor<EvalVisitor> {
public:
  explicit EvalVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitVarDecl(VarDecl *VD) {
    if (VD->hasInit()) {
      const Expr *Init = VD->getInit();
      Expr::EvalResult Result;
      if (Init->isEvaluatable(*Context) &&
          Init->EvaluateAsRValue(Result, *Context)) {
        llvm::outs() << "Variable '" << VD->getName() << "' = ";
        if (Result.Val.isInt())
          llvm::outs() << Result.Val.getInt();
        else if (Result.Val.isFloat())
          llvm::outs() << Result.Val.getFloat().convertToDouble();
        else
          llvm::outs() << "<non-int/float value>";
        llvm::outs() << "\n";
      }
    }
    return true;
  }

private:
  ASTContext *Context;
};

class EvalConsumer : public ASTConsumer {
public:
  explicit EvalConsumer(ASTContext *Context) : Visitor(Context) {}
  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  EvalVisitor Visitor;
};

class EvalAction : public ASTFrontendAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
    return std::make_unique<EvalConsumer>(&CI.getASTContext());
  }
};

static llvm::cl::OptionCategory NamingToolCategory("Const evaluation tool options");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, NamingToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
  return Tool.run(newFrontendActionFactory<EvalAction>().get());
}

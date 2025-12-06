#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Visitor that checks naming conventions
class NamingConventionVisitor : public RecursiveASTVisitor<NamingConventionVisitor> {
private:
  ASTContext *Context;

public:
  explicit NamingConventionVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    // Skip compiler-generated functions and main
    if (FD->isImplicit() || FD->getNameAsString() == "main")
      return true;

    std::string Name = FD->getNameAsString();
    
    // Check if name follows snake_case (lowercase with underscores)
    bool isSnakeCase = true;
    for (char c : Name) {
      if (std::isupper(c)) {
        isSnakeCase = false;
        break;
      }
    }

    if (!isSnakeCase) {
      DiagnosticsEngine &Diags = Context->getDiagnostics();
      unsigned DiagID = Diags.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "Function name '%0' does not follow snake_case convention");
      
      DiagnosticBuilder DB = Diags.Report(FD->getLocation(), DiagID);
      DB.AddString(Name);
    }

    return true;
  }
};

// AST Consumer
class NamingConventionConsumer : public ASTConsumer {
private:
  NamingConventionVisitor Visitor;

public:
  explicit NamingConventionConsumer(ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

// Frontend Action
class NamingConventionAction : public ASTFrontendAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef) override {
    return std::make_unique<NamingConventionConsumer>(&CI.getASTContext());
  }
};

static cl::OptionCategory NamingToolCategory("naming-checker options");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, NamingToolCategory);
  if (!ExpectedParser) {
    errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(newFrontendActionFactory<NamingConventionAction>().get());
}

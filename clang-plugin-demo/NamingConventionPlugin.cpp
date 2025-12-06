#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

// Visitor that checks naming conventions
class NamingConventionVisitor : public RecursiveASTVisitor<NamingConventionVisitor> {
private:
  ASTContext *Context;
  DiagnosticsEngine &Diags;
  unsigned DiagID;

public:
  NamingConventionVisitor(ASTContext *Context)
      : Context(Context), Diags(Context->getDiagnostics()) {
    DiagID = Diags.getCustomDiagID(DiagnosticsEngine::Warning,
                                    "Function name '%0' does not follow snake_case convention");
  }

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
      DiagnosticBuilder DB = Diags.Report(FD->getLocation(), DiagID);
      DB.AddString(Name);
    }

    return true;
  }
};

// AST Consumer that creates the visitor
class NamingConventionConsumer : public ASTConsumer {
private:
  NamingConventionVisitor Visitor;

public:
  NamingConventionConsumer(ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

// Plugin action that creates the consumer
class NamingConventionAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   llvm::StringRef) override {
    return std::make_unique<NamingConventionConsumer>(&CI.getASTContext());
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    // You can parse plugin arguments here
    for (const auto &arg : args) {
      llvm::errs() << "Naming convention plugin arg: " << arg << "\n";
    }
    return true;
  }

  ActionType getActionType() override {
    return AddAfterMainAction;
  }
};

// Register the plugin
static FrontendPluginRegistry::Add<NamingConventionAction>
X("naming-convention", "Check function naming conventions (snake_case)");

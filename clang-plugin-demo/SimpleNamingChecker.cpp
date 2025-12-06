// Simplified naming checker using Clang callbacks - NO RTTI REQUIRED
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"

using namespace clang;

namespace {

// Simple callback-based checker that doesn't use RecursiveASTVisitor
class SimpleNamingConsumer : public ASTConsumer {
private:
  CompilerInstance &CI;
  
  void checkDecl(Decl *D) {
    if (auto *FD = dyn_cast_or_null<FunctionDecl>(D)) {
      if (FD->isImplicit() || FD->getNameAsString() == "main")
        return;
      
      std::string Name = FD->getNameAsString();
      for (char c : Name) {
        if (std::isupper(c)) {
          DiagnosticsEngine &Diags = CI.getDiagnostics();
          unsigned ID = Diags.getCustomDiagID(DiagnosticsEngine::Warning,
              "Function '%0' should use snake_case");
          Diags.Report(FD->getLocation(), ID) << Name;
          break;
        }
      }
    }
  }
  
  void checkDeclContext(DeclContext *DC) {
    for (auto *D : DC->decls()) {
      checkDecl(D);
      if (auto *InnerDC = dyn_cast<DeclContext>(D))
        checkDeclContext(InnerDC);
    }
  }

public:
  SimpleNamingConsumer(CompilerInstance &CI) : CI(CI) {}
  
  void HandleTranslationUnit(ASTContext &Ctx) override {
    checkDeclContext(Ctx.getTranslationUnitDecl());
  }
};

class SimpleNamingAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   llvm::StringRef) override {
    return std::make_unique<SimpleNamingConsumer>(CI);
  }
  
  bool ParseArgs(const CompilerInstance &, const std::vector<std::string> &) override {
    return true;
  }
};

}

static FrontendPluginRegistry::Add<SimpleNamingAction>
X("simple-naming", "Simple snake_case checker (no RTTI)");

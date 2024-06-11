#include "pch.h"

namespace
{
    clang::Visibility g_DefaultTypeVisibility  = clang::DefaultVisibility;
    clang::Visibility g_DefaultValueVisibility = clang::DefaultVisibility;


    bool ShouldExport(clang::Sema& sema, const clang::NamedDecl* decl)
    {
        /************************************************************************
         * Initialize diagnostics                                               *
         ************************************************************************/
        auto&      diag              = sema.getDiagnostics();
        const auto invalidVisibility = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "declaration cannot be exported because of its visibility; exporting skipped!");
        const auto invalidAccess = diag.getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "non-public declaration cannot be exported; exporting skipped!");
        const auto invalidSignature = sema.getDiagnostics().getCustomDiagID(
            clang::DiagnosticsEngine::Warning,
            "%0 function '%1' cannot be exported; exporting skipped!");

        /************************************************************************
         * Ensure visibility of declaration to default                          *
         ************************************************************************/
        if (const auto vis = decl->getExplicitVisibility(clang::isa<clang::RecordDecl>(decl)
                ? clang::TypeDecl::VisibilityForType
                : clang::TypeDecl::VisibilityForValue);
            (vis.has_value() && vis != clang::DefaultVisibility) ||
            (clang::isa<clang::RecordDecl>(decl)
                ? g_DefaultTypeVisibility
                : g_DefaultValueVisibility) != clang::DefaultVisibility)
        {
            sema.Diag(decl->getLocation(), invalidVisibility);
            return false;
        }

        /************************************************************************
         * Check function constraints                                           *
         ************************************************************************/
        if (const auto fn = clang::dyn_cast<clang::FunctionDecl>(decl))
        {
            if (fn->isConsteval() || fn->isConstexpr())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "consteval/constexpr" << decl;
                return false;
            }
            if (fn->isVariadic())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "variadic" << decl;
                return false;
            }
            if (fn->isDeleted())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "deleted" << decl;
                return false;
            }
            if (fn->isInlined())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "inlined" << decl;
                return false;
            }
            if (fn->isPureVirtual())
            {
                sema.Diag(fn->getDefaultLoc(), invalidSignature) << "pure-virtual" << decl;
                return false;
            }
        }

        /************************************************************************
         * Check access modifiers                                               *
         ************************************************************************/
        auto record = clang::dyn_cast<clang::CXXRecordDecl>(decl->getDeclContext());
        if (!record)
            return true;

        auto access = clang::AS_none;
        for (auto* member: record->decls())
        {
            if (clang::isa<clang::AccessSpecDecl>(member))
                access = clang::cast<clang::AccessSpecDecl>(member)->getAccess();
            if (clang::declaresSameEntity(member, decl))
                break;
        }

        if (access == clang::AS_public ||
            (record->isStruct() && access == clang::AS_none))
            return true;

        sema.Diag(decl->getLocation(), invalidAccess);
        return false;
    }
}

namespace lilac::cxx
{
#define ATTR_NAME "apiexport"

    ExportAttrInfo::ExportAttrInfo()
    {
        OptArgs = 0;

        static constexpr Spelling S[] = {
            { clang::ParsedAttr::AS_GNU, ATTR_NAME },
            { clang::ParsedAttr::AS_Declspec, ATTR_NAME },
            { clang::ParsedAttr::AS_C23, ATTR_NAME },
            { clang::ParsedAttr::AS_CXX11, ATTR_NAME },
            { clang::ParsedAttr::AS_CXX11, "lilac::" ATTR_NAME },
        };

        Spellings = S;
    }

    bool ExportAttrInfo::diagAppertainsToDecl(
        clang::Sema&             sema,
        const clang::ParsedAttr& attr,
        const clang::Decl*       decl) const
    {
        /************************************************************************
         * Ensure whether the declaration can be exported                       *
         ************************************************************************/
        if (!clang::isa<clang::FunctionDecl>(decl) &&
            !clang::isa<clang::MSPropertyDecl>(decl) &&
            !clang::isa<clang::RecordDecl>(decl))
        {
            const auto invalidDeclType = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Warning,
                "'%0' attribute can be applied to only functions, records and properties; exporting skipped!");

            sema.Diag(attr.getLoc(), invalidDeclType) << attr;
            return false;
        }
        if (!ShouldExport(sema, clang::cast<clang::NamedDecl>(decl)))
            return false;

        return true;
    }

    clang::ParsedAttrInfo::AttrHandling ExportAttrInfo::handleDeclAttribute(
        clang::Sema&             sema,
        clang::Decl*             decl,
        const clang::ParsedAttr& attr) const
    {
        if (attr.getNumArgs())
        {
            const auto anyArgs = sema.getDiagnostics().getCustomDiagID(
                clang::DiagnosticsEngine::Warning,
                "'" ATTR_NAME "' attribute accepts no argument; exporting skipped!");
            sema.Diag(attr.getLoc(), anyArgs);
            return AttributeNotApplied;
        }

        // TODO

        return AttributeApplied;
    }

    bool ExportAttrInfo::acceptsLangOpts(const clang::LangOptions& LO) const
    {
        g_DefaultTypeVisibility  = LO.getTypeVisibilityMode();
        g_DefaultValueVisibility = LO.getValueVisibilityMode();
        return true;
    }
}

[[maybe_unused]]
static clang::ParsedAttrInfoRegistry::Add<lilac::cxx::ExportAttrInfo> X("apiexport", "");
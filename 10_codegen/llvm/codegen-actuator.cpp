#include <00_common/protocol.hpp>
#include <00_common/utils.hpp>
#include <languages/language-interface.hpp>
#include <light_header>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar.h>



class FuncationDeclareItem
{
public:
    string name;
    llvm::FunctionCallee callee;

    FuncationDeclareItem(
        const string & name,
        const llvm::FunctionCallee & callee):
            name(name),
            callee(callee)
    {
        // constructor
    }
};


vector<FuncationDeclareItem> function_decl_table;


////////////////////////////
// function declarations
////////////////////////////


static llvm::Value * codegen_process_statement_expression(
    const ExpressionAST & expression,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder);


////////////////////////////
// function implementations
////////////////////////////


static llvm::Type* get_llvm_int_N(
    int N,
    llvm::IRBuilder<> & builder)
{
    switch(N)
    {
        case 32:
            return builder.getInt32Ty();
            break;
        case 64:
            return builder.getInt64Ty();
            break;
        default:
            throw runtime_error("Not implemented. A281908327");
    }
}


static llvm::Type* get_llvm_nonpointer_type(
    const Context & frontend_context,
    const TypeDeclAST & type_decl,
    llvm::IRBuilder<> & builder)
{
    if(type_decl.category == TypeCategory::Class)
        throw runtime_error("Not implemented. A324692374027");
    if(type_decl.category == TypeCategory::Builtin)
    {
        switch(type_decl.builtin_type)
        {
            case BuiltinTypes::Invalid:
                throw runtime_error("Invalid builtin type. A239840274809");
            case BuiltinTypes::Int:
            {
                int arch_integer_bits = frontend_context.compiler_params.arch_integer_bits;
                return get_llvm_int_N(arch_integer_bits, builder);
            }
            case BuiltinTypes::Int8:
                return builder.getInt8Ty();
            case BuiltinTypes::Int16:
                return builder.getInt16Ty();
            case BuiltinTypes::Int32:
                return builder.getInt32Ty();
            case BuiltinTypes::Int64:
                return builder.getInt64Ty();
            case BuiltinTypes::UInt:
            {
                int arch_integer_bits = frontend_context.compiler_params.arch_integer_bits;
                return get_llvm_int_N(arch_integer_bits, builder);
            }
            case BuiltinTypes::UInt8:
                return builder.getInt8Ty();
            case BuiltinTypes::UInt16:
                return builder.getInt16Ty();
            case BuiltinTypes::UInt32:
                return builder.getInt32Ty();
            case BuiltinTypes::UInt64:
                return builder.getInt64Ty();
            case BuiltinTypes::Bool:
                throw runtime_error("Not implemented. A3258720398475");
            case BuiltinTypes::Char:
                return builder.getInt8Ty();
            default:
                throw runtime_error("Not implemented. A3049872093847");
        }
    }
    throw runtime_error("Code should not reach here. A246928374039");
}


static llvm::Type* get_llvm_type(
    const Context & frontend_context,
    const TypeDeclAST & type_decl,
    llvm::IRBuilder<> & builder)
{
    llvm::Type* llvm_type = get_llvm_nonpointer_type(frontend_context, type_decl, builder);
    for(int i = 0; i < type_decl.pointer_layers; i++)
        llvm_type = llvm_type->getPointerTo();
    return llvm_type;
}


static void add_function_param(
    std::vector<llvm::Type *> & args,
    const Context & frontend_context,
    const Module & frontend_module,
    const FuncParamDeclAST & param_decl,
    llvm::IRBuilder<> & builder
    )
{
    (void) frontend_context;
    (void) frontend_module;

    args.push_back(get_llvm_type(frontend_context, param_decl.param_type, builder));
}


// this function should handle both functions implementations and declarations
static llvm::Type* get_function_return_type(
    const Context & frontend_context,
    const Module & frontend_module,
    const vector<TypeDeclAST> & output_types,
    llvm::IRBuilder<> & builder
    )
{
    (void) frontend_module;

    if(output_types.size() > 1)
        throw runtime_error("At this point, the number of function return type number should not exceed one. A3820947509384");
    if(output_types.size() == 1)
    {
        const TypeDeclAST & type_decl = output_types[0];
        return get_llvm_type(frontend_context, type_decl, builder);
    }
    else
    {
        // void function
        return builder.getVoidTy();
    }
} 


static llvm::FunctionCallee get_callee(string function_name)
{
    for(const FuncationDeclareItem & fdi : function_decl_table)
    {
        if(fdi.name == function_name)
            return fdi.callee;
    }
    throw runtime_error("Function name '" + function_name + "' is not found in the declaration table. A238704874334");
}


static void codegen_process_only_function_decl(
    int function_decl_index,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder
    )
{
    assert(function_decl_index >= 0);
    assert(function_decl_index < (int) frontend_module.ast.decl_only_functions.size());
    FunctionAST & frontend_function_decl = frontend_module.ast.decl_only_functions[function_decl_index];
    assert(frontend_function_decl.is_decl_only);

    std::vector<llvm::Type *> args; // function input arguments
    for(const FuncParamDeclAST & param_decl : frontend_function_decl.input_params)
    {
        add_function_param(args, frontend_context, frontend_module, param_decl, builder);
    }

    llvm::ArrayRef<llvm::Type*>  argsRef(args);

    llvm::Type* return_type = get_function_return_type(
        frontend_context, frontend_module, frontend_function_decl.output_types, builder);

    llvm::FunctionType *function_type = 
        llvm::FunctionType::get(builder.getInt32Ty(), argsRef, false);

    assert(frontend_function_decl.func_name.length() > 0);
    llvm::FunctionCallee function_callee = llvm_module->getOrInsertFunction(frontend_function_decl.func_name, function_type);

    function_decl_table.push_back(FuncationDeclareItem(
        frontend_function_decl.func_name,
        function_callee));
}


static llvm::Value * codegen_process_statement_expression_literal_string(
    const ExpressionAST & expression,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder)
{
    assert(expression.type == ExpressionType::String);
    assert(expression.func_name == "");
    assert(expression.literal_token_index >= 0);
    const Token & token = get_token(frontend_module, expression.literal_token_index);
    return builder.CreateGlobalStringPtr(token.encoded_text);
}


static llvm::Value * codegen_process_statement_expression_literal_number(
    const ExpressionAST & expression,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder)
{
    // todo: support all ranges of integer numbers
    assert(expression.type == ExpressionType::Number);
    assert(expression.func_name == "");
    assert(expression.literal_token_index >= 0);
    const Token & token = get_token(frontend_module, expression.literal_token_index);
    int int_value = language::get_number_value(token);
    return llvm::ConstantInt::get(llvm_context, llvm::APInt(64, int_value));
}


static llvm::Value * codegen_process_statement_expression_function_call(
    const ExpressionAST & expression,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder)
{
    assert(expression.type == ExpressionType::FunctionCall);
    assert(expression.func_name != "");

    vector<llvm::Value *> args;
    for(const ExpressionAST & param : expression.func_params)
    {
        llvm::Value *param_value = codegen_process_statement_expression(param, frontend_context, frontend_module, llvm_context, llvm_module, builder);
        args.push_back(param_value);
    }

    llvm::FunctionCallee function_callee = get_callee(expression.func_name);
    return builder.CreateCall(function_callee, args);
    // todo: if function has a return value, it should be called this way:
    // Value* recur_2 = builder.CreateCall(gcd, args2.begin(), args2.end(), "tmp");
    // builder.CreateRet(recur_2);
}


static llvm::Value * codegen_process_statement_expression(
    const ExpressionAST & expression,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder)
{
    switch(expression.type)
    {
        case ExpressionType::undefined:
            throw runtime_error("Not implemented. A47534564358");
            break; // to suppress a warning 
        case ExpressionType::id:
            throw runtime_error("Not implemented. A47532983443");
            break; // to suppress a warning 
        case ExpressionType::Number:
            return codegen_process_statement_expression_literal_number(expression, frontend_context, frontend_module, llvm_context, llvm_module, builder);
        case ExpressionType::String:
            return codegen_process_statement_expression_literal_string(expression, frontend_context, frontend_module, llvm_context, llvm_module, builder);
        case ExpressionType::Assignment:
            throw runtime_error("Not implemented. A47533495234");
            break; // to suppress a warning 
        case ExpressionType::Addition:
            throw runtime_error("Not implemented. A47530943239");
            break; // to suppress a warning 
        case ExpressionType::FunctionCall:
            return codegen_process_statement_expression_function_call(expression, frontend_context, frontend_module, llvm_context, llvm_module, builder);
        case ExpressionType::ParenthesisWrap:
            throw runtime_error("Not implemented. A47533948532");
            break; // to suppress a warning 
        default:
            throw runtime_error("Not implemented. A23984503287");
    }

}

static void codegen_process_statement(
    const StatementAST & statement,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder)
{
    switch(statement.type)
    {
        case StatementType::Invalid:
            throw runtime_error("Code should not reach here. A23849702847");
            break;
        case StatementType::Expression:
            codegen_process_statement_expression(*((ExpressionAST *) statement.data), frontend_context, frontend_module, llvm_context, llvm_module, builder);
            break;
        case StatementType::Jump:
            throw runtime_error("Not implemented. A2836483763984");
            break;
        case StatementType::DeclareVar:
            throw runtime_error("Not implemented. A9438750384");
            break;
        case StatementType::DeclareType:
            throw runtime_error("Not implemented. A098465032334");
            break;
        case StatementType::ControlBlock:
            throw runtime_error("Not implemented. A324895703875");
            break;
        default:
            throw runtime_error("Not implemented. A249830573948");
    }
}


static void codegen_process_function(
    int function_index,
    Context & frontend_context,
    Module & frontend_module,
    llvm::LLVMContext & llvm_context,
    llvm::Module *llvm_module,
    llvm::IRBuilder<> & builder
    )
{
    assert(function_index >= 0);
    assert(function_index < (int) frontend_module.ast.functions.size());
    const FunctionAST & frontend_function = frontend_module.ast.functions[function_index];

    // ----------------
    // return value
    llvm::Type * ret_type = nullptr;

    // functions with multiple return values should have been already re-structured
    assert(frontend_function.output_types.size() <= 1);

    ret_type = get_function_return_type(
        frontend_context, frontend_module, 
        frontend_function.output_types, builder);
    assert(ret_type != nullptr);

    // ----------------
    // input arguments
    llvm::ArrayRef<llvm::Type*> input_arg_types;

    if(frontend_function.input_params.size() > 0)
        throw runtime_error("Not implemented. A38429734093873");

    // ----------------
    // function type
    llvm::FunctionType *func_type =
        llvm::FunctionType::get(ret_type, input_arg_types, false);

    llvm::Function *llvm_func = 
        llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, frontend_function.func_name, llvm_module);

    // if main function, set 'entrypoint' label
    if(frontend_function.is_entry)
    {
        cout<< "entry point detected at " << frontend_function.func_name << endl;
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(llvm_context, "entrypoint", llvm_func);
        builder.SetInsertPoint(entry);
    }

    const CompoundStmtAST & function_body = frontend_function.compound_stmt;
    for(const StatementAST & statement : function_body.statements)
    {
        codegen_process_statement(statement, frontend_context, frontend_module, llvm_context, llvm_module, builder);
    }

    if(frontend_function.output_types.size() == 0)
        builder.CreateRetVoid();
    else
        throw runtime_error("Not implemented. A43987520348");
}

// static void optimizeLLVMModule(llvm::Module& llvmModule)
// {
//     llvm::legacy::FunctionPassManager fpm(&llvmModule);
//     fpm.add(llvm::createPromoteMemoryToRegisterPass());
//     fpm.add(llvm::createInstructionCombiningPass());
//     fpm.add(llvm::createCFGSimplificationPass());
//     fpm.add(llvm::createJumpThreadingPass());
//     fpm.add(llvm::createConstantPropagationPass());

//     // This DCE pass is necessary to work around a bug in LLVM's CodeGenPrepare that's triggered
//     // if there's a dead div/rem with limited-range divisor:
//     // https://bugs.llvm.org/show_bug.cgi?id=43514
//     fpm.add(llvm::createDeadCodeEliminationPass());

//     fpm.doInitialization();
//     for(auto functionIt = llvmModule.begin(); functionIt != llvmModule.end(); ++functionIt)
//     {
//         fpm.run(*functionIt);
//     }
// }


static void codegen_process_module(
    Context & frontend_context,
    int module_index,
    llvm::LLVMContext & llvm_context,
    llvm::IRBuilder<> & builder,
    std::string &targetTriple,
    llvm::TargetMachine * theTargetMachine)
{
    Module & frontend_module = frontend_context.modules[module_index];
    if(frontend_module.stage == Stage::Reformer)
        frontend_module.stage = Stage::Codegen;
    else
        throw runtime_error("Wrong stage. A43298750387");

    // code generation

    cout << "....................." << endl;
    cout << "codegen for " <<  frontend_module.filepath <<endl;

    llvm::Module *llvm_module = new llvm::Module(
        frontend_module.filepath, llvm_context);

    for(int i = 0; i < (int) frontend_module.ast.decl_only_functions.size(); i++)
    {
        codegen_process_only_function_decl(
            i, frontend_context, frontend_module,
            llvm_context, llvm_module, builder);
    }

    for(int i = 0; i < (int) frontend_module.ast.functions.size(); i++)
    {
        codegen_process_function(
            i, frontend_context, frontend_module,
            llvm_context, llvm_module, builder);
    }

    // optimize output
    // ????????????????????
    // optimizeLLVMModule(llvm_module);

    ///////////////////////////

    llvm_module->setTargetTriple(targetTriple);

    llvm_module->setDataLayout(theTargetMachine->createDataLayout());

    // path of object and ir files
    // todo: check sanity of path
    fs::path artifact_folder = fs::path(language::artifacts_folder_name()); 
    string obj_extension = language::object_file_extention();
    string ir_extension = language::ir_file_extention();
    fs::path relative_sourthpath =  fs::relative(frontend_module.filepath, fs::current_path());
    fs::path obj_filepath =  artifact_folder / relative_sourthpath.replace_extension(obj_extension);
    fs::path ir_filepath =  artifact_folder / relative_sourthpath.replace_extension(ir_extension);
    fs::path parent_folder {obj_filepath.parent_path()};
    if(!fs::exists(parent_folder))
        fs::create_directories(parent_folder);

    // store ir file
    {
        std::error_code EC;
        cout << "Writing disassembled IR to: " << ir_filepath.c_str() << endl;
        llvm::raw_fd_ostream ir_dest(ir_filepath.c_str(), EC, llvm::sys::fs::OF_None);
        llvm_module->print(ir_dest, nullptr);
        ir_dest.flush();
        if(EC)
        {
            llvm::errs() << "Could not open file: " << EC.message();
            throw runtime_error("There is an error. A43769786564");
        }
    }

    // store object file
    {
        std::error_code EC;
        llvm::raw_fd_ostream dest(obj_filepath.c_str(), EC, llvm::sys::fs::OF_None);
        if(EC)
        {
            llvm::errs() << "Could not open file: " << EC.message();
            throw runtime_error("There is an error. A3943720384");
        }

        llvm::legacy::PassManager pass;
        auto FileType = llvm::LLVMTargetMachine::CGFT_ObjectFile;

        if(theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
        {
            llvm::errs() << "theTargetMachine can't emit a file of this type";
            throw runtime_error("There is an error. A237460448750");
        }

        pass.run(*llvm_module);
        dest.flush();
        
        llvm::outs() << "Wrote obj file: " << obj_filepath << "\n";
    }
}


void codegen_process(Context &code_context)
{
    llvm::LLVMContext llvm_context;

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    llvm::IRBuilder<> builder(llvm_context);
    std::string targetTriple = llvm::sys::getDefaultTargetTriple();

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target)
    {
        llvm::errs() << Error;
        throw runtime_error("There is an error. A38475203847");
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    llvm::TargetMachine * theTargetMachine =
        Target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

    for(int i = 0; i < (int) code_context.modules.size(); i ++)
    {
        Module & module = code_context.modules[i];
        codegen_process_module(code_context, i, 
            llvm_context, builder, targetTriple, theTargetMachine);
        if(has_any_error(module))
            module.stage = Stage::Finalized;
    }

    delete theTargetMachine;
}

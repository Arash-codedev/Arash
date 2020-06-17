
class TypeDeclAST;
class FuncParamDeclAST;
class FunctionAST;
class StatementAST;
class VarDeclAST;
class ControlBlockAST;
class ExpressionAST;


enum class BuiltinTypes
{
    Invalid,
    Int,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Bool,
    Char,
};


enum class TypeCategory
{
    Builtin,
    Class,
};


enum class StatementType
{
    Invalid,
    Expression,
    Jump,
    DeclareVar,
    DeclareType,
    ControlBlock,
};


enum class ExpressionType
{
    undefined,
    //
    id,
    Number,
    String,
    Assignment,
    Addition,
    FunctionCall,
    ParenthesisWrap,
};


enum class JumpType
{
    Invalid,
    Break,
    Continue,
    Return,
};


enum class OperatorType
{
    // ----- Arithmetic -----
    /*  +  */ Addition,
    /*  -  */ Subtract,
    /* +   */ UnaryPlus,
    /* -   */ UnaryMinus,
    /*  *  */ Multiplication,
    /*  /  */ Division,
    /*  %  */ Modulus,
    /* ++  */ IncrementPrefix,
    /*  ++ */ IncrementPostfix,
    /* --  */ DecrementPrefix,
    /*  -- */ DecrementPostfix,
    // ----- Basic Assignment -----
    /*  =  */ IndependentAssignment,
    /*  := */ InblockAssignment,
    // ----- Compound assignment -----
    /*  += */ AdditionAssignment,
    /*  -= */ SubtractionAssignment,
    /*  *= */ MultiplicationAssignment,
    /*  /= */ DivisionAssignment,
    /*  %= */ ModuloAssignment,
    /*  &= */ BitwiseAndAssignment,
    /*  |= */ BitwiseOrAssignment,
    /*  ^= */ BitwiseXorAssignment,
    /* <<= */ BitwiseLeftShiftAssignment,
    /* >>= */ BitwiseRightShiftAssignment,
    // ----- Bitwise -----
    /*  ~  */ BitwiseNot,
    /*  &  */ BitwiseAnd,
    /*  |  */ BitwiseOr,
    /*  ^  */ BitwiseXor,
    /*  << */ BitwiseLeftShift,
    /*  >> */ BitwiseRightShift,
    // ----- Logical -----
    /*  !  */ LogicalNot,
    /*  && */ LogicalAnd, 
    /*  || */ LogicalOr,
    // ----- Comparison -----
    /*  == */ EqualTo,
    /*  != */ NotEqualTo,
    /*  >  */ GreaterThan,
    /*  <  */ LessThan,
    /*  >= */ GreaterThanOrEqual,
    /*  <= */ LessThanOrEqual,
    /* <=> */ ThreeWayComparison,
};


static int ast_counter = 0;



// example:
//     body
//         var x = 5 : Int
//         var y = 6 : Int
//         var z = sum(z,y) : Int
//         print_int(z)
//         return 0
//
class CompoundStmtAST
{
public:

    int ast_index = -1;
    int module_index = -1;

    // vector<ClassAST*> pClasses;
    // vector<ASTCompoundOrder> orders;

    vector<StatementAST> statements;
    // vector<FunctionAST*> functions;
    // vector<VarDeclAST*> declarations;

    CompoundStmtAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(statements)
            ;
    }
};



// example:
//      #sys_decl
//      #packed, ordered
//      #path('/login'), tagged('security', 'user'), no_mangle
class Attribute
{
public:
    int ast_index = -1;
    int module_index = -1;
    int token_index = -1;
    string attr_name;
    vector<ExpressionAST> params;

    Attribute()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(attr_name)
            & BOOST_SERIALIZATION_NVP(params)
            ;
    }
};



// example:
//     func sum
//         copy x : Int
//         copy y : Int
//         -> Int
//     body
//         var output = x + y
//         return output
//
class FunctionAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    string func_name;
    vector<Attribute> attributes;
    bool is_entry = false;
    bool is_decl_only = false;
    bool is_sys_decl = false; // calculated from attributes
    bool is_variadic = false;
    vector<FuncParamDeclAST> input_params;
    vector<TypeDeclAST> output_types;
    CompoundStmtAST compound_stmt;

    FunctionAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(func_name)
            & BOOST_SERIALIZATION_NVP(attributes)
            & BOOST_SERIALIZATION_NVP(is_entry)
            & BOOST_SERIALIZATION_NVP(is_decl_only)
            & BOOST_SERIALIZATION_NVP(is_sys_decl)
            & BOOST_SERIALIZATION_NVP(is_variadic)
            & BOOST_SERIALIZATION_NVP(input_params)
            & BOOST_SERIALIZATION_NVP(output_types)
            & BOOST_SERIALIZATION_NVP(compound_stmt)
            ;
    }
};



// examples:
//     Int
//     MyStruct
//
class TypeDeclAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    bool initialized = false;
    bool is_builtin = true;
    string class_name;
    TypeCategory category = TypeCategory::Builtin;
    BuiltinTypes builtin_type = BuiltinTypes::Invalid;
    int pointer_layers = 0; // 0: is not a pointer, 1: is pointer, 2: pointer to pointer, etc.


    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(initialized)
            & BOOST_SERIALIZATION_NVP(is_builtin)
            & BOOST_SERIALIZATION_NVP(class_name)
            & BOOST_SERIALIZATION_NVP(category)
            & BOOST_SERIALIZATION_NVP(builtin_type)
            & BOOST_SERIALIZATION_NVP(pointer_layers)
            ;
    }

    // int class_ast_ref_index;
    // out of serialization
};


enum class FunctionParamQualifier
{
    Uninitialized,
    Copy,
    Const,
    Cref,
    Ref,
};


// examples:
//     copy y : Int
//     -> Int
//
class FuncParamDeclAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    string var_name;
    bool is_const = false;
    FunctionParamQualifier qualifier = FunctionParamQualifier::Uninitialized;
    int type_ast_index= -1;
    TypeDeclAST param_type;

    FuncParamDeclAST()
    {
        ast_index = ++ast_counter;
    }


    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(var_name)
            & BOOST_SERIALIZATION_NVP(is_const)
            & BOOST_SERIALIZATION_NVP(qualifier)
            & BOOST_SERIALIZATION_NVP(type_ast_index)
            & BOOST_SERIALIZATION_NVP(param_type)
            ;
    }
};


// example:
//      var x = 5 : Int
//
class VarDeclAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    vector<int> var_name_tokens;
    bool is_const = false;
    // int type_ast_index = -1;
    vector<ExpressionAST> initial_val; // with maximum one member
    vector<ExpressionAST> type;        // with maximum one member

    VarDeclAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(var_name_tokens)
            & BOOST_SERIALIZATION_NVP(is_const)
            & BOOST_SERIALIZATION_NVP(initial_val)
            & BOOST_SERIALIZATION_NVP(type)
            ;
    }

    // out of serialization
    // TypeDeclAST * param_type = nullptr;
};


class StringConstLiteral
{
    int ast_index = -1;
    int module_index = -1;
    string value = "";

    StringConstLiteral()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(value)
            ;
    }
};


class ModuleAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    vector<FunctionAST> functions;
    vector<FunctionAST> decl_only_functions;
    vector<StringConstLiteral> string_const_literals;
    // vector<ClassAST*> classes;

    ModuleAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(functions)
            & BOOST_SERIALIZATION_NVP(decl_only_functions)
            ;
    }
};



// example
//     x + y * (z * z)
//
class ExpressionAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    ExpressionType type;
    vector<OperatorType> operators;
    vector<ExpressionAST> operands;
    // question: why do we need two token indexes?
    int literal_token_index = -1; // only if the expression is a single id (e.g. myObj) (i.e type==number|string)
    int id_token_index = -1; // only if the expression is a single id (e.g. myObj) (i.e type==id)
    vector<ExpressionAST> func_params;
    int func_name_token_index = -1;
    string func_name;

    ExpressionAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(type)
            & BOOST_SERIALIZATION_NVP(operators)
            & BOOST_SERIALIZATION_NVP(operands)
            & BOOST_SERIALIZATION_NVP(literal_token_index)
            & BOOST_SERIALIZATION_NVP(id_token_index)
            & BOOST_SERIALIZATION_NVP(func_params)
            & BOOST_SERIALIZATION_NVP(func_name_token_index)
            & BOOST_SERIALIZATION_NVP(func_name)
            ;
    }
};



// example
//     return x + y * (z * z)
//     break
//     continue
//
class JumpAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    JumpType type;
    vector<ExpressionAST> return_values;

    JumpAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(type)
            & BOOST_SERIALIZATION_NVP(return_values)
            ;
    }
};



// examples:
//     x + y * (z * z)
//     break
//     print(x)
//     copy x : Int
//     if x > y
//
class StatementAST
{
public:
    int ast_index = -1;
    int module_index = -1;
    StatementType type = StatementType::Invalid;
    void * data = nullptr;

    StatementAST()
    {
        ast_index = ++ast_counter;
    }

    template <typename Archive>
    void serialize(Archive &ar, unsigned) 
    {
        ar
            & BOOST_SERIALIZATION_NVP(ast_index)
            & BOOST_SERIALIZATION_NVP(module_index)
            & BOOST_SERIALIZATION_NVP(type)
            ;

        if (Archive::is_loading::value)
            assert(this->data == nullptr);

        switch(this->type)
        {
            case StatementType::Expression:
                if (Archive::is_loading::value)
                    this->data = (void *) new ExpressionAST();
                ar & BOOST_SERIALIZATION_NVP(*((ExpressionAST *) data));
                break;
            case StatementType::Jump:
                if (Archive::is_loading::value)
                    this->data = (void *) new JumpAST();
                ar & BOOST_SERIALIZATION_NVP(*((JumpAST *) data));
                break;
            case StatementType::DeclareVar:
                if (Archive::is_loading::value)
                    this->data = (void *) new VarDeclAST();
                ar & BOOST_SERIALIZATION_NVP(*((VarDeclAST *) data));
                break;
            default:
                throw runtime_error("Unknown statement AST type: " + to_string((int) this->type) + ". A348752903487");
        }
    }

    StatementAST(const StatementAST &obj)
    {
        *this = obj;
        if(obj.data != nullptr)
        {
            switch(this->type)
            {
                case StatementType::Expression:
                    replicate_pointers<ExpressionAST>(obj.data, this->data);
                    break;
                case StatementType::Jump:
                    replicate_pointers<JumpAST>(obj.data, this->data);
                    break;
                case StatementType::DeclareVar:
                    replicate_pointers<VarDeclAST>(obj.data, this->data);
                    break;
                default:
                    throw runtime_error("Unknown statement AST type: " + to_string((int) this->type) + ". A23487503499");
            }
        }
    }

    ~StatementAST()
    {
        // make sure there is no other pointer in this class
        switch(this->type)
        {
            case StatementType::Expression:
                assert(data != nullptr);
                delete (ExpressionAST *) data;
                data = nullptr;
                break;
            case StatementType::Jump:
                assert(data != nullptr);
                delete (JumpAST *) data;
                data = nullptr;
                break;
            case StatementType::DeclareVar:
                assert(data != nullptr);
                delete (VarDeclAST *) data;
                data = nullptr;
                break;
            default:
                // Cannot throw an exception in a destructor
                cout << "Unknown statement AST type: " << (int) this->type << endl;
                exit(1);
        }
    }
};


class ControlBlockAST
{
public:
    // to be filled
};

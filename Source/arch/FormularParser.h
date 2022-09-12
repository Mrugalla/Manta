#pragma once
#include <juce_core/juce_core.h>

namespace parser
{
    using Char = juce::juce_wchar;
    using String = juce::String;

    enum class Type
    {
        Operand,
        X,
        Random,
        OpBinary,
        OpUnary,
        NumTypes
    };

    inline String toString(Type t)
    {
        switch (t)
        {
        case Type::Operand: return "oprnd";
        case Type::X: return "x";
        case Type::Random: return "rand";
        case Type::OpBinary: return "opBin";
        case Type::OpUnary: return "upUn";
        default: return "invalid";
        }
    }

    enum class OpUnary
    {
        Asinh,
        Acosh,
        Atanh,
        Floor,
        Log10,
        Noise,
        Asin,
        Acos,
        Atan,
        Ceil,
        Cosh,
        Log2,
        Sinh,
        Sign,
        Sqrt,
        Tanh,
        Abs,
        Cos,
        Exp,
        Sin,
        Tan,
        Log,
        Ln,
        NumOpUnary
    };

    static constexpr int NumOpUnary = static_cast<int>(OpUnary::NumOpUnary);

    enum class OpBinary
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Power,
        Modulo,
        ParenthesisOpen,
        ParenthesisClose,
        NumOpBinary
    };

    static constexpr int NumOpBinary = static_cast<int>(OpBinary::NumOpBinary);

    inline String toString(OpUnary opUnary)
    {
        switch (opUnary)
        {
        case OpUnary::Asinh: return "ainh";
        case OpUnary::Acosh: return "acosh";
        case OpUnary::Atanh: return "atanh";
        case OpUnary::Floor: return "floor";
        case OpUnary::Log10: return "log10";
        case OpUnary::Noise: return "noise";
        case OpUnary::Asin: return "asin";
        case OpUnary::Acos: return "acos";
        case OpUnary::Atan: return "atan";
        case OpUnary::Ceil: return "ceil";
        case OpUnary::Sign: return "sign";
        case OpUnary::Sinh: return "sinh";
        case OpUnary::Sqrt: return "sqrt";
        case OpUnary::Cosh: return "cosh";
        case OpUnary::Log2: return "log2";
        case OpUnary::Tanh: return "tanh";
        case OpUnary::Abs: return "abs";
        case OpUnary::Cos: return "cos";
        case OpUnary::Exp: return "exp";
        case OpUnary::Sin: return "sin";
        case OpUnary::Tan: return "tan";
        case OpUnary::Log: return "log";
        case OpUnary::Ln: return "ln";
        default: return "invalid";
        }
    }

    inline Char toChar(OpBinary opBinary) noexcept
    {
        switch (opBinary)
        {
        case OpBinary::Add: return '+';
        case OpBinary::Subtract: return '-';
        case OpBinary::Multiply: return '*';
        case OpBinary::Divide: return '/';
        case OpBinary::Power: return '^';
        case OpBinary::Modulo: return '%';
        case OpBinary::ParenthesisOpen: return '(';
        case OpBinary::ParenthesisClose: return ')';
        default: return '0';
        }
    }

    inline OpBinary toOpBinary(Char opBinary) noexcept
    {
        switch (opBinary)
        {
        case '+': return OpBinary::Add;
        case '-': return OpBinary::Subtract;
        case '*': return OpBinary::Multiply;
        case '/': return OpBinary::Divide;
        case '^': return OpBinary::Power;
        case '%': return OpBinary::Modulo;
        case '(': return OpBinary::ParenthesisOpen;
        case ')': return OpBinary::ParenthesisClose;
        default: return OpBinary::NumOpBinary;
        }
    }

    inline String toString(OpBinary opBinary)
    {
        auto chr = toChar(opBinary);
        if (chr == '0')
            return "invalid";
        else
            return String::charToString(chr);
    }

    inline int getDigit(const Char ltr) noexcept
    {
        switch (ltr)
        {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default: return -1;
        }
    }

    inline bool isDigit(const Char ltr) noexcept
    {
        return getDigit(ltr) != -1;
    }

    inline int getPrecedence(OpBinary opBinary) noexcept
    {
        switch (opBinary)
        {
        case OpBinary::Add: return 0;
        case OpBinary::Subtract: return 0;

        case OpBinary::Multiply: return 1;
        case OpBinary::Divide: return 1;
        case OpBinary::Modulo: return 1;

        case OpBinary::Power: return 2;

        case OpBinary::ParenthesisOpen: return 3;
        case OpBinary::ParenthesisClose: return 3;

        default: return 4;
        }
    }

    inline int getAssociativity(OpBinary opBinary) noexcept
    {
        switch (opBinary)
        {
        case OpBinary::Add: return -1;
        case OpBinary::Subtract: return -1;
        case OpBinary::Multiply: return -1;
        case OpBinary::Divide: return -1;
        case OpBinary::Modulo: return -1;

        case OpBinary::Power: return 1;
        case OpBinary::ParenthesisOpen: return 1;
        case OpBinary::ParenthesisClose: return 1;

        default: return -1;
        }
    }

    inline OpUnary getUnaryOperator(const String& ltr)
    {
        for (auto i = 0; i < NumOpUnary; ++i)
        {
            auto op = static_cast<OpUnary>(i);
            if (ltr == toString(op))
                return op;
        }
        return OpUnary::NumOpUnary;
    }

    inline OpBinary getBinaryOperator(const String& ltr)
    {
        for (auto i = 0; i < NumOpBinary; ++i)
        {
            auto op = static_cast<OpBinary>(i);
            if (ltr == toString(op))
                return op;
        }
        return OpBinary::NumOpBinary;
    }

    inline bool isOperator(const String& ltr)
    {
        return getBinaryOperator(ltr) != OpBinary::NumOpBinary ||
            getUnaryOperator(ltr) != OpUnary::NumOpUnary;
    }

    inline float getNumber(const String& term) noexcept
    {
        const auto sign = term[0] == '-' ? -1 : 1;
        const auto start = sign == 1 ? 0 : 1;
        
        auto decimal = term.length();
        for (auto i = start; i < term.length(); ++i)
            if (term[i] == '.')
            {
                decimal = i;
                break;
            }

        auto number = 0.f;
        auto multiplier = 1.f;
        for (auto i = decimal - 1; i >= start; --i)
        {
            const auto ltr = term[i];
            const auto ltrNum = getDigit(ltr);
            number += ltrNum * multiplier;
            multiplier *= 10.f;
        }

        multiplier = .1f;
        for (auto i = decimal + 1; i < term.length(); ++i)
        {
            const auto ltr = term[i];
            const auto ltrNum = getDigit(ltr);
            number += ltrNum * multiplier;
            multiplier *= .1f;
        }

        return number * sign;
    }

    struct Token
    {
        Token(Type _type, OpBinary _opBinary, OpUnary _opUnary,
            float _value, int _precedence, int _associativity) :

            type(_type),
            opBinary(_opBinary),
            opUnary(_opUnary),
            value(_value),
            precedence(_precedence),
            associativity(_associativity)
        {}

        Type type;
        OpBinary opBinary;
        OpUnary opUnary;
        float value;
        int precedence, associativity; // asc: -1 == left to right
    };

    inline String toString(Token token)
    {
        String str(toString(token.type));
        
        switch (token.type)
        {
        case Type::Random:
        case Type::Operand:
            str += ": " + String(token.value);
            break;
        case Type::OpBinary:
            str += ": " + toString(token.opBinary);
            break;
        case Type::OpUnary:
            str += ": " + toString(token.opUnary);
            break;
        case Type::X:
            str += ": x";
            break;
        }

        return str + "\nprec: " + String(token.precedence)
            + " assc: " + String(token.associativity) + "\n";
    }

    namespace makeToken
    {
        inline Token makeX()
        {
            return
            {
                Type::X,
                OpBinary::Add,
                OpUnary::Sin,
                0.f,
                4,
                1
            };
        }

        inline Token opBinary(OpBinary opBinary)
        {
            return
            {
                Type::OpBinary,
                opBinary,
                OpUnary::Sin,
                0.f,
                getPrecedence(opBinary),
                getAssociativity(opBinary)
            };
        }

        inline Token opUnary(OpUnary opUnary) noexcept
        {
            return
            {
                Type::OpUnary,
                OpBinary::Add,
                opUnary,
                0.f,
                1,
                1
            };
        }

        inline Token operand(float value) noexcept
        {
            return
            {
                Type::Operand,
                OpBinary::Add,
                OpUnary::Sin,
                value,
                0,
                0
            };
        }

        inline Token rand() noexcept
        {
            return
            {
                Type::Random,
                OpBinary::Add,
                OpUnary::Sin,
                0.f,
                4,
                1
            };
        }
    }

    namespace
    {
        inline float operate(Token& token, float a) noexcept
        {
            switch (token.opUnary)
            {
            case OpUnary::Abs: return std::abs(a);
            case OpUnary::Acos: return std::acos(a);
            case OpUnary::Acosh: return std::acosh(a);
            case OpUnary::Asin: return std::asin(a);
            case OpUnary::Asinh: return std::asinh(a);
            case OpUnary::Atan: return std::atan(a);
            case OpUnary::Atanh: return std::atanh(a);
            case OpUnary::Ceil: return std::ceil(a);
            case OpUnary::Cos: return std::cos(a);
            case OpUnary::Cosh: return std::cosh(a);
            case OpUnary::Exp: return std::exp(a);
            case OpUnary::Floor: return std::floor(a);
            case OpUnary::Log: return std::log(a);
            case OpUnary::Log2: return std::log2(a);
            case OpUnary::Log10: return std::log10(a);
            case OpUnary::Sign: return a < 0 ? -1.f : 1.f;
            case OpUnary::Sin: return std::sin(a);
            case OpUnary::Sinh: return std::sinh(a);
            case OpUnary::Sqrt: return std::sqrt(a);
            case OpUnary::Tan: return std::tan(a);
            case OpUnary::Tanh: return std::tanh(a);
            case OpUnary::Noise: return juce::Random::getSystemRandom().nextFloat() * a;
            case OpUnary::Ln: return std::log(a);
            default: return 0.f;
            }
        }

        inline float operate(Token& token, float a, float b) noexcept
        {
            switch (token.opBinary)
            {
            case OpBinary::Add: return a + b;
            case OpBinary::Subtract: return a - b;
            case OpBinary::Multiply: return a * b;
            case OpBinary::Divide:
                if (b == 0.f)
                    return 1.f;
                return a / b;
            case OpBinary::Modulo: return std::fmod(a, b);
            case OpBinary::Power:
                if (a < 0)
                    return 0;
                else
                    return std::pow(a, b);
            default: return 0.f;
            }
        }
    }

    using Tokens = std::vector<Token>;

    struct Parser
    {
        Parser(int _resolution = 1) :
            tokens(),
#if PPD_DebugFormularParser
            message("enter formular!"),
#endif
            values(),
            v0(),
            v1(),
            res(0)
        {
            setResolution(_resolution, true);
        }

        void setResolution(int r, bool forced = false)
        {
            if (res == r && !forced)
                return;

            values.resize(r);
            v0.resize(r, 0.f);
            v1.resize(r, 0.f);
            res = r;
        }

        bool operator()(String txt)
        {
            if (txt.isEmpty())
            {
#if PPD_DebugFormularParser
                message = "enter formular!";
#endif
                return false;
            }

            txt = txt.toLowerCase();

            tokens.clear();

            for (auto& v : values)
                v.clear();

            if (tokenize(txt))
            {
#if PPD_DebugFormularParser
                dbgTokens();
#endif
                if (toPostfix())
                {
#if PPD_DebugFormularParser
                    dbgTokens();
#endif
                    if (validate())
                    {
                        if (calculate())
                        {
#if PPD_DebugFormularParser
                            dbgCurve();
#endif
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        // GET
        const std::vector<float>& curve() const noexcept { return v0; }
        const float operator[](int i) const noexcept { return v0[i]; }

#if PPD_DebugFormularParser
        const String& getMessage() const noexcept { return message; }
#else
        String getMessage() const noexcept { return ""; }
#endif

        void dbgCurve() const
        {
#if PPD_DebugFormularParser
            String str(message + ":\n");
#else
            String str("DBG:\n");
#endif
            for (auto v = 0; v < v0.size() - 1; ++v)
                str += String(v0[v]) + ", ";
            str += String(v0[v0.size() - 1]);
            DBG(str);
        }

    protected:
        Tokens tokens;
#if PPD_DebugFormularParser
        String message;
#endif
        std::vector<std::vector<float>> values;
        std::vector<float> v0, v1;
        int res;

        bool tokenize(const String& infx)
        {
            String token;
            int parenthesisHelper = 420;
            bool parenthesisInit = false;
            bool parenthesisSearching = false;
            const auto infix = infx.removeCharacters(" ");

            for (auto i = 0; i < infix.length(); ++i)
            {
                const auto ltr = infix[i];

                // check for DIGIT of NUMBER
                if (isDigit(ltr))
                    token += ltr;

                // check for X

                else if (infix[i] == 'x')
                {
                    tokens.push_back(makeToken::makeX());
                    if (token[0] == '-')
                    {
                        tokens.push_back(makeToken::opBinary(OpBinary::Multiply));
                        tokens.push_back(makeToken::operand(-1.f));
                        token.clear();
                    }
                }

                // check for RAND

                else if (infix.substring(i, i + 4) == "rand")
                {
                    tokens.push_back(makeToken::rand());
                    i += 3;
                }

                else
                {
                    // check for FUNCTIONS

                    if (!tokenizedOpUnary(token, infix, i))
                    {
                        // check for CONSTANTS

                        if (!tokenizedConstants(infix, i))
                        {
                            // check for OPERATORS value

                            if (ltr == '.')
                            { // if "."
                                if (i == 0)
                                {
                                    // Y: .9
                                    token += ltr;
                                }
                                else if (i < infix.length() - 1)
                                {
                                    // N: 0.0.2
                                    auto decimalsCount = 0;
                                    
                                    for (auto j = 0; j < token.length(); ++j)
                                        if (token[j] == '.')
                                        {
                                            ++decimalsCount;
                                            if (decimalsCount > 1)
                                            {
#if PPD_DebugFormularParser
                                                message = "syntax error: weird number.";
#endif
                                                return false;
                                            }
                                        }

                                    if (decimalsCount == 0)
                                    {
                                        // Y: 0.9 9.0 -.9 *.9 9.*
                                        token += ltr;
                                    }

                                    else
                                    {

                                        if (isOperator(infix.substring(i - 1, i)))
                                        {
                                            // N: *. /.
#if PPD_DebugFormularParser
                                            message = "syntax error: weird operator.";
#endif
                                            return false;
                                        }
                                        else
                                        {
                                            // Y: 78.
                                            token += ltr;
                                        }
                                    }
                                }
                            }
                            
                            else if (ltr == '-')
                            {

                                // if "-" OP or SIGN
                                if (i == 0)
                                {
                                    // Y: -9 -.9
                                    tokens.push_back(makeToken::operand(0.f));
                                    tokens.push_back(makeToken::opBinary(OpBinary::Subtract));
                                    //token += ltr;
                                }
                                else if (i < infix.length() - 1)
                                {
                                    const auto infx1 = infix[i - 1];
                                    const auto infx2 = infix[i + 1];

                                    if (infx2 == '(')
                                    {
                                        // Y: rand%-(54pi) 1/-(x)
                                        tokens.push_back(makeToken::opBinary(OpBinary::ParenthesisOpen));
                                        tokens.push_back(makeToken::operand(-1.f));
                                        tokens.push_back(makeToken::opBinary(OpBinary::Multiply));

                                        parenthesisSearching = true;
                                        parenthesisHelper = 0;
                                        parenthesisInit = true;
                                    }
                                    else
                                    {
                                        // Y: 3*-.9 3*-.9 3%-.9 symbol is SIGN
                                        if (infx1 == '*' || infx1 == '/' || infx1 == '%' || infx1 == '(' || infx1 == '^')
                                            token += ltr;
                                        else // N: 3-9 3+-.9 symbol is OP
                                            tokens.push_back(makeToken::opBinary(OpBinary::Subtract));
                                    }
                                }
                                else
                                {
                                    // N: 245-
#if PPD_DebugFormularParser
                                    message = "syntax error: math error.";
#endif
                                    return false;
                                }
                            }
                            else
                            {
                                tokenizeOperand(token);

                                auto opBin = toOpBinary(ltr);

                                // other operator
                                tokens.push_back(makeToken::opBinary(opBin));

                                if (parenthesisSearching)
                                {
                                    if (infix[i] == '(')
                                    {
                                        ++parenthesisHelper;
                                        parenthesisInit = false;
                                    }
                                    else if (infix[i] == ')')
                                    {
                                        --parenthesisHelper;
                                        parenthesisInit = false;
                                    }
                                    if (!parenthesisInit && parenthesisHelper == 0)
                                    {
                                        parenthesisSearching = false;
                                        tokens.push_back(makeToken::opBinary(OpBinary::ParenthesisClose));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (token.isNotEmpty())
                tokens.push_back(makeToken::operand(getNumber(token))); //rly?

            for (auto t = 1; t < tokens.size(); ++t)
            {
                const auto& t0 = tokens[t];
                const auto& t1 = tokens[t - 1];

                const bool t0Valid = t0.type == Type::Operand ||
                    t0.type == Type::X ||
                    t0.type == Type::Random;

                const bool t1Valid = t1.type == Type::Operand ||
                    t1.type == Type::X ||
                    t1.type == Type::Random;

                // Y: sin(xpi) instead sin(x*pi)
                if (t0Valid && t1Valid)
                {
                    tokens.insert(tokens.begin() + t, makeToken::opBinary(OpBinary::Multiply));
                    ++t;
                }
            }

            return true;
        }

        void tokenizeOperand(String& token)
        {
            // operand (number is finished)
            if (token.isNotEmpty())
            {
                const auto operand = getNumber(token);
                tokens.push_back(makeToken::operand(operand));
                token.clear();
            }
        }

        bool tokenizedOpUnary(String& token, const String& infix, int& i)
        {
            auto opUnary = static_cast<OpUnary>(0);
            auto opUnaryStr = toString(opUnary);

            auto substr = infix.substring(i, i + opUnaryStr.length());

            if (substr == opUnaryStr)
            {
                tokenizeOperand(token);
                if (tokens.back().type == Type::Operand)
                    tokens.push_back(makeToken::opBinary(OpBinary::Multiply));

                tokens.push_back(makeToken::opUnary(opUnary));
                i += opUnaryStr.length() - 1;

                return true;
            }

            for (auto u = 1; u < NumOpUnary; ++u)
            {
                opUnary = static_cast<OpUnary>(u);
                opUnaryStr = toString(opUnary);

                const auto len = opUnaryStr.length();

                if (substr.length() != len)
                    substr = substr.substring(0, len);

                if (substr == opUnaryStr)
                {
                    tokenizeOperand(token);
                    if(!tokens.empty())
                        if (tokens.back().type == Type::Operand)
                            tokens.push_back(makeToken::opBinary(OpBinary::Multiply));

                    tokens.push_back(makeToken::opUnary(opUnary));
                    i += len - 1;

                    return true;
                }
            }

            return false;
        }

        bool tokenizedConstants(const String& infix, int& i)
        {
            if (infix.substring(i, i + 3) == "tau")
            {
                tokens.push_back(makeToken::operand(6.28318530718f));
                i += 2;
                return true;
            }
            else if (infix.substring(i, i + 2) == "pi")
            {
                tokens.push_back(makeToken::operand(3.14159265359f));
                ++i;
                return true;
            }
            else if (infix[i] == 'e')
            {
                tokens.push_back(makeToken::operand(2.71828182846f));
                return true;
            }
            return false;
        }

        bool toPostfix()
        {
            // src: https://www.youtube.com/watch?v=PAceaOSnxQs
            juce::Random rand;
            Tokens postfix, stack;
            for (auto i = 0; i < tokens.size(); ++i)
            {
                switch (tokens[i].type)
                {
                case Type::Operand: postfix.push_back(tokens[i]); break;
                case Type::X: postfix.push_back(tokens[i]); break;
                case Type::Random: postfix.push_back(makeToken::operand(rand.nextFloat())); break;
                default:
                    if (stack.empty()) stack.push_back(tokens[i]);
                    else if (!toPostfixOp(postfix, stack, i))
                        return false;
                    break;
                }
            }
            if (!stack.empty())
                for (auto j = static_cast<int>(stack.size() - 1); j >= 0; --j)
                    postfix.push_back(stack[j]);
            tokens = postfix;
            return true;
        }

        bool toPostfixOp(std::vector<Token>& postfix, std::vector<Token>& stack, int i)
        {
            int j;
            if (tokens[i].opBinary == OpBinary::ParenthesisClose)
            {
                bool parenthesisOpenFound = false;
                for (j = static_cast<int>(stack.size() - 1); j >= 0; --j)
                {
                    if (stack[j].opBinary == OpBinary::ParenthesisOpen)
                    {
                        stack.pop_back();
                        parenthesisOpenFound = true;
                        j = 0;
                    }
                    else
                    {
                        postfix.push_back(stack[j]);
                        stack.pop_back();
                    }
                }
                if (!parenthesisOpenFound)
                {
#if PPD_DebugFormularParser
                    message = "parenthesis error.";
#endif
                    return false;
                }
            }
            else {
                for (j = static_cast<int>(stack.size() - 1); j >= 0; --j)
                {
                    if (stack[j].opBinary == OpBinary::ParenthesisOpen ||
                        tokens[i].precedence > stack[stack.size() - 1].precedence)
                        j = 0;
                    else if (tokens[i].precedence < stack[stack.size() - 1].precedence)
                    {
                        postfix.push_back(stack[j]);
                        stack.pop_back();
                    }
                    else
                    {
                        // current prec SAME AS stack prec

                        if (tokens[i].associativity == -1)
                        {
                            // LEFT to RIGHT asc
                            postfix.push_back(stack[j]);
                            stack.pop_back();
                            j = 0;
                        }
                        else
                        {
                            // RIGHT to LEFT asc
                            j = 0;
                        }
                    }
                }
                stack.push_back(tokens[i]);
            }
            return true;
        }

        bool validate()
        {
            // https://stackoverflow.com/questions/789847/postfix-notation-validation
            if (tokens.empty())
            {
#if PPD_DebugFormularParser
                message = "enter formular..";
#endif
                return false;
            }

            else if (tokens[0].type != Type::Operand && tokens[0].type != Type::X)
            {
#if PPD_DebugFormularParser
                message = "math error.";
#endif
                return false;
            }

            else if (tokens.size() == 1)
                return true;

            auto counter = 0;
            for (auto i = 0; i < tokens.size(); ++i)
            {
                switch (tokens[i].type)
                {
                case Type::OpUnary: --counter; break;
                case Type::OpBinary: counter -= 2; break;
                }
                
                if (counter < 0)
                {
#if PPD_DebugFormularParser
                    message = "math error.";
#endif
                    return false;
                }

                ++counter;
            }

            if (counter != 1 ||
                (tokens[tokens.size() - 1].type != Type::OpUnary &&
                    tokens[tokens.size() - 1].type != Type::OpBinary))
            {
#if PPD_DebugFormularParser
                message = "math error.";
#endif
                return false;
            }

            return true;
        }

        void calculateX()
        {
            const auto resInv = 1.f / static_cast<float>(res);
            for (auto i = 0; i < res; ++i)
            {
                const auto r = static_cast<float>(i) * resInv;
                const auto x = 2.f * (r - .5f);
                values[i].push_back(x);
            }
        }

        bool calculate()
        {
            for (auto t = 0; t < tokens.size(); ++t)
            {
                switch (tokens[t].type)
                {
                case Type::Operand:
                    for (auto& v : values)
                        v.push_back(tokens[t].value);
                    break;
                case Type::Random:
                    for (auto& v : values)
                        v.push_back(tokens[t].value);
                    break;
                case Type::X: calculateX(); break;
                case Type::OpUnary:
                    for (auto i = 0; i < res; ++i)
                    {
                        v0[i] = values[i][values[i].size() - 1];
                        values[i].pop_back();
                        values[i].push_back(operate(tokens[t], v0[i]));
                    }
                    break;
                case Type::OpBinary:
                    for (auto i = 0; i < res; ++i) {
                        v0[i] = values[i][values[i].size() - 1];
                        values[i].pop_back();
                        v1[i] = values[i][values[i].size() - 1];
                        values[i].pop_back();
                        values[i].push_back(operate(tokens[t], v1[i], v0[i]));
                    }
                    break;
                default:
#if PPD_DebugFormularParser
                    message = "calc error.";
#endif
                    return false;
                }
            }
            for (auto i = 0; i < res; ++i)
            {
                if (std::isnan(values[i][0])) v0[i] = 0;
                else v0[i] = values[i][0];
            }
#if PPD_DebugFormularParser
            message = "parsed successfully.";
#endif
            return true;
        }

        //
        void dbgTokens()
        {
            String str("T: " + String(tokens.size()) + String("\n\n"));
            for (auto t = 0; t < tokens.size() - 1; ++t)
                str += toString(tokens[t]) + "\n";
            str += toString(tokens[tokens.size() - 1]);
            DBG(str);
        }

    };
}
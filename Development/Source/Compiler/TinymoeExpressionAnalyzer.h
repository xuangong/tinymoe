#ifndef VCZH_TINYMOEEXPRESSIONANALYZER
#define VCZH_TINYMOEEXPRESSIONANALYZER

#include "TinymoeLexicalAnalyzer.h"

namespace tinymoe
{
	namespace compiler
	{
		/*************************************************************
		Symbol
		*************************************************************/

		enum class GrammarFragmentType
		{
			Name,					// for identifier list,				e.g. [repeat with] the current number [from] 1 [to] 100
			Type,					// for type name,					e.g. set names to new [hash set]
			Primitive,				// for primitive expression,		e.g. sum from 1 to [10]
			Expression,				// for all kinds of expressions,	e.g. repeat with the current number from [1] to [100]
			List,					// for tuple (marshalled as array),	e.g. set names to collection of [("a", "b", "c")]
			Assignable,				// for left value expression, create a new symbol in the containing block if the <assignable> does not exist
			//									e.g. [field unique identifier of person]
			//									e.g. [a variable]
			Argument,				// always create a new symbol in the block body
			//									e.g. repeat with [the current number] from 1 to sum from 1 to 10
		};

		class GrammarFragment
		{
		public:
			typedef shared_ptr<GrammarFragment>			Ptr;
			typedef vector<Ptr>							List;

			GrammarFragmentType			type;
			vector<string>				identifiers;

			GrammarFragment(GrammarFragmentType _type);

			string						GetUniqueIdFragment();
		};

		enum class GrammarSymbolTarget
		{
			Custom,					// user defined symbol

			Array,					// (type)		array
			String,					// (type)		string
			Integer,				// (type)		integer
			Float,					// (type)		float
			Symbol,					// (type)		symbol

			True,					// (primitive)	true
			False,					// (primitive)	false
			Null,					// (primitive)	null

			NewType,				// (primitive)	new <type>
			NewArray,				// (primitive)	new array of <expression> items
			GetArrayItem,			// (primitive)	item <expression> of array <primitive>
			GetArrayLength,			// (primitive)	length of array <primitive>
			Invoke,					// (primitive)	invoke <primitive>
			InvokeWith,				// (primitive)	invoke <expression> with <list>
			IsType,					// (primitive)	<primitive> is <type>
			IsNotType,				// (primitive)	<primitive> is not <type>
			GetField,				// (primitive)	field <argument> of <primitive>

			End,					// (sentence)	end
			Exit,					// (sentence)	exit
			Select,					// (block)		select <expression>
			Case,					// (sentence)	case <expression>
			Call,					// (sentence)	call <expression>
			RedirectTo,				// (sentence)	redirect to <expression>
			Assign,					// (sentence)	set <assignable> to <expression>
			SetArrayItem,			// (sentence)	set item <expression> of array <expression> to <expression>
			SetField,				// (sentence)	set field <argument> of <expression> to <expression>
		};

		enum class GrammarSymbolType : int
		{
			Type = 1,				// <type>
			Symbol = 2,				// <primitive>
			Phrase = 4,				// <primitive>
			Sentence = 8,			// <sentence>
			Block = 16,				// <block>
		};

		class GrammarSymbol
		{
		public:
			typedef shared_ptr<GrammarSymbol>			Ptr;
			typedef vector<Ptr>							List;
			typedef multimap<string, Ptr>				MultiMap;

			GrammarFragment::List		fragments;		// grammar fragments for this symbol
			// a statement cannot be an expression
			// the top invoke expression's function of a statement should reference to a statement symbol
			string						uniqueId;		// a string that identifies the grammar structure
			GrammarSymbolTarget			target;
			GrammarSymbolType			type;

			GrammarSymbol(GrammarSymbolType _type, GrammarSymbolTarget _target = GrammarSymbolTarget::Custom);

			void						CalculateUniqueId();
		};

		GrammarSymbol::Ptr				operator+(GrammarSymbol::Ptr symbol, const string& name);
		GrammarSymbol::Ptr				operator+(GrammarSymbol::Ptr symbol, GrammarFragmentType type);

		/*************************************************************
		Expression
		*************************************************************/

		class Expression : public CodeFragment
		{
		public:
			typedef shared_ptr<Expression>				Ptr;
			typedef vector<Ptr>							List;

			virtual string				ToLog() = 0;
			virtual string				ToCode() = 0;
			virtual void				CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments) = 0;
		};

		// for numbers and strings
		class LiteralExpression : public Expression
		{
		public:
			CodeToken					token;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		// for new created symbols in <assignable> and <argument>
		class ArgumentExpression : public Expression
		{
		public:
			CodeToken::List				tokens;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		// for symbol referencing
		class ReferenceExpression : public Expression
		{
		public:
			GrammarSymbol::Ptr			symbol;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		// for function invoking
		class InvokeExpression : public Expression
		{
		public:
			Expression::Ptr				function;
			Expression::List			arguments;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		// for <list>
		class ListExpression : public Expression
		{
		public:
			Expression::List			elements;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		enum class UnaryOperator
		{
			Positive,
			Negative,
			Not,
		};

		// for unary operator invoking
		class UnaryExpression : public Expression
		{
		public:
			Expression::Ptr				operand;
			UnaryOperator				op;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& newAssignables, Expression::List& newArguments)override;
		};

		enum class BinaryOperator
		{
			Concat,
			Add,
			Sub,
			Mul,
			Div,
			LT,
			GT,
			LE,
			GE,
			EQ,
			NE,
			And,
			Or,
		};

		// for binary operator invoking
		class BinaryExpression : public Expression
		{
		public:
			Expression::Ptr				first;
			Expression::Ptr				second;
			BinaryOperator				op;

			string						ToLog()override;
			string						ToCode()override;
			void						CollectNewAssignable(Expression::List& assignables, Expression::List& arguments)override;
		};

		/*************************************************************
		Symbol Stack
		*************************************************************/

		class GrammarStackItem
		{
		public:
			typedef shared_ptr<GrammarStackItem>		Ptr;
			typedef vector<Ptr>							List;

			GrammarSymbol::List			symbols;

			void						FillPredefinedSymbols();
		};

		class GrammarStack
		{
		public:
			typedef shared_ptr<GrammarStack>					Ptr;
			typedef CodeToken::List::iterator					Iterator;
			typedef vector<pair<Iterator, Expression::Ptr>>		ResultList;
			typedef CodeError(GrammarStack::* ParseFunctionType)(Iterator, Iterator, ResultList&);

			GrammarStackItem::List		stackItems;				// available symbols organized in a scope based structure
			GrammarSymbol::MultiMap		availableSymbols;		// available symbols grouped by the unique identifier
			// the last symbol overrides all other symbols in the same group

			struct ExpressionLink
			{
				typedef shared_ptr<ExpressionLink>		Ptr;

				Expression::Ptr			expression;
				Ptr						previous;
			};

			void						Push(GrammarStackItem::Ptr stackItem);
			GrammarStackItem::Ptr		Pop();

			CodeError					SuccessError();
			CodeError					ParseToken(const string& token, Iterator input, Iterator end, vector<Iterator>& result);
			CodeError					FoldError(CodeError error1, CodeError error2);

			CodeError					ParseGrammarFragment(GrammarFragment::Ptr fragment, Iterator input, Iterator end, ResultList& result);
			CodeError					ParseGrammarSymbolStep(GrammarSymbol::Ptr symbol, int fragmentIndex, ExpressionLink::Ptr previousExpression, Iterator input, Iterator end, vector<pair<Iterator, ExpressionLink::Ptr>>& result);
			CodeError					ParseGrammarSymbol(GrammarSymbol::Ptr symbol, int beginFragment, ExpressionLink::Ptr previousExpression, Iterator input, Iterator end, ResultList& result);
			CodeError					ParseGrammarSymbol(GrammarSymbol::Ptr symbol, Iterator input, Iterator end, ResultList& result);

			CodeError					ParseType(Iterator input, Iterator end, ResultList& result);			// <type>
			CodeError					ParseShortPrimitive(Iterator input, Iterator end, ResultList& result);	// <literal>, op <primitive>, (<expression>), <phrase>
			CodeError					ParsePrimitive(Iterator input, Iterator end, ResultList& result);		// left recursive <phrase>
			CodeError					ParseList(Iterator input, Iterator end, ResultList& result);			// (<expression>, ...)
			CodeError					ParseAssignable(Iterator input, Iterator end, ResultList& result);		// <symbol> or <argument>
			CodeError					ParseArgument(Iterator input, Iterator end, ResultList& result);		// <argument>

			CodeError					ParseBinary(Iterator input, Iterator end, ParseFunctionType parser, CodeTokenType* tokenTypes, BinaryOperator* binaryOperators, int count, ResultList& result);
			CodeError					ParseExp1(Iterator input, Iterator end, ResultList& result);			// * /
			CodeError					ParseExp2(Iterator input, Iterator end, ResultList& result);			// + -
			CodeError					ParseExp3(Iterator input, Iterator end, ResultList& result);			// &
			CodeError					ParseExp4(Iterator input, Iterator end, ResultList& result);			// < > <= >= = <>
			CodeError					ParseExp5(Iterator input, Iterator end, ResultList& result);			// and
			CodeError					ParseExpression(Iterator input, Iterator end, ResultList& result);		// or, aka. <expression>

			CodeError					ParseStatement(Iterator input, Iterator end, ResultList& result);
			int							CountStatementAssignables(Expression::List& assignables);				// -1: illegal assignable (e.g. the assignable is a legal expression)
			int							CountStatementAssignables(Expression::List& assignables, Expression::Ptr& illegalConvertedAssignable);
		};
	}
}

#endif
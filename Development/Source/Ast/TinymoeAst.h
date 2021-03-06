#ifndef VCZH_TINYMOEAST
#define VCZH_TINYMOEAST

#include "../TinymoeSTL.h"

namespace tinymoe
{
	namespace ast
	{
		/*************************************************************
		Common
		*************************************************************/

		class AstNode
		{
		public:
			typedef shared_ptr<AstNode>				Ptr;
			typedef weak_ptr<AstNode>				WeakPtr;

			WeakPtr							parent;

			AstNode();
			virtual ~AstNode();
		};

		class AstType : public AstNode
		{
		public:
			typedef shared_ptr<AstType>				Ptr;
			typedef vector<Ptr>						List;
		};

		class AstExpression : public AstNode
		{
		public:
			typedef shared_ptr<AstExpression>		Ptr;
			typedef vector<Ptr>						List;
		};

		class AstStatement : public AstNode
		{
		public:
			typedef shared_ptr<AstStatement>		Ptr;
			typedef vector<Ptr>						List;
		};

		class AstDeclaration : public AstNode
		{
		public:
			typedef shared_ptr<AstDeclaration>		Ptr;
			typedef weak_ptr<AstNode>				WeakPtr;
			typedef vector<Ptr>						List;

			string							moduleName;
			string							composedName;
		};

		/*************************************************************
		Declaration
		*************************************************************/

		class AstSymbolDeclaration : public AstDeclaration
		{
		public:
			typedef shared_ptr<AstSymbolDeclaration>			Ptr;
			typedef vector<Ptr>									List;
		};

		class AstTypeDeclaration : public AstDeclaration
		{
		public:
			AstSymbolDeclaration::List		fields;
		};

		class AstFunctionDeclaration : public AstDeclaration
		{
		public:
			AstType::Ptr					ownerType;			// (optional) if ownerType is not null, this function is added to this type as a virtual function.
			AstSymbolDeclaration::List		arguments;
			AstSymbolDeclaration::Ptr		resultVariable;
			AstStatement::Ptr				statement;
		};

		/*************************************************************
		Expression
		*************************************************************/

		enum class AstLiteralName
		{
			Null,
			True,
			Falst,
		};

		enum class AstUnaryOperator
		{
			Positive,
			Negative,
			Not,
		};

		enum class AstBinaryOperator
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

		class AstThisExpression : public AstExpression
		{
		public:
		};

		class AstLiteralExpression : public AstExpression
		{
		public:
			AstLiteralName					literalName;
		};

		class AstIntegerExpression : public AstExpression
		{
		public:
			int64_t							value;
		};

		class AstFloatExpression : public AstExpression
		{
		public:
			double							value;
		};

		class AstStringExpression : public AstExpression
		{
		public:
			string							value;
		};

		class AstReferenceExpression : public AstExpression
		{
		public:
			AstDeclaration::WeakPtr			reference;			// could be
																//     AstSymbolDeclaration
																//     AstFunctionDeclaration
		};

		class AstUnaryExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				operand;
			AstUnaryOperator				op;
		};

		class AstBinaryExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				first;
			AstExpression::Ptr				second;
			AstBinaryOperator				op;
		};

		class AstNewTypeExpression : public AstExpression
		{
		public:
			AstType::Ptr					type;
		};

		class AstTestTypeExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				target;
			AstType::Ptr					type;
		};

		class AstNewArrayExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				length;
		};

		class AstArrayLengthExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				target;
		};

		class AstArrayAccessExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				target;
			AstExpression::Ptr				index;
		};

		class AstFieldAccessExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				target;
			string							composedFieldName;
		};

		class AstInvokeExpression : public AstExpression
		{
		public:
			AstExpression::Ptr				function;
			AstExpression::List				arguments;
		};

		class AstLambdaArgument : public AstNode
		{
		public:
			typedef shared_ptr<AstLambdaArgument>	Ptr;
			typedef vector<Ptr>						List;

			string							composedName;
		};

		class AstLambdaExpression : public AstExpression
		{
		public:
			AstSymbolDeclaration::List		arguments;
			AstStatement::Ptr				statement;
		};

		/*************************************************************
		Statement
		*************************************************************/

		class AstBlockStatement : public AstStatement
		{
		public:
			AstStatement::List				statements;
		};

		class AstExpressionStatement : public AstStatement
		{
		public:
			AstExpression::Ptr				expression;
		};

		class AstDeclarationStatement : public AstStatement
		{
		public:
			AstDeclaration::Ptr				declaration;
		};

		class AstAssignmentStatement : public AstStatement
		{
		public:
			AstExpression::Ptr				target;				// could be
																//     AstReferenceExpression
																//     AstFieldAccessExpression
																//     AstArrayAccessExpression
			AstExpression::Ptr				value;
		};

		
		class AstIfStatement : public AstStatement
		{
		public:
			AstExpression::Ptr				condition;
			AstStatement::Ptr				trueBranch;
			AstStatement::Ptr				falseBranch;		// (optional)
		};

		/*************************************************************
		Type
		*************************************************************/

		enum class AstPredefinedTypeName
		{
			Object,
			Symbol,
			Array,
			Integer,
			Float,
			String,
		};

		class AstPredefinedType : public AstType
		{
		public:
			AstPredefinedTypeName			typeName;
		};

		class AstReferenceType : public AstType
		{
		public:
			weak_ptr<AstTypeDeclaration>	typeDeclaration;
		};

		/*************************************************************
		Assembly
		*************************************************************/

		class AstAssembly : public AstNode
		{
		public:
			typedef shared_ptr<AstAssembly>			Ptr;

			AstDeclaration::List			declarations;
		};
	}
}

#endif
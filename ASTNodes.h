
#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>
#include <memory>

using std::cout;
using std::endl;
using std::string;

class CodeGenContext;
class NBlock;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement *> StatementList;
typedef std::vector<NExpression *> ExpressionList;
typedef std::vector<NVariableDeclaration *> VariableList;

class Node {
protected:
	const char m_DELIM = ':';
	const char* m_PREFIX = "--";
public:
	virtual ~Node() {}
	virtual string getName() const = 0;
	virtual void print(string prefix) const{}
	virtual llvm::Value *codeGen(CodeGenContext &context) { return (llvm::Value *)0; }
};

class NExpression : public Node {
	string getName() const override {
		return "NExpression";
	}
};

class NStatement : public Node {
	string getName() const override {
		return "NStatement";
	}
};

class NInteger : public NExpression {
  public:
	long long value;

	NInteger(long long value)
		: value(value) {
			
	}

	string getName() const override {
		return "NInteger";
	}

	void print(string prefix) const override{
		cout << prefix << getName() << this->m_DELIM << value << endl;
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
  public:
	double value;

	NDouble(double value)
		: value(value) {
		// return "NDoub le=" << value << endl;
	}

	string getName() const override {
		return "NDouble";
	}
	
	void print(string prefix) const override{
		cout << prefix << getName() << this->m_DELIM << value << endl;
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
  public:
	std::string name;

	NIdentifier(const std::string &name)
		: name(name) {
		// return "NIdentifier=" << name << endl;
	}

	string getName() const override {
		return "NIdentifier";
	}

	void print(string prefix) const override{
		cout << prefix << getName() << this->m_DELIM << name << endl;
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall
	: public NExpression {
  public:
	const NIdentifier &id;
	ExpressionList arguments;

	NMethodCall(const NIdentifier &id, ExpressionList &arguments)
		: id(id), arguments(arguments) {
	}

	NMethodCall(const NIdentifier &id)
		: id(id) {
	}

	string getName() const override {
		return "NMethodCall";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		this->id.print(nextPrefix);
		for(auto it=arguments.begin(); it!=arguments.end(); it++){
			(*it)->print(nextPrefix);
		}
	}

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
  public:
	int op;
	NExpression &lhs;
	NExpression &rhs;

	NBinaryOperator(NExpression &lhs, int op, NExpression &rhs)
		: lhs(lhs), rhs(rhs), op(op) {
	}

	string getName() const override {
		return "NBinaryOperator";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << op << endl;
		
		lhs.print(nextPrefix);
		rhs.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
  public:
	NIdentifier &lhs;
	NExpression &rhs;

	NAssignment(NIdentifier &lhs, NExpression &rhs)
		: lhs(lhs), rhs(rhs) {
	}

	string getName() const override {
		return "NAssignment";
	}
	
	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		
		lhs.print(nextPrefix);
		rhs.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
  public:
	StatementList statements;

	NBlock() {}

	string getName() const override {
		return "NBlock";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		for(auto it=statements.begin(); it!=statements.end(); it++){
			(*it)->print(nextPrefix);
		}
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement {
  public:
	NExpression &expression;

	NExpressionStatement(NExpression &expression)
		: expression(expression) {
	}

	string getName() const override {
		return "NExpressionStatement";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		expression.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
  public:
	const NIdentifier &type;
	NIdentifier &id;
	NExpression *assignmentExpr;

	NVariableDeclaration(const NIdentifier &type, NIdentifier &id)
		: type(type), id(id) {
	}

	NVariableDeclaration(const NIdentifier &type, NIdentifier &id, NExpression *assignmentExpr)
		: type(type), id(id), assignmentExpr(assignmentExpr) {
	}

	string getName() const override {
		return "NVariableDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		type.print(nextPrefix);
		id.print(nextPrefix);
		assignmentExpr->print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
  public:
	const NIdentifier &type;
	const NIdentifier &id;
	VariableList arguments;
	NBlock &block;

	NFunctionDeclaration(const NIdentifier &type, const NIdentifier &id, const VariableList &arguments, NBlock &block)
		: type(type), id(id), arguments(arguments), block(block) {
	}

	string getName() const override {
		return "NFunctionDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getName() << this->m_DELIM << endl;
		
		type.print(nextPrefix);
		id.print(nextPrefix);
		for(auto it=arguments.begin(); it!=arguments.end(); it++){
			(*it)->print(nextPrefix);
		}
		block.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

std::unique_ptr<NExpression> LogError(const char* str){
	fprintf(stderr, "LogError: %s\n", str);
	return nullptr;
}


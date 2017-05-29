
#ifndef __ASTNODES_H__
#define __ASTNODES_H__

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
	virtual string getTypeName() const = 0;
	virtual void print(string prefix) const{}
	virtual llvm::Value *codeGen(CodeGenContext &context) { return (llvm::Value *)0; }
};

class NExpression : public Node {
public:
//    NExpression(){}
//
//    NExpression operator=(const NExpression& ) = default;

	string getTypeName() const override {
		return "NExpression";
	}
};

class NStatement : public Node {
	string getTypeName() const override {
		return "NStatement";
	}
};

class NDouble : public NExpression {
public:
	double value;

	NDouble(double value)
		: value(value) {
		// return "NDoub le=" << value << endl;
	}

	string getTypeName() const override {
		return "NDouble";
	}

	void print(string prefix) const override{
		cout << prefix << getTypeName() << this->m_DELIM << value << endl;
	}

	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NInteger : public NExpression {
public:
    uint64_t value;

    NInteger(uint64_t value)
            : value(value) {

    }

    string getTypeName() const override {
        return "NInteger";
    }

    void print(string prefix) const override{
        cout << prefix << getTypeName() << this->m_DELIM << value << endl;
    }

    operator NDouble(){
        return NDouble(value);
    }

    virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NIdentifier : public NExpression {
public:
	std::string name;

	NIdentifier(const std::string &name)
		: name(name) {
		// return "NIdentifier=" << name << endl;
	}

	string getTypeName() const override {
		return "NIdentifier";
	}

	void print(string prefix) const override{
		cout << prefix << getTypeName() << this->m_DELIM << name << endl;
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
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

	string getTypeName() const override {
		return "NMethodCall";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		this->id.print(nextPrefix);
		for(auto it=arguments.begin(); it!=arguments.end(); it++){
			(*it)->print(nextPrefix);
		}
	}

	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NBinaryOperator : public NExpression {
public:
	int op;
	NExpression &lhs;
	NExpression &rhs;

	NBinaryOperator(NExpression &lhs, int op, NExpression &rhs)
		: lhs(lhs), rhs(rhs), op(op) {
	}

	string getTypeName() const override {
		return "NBinaryOperator";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << op << endl;

		lhs.print(nextPrefix);
		rhs.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NAssignment : public NExpression {
public:
	NIdentifier &lhs;
	NExpression &rhs;

	NAssignment(NIdentifier &lhs, NExpression &rhs)
		: lhs(lhs), rhs(rhs) {
	}

	string getTypeName() const override {
		return "NAssignment";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		lhs.print(nextPrefix);
		rhs.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NBlock : public NExpression {
public:
	StatementList statements;

	NBlock() {}

	string getTypeName() const override {
		return "NBlock";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		for(auto it=statements.begin(); it!=statements.end(); it++){
			(*it)->print(nextPrefix);
		}
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NExpressionStatement : public NStatement {
public:
	NExpression &expression;

	NExpressionStatement(NExpression &expression)
		: expression(expression) {
	}

	string getTypeName() const override {
		return "NExpressionStatement";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		expression.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NVariableDeclaration : public NStatement {
public:
	const NIdentifier &type;
	NIdentifier &id;
	NExpression *assignmentExpr;

	NVariableDeclaration(const NIdentifier &type, NIdentifier &id)
		: type(type), id(id), assignmentExpr(nullptr) {
	}

	NVariableDeclaration(const NIdentifier &type, NIdentifier &id, NExpression *assignmentExpr)
		: type(type), id(id), assignmentExpr(assignmentExpr) {
	}

	string getTypeName() const override {
		return "NVariableDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		type.print(nextPrefix);
		id.print(nextPrefix);
        if( assignmentExpr != nullptr )
		    assignmentExpr->print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
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

	string getTypeName() const override {
		return "NFunctionDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;

		type.print(nextPrefix);
		id.print(nextPrefix);

		for(auto it=arguments.begin(); it!=arguments.end(); it++){
			(*it)->print(nextPrefix);
		}
		block.print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NReturnStatement: public NStatement{
public:
    NExpression &expression;

    NReturnStatement(NExpression& expression)
            : expression(expression) {

    }
    virtual llvm::Value* codeGen(CodeGenContext& context) override ;

};

std::unique_ptr<NExpression> LogError(const char* str);

#endif
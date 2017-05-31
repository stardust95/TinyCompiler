
#ifndef __ASTNODES_H__
#define __ASTNODES_H__

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>
#include <memory>

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::make_shared;

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
    NExpression(){}
//
//    NExpression operator=(const NExpression& ) = default;

	string getTypeName() const override {
		return "NExpression";
	}

    virtual void print(string prefix) const override{
        cout << prefix << getTypeName() << endl;
    }

};

class NStatement : public Node {
public:
	string getTypeName() const override {
		return "NStatement";
	}
    virtual void print(string prefix) const override{
        cout << prefix << getTypeName() << endl;
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
    bool isType;
    bool isArray;

    shared_ptr<NInteger> arraySize;

	NIdentifier(const std::string &name)
		: name(name), arraySize(nullptr) {
		// return "NIdentifier=" << name << endl;
	}

	string getTypeName() const override {
		return "NIdentifier";
	}

	void print(string prefix) const override{
        string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << name << (isArray ? "(Array)" : "") << endl;
        if( isArray ){

            assert(arraySize != nullptr);

            arraySize->print(nextPrefix);
        }
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

//	NVariableDeclaration(const NIdentifier &type, NIdentifier &id)
//		: type(type), id(id), assignmentExpr(nullptr) {
//        assert(type.isType);
//        assert()
//	}

	NVariableDeclaration(const NIdentifier &type, NIdentifier &id, NExpression *assignmentExpr = NULL)
		: type(type), id(id), assignmentExpr(assignmentExpr) {
        assert(type.isType);
        assert(!type.isArray || (type.isArray && type.arraySize != nullptr));
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
        assert(type.isType);
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

class NStructDeclaration: public NStatement{
public:
    const NIdentifier name;
    VariableList members;

    NStructDeclaration(const NIdentifier& id, const VariableList& arguments)
            : name(id), members(arguments){

    }

    string getTypeName() const override {
        return "NStructDeclaration";
    }

    void print(string prefix) const override {
        string nextPrefix = prefix+this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << this->name.name << endl;

        for(auto it=members.begin(); it!=members.end(); it++){
            (*it)->print(nextPrefix);
        }
    }

    virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NReturnStatement: public NStatement{
public:
    NExpression &expression;

    NReturnStatement(NExpression& expression)
            : expression(expression) {

    }


    string getTypeName() const override {
        return "NReturnStatement";
    }


    virtual llvm::Value* codeGen(CodeGenContext& context) override ;

};

class NIfStatement: public NStatement{
public:

    NExpression& condition;
    NBlock* trueBlock;          // should not be null
    NBlock* falseBlock;         // can be null


    NIfStatement(NExpression& cond, NBlock* blk, NBlock* blk2 = nullptr)
            : condition(cond), trueBlock(blk), falseBlock(blk2){

    }


    string getTypeName() const override {
        return "NIfStatement";
    }

    void print(string prefix) const override{
        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        condition.print(nextPrefix);

        trueBlock->print(nextPrefix);

        if( falseBlock ){
            falseBlock->print(nextPrefix);
        }

    }


    llvm::Value *codeGen(CodeGenContext &context) override ;


};

class NForStatement: public NStatement{
public:
    NExpression * initial, * condition, * increment;
    NBlock & block;

    NForStatement(NBlock& b, NExpression* init = nullptr, NExpression* cond = nullptr, NExpression* incre = nullptr)
            : block(b), initial(init), condition(cond), increment(incre){
        if( condition == nullptr ){
            condition = new NInteger(1);
        }
    }

    string getTypeName() const override{
        return "NForStatement";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        if( initial )
            initial->print(nextPrefix);
        if( condition )
            condition->print(nextPrefix);
        if( increment )
            increment->print(nextPrefix);

        block.print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NStructMember: public NExpression{
public:
	const NIdentifier& id;
	const NIdentifier& member;

    NStructMember(const NIdentifier &structName, const NIdentifier &member)
            : id(structName),member(member) {
    }

    string getTypeName() const override{
        return "NStructMember";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        id.print(nextPrefix);
        member.print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NArrayIndex: public NExpression{
public:
    const NIdentifier& arrayName;
    NExpression& expression;

    NArrayIndex(const NIdentifier& name, NExpression& exp)
            : arrayName(name), expression(exp){

    }

    string getTypeName() const override{
        return "NArrayIndex";
    }


    void print(string prefix) const override{
        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        arrayName.print(nextPrefix);
        expression.print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NArrayAssignment: public NExpression{
public:
    const NArrayIndex& arrayIndex;
    NExpression& expression;

    NArrayAssignment(const NArrayIndex& index, NExpression& exp)
            : arrayIndex(index), expression(exp){

    }

    string getTypeName() const override{
        return "NArrayAssignment";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        arrayIndex.print(nextPrefix);
        expression.print(nextPrefix);
    }


    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NStructAssignment: public NExpression{
public:
    const NStructMember& structMember;
    NExpression& expression;

    NStructAssignment(const NStructMember& member, NExpression& exp)
            : structMember(member), expression(exp){

    }

    string getTypeName() const override{
        return "NStructAssignment";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        structMember.print(nextPrefix);
        expression.print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};



std::unique_ptr<NExpression> LogError(const char* str);

#endif
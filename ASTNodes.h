
#ifndef __ASTNODES_H__
#define __ASTNODES_H__

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
//puts("$1"); return $1;
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

typedef std::vector<shared_ptr<NStatement>> StatementList;
typedef std::vector<shared_ptr<NExpression>> ExpressionList;
typedef std::vector<shared_ptr<NVariableDeclaration>> VariableList;

class Node {
protected:
	const char m_DELIM = ':';
	const char* m_PREFIX = "--";
public:
    Node(){}
	virtual ~Node() {}
	virtual string getTypeName() const = 0;
	virtual void print(string prefix) const{}
	virtual llvm::Value *codeGen(CodeGenContext &context) { return (llvm::Value *)0; }
};

class NExpression : public Node {
public:
    NExpression(){}

	string getTypeName() const override {
		return "NExpression";
	}

    virtual void print(string prefix) const override{
        cout << prefix << getTypeName() << endl;
    }

};

class NStatement : public Node {
public:
    NStatement(){}

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

    NDouble(){}

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

    NInteger(){}

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

    NIdentifier(){}

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
        if( isArray && arraySize != nullptr ){
//            assert(arraySize != nullptr);
            arraySize->print(nextPrefix);
        }
	}

	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NMethodCall: public NExpression {
public:
	const shared_ptr<NIdentifier> id;
	shared_ptr<ExpressionList> arguments = make_shared<ExpressionList>();

    NMethodCall(){

    }

	NMethodCall(const shared_ptr<NIdentifier> id, shared_ptr<ExpressionList> arguments)
		: id(id), arguments(arguments) {
	}

	NMethodCall(const shared_ptr<NIdentifier> id)
		: id(id) {
	}

	string getTypeName() const override {
		return "NMethodCall";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		this->id->print(nextPrefix);
		for(auto it=arguments->begin(); it!=arguments->end(); it++){
			(*it)->print(nextPrefix);
		}
	}

	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NBinaryOperator : public NExpression {
public:
	int op;
	shared_ptr<NExpression> lhs;
	shared_ptr<NExpression> rhs;

    NBinaryOperator(){}

    NBinaryOperator(shared_ptr<NExpression> lhs, int op, shared_ptr<NExpression> rhs)
            : lhs(lhs), rhs(rhs), op(op) {
    }

	string getTypeName() const override {
		return "NBinaryOperator";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << op << endl;

		lhs->print(nextPrefix);
		rhs->print(nextPrefix);
	}

	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NAssignment : public NExpression {
public:
	shared_ptr<NIdentifier> lhs;
	shared_ptr<NExpression> rhs;

    NAssignment(){}

	NAssignment(shared_ptr<NIdentifier> lhs, shared_ptr<NExpression> rhs)
		: lhs(lhs), rhs(rhs) {
	}

	string getTypeName() const override {
		return "NAssignment";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		lhs->print(nextPrefix);
		rhs->print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NBlock : public NExpression {
public:
	shared_ptr<StatementList> statements = make_shared<StatementList>();

    NBlock(){

    }

	string getTypeName() const override {
		return "NBlock";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		for(auto it=statements->begin(); it!=statements->end(); it++){
			(*it)->print(nextPrefix);
		}
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NExpressionStatement : public NStatement {
public:
	shared_ptr<NExpression> expression;

    NExpressionStatement(){}

	NExpressionStatement(shared_ptr<NExpression> expression)
		: expression(expression) {
	}

	string getTypeName() const override {
		return "NExpressionStatement";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		expression->print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NVariableDeclaration : public NStatement {
public:
	const shared_ptr<NIdentifier> type;
	shared_ptr<NIdentifier> id;
	shared_ptr<NExpression> assignmentExpr = nullptr;

//	NVariableDeclaration(const shared_ptr<NIdentifier> type, shared_ptr<NIdentifier> id)
//		: type(type), id(id), assignmentExpr(nullptr) {
//        assert(type.isType);
//        assert()
//	}

    NVariableDeclaration(){}

	NVariableDeclaration(const shared_ptr<NIdentifier> type, shared_ptr<NIdentifier> id, shared_ptr<NExpression> assignmentExpr = NULL)
		: type(type), id(id), assignmentExpr(assignmentExpr) {
        assert(type->isType);
        assert(!type->isArray || (type->isArray && type->arraySize != nullptr));
	}

	string getTypeName() const override {
		return "NVariableDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;
		type->print(nextPrefix);
		id->print(nextPrefix);
        if( assignmentExpr != nullptr ){
            assignmentExpr->print(nextPrefix);
        }
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NFunctionDeclaration : public NStatement {
public:
	shared_ptr<NIdentifier> type;
    shared_ptr<NIdentifier> id;
	shared_ptr<VariableList> arguments = make_shared<VariableList>();
	shared_ptr<NBlock> block;
    bool isExternal = false;

    NFunctionDeclaration(){}

	NFunctionDeclaration(shared_ptr<NIdentifier> type, shared_ptr<NIdentifier> id, shared_ptr<VariableList> arguments, shared_ptr<NBlock> block, bool isExt = false)
		: type(type), id(id), arguments(arguments), block(block), isExternal(isExt) {
        assert(type->isType);
	}

	string getTypeName() const override {
		return "NFunctionDeclaration";
	}

	void print(string prefix) const override{
		string nextPrefix = prefix+this->m_PREFIX;
		cout << prefix << getTypeName() << this->m_DELIM << endl;

		type->print(nextPrefix);
		id->print(nextPrefix);

		for(auto it=arguments->begin(); it!=arguments->end(); it++){
			(*it)->print(nextPrefix);
		}

        assert(isExternal || block != nullptr);
        if( block )
		    block->print(nextPrefix);
	}
	virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NStructDeclaration: public NStatement{
public:
    shared_ptr<NIdentifier> name;
    shared_ptr<VariableList> members = make_shared<VariableList>();

    NStructDeclaration(){}

    NStructDeclaration(shared_ptr<NIdentifier>  id, shared_ptr<VariableList> arguments)
            : name(id), members(arguments){

    }

    string getTypeName() const override {
        return "NStructDeclaration";
    }

    void print(string prefix) const override {
        string nextPrefix = prefix+this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << this->name->name << endl;

        for(auto it=members->begin(); it!=members->end(); it++){
            (*it)->print(nextPrefix);
        }
    }

    virtual llvm::Value* codeGen(CodeGenContext& context) override ;
};

class NReturnStatement: public NStatement{
public:
    shared_ptr<NExpression> expression;

    NReturnStatement(){}

    NReturnStatement(shared_ptr<NExpression>  expression)
            : expression(expression) {

    }


    string getTypeName() const override {
        return "NReturnStatement";
    }


    virtual llvm::Value* codeGen(CodeGenContext& context) override ;

};

class NIfStatement: public NStatement{
public:

    shared_ptr<NExpression>  condition;
    shared_ptr<NBlock> trueBlock;          // should not be null
    shared_ptr<NBlock> falseBlock;         // can be null


    NIfStatement(){}

    NIfStatement(shared_ptr<NExpression>  cond, shared_ptr<NBlock> blk, shared_ptr<NBlock> blk2 = nullptr)
            : condition(cond), trueBlock(blk), falseBlock(blk2){

    }


    string getTypeName() const override {
        return "NIfStatement";
    }

    void print(string prefix) const override{
        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        condition->print(nextPrefix);

        trueBlock->print(nextPrefix);

        if( falseBlock ){
            falseBlock->print(nextPrefix);
        }

    }


    llvm::Value *codeGen(CodeGenContext &context) override ;


};

class NForStatement: public NStatement{
public:
    shared_ptr<NExpression> initial, condition, increment;
    shared_ptr<NBlock>  block;

    NForStatement(){}

    NForStatement(shared_ptr<NBlock> b, shared_ptr<NExpression> init = nullptr, shared_ptr<NExpression> cond = nullptr, shared_ptr<NExpression> incre = nullptr)
            : block(b), initial(init), condition(cond), increment(incre){
        if( condition == nullptr ){
            condition = make_shared<NInteger>(1);
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

        block->print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NStructMember: public NExpression{
public:
	shared_ptr<NIdentifier> id;
	shared_ptr<NIdentifier> member;

    NStructMember(){}
    
    NStructMember(shared_ptr<NIdentifier> structName, shared_ptr<NIdentifier>member)
            : id(structName),member(member) {
    }

    string getTypeName() const override{
        return "NStructMember";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        id->print(nextPrefix);
        member->print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NArrayIndex: public NExpression{
public:
    shared_ptr<NIdentifier>  arrayName;
    shared_ptr<NExpression>  expression;

    NArrayIndex(){}

    NArrayIndex(shared_ptr<NIdentifier>  name, shared_ptr<NExpression>  exp)
            : arrayName(name), expression(exp){

    }

    string getTypeName() const override{
        return "NArrayIndex";
    }


    void print(string prefix) const override{
        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        arrayName->print(nextPrefix);
        expression->print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NArrayAssignment: public NExpression{
public:
    shared_ptr<NArrayIndex> arrayIndex;
    shared_ptr<NExpression>  expression;

    NArrayAssignment(){}

    NArrayAssignment(shared_ptr<NArrayIndex> index, shared_ptr<NExpression>  exp)
            : arrayIndex(index), expression(exp){

    }

    string getTypeName() const override{
        return "NArrayAssignment";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        arrayIndex->print(nextPrefix);
        expression->print(nextPrefix);
    }


    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NArrayInitialization: public NStatement{
public:

    NArrayInitialization(){}

    shared_ptr<NVariableDeclaration> declaration;
    shared_ptr<ExpressionList> expressionList = make_shared<ExpressionList>();

    NArrayInitialization(shared_ptr<NVariableDeclaration> dec, shared_ptr<ExpressionList> list)
            : declaration(dec), expressionList(list){

    }

    string getTypeName() const override{
        return "NArrayInitialization";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        declaration->print(nextPrefix);
        for(auto it=expressionList->begin(); it!=expressionList->end(); it++){
            (*it)->print(nextPrefix);
        }
    }


    llvm::Value *codeGen(CodeGenContext &context) override ;

};

class NStructAssignment: public NExpression{
public:
    shared_ptr<NStructMember> structMember;
    shared_ptr<NExpression>  expression;

    NStructAssignment(){}

    NStructAssignment(shared_ptr<NStructMember> member, shared_ptr<NExpression>  exp)
            : structMember(member), expression(exp){

    }

    string getTypeName() const override{
        return "NStructAssignment";
    }

    void print(string prefix) const override{

        string nextPrefix = prefix + this->m_PREFIX;
        cout << prefix << getTypeName() << this->m_DELIM << endl;

        structMember->print(nextPrefix);
        expression->print(nextPrefix);
    }

    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NLiteral: public NExpression{
public:
    string value;

    NLiteral(){}

    NLiteral(const string &str) {
        value = str.substr(1, str.length()-2);
    }

    string getTypeName() const override{
        return "NLiteral";
    }

    void print(string prefix) const override{

        cout << prefix << getTypeName() << this->m_DELIM << value << endl;

    }

    llvm::Value *codeGen(CodeGenContext &context) override;

};


std::unique_ptr<NExpression> LogError(const char* str);

#endif
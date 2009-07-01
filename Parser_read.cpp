#include "Parser.h"
#include "error.h"

Element* Parser::readElement() {
	switch (whattoken()) {
		case T_IF: return readIf();
		case T_WHILE: return readWhile();
		case T_FOR: return readFor();
		case T_STARTBLOCK: return readBlock();
		case T_STARTSBLOCK: return readSBlock();
		case T_FN: return readFunction();		// fn, class�� readElement ȣ���� �ʿ��� �˾Ƽ� �Ұ���
		case T_CLASS: return readClass();
		case T_ASSIGN: return readAssign();
		case T_RETURN: return readReturn();
		case T_USE: return readUse();
		case T_OPERATOR: return readOperator();
		case T_VAR: return readVar();
			// private
		default: return readExpression();
	}
}

Eif* Parser::readIf() {
	Element *condition, *then, *elsethen;

	newtoken(); condition=readElement();
	newtoken(); then=readElement();
	newtoken();
	switch (whattoken()) {
		case T_ELIF: newtoken(); elsethen=readElement(); break;
		case T_ELSE: newtoken(); elsethen=readElement(); break;
		default: reusetoken=true; elsethen=0;
	}
	// ���⼭ condition�� �˻��ؼ� "�����" bool���� ���� �� ���� ��� ��� �߻�
	return new Eif(condition, then, elsethen);
}

Eif* Eif::clone() {
	Eif *i;

	if (elsethen!=0) i=new Eif(condition->clone(), then->clone(), elsethen->clone());
	else i=new Eif(condition->clone(), then->clone(), 0);
	return i;
}

Ewhile* Parser::readWhile() {
	Element *condition, *body;

	newtoken(); condition=readElement();
	newtoken(); body=readElement();
	// ���⼭ condition�� �˻��ؼ� "�����" bool���� ���� �� ���� ��� ��� �߻�
	return new Ewhile(condition, body);
}

Ewhile* Ewhile::clone() {
	return new Ewhile(condition->clone(), body->clone());
}

Efor* Parser::readFor() {
	GYstring variablename;
	Element *list, *body;

	newtoken(); variablename=token;
	newtoken(); if (whattoken()!=T_IN) return 0;	// "in"�� �ƴϸ� ���� �߻�
	newtoken(); list=readElement();
	newtoken(); body=readElement();
	// ���⼭ list�� �˻��ؼ� "�����" ����Ʈ���� ���� �� ���� ��� ��� �߻�
	return new Efor(variablename, list, body);
}

Efor* Efor::clone() {
	return new Efor(varname, list->clone(), body->clone());
}

Eexpression* Parser::readExpression() {
	Eexpression *expr;
	Piece *p;
	Pcall *pc;
	TokenType w;

	expr=new Eexpression();
	w=whattoken();
	while (w!=T_ENDELEMENT && w!=T_CLOSECALLR && w!=T_CLOSECALLS) {
		switch (w) {
			case T_OPENCALLR:
				p=pc=new Pcall(CT_ROUND);
				newtoken(); w=whattoken();
				while (w!=T_CLOSECALLR) {
					pc->getListPointer()->push_back(readElement());
					w=whattoken();
					if (w!=T_CLOSECALLR) newtoken();
				}
				break;
			case T_OPENCALLS:
				p=pc=new Pcall(CT_SQUARE);
				newtoken(); w=whattoken();
				while (w!=T_CLOSECALLS) {
					pc->getListPointer()->push_back(readElement());
					w=whattoken();
					if (w!=T_CLOSECALLS) newtoken();
				}
				break;
			case T_DOT: p=new Pdot(); break;
			case T_TRUE: p=new Pbool(true); break;
			case T_FALSE: p=new Pbool(false); break;
			case T_NOTKEYWORD:
				if (isdigit(token[0])) {
					if (token.rfind('.')<token.length()) p=new Pfloat(token);
					else p=new Pint(token);
				} else {
					if (token[0]=='\'' || token[0]=='\"') p=new Pstr(token);
					else p=new Pidentifier(token);
				}
				break;
			default:
				p=new Pelement(readElement());
		}
		expr->getBodyPointer()->push_back(p);
		newtoken();
		w=whattoken();
	}
	return expr;
}

Piece* Eexpression::clonePiece(Piece* p) {
	switch (p->getType()) {
	case PT_CALL: {
		Pcall *pcall=dynamic_cast<Pcall*>(p);
		Pcall *n;
		GYarray<Element*> *list;
		GYuint i, s;

		n=new Pcall(pcall->getCallType());
		list=pcall->getListPointer();
		s=list->size();
		for (i=0; i<s; i++)
			n->getListPointer()->push_back(list->at(i)->clone());
		return n;
				  }
	}
	return p;
}

void Eexpression::clonePieces() {
	GYuint i, s;

	s=body.size();
	for (i=0; i<s; i++)
		body[i]=clonePiece(body[i]);
}

Eexpression* Eexpression::clone() {
	Eexpression *e;

	e=new Eexpression(*this);
	// Pcall�� ���� ������ ó���� �ʿ���
	// Pcall�� �����͸� �����ϹǷ� clone�� �� ��ü ���̿� ������ �ش�
	e->clonePieces();
	return e;
}

Eblock* Parser::readBlock() {
	Eblock *block;

	block=new Eblock();
	return readBlock(block);
}

Eblock* Parser::readBlock(Eblock *block) {
	Element *element;
	Efn *fn;
	Eclass *clss;
	Use *use;
	Operators *ops;

	newtoken();
	while (!oss.eof() && whattoken()!=T_ENDBLOCK) {
		element=readElement();
		if (element!=0) {
			fn=dynamic_cast<Efn*>(element);
			clss=dynamic_cast<Eclass*>(element);
			if (fn!=0) block->registerFunction(fn);
			else if (clss!=0) block->registerClass(clss);
			else {
				use=dynamic_cast<Use*>(element);
				ops=dynamic_cast<Operators*>(element);
				if (use!=0) block->registerUse(use);
				else if (ops!=0) block->registerOperators(ops);
				else block->push_back(element);
			}
		} else ;			// ���� �߻�
		newtoken();
	}
	return block;
}

Eblock* Eblock::clone() {
	return clone(new Eblock());
}

Eblock* Eblock::clone(Eblock *b) {
	GYuint i, s;
	GYarray<Efn*>* fns;
	GYmap<GYstring, Eclass*>::iterator itc, endc;
	GYmap<GYstring, Operators*>::iterator ito, endo;

	// Scope�� ������ ����
	// �Լ�, Ŭ����, (use), ������, ����
	// �Լ�
	fns=getFnListPointer();
	s=fns->size();
	for (i=0; i<s; i++)
		b->registerFunction(fns->at(i)->clone());
	// Ŭ����
	itc=getClassIterator();
	endc=getClassEndIterator();
	while (itc!=endc) {
		b->registerClass(itc->second->clone());
		itc++;
	}
	// ������
	ito=getOperatorIterator();
	endo=getOperatorEndIterator();
	while (ito!=endo) {
		b->registerOperators(ito->second->clone());
		ito++;
	}
	// ����
	s=body.size();
	for (i=0; i<s; i++)
		b->push_back(body[i]->clone());
	return b;
}

Efns* Parser::readSBlock() {
	Efns *functions;
	Element *element;
	Efn *function;

	functions=new Efns();
	newtoken();
	while (whattoken()!=T_ENDSBLOCK) {
		element=readElement();
		function=dynamic_cast<Efn*>(element);
		if (function==0) ;			// ���� �߻�
		else functions->addFunction(function);
		newtoken();
	}
	return functions;
}

Efns* Efns::clone() {
	Efns *f;
	GYuint i, s;

	f=new Efns();
	s=functions.size();
	for (i=0; i<s; i++)
		f->addFunction(functions[i]->clone());
	return f;
}

Efn* Parser::readFunction() {
	Efn *function;
	FNparam *param;
	Element *element;
	TokenType w, ow;
	GYstring name;

	function=new Efn();
	newtoken(); w=whattoken();
	if (w!=T_OPENCALLR && w!=T_OPENCALLS) {
		if (!tokenisidentifier()) error(ERROR_PARSING_WRONGIDENTIFIER);
		function->setName(token);
		newtoken(); w=whattoken();
	}
	if (w!=T_OPENCALLR && w!=T_OPENCALLS)
		error(ERROR_PARSING_WRONGFUNCTIONDEFINITION);
	if (w==T_OPENCALLR) function->setCalltype(CT_ROUND);
	if (w==T_OPENCALLS) function->setCalltype(CT_SQUARE);
	newtoken(); w=whattoken();
	while (w!=T_CLOSECALLR && w!=T_CLOSECALLS && w!=T_IF) {
		param=function->addParam();
		if (w!=T_VAR && !tokenisidentifier())
			error(ERROR_PARSING_WRONGIDENTIFIER); // identifier ������ ������Ű�� ���ϹǷ� ���� �߻�
		name=token;
		ow=w; newtoken(); w=whattoken();
		while (w!=T_CLOSECALLR && w!=T_CLOSECALLS && w!=T_IF && w!=T_ENDELEMENT) {
			if (ow==T_VAR) param->setCallbyref(true);
			else param->addType(name);
			name=token;
			// name�� identifier���� �ϴµ� �ƴ� ��� ���� �߻�
			ow=w; newtoken(); w=whattoken();
			if (w==T_DOT) {
				if (function->getHasVararg())
					error(ERROR_PARSING_HASTWOVARARG);		// vararg�� �ϳ��� �־�� ��
				param->setVararg(true);
				function->setHasVararg(true);
				while (w==T_DOT) {
					ow=w; newtoken(); w=whattoken();
				}
			}
		}
		param->setName(name);
		if (w==T_ENDELEMENT) {
			newtoken(); w=whattoken();
		}
	}
	if (w==T_IF) {
		newtoken(); element=readElement();
		function->setCondition(element);
		w=whattoken();
	}
	if ((function->getCalltype()==CT_ROUND && w==T_CLOSECALLS) ||
		(function->getCalltype()==CT_SQUARE && w==T_CLOSECALLR))
		error(ERROR_PARSING_WRONGFUNCTIONCLOSURE);		// () [] doesn't match error
	newtoken();
	element=readElement();
	function->setBody(element);
	return function;
}

Efn* Efn::clone() {
	Efn *f;
	FNparam *t, *o;
	GYarray<GYstring>* types;
	GYuint i, is, j, js;

	f=new Efn();
	if (!isAnonymous()) f->setName(name);
	if (hasvararg) f->setHasVararg(true);
	f->setCalltype(getCalltype());
	is=parameters.size();
	for (i=0; i<is; i++) {
		t=f->addParam();
		o=parameters[i];
		t->setName(o->getName());
		t->setCallbyref(o->getCallbyref());
		types=o->getTypeNames();
		js=types->size();
		for (j=0; j<js; j++)
			t->addType(types->at(j));
	}
	if (condition!=0) f->setCondition(condition->clone());
	f->setBody(body->clone());
	return f;
}

Eclass* Parser::readClass() {
	Eclass *clss;
	TokenType w;

	clss=new Eclass();
	newtoken(); w=whattoken();
	if (w!=T_OPENCALLR && w!=T_STARTBLOCK) {
		if (!tokenisidentifier())
			error(ERROR_PARSING_WRONGIDENTIFIER);			// Ŭ���� �̸��� identifier ������ ������Ű�� ���ϹǷ� ���� �߻�
		// class gy.lang.basic ���� �̸��� ���� �������� �ʾƿ�~
		clss->setName(token);
		newtoken(); w=whattoken();
	}
	if (w==T_OPENCALLR) {
		newtoken(); w=whattoken();
		while (w!=T_CLOSECALLR) {
			// ��ӹ��� Ŭ���� �̸����� "gy.lang"�����ε� �� �� �ֵ��� �����ؾ� ��
			clss->addSuper(token);
			newtoken(); w=whattoken();
			if (w==T_ENDELEMENT) { newtoken(); w=whattoken(); }
			else if (w!=T_CLOSECALLR) ;		// ���� �߻�
		}
		newtoken();
	}
	if (whattoken()!=T_STARTBLOCK)
		error(ERROR_PARSING_WRONGCLASSDEFINITION);		// ���� �߻� - �ݵ�� ���� ���;� ��
	readBlock(clss);
	return clss;
}

Eclass* Eclass::clone() {
	Eclass *c;

	c=new Eclass();
	c->setName(name);
	Eblock::clone(c);
	// super ó��
	// private ����� ó��
	return c;
}

Eassign* Parser::readAssign() {
	GYstring targetname;
	Element *body;

	newtoken(); targetname=token;
	newtoken(); body=readElement();
	return new Eassign(targetname, body);
}

Eassign* Eassign::clone() {
	return new Eassign(targetname, body->clone());
}

Ereturn* Parser::readReturn() {
	Element *body;

	newtoken(); body=readElement();
	return new Ereturn(body);
}

Ereturn* Ereturn::clone() {
	return new Ereturn(body->clone());
}

Use* Parser::readUse() {
	Use *use;

	newtoken();
	use=new Use();
	while (whattoken()!=T_ENDELEMENT) {
		if (!tokenisidentifier())
			error(ERROR_PARSING_WRONGIDENTIFIER);			// ���� �߻�
		use->addRefPath(token);
		newtoken();
		if (whattoken()==T_DOT) {
			newtoken();
			if (whattoken()==T_ENDELEMENT)
				error(ERROR_PARSING_WRONGUSEDEFINITION);	// ���⼭ �׳� T_ENDELEMENT�� ���������� �ȵ� - ���� �߻�
		}
	}
	return use;
}

Use* Use::clone() {
	return 0;
}

Operators* Parser::readOperator() {
	Operators *ops;
	TokenType w;
	OperatorType type;
	GYuchar priority;
	bool multiple;

	ops=new Operators();
	newtoken(); w=whattoken();
	if (w==T_STARTBLOCK) {
		multiple=true; newtoken(); w=whattoken();
	} else multiple=false;
	do {
		switch(w) {
			case T_INFIXL: type=INFIXL; break;
			case T_INFIXR: type=INFIXR; break;
			case T_PREFIX: type=PREFIX; break;
		}
		newtoken();
		// token�� 0~127 ������ ���ڰ� �ƴϸ� ���� �߻�
		priority=(GYuchar)atoi(token.c_str());
		newtoken();
		while (whattoken()!=T_ENDELEMENT) {
			if (!tokenisidentifier())
				error(ERROR_PARSING_WRONGIDENTIFIER);
			ops->add(new Operator(type, priority, token));
			newtoken();
		}
		if (multiple) {
			newtoken(); w=whattoken();
		}
	} while (multiple && w!=T_ENDBLOCK);
	return ops;
}

Operator* Operator::clone() {
	Operator *o;

	o=new Operator(type, priority, name);
	return o;
}

Operators* Operators::clone() {
	Operators *o;
	GYuint i, s;

	o=new Operators();
	s=list.size();
	for (i=0; i<s; i++)
		o->add(list[i]->clone());
	return o;
}

Evar* Parser::readVar() {
	Evar *v;

	v=new Evar();
	newtoken();
	while (whattoken()!=T_ENDELEMENT) {
		v->registerVariable(token);
		newtoken();
	}
	return v;
}

Evar* Evar::clone() {
	return new Evar(*this);
}

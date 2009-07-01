#include "Element.h"
#include "Eexpression.h"
#include "ASTnode.h"
#include "Value.h"
#include "Types.h"
#include "Value_basictype.h"

Value* Nelement::run(Runner* runner) {
	switch (pc->getType()) {
	case PT_BOOL:
		return new Vbool(runner, dynamic_cast<Pbool*>(pc)->getValue());
	case PT_INT:
		return new Vint(runner, dynamic_cast<Pint*>(pc)->getValue());
	case PT_FLOAT:
		return new Vfloat(runner, dynamic_cast<Pfloat*>(pc)->getValue());
	case PT_STR:
		return new Vstr(runner, dynamic_cast<Pstr*>(pc)->getValue());
	case PT_ELEMENT:
		return (dynamic_cast<Pelement*>(pc)->getValue())->run(runner);
	case PT_FN:
		return new Vfns(dynamic_cast<Pfns*>(pc)->getValue(), runner);
	case PT_CLASS:
		return new Vclass(dynamic_cast<Pclass*>(pc)->getValue());
	case PT_LIST: {
		Vlist *list=new Vlist(runner);
		GYarray<Element*> *el=dynamic_cast<Plist*>(pc)->getListPointer();
		GYuint i, s=el->size();

		for (i=0; i<s; i++)
			list->push_back(el->at(i)->run(runner)->unwrap());
		return list;
				  }
	case PT_VARIABLE:
		return new Vvariable(dynamic_cast<Pvariable*>(pc)->getValue());
	}
	return 0;
}

Value* Nbincall::run(Runner* runner) {
	GYarray<Value*> list;

	list.push_back(tar2->run(runner));
	list.push_back(tar1->run(runner));
	return op->getFns()->run(runner)->call(CT_ROUND, &list, runner);
}

Value* Nuncall::run(Runner* runner) {
	GYarray<Value*> list;

	list.push_back(tar1->run(runner));
	return op->getFns()->run(runner)->call(CT_ROUND, &list, runner);
}

Value* Nfncall::run(Runner* runner) {
	Vfns *f;

	f=fns->run(runner);
	return f->call(prms->getCallType(), prms->run(runner), runner);
}

Value* Nclasscall::run(Runner* runner) {
	return clss->run(runner)->call(prms->getCallType(), prms->run(runner), runner);
}

Value* Nvariablecall::run(Runner* runner) {
	// var�� ����ִ� �� ���� �� ���� ȣ��
	Value* target=var->getValue();
	Vfn* fn=dynamic_cast<Vfn*>(target);
	Vfns* fns=dynamic_cast<Vfns*>(target);
	Vclass* clss=dynamic_cast<Vclass*>(target);
	Vinstance* inst=dynamic_cast<Vinstance*>(target);
	Value* result=0;
	if (fn!=0) result=fn->call(prms->run(runner), runner);
	if (fns!=0) result=fns->call(prms->getCallType(), prms->run(runner), runner);
	if (clss!=0) result=clss->call(prms->getCallType(), prms->run(runner), runner);
	if (inst!=0) result=inst->call(prms->getCallType(), prms->run(runner), runner);
	return result;
}

Value* Nvaluecall::run(Runner* runner) {
	// var�� ����ִ� �� ���� ���� ȣ��
	Value* target=node->run(runner);
	Vfn* fn=dynamic_cast<Vfn*>(target);
	Vfns* fns=dynamic_cast<Vfns*>(target);
	Vclass* clss=dynamic_cast<Vclass*>(target);
	Vinstance* inst=dynamic_cast<Vinstance*>(target);
	Value* result=0;
	if (fn!=0) result=fn->call(prms->run(runner), runner);
	if (fns!=0) result=fns->call(prms->getCallType(), prms->run(runner), runner);
	if (clss!=0) result=clss->call(prms->getCallType(), prms->run(runner), runner);
	if (inst!=0) result=inst->call(prms->getCallType(), prms->run(runner), runner);
	return result;
}

// integration ���������� run ������ class �Լ� ���� ������ ��� ���������� �� �� ���⵵ �ϰ�..
Value* Nanonymcall::run(Runner* runner) {
	// obj�� �����ؼ� �ű⿡ call
	Value* target=obj->run(runner);
	Vfn* fn=dynamic_cast<Vfn*>(target);
	Vfns* fns=dynamic_cast<Vfns*>(target);
	Vclass* clss=dynamic_cast<Vclass*>(target);
	Vinstance* inst=dynamic_cast<Vinstance*>(target);
	Value* result=0;
	obj->push(runner);
	if (fn!=0) result=fn->call(prms->run(runner), runner);
	if (fns!=0) result=fns->call(prms->getCallType(), prms->run(runner), runner);
	if (clss!=0) result=clss->call(prms->getCallType(), prms->run(runner), runner);
	if (inst!=0) result=inst->call(prms->getCallType(), prms->run(runner), runner);
	obj->pop(runner);
	return result;
}

void Ndot::push(Runner* runner) {
	// clss���� ���� ����� �� ���� ������ �ӽ÷� ����
	if (inst!=0) inst->push(runner);

	/*
	GYmap<GYstring, Variable*>::iterator it, end;
	values.clear();
	it=clss->getBody()->getVariableIterator();
	end=clss->getBody()->getVariableEndIterator();
	while (it!=end) {
		values.insert(GYpair<Variable*, Value*>(it->second, it->second->getValue()));
		it++;
	}
	*/
}

void Ndot::pop(Runner* runner) {
	// clss���� ���� ����� �� ���� ������ ���󺹱�
	if (inst!=0) inst->pop(runner);

	/*
	GYmap<Variable*, Value*>::iterator it, end;
	it=values.begin();
	end=values.end();
	while (it!=end) {
		it->first->setValue(it->second);
		it++;
	}
	values.clear();
	*/
}

Value* Ndot::run(Runner* runner) {
	Value *n;
	Element *result=0;

	n=node->run(runner)->unwrap();
	switch (n->getType()) {
	case VT_CLASS:
		inst=0;
		clss=(dynamic_cast<Vclass*>(n));
		break;
	case VT_FUNCTION:
		inst=dynamic_cast<Vfn*>(n);
		return n;
	case VT_FUNCTIONS:
		inst=dynamic_cast<Vfns*>(n);
		return n;				// �Լ� �̸����� ã�Ƽ� ��ȯ�ؾ� �Ǵµ�..
	default:				// Vinstance�� ���
		inst=dynamic_cast<ValueScope*>(n);
		clss=dynamic_cast<Vinstance*>(n)->getClass();
	}

	if ((result=clss->findFunction(name))!=0) return result->run(runner);
	if ((result=clss->findClass(name))!=0) return result->run(runner);
	return 0;
}

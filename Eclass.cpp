#include "Element.h"
#include "Efn.h"
#include "Eclass.h"
#include "Operator.h"

void Eclass::merge(Eclass *target) {			// �ٸ� EclassŬ������ ������ ���Ѵ�
	// this�� �տ� ���� target�� �ڷ� ������
	// �����͸� ������ ���� �ƴ϶� �ƿ� ���縦 �ؼ� ���ο� ��ü�� ������ �� ��
	GYarray<GYstring> *supers;
	int i, s;

	// ���� Ŭ���� ��ġ��
	supers=target->getSupersNames();			// supersname���� �ؾ� �ɱ�?
	s=supers->size();
	for (i=0; i<s; i++)
		addSuper(supers->at(i));
	// Eblock::merge�� �ؼ� ������ ���ľ� �ǰڴ�
	Scope::merge(target);
}

bool Eclass::istypeof(Eclass* e) {
	GYuint i, s;

	if (this==e) return true;
	s=supers.size();
	for (i=0; i<s; i++)
		if (supers[i]->istypeof(e)) return true;
	return false;
}

// Eclass������ variable, fn, op, class�� ã�� �� ����Ŭ�����鵵 ����ؾ� ��
Variable* Eclass::findVariable(GYstring name) {
	return Scope::findVariable(name);
}

Efns* Eclass::findFn(GYstring name, bool chkout) {
	return Scope::findFn(name, chkout);
}

Efns* Eclass::findFn(GYstring name, CallType type, bool chkout) {
	return Scope::findFn(name, type, chkout);
}

Operators* Eclass::findOperators(GYstring name, bool chkout) {
	return Scope::findOperators(name, chkout);
}

Eclass* Eclass::findClass(GYstring name, bool chkout) {
	return Scope::findClass(name, chkout);
}

#include <iostream>
#include <vector>

using namespace std;

class Item {
public:
	Item() : m_nx(0) { cout <<  this << "기본 생성자 호출" << endl; }

	Item(const int _n) : m_nx(_n) { cout << this << "일반 생성자 호출" << endl; }

	Item(const Item& rhs) : m_nx(rhs.m_nx) { cout << this << "복사 생성자 호출" << endl; }

	Item(const Item&& rhs) : m_nx(std::move(rhs.m_nx)) { cout << this << "이동 생성자 호출" << endl; }

	~Item() { cout << this << "소멸자 호출" << endl; }

private:
	int m_nx;
};

int main() {
	std::vector<Item> v;

//	cout << "push_back 호출" << endl;
//	v.push_back(Item(3));

//	cout << "emplace_back 3 호출" << endl;
//	v.emplace_back(3)

//	cout << "emplace_back Item()호출" << endl;
//	v.push_back(Item());

	cout << "emplace_back 호출" << endl;
	v.emplace_back();

	cout << "emplace_back 호출" << endl;
	v.emplace_back();


	cout << "끝" << endl;
}
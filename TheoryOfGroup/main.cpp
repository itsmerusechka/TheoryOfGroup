#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

// Работа с элементами каждой группы

// Элементы группы S3 (симметрическая группа из 3 элементов)
// Определяем через генераторы a и b: a^3 = e, b^2 = e, bab = a^2
class ElementS3 {
public:
    int i, j; // i = степень a (0..2), j = степень b (0..1)
    ElementS3(int i_=0,int j_=0):i(i_),j(j_){}
    
    // Оператор сравнения элементов
    bool operator==(const ElementS3 &other) const {return i==other.i && j==other.j;}
    
    // Строковое представление элемента для вывода
    string str() const {
        if(i==0 && j==0) return "e";   // единичный элемент
        if(i==1 && j==0) return "a";   // a
        if(i==2 && j==0) return "a²";  // a^2
        if(i==0 && j==1) return "b";   // b
        if(i==1 && j==1) return "ab";  // a*b
        if(i==2 && j==1) return "a²b"; // a^2*b
        return "?";                     // ошибка
    }
};

// Элементы группы D8 (диэдральная группа из 8 элементов)
class ElementD8 {
public:
    int k, t; // k = степень a (0..3), t = 0 если a^k, t = 1 если a^k*b
    ElementD8(int k_=0,int t_=0):k(k_),t(t_){}
    bool operator==(const ElementD8 &other) const {return k==other.k && t==other.t;}
    
    // Строковое представление элемента
    string str() const {
        if(k==0 && t==0) return "e";      // единичный элемент
        if(k==1 && t==0) return "a";
        if(k==2 && t==0) return "a²";
        if(k==3 && t==0) return "a³";
        if(k==0 && t==1) return "b";
        if(k==1 && t==1) return "ab";
        if(k==2 && t==1) return "a²b";
        if(k==3 && t==1) return "a³b";
        return "?";                       // ошибка
    }
};

// Элементы группы Q8 (кватернионы)
// idx = индекс элемента (0..7)
class ElementQ8 {
public:
    int idx;
    ElementQ8(int idx_=0):idx(idx_){}
    bool operator==(const ElementQ8 &other) const {return idx==other.idx;}
    
    // Строковое представление элемента
    string str() const {
        string labels[8] = {"e","a","a²","a³","b","ab","a²b","a³b"};
        return labels[idx];
    }
};

// Элементы группы тэтраэдра 12 порядка
// idx = индекс элемента 0..11
class ElementA4 {
public:
    int idx;
    ElementA4(int idx_=0):idx(idx_){}
    bool operator==(const ElementA4 &other) const {return idx==other.idx;}
    
    // Строковое представление элемента через "имена" элементов
    string str() const {
        string labels[12] = {
            "e", "a", "a²", "a²b", "ba", "bab",
            "a^2ba", "aba^2", "ab", "b", "aba^2", "a^2ba"
        };
        return labels[idx];
    }
};

// Класс группы
// Используем шаблон, чтобы поддерживать разные типы элементов
template<typename E>
class Group {
protected:
    vector<E> elems; // Список элементов группы
public:
    Group(const vector<E>& elements):elems(elements){}
    
    // Функция умножения (переопределяется в конкретной группе)
    virtual E multiply(const E &x, const E &y) const {return E();}

    // Вывод таблицы Кэли
    void showCayleyTable(const string &name) const {
        int w=8; // ширина колонки
        cout << "=== Таблица Кэли группы " << name << " ===\n\n";
        cout << setw(w) << "";
        for(auto &h: elems) cout << setw(w) << h.str();
        cout << "\n";
        for(auto &x: elems){
            cout << setw(w) << x.str();
            for(auto &y: elems) cout << setw(w) << multiply(x,y).str();
            cout << "\n";
        }
        cout << "\n";
    }

    // Вычисление порядка элемента
    int order(const E &g) const {
        E cur = g;
        for(int n=1;n<=elems.size();++n){
            if(cur == elems[0]) return n; // если вернулись к единице, возвращаем порядок
            cur = multiply(cur,g);         // продолжаем умножение
        }
        return -1; // если элемент не вернулся к единице (не должно быть)
    }

    // Вывод порядков всех элементов
    void showOrders() const{
        cout << "=== Порядки элементов ===\n";
        for(auto &x: elems){
            int ord = order(x);
            cout << "ord("<<x.str()<<")="<<ord<<"   ⇒   "<<ord<<" | "<<elems.size()<<"\n";
        }
        cout << "\n";
    }

    // Проверка аксиом группы
    void checkAxioms() const {
        cout << "=== Проверка аксиом группы ===\n";

        // 1. Замкнутость: для любых x, y, x*y ∈ G
        bool closed=true;
        for(auto &x: elems) for(auto &y: elems)
            if(find(elems.begin(),elems.end(),multiply(x,y))==elems.end()) closed=false;
        cout << "1. Замкнутость: " << (closed?"выполнено":"нарушено") << "\n";

        // 2. Нейтральный элемент: существует e, что e*g = g*e = g
        bool neutral=true;
        E e = elems[0];
        for(auto &g: elems)
            if(!(multiply(e,g)==g && multiply(g,e)==g)) neutral=false;
        cout << "2. Нейтральный элемент: " << (neutral?"выполнено":"нарушено") << "\n";

        // 3. Обратные элементы: для каждого g существует h, g*h = h*g = e
        bool inverses=true;
        for(auto &g: elems){
            bool found=false;
            for(auto &h: elems)
                if(multiply(g,h)==e && multiply(h,g)==e){found=true; break;}
            if(!found) inverses=false;
        }
        cout << "3. Обратные элементы: " << (inverses?"выполнено":"нарушено") << "\n";

        // 4. Ассоциативность проверяется теоретически (не вычисляется)
        cout << "4. Ассоциативность: (x·y)·z = x·(y·z)   (по определению)\n\n";
    }

    // Проверка, является ли подмножество H подгруппой
    bool isSubgroup(const vector<E> &H) const {
        E e = elems[0];
        if(find(H.begin(), H.end(), e) == H.end()) return false; // должен содержать единицу
        for(auto &x:H) for(auto &y:H) if(find(H.begin(), H.end(), multiply(x,y))==H.end()) return false; // замкнутость
        for(auto &x:H){ // наличие обратного элемента
            bool hasInv=false;
            for(auto &y:H) if(multiply(x,y)==e && multiply(y,x)==e) hasInv=true;
            if(!hasInv) return false;
        }
        return true;
    }

    // Поиск всех подгрупп
    void findSubgroups() const{
        cout << "=== Подгруппы ===\n";
        int n = elems.size();
        for(int mask=1;mask<(1<<n);++mask){
            vector<E> H;
            for(int i=0;i<n;++i) if(mask&(1<<i)) H.push_back(elems[i]);
            if(isSubgroup(H)){
                cout<<"H={ ";
                for(auto &x:H) cout<<x.str()<<" ";
                cout<<"} |H|="<<H.size()<<" | |G|\n";
            }
        }
        cout<<"\n";
    }
};

// S3: симметрическая группа из 3 элементов
class S3: public Group<ElementS3>{
public:
    S3():Group({{0,0},{1,0},{2,0},{0,1},{1,1},{2,1}}){}
    ElementS3 multiply(const ElementS3 &x, const ElementS3 &y) const{
        int i=x.i,j=x.j,k=y.i,l=y.j;
        if(j==0) return {(i+k)%3,l}; // если первый элемент без b
        else{
            if(k==0) return {i,(1+l)%2};    // если второй элемент без a
            if(k==1) return {(i+2)%3,(1+l)%2};
            if(k==2) return {(i+1)%3,(1+l)%2};
        }
        return {0,0};
    }
};

// D8
class D8: public Group<ElementD8>{
public:
    D8():Group({{0,0},{1,0},{2,0},{3,0},{0,1},{1,1},{2,1},{3,1}}){}
    ElementD8 multiply(const ElementD8 &x, const ElementD8 &y) const{
        int k1=x.k,t1=x.t,k2=y.k,t2=y.t;
        int nk, nt;
        if(t1==0 && t2==0){nk=(k1+k2)%4; nt=0;}
        else if(t1==0 && t2==1){nk=(k1+k2)%4; nt=1;}
        else if(t1==1 && t2==0){nk=(k1-k2+4)%4; nt=1;}
        else {nk=(k1-k2+4)%4; nt=0;}
        return {nk,nt};
    }
};

// Q8
class Q8: public Group<ElementQ8>{
public:
    Q8():Group({0,1,2,3,4,5,6,7}){}
    ElementQ8 multiply(const ElementQ8 &x, const ElementQ8 &y) const{
        static int M[8][8]={
            {0,1,2,3,4,5,6,7},
            {1,2,3,0,5,6,7,4},
            {2,3,0,1,6,7,4,5},
            {3,0,1,2,7,4,5,6},
            {4,5,6,7,1,0,3,2},
            {5,6,7,4,0,1,2,3},
            {6,7,4,5,3,2,1,0},
            {7,4,5,6,2,3,0,1}
        };
        return {M[x.idx][y.idx]};
    }
};

// Тэтраэдр
class A4 : public Group<ElementA4> {
public:
    A4():Group({0,1,2,3,4,5,6,7,8,9,10,11}){}

    // Таблица умножения через индексы элементов
    ElementA4 multiply(const ElementA4 &x, const ElementA4 &y) const {
        static int M[12][12] = {
            {0,1,2,3,4,5,6,7,8,9,10,11},
            {1,2,0,7,8,9,3,4,5,6,10,11},
            {2,0,1,4,5,6,7,8,9,3,11,10},
            {3,7,4,0,9,10,1,5,2,8,6,11},
            {4,8,5,1,10,0,2,6,3,7,9,11},
            {5,9,6,2,0,1,3,7,4,8,10,11},
            {6,3,7,5,1,2,0,4,8,9,10,11},
            {7,4,8,6,2,3,1,5,9,0,11,10},
            {8,5,9,7,3,4,2,6,0,1,10,11},
            {9,6,10,8,4,5,3,7,1,2,11,0},
            {10,7,11,9,5,6,4,8,2,3,0,1},
            {11,8,10,10,6,7,5,9,3,4,1,2}
        };
        return {M[x.idx][y.idx]};
    }
};

// Начало программы
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Выбор группы пользователем
    cout << "Выберите группу:\n1) S3\n2) D8\n3) Q8\n4) G12\n";
    cout << "Напишите Ваш выбор: ";
    int ch; cin >> ch;

    // Создание и работа с выбранной группой
    switch(ch){
        case 1:{
            S3 G;
            G.showCayleyTable("S3"); // таблица Кэли
            G.showOrders();          // порядки элементов
            G.checkAxioms();         // проверка аксиом
            G.findSubgroups();       // поиск всех подгрупп
            break;
        }
        case 2:{
            D8 G;
            G.showCayleyTable("D8");
            G.showOrders();
            G.checkAxioms();
            G.findSubgroups();
            break;
        }
        case 3:{
            Q8 G;
            G.showCayleyTable("Q8");
            G.showOrders();
            G.checkAxioms();
            G.findSubgroups();
            break;
        }
        case 4:{
            A4 G;
            G.showCayleyTable("G12");
            G.showOrders();
            G.checkAxioms();
            G.findSubgroups();
            break;
        }
        default: cout << "Неверный выбор\n";
    }
    return 0;
}

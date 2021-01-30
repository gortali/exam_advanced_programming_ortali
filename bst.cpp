#include<iostream>
#include<utility>
#include<iterator>
#include<memory>
#include<vector>

template<typename K, typename V, typename COMP = std::less<K>>
class Bst{

    COMP comp = COMP();
    using pair_type = std::pair<const K, V>;

    struct Node{
        pair_type item;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;     
        Node* up;     

        explicit Node(const pair_type& x): item{x}, up{nullptr} {}
        explicit Node(pair_type&& x): item{std::move(x)}, up{nullptr} {}
        
        //recursive build of tree starting from x
        explicit Node(const std::unique_ptr<Node>& x) : item{x->item}, up{nullptr}{
            std::cout << "Node ctor"<< std::endl;
            if(x->left){
                left.reset(new Node{x->left});
                left->up = this;
            }
            if(x->right){
                right.reset(new Node{x->right});
                right->up = this;
            }
        }
        
        friend 
        bool operator==(Node& a, Node& b){
            return a.item.first == b.item.first;
        }
    };

    std::unique_ptr<Node> root;

    template <typename O>   //forwarding reference
    class _iterator{
        Node* current;

        public:    
        
        using value_type = O;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        explicit _iterator(Node *x): current{x} {}

        reference operator*() const {return current->item;}
        pointer operator->() const {return &**this;}

        Node* get_current() { return current;}

        //pre-increment
        _iterator& operator++(){   
            if(!current)
                return *this;
            else if(current->right)
            {
                current=current->right.get();
                while(current->left)
                    current=current->left.get();
            }
            else
            {
                Node* p = current->up; 
                while(p && current==p->right.get()){
                    current=p;
                    p=p->up;
                }
                current=p;
            }
            return *this;
        }

        //post-increment
        _iterator operator++(int){
            auto tmp{*this};    //copy yourself into temporary object
            ++(*this);          //increment yourself
            return tmp;         //return temporary object (not incremented!)
        }

        friend 
        bool operator==(const _iterator& a, const _iterator& b){
            return a.current == b.current;
        }
        
        friend 
        bool operator!=(const _iterator& a, const _iterator& b){
            return !(a==b);
        }
    };
   
    
    public:

    auto returnMin(Node* p) const noexcept{
        while(p->left){
            p=p->left.get();
        }
        return p;
    }
    
    using iterator = _iterator<pair_type>;
    using const_iterator = _iterator<const pair_type>;

    //insert pair x in subtree having root in y
    //forwarding reference
    template<typename O>
    std::pair<iterator, bool> _insert(O&& x, std::unique_ptr<Node>& y){
        if(!y){
            std::cout << "writing " << x.first << std::endl;
            y.reset(new Node{std::forward<O>(x)});
            y->up=nullptr;
            return {iterator{y.get()},true};
        }
        else if(comp(x.first,y->item.first)){
            std::cout << "descending left..." << std::endl;
            auto out = _insert(x, y->left);
            y->left->up = y.get();      //assign up
            return out;
        }
        else if(comp(y->item.first,x.first)){
            std::cout << "descending right..." << std::endl;
            auto out = _insert(x, y->right);
            y->right->up = y.get();     //assign up
            return out;
        }
        else{
            std::cout << "already present!" << std::endl;
            return{iterator{y.get()}, false};
        }
    }
    
    Bst() = default;    //default ctor
    ~Bst() = default;   //default dtor

    Bst(Bst&&) = default;               //move ctor TODO: check!
    Bst& operator=(Bst&&) = default;    //move assignment TODO: check!

    Bst(const Bst& x) {             //copy ctor
        if(x.root){
            root.reset(new Node{x.root});
        }
    }

    Bst& operator=(const Bst& x){   //copy assignment
        root.reset();
        auto tmp = x; //call copy ctor
        *this = std::move(tmp); //cal move assignment
        return *this;
    }

    std::pair<iterator, bool> insert(const pair_type& x){ 
        std::cout << "call to insert l-value " << std::endl;
        return _insert(x,root);
    }
    std::pair<iterator, bool> insert(pair_type&& x){ 
        std::cout << "call to insert r-value" << std::endl;
        return _insert(std::move(x),root);
    }
    

    template< class... Types >
    std::pair<iterator,bool> emplace(Types&&... args){  
        //assume correct argument passed...
        return insert(pair_type{args...});
    }

    void clear(){
        root.reset();
    }

    auto begin() noexcept           {return iterator{returnMin(root.get())};}
    auto begin() const noexcept     {return const_iterator{returnMin(root.get())};}
    auto cbegin() const noexcept    {return const_iterator{returnMin(root.get())};}
    auto end() noexcept             {return iterator{nullptr};}
    auto end() const noexcept       {return const_iterator{nullptr};}
    auto cend() const noexcept      {return const_iterator{nullptr};}


    iterator find(const K& x){
        Node* p= root.get();
        while(p){
            if(comp(x,p->item.first)){      //if smaller...
                p=p->left.get();
            }
            else if(comp(p->item.first,x)){ //if bigger...
                p=p->right.get();
            }
            else{   //found!
                break;
            }
        }
        return iterator{p}; //returns iterator w/ nullptr if not found. TODO: ok?

    }
    const_iterator find(const K& x) const{
        return const_cast<const_iterator>(find(x));
    }

    template<typename O>
    V& _subscript(O&& x){
        iterator i = find(std::forward<O>(x));
        if(i!=end()){   //found!
            return (*i).second;
        }
        else{           //not found, insert...
            //auto ins = insert(pair_type(x,V()));
            //if(ins.second)  //if insertion went fine
            return (*(insert(pair_type{std::forward<O>(x),V{}}).first)).second;    //assume insertion went fine
        }
    }

    V& operator[](const K& x){
        return _subscript(x);
    };
    
    V& operator[](K&& x){
        return _subscript(std::move(x));  
    };


    void eraseFromSubtree(std::unique_ptr<Node>& root_, const K& x){
        if(!root_)
            return;
        if(comp(x,root_->item.first)){
            //std::cout << "going left..." << std::endl;
            eraseFromSubtree(root_->left, x);
        }
        else if(comp(root_->item.first,x)){
            //std::cout << "going right..." << std::endl;
            eraseFromSubtree(root_->right, x);
        }
        else{
            //std::cout << "found " << root_->item.first << "..." << std::endl;
            if(!root_->left && !root_->right){
                //std::cout << "no left and right..."<< std::endl;
                root_.reset();
            }
            else if(!root_->left){
                //std::cout << "no left..."<< std::endl;
                Node* tmp = root_->right.release();
                tmp->up = root_->up;
                root_.reset(tmp);
            }
            else if(!root_->right){
                //std::cout << "no right..." << std::endl;
                Node* tmp = root_->left.release();
                tmp->up = root_->up;
                root_.reset(tmp);
            
            }
            else{
                //std::cout << "both left and right..." << std::endl;
                
                Node* tmp = returnMin(root_->right.get());
               
                Node* tmp_up = root_->up;
                Node* tmp_l  = root_->left.release();
                Node* tmp_r  = root_->right.release();

                root_.reset(new Node{tmp->item});

                root_->up = tmp_up;
                root_->left.reset(tmp_l);
                root_->right.reset(tmp_r);
  
                root_->left->up = root_.get();
                root_->right->up = root_.get();

                eraseFromSubtree(root_->right, tmp->item.first);
            }

        }
        return;
    }

    void erase(const K& x){
        eraseFromSubtree(root, x);
    }

    /*void erase(const K& x){
        iterator i = find(x);
        if(i != end()){   //if node exists
            std::cout << "found! deleting... " << std::endl;
            
            Node* p = i.get_current();    
            Node* p_up = p->up;

            bool is_left = (p == p_up->left.get());


            if(!p->left.get() && !p->right.get()){
                std::cout << "is leaf! " << std::endl;
                is_left ? p_up->left.reset() : p_up->right.reset() ;
            }
            else if(p->left.get() && p->right.get()){
                std::cout << "has left && right... " << std::endl;
    
                Node* ps = (++i).get_current();
                Node* ps_up = ps->up;

                bool is_left_s = (ps == ps_up->left.get());

                Node* tmp_l = p->left.get();
                Node* tmp_r = p->right.get();
                if(is_left && is_left_s){
                    p_up->left.reset(ps_up->left.release());
                    p = p_up->left.get();
                }
                if(!is_left && is_left_s){
                    p_up->right.reset(ps_up->left.release());
                    p = p_up->right.get();
                }
                if(is_left && !is_left_s){
                    p_up->left.reset(ps_up->right.release());
                    p = p_up->left.get();
                }
                if(!is_left && !is_left_s){
                    p_up->right.reset(ps_up->right.release());
                    p = p_up->right.get();
                }
                std::cout << p << std::endl;
                std::cout << p->left.get() << std::endl;
                std::cout << p->right.get() << std::endl;
                p->up = p_up;
                p->left.reset(tmp_l);
                p->right.reset(tmp_r);
                }

            }
            else if(p->left.get()){
                std::cout << "has left... " << std::endl;
                p->left.get()->up=p_up;
                if(is_left)
                    p_up->left.reset(p->left.release());
                else
                    p_up->right.reset(p->left.release());

            }
            else{
                std::cout << "has right... " << std::endl;
                p->right.get()->up=p_up;
                if(is_left)
                    p_up->left.reset(p->right.release()); 
                else
                    p_up->right.reset(p->right.release());
            }
        }  
    }*/

    Node* buildBalancedTree(std::vector<pair_type> x, int a, int b){
        if(a>b){
            std::cout << a << ">" << b << "!" << std::endl;
            return nullptr;
        }
        int h = a+(b-a)/2;  //half
        std::cout << "building subtree from "<< a << " to " << b << " (half=" << h << ")" << std::endl;
        Node* root_ = new Node{x[h]};
        std::cout << "building left subtree from "<< a << " to " << h-1 << std::endl;
        root_->left.reset(buildBalancedTree(x,a,h-1));
        std::cout << "building left subtree from "<< h+1 << " to " << b << std::endl;
        root_->right.reset(buildBalancedTree(x,h+1,b));
        if(root_->left){
            //std::cout << "left subtree of " << a << ", " <<  b << " has been built" << std::endl; 
            root_->left->up=root_;
        }
        if(root_->right){
            root_->right->up=root_;
        }
        return root_;
    }

    void balance(){
        std::cout << "balancing..."<< std::endl;
        std::vector<pair_type> v;
        for(auto i : *this){
            v.push_back(i);
        }
        std::cout << "built support vector"<< std::endl;
        root.reset(buildBalancedTree(v,0,v.size()-1));
    };

    void print_depth(Node* root_){
        if(!root_)
            return;
        std::cout << root_->item.first << "\n";
        std::cout << "going left..." << std::endl;
        print_depth(root_->left.get());
        std::cout << "going right..." << std::endl;
        print_depth(root_->right.get());
    }
    
    void print(){
        print_depth(root.get());
        std::cout << std::endl;
    }

    friend
    std::ostream& operator<<(std::ostream& os, const Bst& x){
        std::cout << "printing..." << std::endl;
        //if(!x.root)
        //    return os;
        for(auto tmp : x){
            os << tmp.second << ", ";
        }
        os << std::endl;    
        return os;
    }


};


int main(){
    Bst<int,std::string> a;
    a.insert(std::pair<int,std::string>{0,"zero"});
    a.insert(std::pair<int,std::string>{1,"one"});
    a.emplace(2,"two");
    a.emplace(3,"three");
    a[-3]=std::string("minus three");
    a[-7]=std::string("minus seven");
    //a.insert(std::pair<int,std::string>{-3,"minus three"});
    a.insert(std::pair<int,std::string>{-3,"minus three - bis"});
    a.insert(std::pair<int,std::string>{100,"one hundred"});
    
    //a.print();
    std::cout << a << std::endl;
    //std::cout << a[100] << std::endl;

    a.balance();
    //a.print();
    //std::cout << a << std::endl;

    a.emplace(101,"one hundred and one");
    a.emplace(-8,"minus eight");
    
    a.emplace(-6,"minus six");
    a.emplace(-5,"minus five");
    a.emplace(-4,"minus four");
    
    //a.erase(100);
    std::cout << a << std::endl;
    a.erase(1);
    std::cout << a << std::endl;
    a.erase(-5);
    std::cout << a << std::endl;
    a.erase(-7);
    std::cout << a << std::endl;
    a.erase(3);
    std::cout << a << std::endl;
/*

    std::cout << "found " << a.find(-5)->second << "!" << std::endl;
    std::cout << "found -55? " << (a.find(-55)!=a.end()) << std::endl; 

    Bst<int,std::string> b=a;
    b.insert(std::pair<int,std::string>{-7,"minus seven"});
    std::cout << b << std::endl;

    b.clear();
    std::cout << b << std::endl;
    b.insert(std::pair<int,std::string>{-7,"minus seven"});
    std::cout << b << std::endl;

    Bst<int,std::string> c;
    c=a;
    c.insert(std::pair<int,std::string>{7,"seven"});
    std::cout << c << std::endl;

*/

}


// Depth-first tree traversal using generators

#include "coroutines/generator.hpp"

#include <memory>
#include <random>
#include <algorithm>
#include <iterator>

template<typename T>
class binary_tree
{
private:

    struct node;
    typedef std::unique_ptr<node> node_ptr;

    struct node
    {
        node(const T& v) : data(v) { }

        T data;
        node_ptr left;
        node_ptr right;
    };

    node_ptr root_;

    void insert(const T &v, node_ptr& n)
    {
        if (!n)
        {
            n.reset(new node(v));
        }
        else
        {
            if (v < n->data)
                insert(v, n->left);
            else
                insert(v, n->right);
        }
    }

    template<typename OutIt>
    void pre_order(const node_ptr& n, OutIt& out)
    {
        if (n)
        {
            *(out++) = n->data;
            pre_order(n->left, out);
            pre_order(n->right, out);
        }
    }

    template<typename OutIt>
    void in_order(const node_ptr& n, OutIt& out)
    {
        if (n)
        {
            in_order(n->left, out);
            *out++ = n->data;
            in_order(n->right, out);
        }
    }

public:

    void insert(const T& v)
    {
        insert(v, root_);
    }

    // traversal - output to iterator

    template<typename OutIt>
    void pre_order(OutIt& out)
    {
        pre_order(root_, out);
    }

    template<typename OutIt>
    void in_order(OutIt& out)
    {
        in_order(root_, out);
    }

    // traversal - output via generator
    crs::generator<T> pre_order()
    {
        return crs::generator<T>(
            [this](crs::generator_output<T> out) {
                pre_order(out);
        });
    }

    crs::generator<T> in_order()
    {
        return crs::generator<T>(
            [this](crs::generator_output<T> out) {
                in_order(out);
        });
    }

};


int main(int, char**)
{
    binary_tree<int> tree;

    // populate tree
    std::default_random_engine reng;
    reng.seed();
    std::uniform_int_distribution<int> rdist(0, 100);

    for(int i = 0; i < 10; i++)
    {
        tree.insert(rdist(reng));
    }

    // generator does not work with std algorithms nowm is not a valid iterator
    /*
    std::cout << "Tree traversed pre-order: " << std::endl;
    std::copy(tree.pre_order(), crs::generator<int>(), std::ostream_iterator<int>(std::cout, " ,"));
    std::cout << std::endl;

    std::cout << "Tree traversed in-order: " << std::endl;
    std::copy(tree.in_order(), crs::generator<int>(), std::ostream_iterator<int>(std::cout, " ,"));
    std::cout << std::endl;
    */

    crs::generator<int> io = tree.in_order();
    crs::generator<int> po = tree.pre_order();

    std::cout << "Tree traversal result:" << std::endl;
    std::cout << "pre-order\tin-order" << std::endl;

    for(; po != crs::generator<int>(); ++po, ++io)
    {
        std::cout << *po << "\t\t" << *io << std::endl;
    }
}

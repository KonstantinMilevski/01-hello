#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>
int RandomNumber()
{
    return (std::rand() % 100);
}
int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;
    vector<int> v11 = { 1, 4, 6, 8, 10 };

    /// std::all_of, std::any_of, std::none_of; return true/false
    auto it01 =
        all_of(begin(v11), end(v11), [](int& a) { return (a % 2 == 0); });
    if (it01)
        cout << "ALL EVEN" << endl;
    else

        cout << "ALL_UNEVEN" << endl;
    /// std::for_each, std::for_each_n - for_each_n(begin(v), 3, [&](int& a) {
    /// a++;  });
    int sum = 0;
    for_each(begin(v11), end(v11), [&](int& a) {
        a++;
        sum += a;
    });
    for (auto var : v11)
        cout << var << "; "; /// 2; 5; 7; 9; 11;
    cout << sum << endl;     /// 34
    /// std::count, std::count_if; return quantity
    int it02 = count_if(begin(v11), end(v11), [](int& a) { return (a > 5); });
    cout << it02 << endl;
    /// std::mismatch Возвращает первую пару несовпадающих элементов из
    ///двух
    /// диапазонов

    /// std::find, std::find_if, std::find_if_not; return iterator ==
    auto it03 = std::find(v11.begin(), v11.end(), 5);
    cout << *it03 << endl; // 5
    /// std::adjacent_find ; return iterator position
    v11       = { 1, 89, 9, 9, 10, 15 };
    auto it04 = adjacent_find(begin(v11), end(v11));
    cout << *it04 << endl; // 9-first
    auto it05 = adjacent_find(begin(v11), end(v11),
                              [](int& a, int& b) { return (b - a == 5); });
    cout << *it05 << endl; // 10-first

    /// std::search_n - Ищет в диапазоне [first, last) первую
    ///последовательность
    /// count одинаковых элементов, каждый из которых равен заданному
    ///значению
    /// value
    /// std::search -  Ищет первое вхождение последовательности
    /// элементов [s_first, s_last) в диапазон [first, last - (s_last -
    /// s_first))
    //--------------------------------------------------------------------------------

    /// std::copy, std::copy_if
    vector<int> v1 = { 1, 4, 6, 8, 10 };
    vector<int> v2(5);
    std::copy(begin(v1), std::end(v1), begin(v2));
    /// std::copy_n
    // copy_n(begin(v), 2, back_inserter(v2)); // back_inserter(v2)
    /// std::copy_backward - "copy" from end till begining

    /// std::move  OutputIt move( InputIt first, InputIt last, OutputIt
    /// d_first
    /// )  it=(d_first + (last - first))

    /// std::move_backward  BidirIt2 move_backward( BidirIt1 first,
    /// BidirIt1
    /// last, BidirIt2 d_last )

    /// std::fill    void fill( ForwardIt first, ForwardIt last,
    /// const T&
    /// value
    /// )

    /// std::fill_n  OutputIt fill_n( OutputIt first,
    /// Size count, const T&
    /// value
    /// );
    /// if count>0? It==&(count+1): it==&(
    /// first).

    /// std::transform
    /// OutputIt transform( InputIt first1,
    /// InputIt last1, OutputIt
    /// d_first,
    ///              UnaryOperation unary_op );
    /// OutputIt transform( InputIt1 first1, InputIt1 last1,
    /// InputIt2
    /// first2,
    ///  OutputIt d_first, BinaryOperation binary_op );
    /// Output-итератор на элемент, следующий за последним
    /// изменённым.
    std::string s("hello");
    std::transform(s.begin(), s.end(), s.begin(),
                   [](char c) { return std::toupper(c); });
    std::cout << s << endl;
    vector<int> v3 = { 1, 4, 6, 8, 10 };
    std::transform(v3.begin(), v3.end(), v3.begin(),
                   [](int& c) { return c * 2; });
    std::copy(v3.begin(), v3.end(), std::ostream_iterator<int>(std::cout, " "));
    cout << endl;
    /// std::lower_bound,std::upper_bound, more or less pos element
    //    std::vector<int> d = { 1, 2, 3, 4, 5, 6 };

    //    auto lower = std::lower_bound(d.begin(), d.end(), 2);
    //    auto upper = std::upper_bound(d.begin(), d.end(), 4);

    //    std::copy(lower, upper, std::ostream_iterator<int>(std::cout,
    //    "
    //"));
    //    cout << endl;

    /// std::generate  void generate( ForwardIt first, ForwardIt
    /// last,        Generator/// g );

    std::vector<int> v4 = { 1, 4, 6, 8, 10 };
    generate(begin(v4), end(v4), RandomNumber);
    std::copy(v4.begin(), v4.end(), std::ostream_iterator<int>(std::cout, " "));
    cout << endl;

    /// std::remove, std::remove_if
    /// ForwardIt remove( ForwardIt first, ForwardIt last, const T& value );
    /// ForwardIt remove_if( ForwardIt first, ForwardIt last, UnaryPredicate p
    /// );
    std::vector<int> v5 = { 1, 4, 6, 8, 10 };
    remove_if(v5.begin(), v5.end(), [](int c) { return c == 4; });
    std::copy(v5.begin(), v5.end(), std::ostream_iterator<int>(std::cout, " "));
    cout << endl;

    /// std::remove_copy, std::remove_copy_if
    /// OutputIt remove_copy( InputIt first, InputIt last, OutputIt d_first,
    /// const T& value );

    /// OutputIt remove_copy_if( InputIt first, InputIt last, OutputIt
    /// d_first,UnaryPredicate p );
    /// ///--------------------------------------
    std::vector<int> v6 = { 1, 4, 6, 8, 10 };
    replace_if(
        v6.begin(), v6.end(), [](int c) { return c < 7; }, 11);
    std::copy(v6.begin(), v6.end(), std::ostream_iterator<int>(std::cout, " "));
    cout << endl;
    /// std::replace_copy, std::replace_copy_if  --like remove
    /// OutputIt replace_copy( InputIt first, InputIt last, OutputIt d_first,
    ///                  const T& old_value, const T& new_value );
    /// OutputIt replace_copy_if( InputIt first, InputIt last, OutputIt d_first,
    /// UnaryPredicate p, const T& new_value );
    /// --------------------------------------
    /// std::reverse  void reverse( BidirIt first, BidirIt last );
    /// ---------------------------------------------------------
    /// std::rotate  ForwardIt rotate( ForwardIt first, ForwardIt n_first,
    /// ForwardIt last );
    /// ---------------------------------------------------
    /// std::partition
    /// Переставляет элементы в диапазоне [first, last) так, чтобы все элементы,
    /// для которых предикат p возвращает true, предшествовали элементам, для
    /// которых предикат p возвращает false. Относительный порядок элементов не
    /// сохраняется.
    /// ---------------------------
    /// std::is_partitioned  -- true if partition
    /// bool is_partitioned( InputIt first, InputIt last, UnaryPredicate p )
    /// -------------------------------
    /// std::sort  void sort( RandomIt first, RandomIt last, Compare comp );
    /// ---------------------------------
    /// std::accumulate
    /// T accumulate( InputIt first, InputIt last, T init );
    /// T accumulate( InputIt first, InputIt last, T init,
    ///        BinaryOperation op );
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    //    int         sum     = accumulate(v.begin(), v.end(), 0);
    //    int         product = accumulate(v.begin(), v.end(), 1, multiply);
    //    std::string magic =
    //        std::accumulate(v.begin(), v.end(), std::string(),
    //        magic_function);

    //    std::cout << sum << '\n' << product << '\n' << magic << '\n';

    return 0;
}

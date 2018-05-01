#include <vector>
#include <functional>
#include <assert.h>
#include <limits>
#include <cmath>
#include <cstdlib>

template <class T>
class polynomial; 

template <typename T>
    std::pair< polynomial<T>, polynomial<T> >
quotient_remainder(const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> r = a;
    polynomial<T> D = b;
    if (r.degree() >= 0) {
        int deg_diff = r.degree() - D.degree();
        std::vector<T> q(deg_diff + 1);
        polynomial<T> d;
        while (r.degree() >= D.degree()) 
        { 
            d = D;
            d.shift(deg_diff);
            q[deg_diff] = r[0] / D[0];

            d *= q[deg_diff];
            r -= d;

            deg_diff = r.degree() - D.degree();
        }
        std::reverse(q.begin(), q.end());
        return std::pair< polynomial<T>, polynomial<T> >(polynomial<T>(q), r); 
    } else {
        assert(false);
    }
}

template <class T>
class polynomial
{
    public:
        // typedefs:
        typedef typename std::vector<T>::value_type value_type;
        typedef typename std::vector<T>::size_type size_type;

        // construct:
        polynomial(){}
        polynomial(std::initializer_list<T> l) :
            m_degree(l.size() - 1),
            m_data(l)
    {}

        polynomial(std::vector<T> l) :
            m_degree(l.size() - 1),
            m_data(l)
    {}

        polynomial& operator=(const polynomial& rhs) 
        {
            m_data = rhs.data();
            m_degree = rhs.degree();
            return (*this);
        };

        // access:
        int degree()const
        {
            return m_degree;
        }
        value_type& operator[](polynomial<T>::size_type i)
        {
            return m_data[i];
        }

        const value_type& operator[](polynomial<T>::size_type i)const
        {
            return m_data[i];
        }
        std::vector<T> const& data() const
        {
            return m_data;
        }
        std::vector<T>& data()
        {
            return m_data;
        }

        // operators: 
        template <class U>
            polynomial& operator +=(const U& value) 
            {
                m_data[m_degree] += value;
                return (*this);
            }
        template <class U>
            polynomial& operator -=(const U& value)
            {
                m_data[m_degree] -= value;
                return (*this);
            }
        template <class U>
            polynomial& operator *=(const U& value)
            {
                std::transform(m_data.begin(), m_data.end(), m_data.begin(), 
                        std::bind(std::multiplies<T>(), std::placeholders::_1, value));
                return (*this);
            }
        template <class U>
            polynomial& operator /=(const U& value)
            {
                std::transform(m_data.begin(), m_data.end(), m_data.begin(), 
                        std::bind(std::divides<T>(), std::placeholders::_1, value));
                return (*this);
            }
        template <class U>
            polynomial& operator %=(const U& value)
            {
                std::transform(m_data.begin(), m_data.end(), m_data.begin(), 
                        std::bind(std::modulus<T>(), std::placeholders::_1, value));
                return (*this);
            }
        template <class U>
            polynomial& operator +=(const polynomial<U>& p) 
            {
                pad(p.degree());
                assert(m_degree >= p.degree());
                auto start = m_data.begin() + (m_degree - p.degree());
                std::transform(start, m_data.end(), p.data().begin(), start, std::plus<U>());        
                trim();
                return (*this);
            }
        template <class U>
            polynomial& operator -=(const polynomial<U>& p)
            {
                pad(p.degree());
                assert(m_degree >= p.degree());

                auto start = m_data.begin() + (m_degree - p.degree());
                std::transform(start, m_data.end(), p.data().begin(), start, std::minus<U>());        
                trim();
                return (*this);
            }
        template <class U>
            polynomial& operator *=(const polynomial<U>& p)
            {
                pad(p.degree());
                assert(m_degree == p.degree());
                std::vector<T> results = std::vector<T>(p.degree() + degree() + 1);

                for (int i = 0; i < p.degree(); i++) {
                    for (int j = 0; j < degree(); j++) {
                        results[i + j] += p[i] * (*this)[j];
                    }
                }

                m_data.assign(results);
                trim();
                return (*this);
            }
        template <class U>
            polynomial& operator /=(const polynomial<U>& p)
            {
                m_data = quotient_remainder<T>((*this), p).first.data();
                trim();
                return (*this);
            }
        template <class U>
            polynomial& operator %=(const polynomial<U>& p)
            {
                m_data = quotient_remainder<T>((*this), p).second.data();
                trim();
                return (*this);
            }
        // methods
        polynomial<T> derivative()
        {
            std::vector<T> deriv = std::vector<T>(m_degree);
            for (int i = 0; i < m_degree; i++) 
            {
                deriv[i] = m_data[i] * static_cast<T>(m_degree - i);
            }
            return polynomial<T>(deriv);
        }

        T eval(T x)
        {
            auto reverse = m_data; 
            std::reverse_copy(m_data.begin(), m_data.end(), reverse.begin());
            T result = 0;
            T x_pow = 1;
            std::for_each(reverse.begin(), reverse.end(), 
                [&x, &x_pow, &result](T c)
                {
                    result += static_cast<T>(c) * x_pow;
                    x_pow *= x;
                    });
            return result;
        }

        void shift(int i) 
        {
            m_data.resize(m_degree + i + 1, 0);
            m_degree += i;
        }

    private:
        int m_degree;
        std::vector<T> m_data;

        void trim() {
            typename std::vector<T>::iterator it = m_data.begin();

            while(it != m_data.end()) {
                // TODO cover floating cases by checking type
                // T epsilon = std::numeric_limits<T>::min();
                // cover float, double, and int cases
                if (std::abs(*it) == 0) {
                    it = m_data.erase(it);
                } else {
                    break;
                }
            }
            m_degree = m_data.size() - 1;
        }

        void pad(size_type deg) {
            if (deg > m_degree) {
                m_data.resize(deg + 1, 0);
                std::rotate(m_data.begin(), m_data.begin() + m_degree + 1, m_data.end());
                m_degree = deg;
            }
        }
};

    template <class T>
polynomial<T> operator + (const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy += b;
    return copy;
}
    template <class T>
polynomial<T> operator - (const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy -= b;
    return copy;
}
    template <class T>
polynomial<T> operator * (const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy *= b;
    return copy;
}
    template <class T>
polynomial<T> operator / (const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy /= b;
    return copy;
}
    template <class T>
polynomial<T> operator % (const polynomial<T>& a, const polynomial<T>& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy %= b;
    return copy;
}

    template <class T, class U>
polynomial<T> operator + (const polynomial<T>& a, const U& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy += b;
    return copy;
}
    template <class T, class U>
polynomial<T> operator - (const polynomial<T>& a, const U& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy -= b;
    return copy;
}
    template <class T, class U>
polynomial<T> operator * (const polynomial<T>& a, const U& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy *= b;
    return copy;
}
    template <class T, class U>
polynomial<T> operator / (const polynomial<T>& a, const U& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy /= b;
    return copy;
}
    template <class T, class U>
polynomial<T> operator % (const polynomial<T>& a, const U& b)
{
    polynomial<T> copy = polynomial<T>(a);
    copy %= b;
    return copy;
}

    template <class U, class T>
polynomial<T> operator + (const U& a, const polynomial<T>& b)
{
    return b + a;
}
    template <class U, class T>
polynomial<T> operator - (const U& a, const polynomial<T>& b)
{
    return b - a;
}
    template <class U, class T>
polynomial<T> operator * (const U& a, const polynomial<T>& b)
{
    return b * a;
}

    template <class T>
polynomial<T> operator - (const polynomial<T>& a)
{
    return a * -1;
}

    template <class T>
bool operator == (const polynomial<T> &a, const polynomial<T> &b)
{
    return ((a.degree() == b.degree()) and (a == b));
}
    template <class T>
bool operator != (const polynomial<T> &a, const polynomial<T> &b)
{
    return (not (a==b));
}

template <class charT, class traits, class T>
    std::basic_ostream<charT, traits>& operator <<
(std::basic_ostream<charT, traits>& os, const polynomial<T>& poly)
{
    int i;
    if (poly.degree() == -1) {
        os << 0 << "\n";
    } else {
        for (i = 0; i < poly.degree(); i++) {
            os << poly[i] << "x^" << poly.degree() - i << " + " ;
        }
        os << poly[i] << "\n"; 
    }
    return os;
}
// Gets x for a pol
template <class T>
T newton(polynomial<T> pol) {
    T r = pol.eval(0);
    T x = 0;
    
    polynomial<T> deriv = pol.derivative();
    
    while (r != 0) {
        T delta = r / deriv.eval(x);
        x -= delta;
        r = pol.eval(x);
    }
    
    return x;
}
// Factors the pol and returns a vector with the factos
template <class T>
std::vector<T> solve(polynomial<T> pol){
    polynomial<T> p = pol;
    int degree = p.degree();
    std::vector<T> factors;
    while (degree != 0) {
        T factor = newton(p);
        factors.push_back(factor);
        polynomial<T> q = polynomial<T>({1,-1*factor});
        p = p/q;
        degree = p.degree();
    }
    return factors;
}

